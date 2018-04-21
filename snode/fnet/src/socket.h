#ifndef SOCKET_H_FNET
#define SOCKET_H_FNET
#include <stdbool.h>
#include <stddef.h>
#include "ip_address.h"

typedef unsigned fnet_socket_t;

extern fnet_socket_t const FNET_INVALID_SOCKET;

typedef enum
{
    FNET_SOCK_STREAM,
    FNET_SOCK_DGRAM
} fnet_sock_t;

enum
{
    FNET_SOCK_BROADCAST     = 1 << 0
};

bool            fnet_socket_init();
void            fnet_socket_uninit();
fnet_socket_t   fnet_socket_create(fnet_sock_t sock_type);
fnet_socket_t   fnet_socket_connect(fnet_address_t const *addr);
void            fnet_socket_shutdown(fnet_socket_t sock);
void            fnet_socket_close(fnet_socket_t sock);
fnet_socket_t   fnet_socket_bind(fnet_sock_t sock_type, fnet_address_t const *addr);
fnet_socket_t   fnet_socket_accept(fnet_socket_t sock, fnet_address_t *addr);
bool            fnet_socket_send(fnet_socket_t sock, const char *buf, size_t len);
bool            fnet_socket_sendto(fnet_socket_t sock, const char *buf, size_t lenf, fnet_address_t const *addr);
bool            fnet_socket_recv(fnet_socket_t sock, char *buf, size_t len, size_t *read_len);
bool            fnet_socket_recvfrom(fnet_socket_t sock, char *buf, size_t len, size_t *read_len, fnet_address_t *addr);
bool            fnet_socket_select(fnet_socket_t *sockets,
                                   size_t num,
                                   fnet_socket_t *rs,
                                   size_t *rs_num,
                                   fnet_socket_t *es,
                                   size_t *es_num,
                                   size_t msec);
size_t          fnet_socket_bind_all(fnet_sock_t sock_type, fnet_socket_t *ifaces, size_t size, int flags);

#endif
