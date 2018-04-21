#include "../../socket.h"
#include <fcommon/limits.h>
#include <fcommon/log.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <net/if.h>
#include <errno.h>

fnet_socket_t const FNET_INVALID_SOCKET = (fnet_socket_t)~0u;

static int fnet_sock_type2posix(fnet_sock_t sock_type)
{
    switch(sock_type)
    {
        case FNET_SOCK_STREAM:  return SOCK_STREAM;
        case FNET_SOCK_DGRAM:   return SOCK_DGRAM;
    }
    return -1;
}

static struct timeval msec2timeval(size_t msec)
{
    struct timeval ts;
    ts.tv_sec = msec / 1000;
    ts.tv_usec = (msec - ts.tv_sec * 1000) * 1000;
    return ts;
}

bool fnet_socket_init()
{
    return true;
}

void fnet_socket_uninit()
{}

bool fnet_socket_select(fnet_socket_t *sockets,
                        size_t num,
                        fnet_socket_t *rs,
                        size_t *rs_num,
                        fnet_socket_t *es,
                        size_t *es_num,
                        size_t msec)
{
    if (!sockets || !num)   return false;
    if (!rs && !es)         return false;
    if (rs && !rs_num)      return false;
    if (es && !es_num)      return false;

    fd_set readfds;
    fd_set exceptfds;
    fnet_socket_t nfds = 0;

    for(size_t i = 0; i < num; ++i)
        nfds = nfds < sockets[i] ? sockets[i] : nfds;

    if (rs)
    {
        FD_ZERO(&readfds);
        for(size_t i = 0; i < num; ++i)
            FD_SET(sockets[i], &readfds);
        *rs_num = 0;
    }

    if (es)
    {
        FD_ZERO(&exceptfds);
        for(size_t i = 0; i < num; ++i)
            FD_SET(sockets[i], &exceptfds);
        *es_num = 0;
    }

    struct timeval timeout;
    if (msec != (size_t)-1)
        timeout = msec2timeval(msec);

    int ret = select(nfds + 1, rs ? &readfds : 0, 0, es ? &exceptfds : 0, msec != (size_t)-1 ? &timeout : 0);
    if (ret < 0)
    {
        FLOG_ERR("Socket waiting was failed");
        return false;
    }

    size_t rsi = 0;
    size_t esi = 0;

    for(size_t i = 0; i < num; ++i)
    {
        if (rs && FD_ISSET(sockets[i], &readfds))
            rs[rsi++] = sockets[i];
        if (es && FD_ISSET(sockets[i], &exceptfds))
            es[esi++] = sockets[i];
    }

    if (rs_num) *rs_num = rsi;
    if (es_num) *es_num = esi;
    return true;
}

void fnet_socket_close(fnet_socket_t sock)
{
    if (sock != FNET_INVALID_SOCKET)
        close((int)sock);
}

fnet_socket_t fnet_socket_bind(fnet_sock_t sock_type, fnet_address_t const *addr)
{
    if (!addr)
    {
        FLOG_ERR("Invalid address");
        return FNET_INVALID_SOCKET;
    }

    int sock = socket(addr->ss_family, fnet_sock_type2posix(sock_type), 0);
    if (sock == -1)
    {
        FLOG_ERR("Unable to create new socket");
        return FNET_INVALID_SOCKET;
    }

    if (bind(sock, (const struct sockaddr*)addr, sizeof *addr) == -1)
    {
        FLOG_ERR("Socket bind error");
        close(sock);
        return FNET_INVALID_SOCKET;
    }

    if (sock_type == FNET_SOCK_STREAM
        && listen(sock, FMAX_ACCEPT_CONNECTIONS) == -1)
    {
        FLOG_ERR("Socket listen error");
        close(sock);
        return FNET_INVALID_SOCKET;
    }

    return (fnet_socket_t)sock;
}

