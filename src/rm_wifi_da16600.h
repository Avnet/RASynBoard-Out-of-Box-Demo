/*
 * rm_wifi_da16600.h
 *
 *  Created on: 2022年12月7日
 *      Author: Wenxue
 */

#ifndef RM_WIFI_DA16600_H_
#define RM_WIFI_DA16600_H_

#include "rm_wifi_api.h"
#include "rm_wifi_config.h"
#include "rm_atcmd.h"

#ifndef IPADDR_LEN
#define IPADDR_LEN       16
#endif

typedef enum
{
    MODE_STATION,
    MODE_SOFTAP,
} da16600_workmode_t;

typedef enum
{
    TKIP,
    AES,
    TKIP_AES,
} da16600_enc_mode;

typedef struct
{
    char               chipname[16];
    char               sdkver[16];
    char               swver[64];
    char               macaddr[64];
}da16600_hwinfo_t;

typedef enum
{
    DISCONNECT,
    CONNECTING,
    ASSOCIATED,
    OBTAINIP,
    CONNECTED = OBTAINIP,
} da16600_status_t;

typedef struct
{
    da16600_workmode_t mode;
    da16600_status_t   status;
    char               ssid[wificonfigMAX_SSID_LEN];
    char               ipaddr[IPADDR_LEN];
    char               netmask[IPADDR_LEN];
    char               gateway[IPADDR_LEN];
}da16600_netinfo_t;

fsp_err_t rm_wifi_da16600_init(void);
fsp_err_t rm_wifi_da16600_deinit(void);

fsp_err_t rm_wifi_da16600_print_hwinfo(void);
void rm_wifi_da16600_print_netinfo(da16600_netinfo_t info);

fsp_err_t rm_wifi_da16600_echo_enable(void);
fsp_err_t rm_wifi_da16600_check_at(uint32_t retries);

fsp_err_t rm_wifi_da16600_get_netinfo(da16600_netinfo_t *info);
fsp_err_t rm_wifi_da16600_set_workmode(da16600_workmode_t mode);
fsp_err_t rm_wifi_da16600_get_macaddr(uint8_t *macaddr);
fsp_err_t rm_wifi_da16600_scan(WIFIScanResult_t *results, uint32_t maxNetworks);
fsp_err_t rm_wifi_da16600_connect(da16600_netinfo_t *netinfo, const char *ssid, WIFISecurity_t security, const char *passphrase);
fsp_err_t rm_wifi_da16600_disconnect(void);
void rm_wifi_da16600_check_disconnect(da16600_netinfo_t *netinfo);
fsp_err_t rm_wifi_da16600_enable_dhcp(int enable);
fsp_err_t rm_wifi_da16600_set_ipaddr(char *ipaddr, char *netmask, char *gateway);
fsp_err_t rm_wifi_da16600_get_ipaddr(char *ipaddr, char *netmask, char *gateway);
fsp_err_t rm_wifi_da16600_set_dns(char *dns1, char *dns2);
fsp_err_t rm_wifi_da16600_dns_query(const char *domain, char *ipaddr);
fsp_err_t rm_wifi_da16600_ping(char *ipaddr, uint32_t count, uint32_t interval_ms);

/* DA16600 can only support 1 TCP client and 1 TCP server */
typedef enum
{
    CID_TCP_SERVER,
    CID_TCP_CLIENT,
    CID_UDP,
} cid_type_t;

fsp_err_t rm_wifi_da16600_tcp_listen(int port);
fsp_err_t rm_wifi_da16600_tcp_connect(char *host, int port);
fsp_err_t rm_wifi_da16600_socket_disconnect(cid_type_t cid);
fsp_err_t rm_wifi_da16600_socket_info(cid_type_t cid, int *lport, char *rIP, int *rport);
int32_t rm_wifi_da16600_socket_send(cid_type_t cid, char *ipaddr, int port, uint8_t *data, uint32_t bytes);

#endif /* RM_WIFI_DA16600_H_ */
