#include "service.h"
#include "commands.h"
#include <fcommon/log.h>
#include <fnet/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>


static char const *FNODE_BROADCAST_ADDR = "255.255.255.255:9999";
static size_t const FNODE_HELLO_FREQ = 1000;    // msec

typedef enum
{
    FSVC_INIT = 0,
    FSVC_PROCESS_COMMANDS,
    FSVC_NOTIFY_STATUS
} fnode_service_state_t;

struct fnode_service
{
    volatile int                ref_counter;
    char                        sn[FSN_LENGTH];
    char                        dev_type[FDEV_TYPE_LENGTH];
    fnet_socket_t               socket;
    fnet_socket_t               ifaces[256];
    size_t                      ifaces_num;
    fnode_service_state_t       state;
    fnode_service_cmd_handler_t data_handler;
    uint32_t                    keepalive;
    uint32_t                    resp_freq;
    fnet_address_t              server;
    size_t                      last_cmd_time;
};

static fnode_service_state_t fnode_service_init_handler(fnode_service_t *svc);
static bool                  fnode_service_recv_cmd(fnode_service_t *svc);
static fnode_service_state_t fnode_service_notify_status(fnode_service_t *svc);
static fnode_service_state_t fnode_service_process_commands(fnode_service_t *svc);
static size_t                fnode_service_time();

static void fnode_service_create_iface_sockets(fnode_service_t *svc)
{
    svc->ifaces_num = fnet_socket_bind_all(FNET_SOCK_DGRAM, svc->ifaces, sizeof svc->ifaces / sizeof *svc->ifaces, FNET_SOCK_BROADCAST);
}

static void fnode_service_close_iface_sockets(fnode_service_t *svc)
{
    for (size_t i = 0; i < svc->ifaces_num; ++i)
        fnet_socket_close(svc->ifaces[i]);
}

fnode_service_t *fnode_service_create(char const sn[FSN_LENGTH], char const dev_type[FDEV_TYPE_LENGTH])
{
    fnode_service_t *svc = malloc(sizeof(fnode_service_t));
    if (!svc)
    {
        FLOG_ERR("Unable to allocate memory for nodes interlink");
        return 0;
    }
    memset(svc, 0, sizeof *svc);

    svc->ref_counter = 1;
    memcpy(svc->sn, sn, FSN_LENGTH);
    memcpy(svc->dev_type, dev_type, FDEV_TYPE_LENGTH);
    svc->state = FSVC_INIT;
    svc->socket = FNET_INVALID_SOCKET;

    fnet_socket_init();
    fnode_service_create_iface_sockets(svc);

    return svc;
}

fnode_service_t *fnode_service_retain(fnode_service_t *svc)
{
    if (svc)
        svc->ref_counter++;
    else
        FLOG_ERR("Invalid interlink");
    return svc;
}

void fnode_service_release(fnode_service_t *svc)
{
    if (svc)
    {
        if (!svc->ref_counter)
            FLOG_ERR("Invalid interlink");
        else if (!--svc->ref_counter)
        {
            fnode_service_close_iface_sockets(svc);
            fnet_socket_uninit();
            memset(svc, 0, sizeof *svc);
            free(svc);
        }
    }
    else
        FLOG_ERR("Invalid interlink");
}

void fnode_service_notify_state(fnode_service_t *svc, char const state[FMAX_DATA_LENGTH], uint32_t size)
{
    if (svc)
    {
        size_t time_now = fnode_service_time();

        fcmd_node_status cmd = {{ FCMD_CHARS(FCMD_DATA) }};
        memcpy(cmd.sn,       svc->sn,       sizeof svc->sn);
        memcpy(cmd.data,     state,         size > FMAX_DATA_LENGTH ? FMAX_DATA_LENGTH : size);

        if (fnet_socket_sendto(svc->socket, (const char *)&cmd, sizeof cmd, &svc->server))
            svc->last_cmd_time = time_now;
    }
}

void fnode_service_reg_handler(fnode_service_t *svc, fnode_service_cmd_handler_t handler)
{
    if (svc)
        svc->data_handler = handler;
}

