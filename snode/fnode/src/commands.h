#ifndef FNODE_COMMANDS_H
#define FNODE_COMMANDS_H
#include <fcommon/limits.h>

#define FCMD(a, b, c, d) a | (b << 8) | (c << 16) | (d << 24)

enum
{
    FCMD_NVLD = FCMD('N', 'V', 'L', 'D'),
    FCMD_HELO = FCMD('H', 'E', 'L', 'O'),
    FCMD_CONF = FCMD('C', 'O', 'N', 'F'),
    FCMD_PING = FCMD('P', 'I', 'N', 'G'),
    FCMD_PONG = FCMD('P', 'O', 'N', 'G'),
    FCMD_DATA = FCMD('D', 'A', 'T', 'A')
};

#define FCMD_CHARS(cmd) (cmd & 0xFF), ((cmd >> 8) & 0xFF), ((cmd >> 16) & 0xFF), ((cmd >> 24) & 0xFF)

typedef struct
{
    char cmd[FCMD_ID_LENGTH];
    char sn[FSN_LENGTH];
    char dev_type[FDEV_TYPE_LENGTH];
} fcmd_hello;

typedef struct
{
    char cmd[FCMD_ID_LENGTH];
    char keepalive[4];
    char resp_freq[4];
} fcmd_conf;

typedef struct
{
    char cmd[FCMD_ID_LENGTH];
    char sn[FSN_LENGTH];
} fcmd_ping;

typedef struct
{
    char cmd[FCMD_ID_LENGTH];
} fcmd_pong;

typedef struct
{
    char cmd[FCMD_ID_LENGTH];
    char sn[FSN_LENGTH];
    char data[FMAX_DATA_LENGTH];
} fcmd_node_status;

typedef struct
{
    char cmd[FCMD_ID_LENGTH];
    char data[FMAX_DATA_LENGTH];
} fcmd_data;

/*
    Device              Controller
    HELO     ->
        SN[8]
        DEV_TYPE[4]
             <-         CONF
                            KEEPALIVE[4]     (mSec)
                            MAX_RESP_FREQ[4] (mSec)

    PING     ->
        SN[8]
             <-         PONG

    DATA     ->
        SN[8]
        VAL[]

             <-         DATA
                            VAL[]
 */
#endif