fnet_socket_t fnet_socket_accept(fnet_socket_t sock, fnet_address_t *addr)
{
    socklen_t addr_len = sizeof(fnet_address_t);
    int client_sock = accept((int)sock, (struct sockaddr *)addr, addr ? &addr_len : 0);
    if (client_sock == -1)
        return FNET_INVALID_SOCKET;
    return (fnet_socket_t)client_sock;
}

void fnet_socket_shutdown(fnet_socket_t sock)
{
    if (sock != FNET_INVALID_SOCKET)
        shutdown((int)sock, SHUT_RDWR);
}

fnet_socket_t fnet_socket_connect(fnet_address_t const *addr)
{
    if (!addr)
    {
        FLOG_ERR("Invalid address");
        return FNET_INVALID_SOCKET;
    }

    int sock = socket(addr->ss_family, FNET_SOCK_STREAM, 0);
    if (sock == -1)
    {
        FLOG_ERR("Unable to create new socket");
        return FNET_INVALID_SOCKET;
    }

    if (connect(sock, (const struct sockaddr*)addr, sizeof *addr) == -1)
    {
        FLOG_ERR("Unable to establish new connection");
        close(sock);
        return FNET_INVALID_SOCKET;
    }

    return (fnet_socket_t)sock;
}

fnet_socket_t fnet_socket_create(fnet_sock_t sock_type)
{
    int sock = socket(AF_INET, fnet_sock_type2posix(sock_type), 0);
    if (sock == -1)
    {
        FLOG_ERR("Unable to create dummy socket");
        return FNET_INVALID_SOCKET;
    }
    return (fnet_socket_t)sock;
}

bool fnet_socket_sendto(fnet_socket_t sock, const char *buf, size_t len, fnet_address_t const *addr)
{
    if (!buf)
    {
        FLOG_ERR("Invalid buffer");
        return false;
    }

    int res = sendto(sock, buf, len, 0, (const struct sockaddr *)addr, sizeof *addr);
    if (res == -1)
    {
        FLOG_ERR("Unable to send data");
        return false;
    }

    return true;
}

bool fnet_socket_recvfrom(fnet_socket_t sock, char *buf, size_t len, size_t *read_len, fnet_address_t *addr)
{
    if (!read_len || !buf)
    {
        FLOG_ERR("Invalid arguments");
        return false;
    }

    *read_len = 0;
    socklen_t sockaddr_len = sizeof *addr;

    int res = recvfrom(sock, buf, len, 0, (struct sockaddr *)addr, &sockaddr_len);
    switch(res)
    {
        case -1:
        {
            FLOG_ERR("Unable to recv data");
            return false;
        }

        default:
            break;
    }

    *read_len = res;

    return true;
}

size_t fnet_socket_bind_all(fnet_sock_t sock_type, fnet_socket_t *ifaces, size_t size, int flags)
{
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1)
    {
        FLOG_ERR("getifaddrs failed");
        return 0;
    }

    size_t ifaces_num = 0;

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifaces_num >= size)
            break;

        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family != AF_INET)
            continue;

        // create socket
        fnet_socket_t socket = fnet_socket_create(sock_type);
        if (socket == FNET_INVALID_SOCKET)
        {
            FLOG_ERR("Unable to create socket");
            continue;
        }

        // set flags
        if (flags & FNET_SOCK_BROADCAST)
        {
            int broadcast_enable = 1;
            if(setsockopt(socket, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0)
            {
                FLOG_ERR("broadcast options");
                fnet_socket_close(socket);
                continue;
            }
        }

        if (bind(socket, (struct sockaddr_in const *)ifa->ifa_addr, sizeof(struct sockaddr_in)) < 0)
        {
            FLOG_ERR("Socket bind error");
            fnet_socket_close(socket);
            continue;
        }

        // remember
        ifaces[ifaces_num++] = socket;
    }

    freeifaddrs(ifaddr);

    FLOG_INFO("Ifaces: %d\n", ifaces_num);

    return ifaces_num;
}