void fnode_service_update(fnode_service_t *svc)
{
    if (!svc)
        return;

    switch(svc->state)
    {
        case FSVC_INIT:
        {
            svc->state = fnode_service_init_handler(svc);
            break;
        }

        case FSVC_NOTIFY_STATUS:
        {
            svc->state = fnode_service_notify_status(svc);
            break;
        }

        case FSVC_PROCESS_COMMANDS:
        {
            svc->state = fnode_service_process_commands(svc);
            break;
        }
    }
}

fnode_service_state_t fnode_service_init_handler(fnode_service_t *svc)
{
    // get broadcast address
    fnet_address_t broadcast_addr;
    if (!fnet_str2addr(FNODE_BROADCAST_ADDR, &broadcast_addr))
    {
        FLOG_ERR("Invalid broadcast address");
        return FSVC_INIT;
    }

    // prepare command buffer
    fcmd_hello cmd = {{ FCMD_CHARS(FCMD_HELO) }};
    memcpy(cmd.sn,       svc->sn,       sizeof svc->sn);
    memcpy(cmd.dev_type, svc->dev_type, sizeof svc->dev_type);

    // broadcast hello message to each interface
    for(size_t i = 0; i < svc->ifaces_num; ++i)
        fnet_socket_sendto(svc->ifaces[i], (const char *)&cmd, sizeof cmd, &broadcast_addr);

    fnet_socket_t rs[svc->ifaces_num], es[svc->ifaces_num];
    size_t rs_num = 0, es_num = 0;
    if (fnet_socket_select(svc->ifaces, svc->ifaces_num, &rs, &rs_num, &es, &es_num, FNODE_HELLO_FREQ) && rs_num)
    {
        if (rs_num)
            svc->socket = rs[0];
        if (fnode_service_recv_cmd(svc))
            return FSVC_NOTIFY_STATUS;
    }

    return FSVC_INIT;
}

fnode_service_state_t fnode_service_notify_status(fnode_service_t *svc)
{
    size_t time_now = fnode_service_time();
    size_t time_diff = time_now - svc->last_cmd_time;

    if (time_diff >= svc->keepalive)
    {
        fcmd_ping cmd = {{ FCMD_CHARS(FCMD_PING) }};
        memcpy(cmd.sn,       svc->sn,       sizeof svc->sn);
        fnet_socket_sendto(svc->socket, (const char *)&cmd, sizeof cmd, &svc->server);
        svc->last_cmd_time = time_now;
    }

    return FSVC_PROCESS_COMMANDS;
}

fnode_service_state_t fnode_service_process_commands(fnode_service_t *svc)
{
    fnet_socket_t rs, es;
    size_t rs_num = 0, es_num = 0;

    size_t time_now = fnode_service_time();
    size_t time_elapsed = time_now - svc->last_cmd_time;
    size_t time_wait = time_elapsed < svc->keepalive ? svc->keepalive - time_elapsed : 1;

    if (fnet_socket_select(svc->socket, 1, &rs, &rs_num, &es, &es_num, time_wait) && rs_num)
    {
        // TODO
    }

    return FSVC_NOTIFY_STATUS;
}

bool fnode_service_recv_cmd(fnode_service_t *svc)
{
    char buf[65536];
    size_t read_len = 0;
    fnet_address_t addr;

    if (fnet_socket_recvfrom(svc->socket, buf, sizeof buf, &read_len, &addr))
    {
        if (read_len < FCMD_ID_LENGTH)
            return false;

        uint32_t const cmd = *(uint32_t const*)buf;

        switch(cmd)
        {
            case FCMD_CONF:
            {
                if (read_len < sizeof cmd + 4 + 4)
                    return false;
                char keepalive_str[5], resp_freq_str[5];
                memcpy(keepalive_str, buf + sizeof cmd, 4);
                memcpy(resp_freq_str, buf + sizeof cmd + 4, 4);
                keepalive_str[4] = 0;
                resp_freq_str[4] = 0;
                svc->keepalive = atoi(keepalive_str);
                svc->resp_freq = atoi(resp_freq_str);
                svc->server = addr;
                return true;
            }
        }
    }
    return false;
}

size_t fnode_service_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
