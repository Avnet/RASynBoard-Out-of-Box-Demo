/*
 * rm_wifi_da16600.c
 *
 *  Created on: 2022年12月7日
 *      Author: Wenxue
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rm_wifi_da16600.h>

#define DEBUG_DA16600_PRINT

#ifdef  DEBUG_DA16600_PRINT
#define wifi_print(format,args...) printf(format, ##args)
#else
#define wifi_print(format,args...) do{} while(0);
#endif

static fsp_err_t parser_macaddr(char *buf, uint8_t *macaddr, int size);

static void rm_wifi_da16600_hwrst(void)
{
    R_BSP_PinWrite(DA16600_RstPin, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
    R_BSP_PinWrite(DA16600_RstPin, BSP_IO_LEVEL_HIGH);
}

fsp_err_t rm_wifi_da16600_init(void)
{
    fsp_err_t            err = FSP_SUCCESS;

    err =  rm_atcmd_init();
    if( FSP_SUCCESS != err )
    {
        return err;
    }

    /* Hardware reset DA16600 WiFi module */
    rm_wifi_da16600_hwrst();

    return 0;
}

fsp_err_t rm_wifi_da16600_deinit(void)
{
    return rm_atcmd_deinit();
}

fsp_err_t rm_wifi_da16600_echo_enable(void)
{
    fsp_err_t          err;
    char               buf[256]={'\0'};

    /* Don't know it's echo on/off now */
    rm_atcmd_send("ATE", 500, buf, sizeof(buf));

    err = rm_atcmd_check_ok("AT", 500);
    if( FSP_SUCCESS == err )
        return FSP_SUCCESS;

    /* Try again */
    rm_atcmd_send("ATE", 500, buf, sizeof(buf));

    err = rm_atcmd_check_ok("AT", 500);
    if( FSP_SUCCESS == err )
        return FSP_SUCCESS;

    return err;
}

fsp_err_t rm_wifi_da16600_check_at(uint32_t retries)
{
    for( ; retries>0; retries--)
    {
        if( FSP_SUCCESS==rm_wifi_da16600_echo_enable() )
            return FSP_SUCCESS;

        vTaskDelay (500);
    }

    return FSP_ERR_WIFI_FAILED;
}

void rm_wifi_da16600_print_netinfo(da16600_netinfo_t info)
{
    printf("\r\n");
    printf("+----------------------------------+\r\n");
    printf("|   Wireless Network Information   |\r\n");
    printf("+----------------------------------+\r\n");
    printf("   WorkMode: %s\r\n", info.mode?"SoftAP":"Station");
    printf("     Status: %d\r\n", info.status);

    if( CONNECTED == info.status )
    {
        printf("       SSID: %s\n", info.ssid);
        printf(" IP address: %s\n", info.ipaddr);
        printf("    Netmask: %s\n", info.netmask);
        printf("    Gateway: %s\n", info.gateway);
    }
    printf("\r\n");
}

fsp_err_t rm_wifi_da16600_print_hwinfo(void)
{
    fsp_err_t          err;
    char               buf[256]={'\0'};
    da16600_hwinfo_t   hwinfo;

    memset(&hwinfo, 0, sizeof(hwinfo));

    /*+----------------------+
     *| Parser Hardware info |
     *+----------------------+*/
    err = rm_atcmd_check_value("AT+CHIPNAME", 500, buf, sizeof(buf));
    if( FSP_SUCCESS == err )
    {
        strncpy(hwinfo.chipname, buf, sizeof(hwinfo.chipname));
        hwinfo.chipname[sizeof(hwinfo.chipname)-1] = '\0'; /* To remove the noisy warning */
    }

    err = rm_atcmd_check_value("AT+SDKVER", 500, buf, sizeof(buf));
    if( FSP_SUCCESS == err )
    {
        strncpy(hwinfo.sdkver, buf, sizeof(hwinfo.sdkver));
        hwinfo.sdkver[sizeof(hwinfo.sdkver)-1] = '\0'; /* To remove the noisy warning */
    }

    err = rm_atcmd_check_value("AT+VER", 500, buf, sizeof(buf));
    if( FSP_SUCCESS == err )
    {
        strncpy(hwinfo.swver, buf, sizeof(hwinfo.swver));
        hwinfo.swver[sizeof(hwinfo.swver)-1] = '\0'; /* To remove the noisy warning */
    }

    err = rm_atcmd_check_value("AT+WFMAC=?", 500, buf, sizeof(buf));
    if( FSP_SUCCESS == err )
    {
        strncpy(hwinfo.macaddr, buf, sizeof(hwinfo.macaddr));
        hwinfo.macaddr[sizeof(hwinfo.macaddr)-1] = '\0'; /* To remove the noisy warning */
    }

    printf("\r\n");
    printf("+----------------------------------+\r\n");
    printf("|   Wireless Hardware Information  |\r\n");
    printf("+----------------------------------+\r\n");
    printf("Chipname   : %s\r\n", hwinfo.chipname);
    printf("SDK Version: %s\r\n", hwinfo.sdkver);
    printf("SW Version : %s\r\n", hwinfo.swver);
    printf("MAC Address: %s\r\n", hwinfo.macaddr);
    printf("\r\n");

    return err;
}

fsp_err_t rm_wifi_da16600_get_netinfo(da16600_netinfo_t *info)
{
    fsp_err_t          err;
    char               buf[256]={'\0'};
    char              *ptr, *end;
    size_t             len;

    /*+---------------------+
     *|  Parser WiFi Status |
     *+---------------------+*/
    err = rm_atcmd_check_value("AT+WFSTAT", 1000, buf, sizeof(buf));
    if( FSP_SUCCESS == err )
    {
        if( !strncmp(buf, "softap", 6) )
        {
            info->mode = MODE_SOFTAP;
        }
        else if( !strncmp(buf, "sta", 3) )
        {
            info->mode = MODE_STATION;
        }

        if( strstr(buf, "wpa_state=COMPLETED") )
        {
            info->status = ASSOCIATED;
        }

        if( NULL != (ptr=strstr(buf, "\nssid=")) )
        {
            ptr = strchr(ptr, '=') + 1;
            end = strchr(ptr, '\n');
            if( end )
            {
                len = (size_t)(end-ptr);
                len = len>wificonfigMAX_SSID_LEN ? wificonfigMAX_SSID_LEN : len;
                strncpy(info->ssid, ptr, len);
            }
        }
    }

    /*+---------------------+
     *| Parser Network info |
     *+---------------------+*/
    if( ASSOCIATED == info->status )
    {
        err = rm_wifi_da16600_get_ipaddr(info->ipaddr, info->netmask, info->gateway);
        if( err )
            goto cleanup;

        if( !strstr(info->ipaddr, "0.0.0.0") )
        {
            info->status = CONNECTED;
        }
    }

cleanup:
    return FSP_SUCCESS;
}

fsp_err_t rm_wifi_da16600_set_workmode(da16600_workmode_t mode)
{
    fsp_err_t          err;
    char               buf[256]={'\0'};
    char               atcmd[256]={'\0'};

    wifi_print("Set WiFi module to %s mode now.\r\n", MODE_STATION==mode?"Station":"AP");

    err = rm_atcmd_check_value("AT+WFMODE=?", 1000, buf, sizeof(buf));
    if( err )
    {
        wifi_print("%s() AT command failed, err=%d\r\n", __func__, err);
        return err;
    }

    if( atoi(buf) == mode )
    {
        return FSP_SUCCESS;
    }

    snprintf(atcmd, sizeof(atcmd), "AT+WFMODE=%d", mode);
    return rm_atcmd_check_ok(atcmd, 500);
}

fsp_err_t parser_macaddr(char *buf, uint8_t *macaddr, int size)
{
    fsp_err_t          err;
    unsigned int       mac[6];

    err = sscanf(buf, "%2x:%2x:%2x:%2x:%2x:%2x\r",
                 &mac[0], &mac[1], &mac[2],
                 &mac[3], &mac[4], &mac[5]);

    if (6 != err)
        return FSP_ERR_WIFI_FAILED;

    for (int i=0; i < size; i++)
    {
        macaddr[i] = (uint8_t) mac[i];
    }

    return FSP_SUCCESS;
}

fsp_err_t rm_wifi_da16600_get_macaddr(uint8_t * macaddr)
{
    fsp_err_t          err;
    char               buf[256]={'\0'};

    err = rm_atcmd_check_value("AT+WFMAC=?", 1000, buf, sizeof(buf));
    if( err )
    {
        return err;
    }

    return parser_macaddr(buf, macaddr, 6);
}

fsp_err_t rm_wifi_da16600_scan(WIFIScanResult_t *results, uint32_t maxNetworks)
{
    fsp_err_t          err;
    char               buf[1024]={'\0'};
    char               line[256]={'\0'};
    char              *ptr, *end;
    uint32_t           i, j;
    int                rv;
    size_t             len;
    char               mac[20];
    char               freq[10];
    char               rssi[10];
    char               sec[64];
    char               ssid[64];

    err = rm_atcmd_check_value("AT+WFSCAN", 10000, buf, sizeof(buf));
    if( err )
    {
        return err;
    }

    memset(results, 0, maxNetworks*sizeof(*results));
    ptr = buf;

    /*+---------------+
     *| parser lines  |
     *+---------------+*/
    for(i=0; i<maxNetworks; i++)
    {
        /* every result line end by '\n' */
        if( !(end = strchr(ptr, '\n')) )
            break;

        memset(line, 0, sizeof(line));
        strncpy(line, ptr, (size_t)(end-ptr));

        /* 80:8f:1d:8a:f6:d0   2462    -50 [WPA-PSK-CCMP][WPA2-PSK-CCMP][ESS]  MAX8DEV */
        rv = sscanf(line, "%s %s %s %s %s", mac, freq, rssi, sec, ssid);
        if( 5 != rv )
        {
            goto NEXT_LOOP;
        }

        /* parser BSSID(MAC) */
        parser_macaddr(line, results[i].ucBSSID, wificonfigMAX_BSSID_LEN);

        /* parser SSID */
        len = strlen(ssid)>wificonfigMAX_SSID_LEN ? wificonfigMAX_SSID_LEN : strlen(ssid);
        results[i].ucSSIDLength = (uint8_t)len;
        for(j=0; j<len; j++)
        {
            results[i].ucSSID[j] = (uint8_t)ssid[j];
        }

        /* parser RSSI */
        results[i].cRSSI = (int8_t) atoi(rssi);

        rv = atoi(freq);
        for(j=0; j<13; j++)
        {
            if(rv == (int)(2412+j*5))
            {
                results[i].ucChannel = (uint8_t)i+1;
                break;
            }
        }

        if ( strstr(sec, "WPA2"))
            results[i].xSecurity = eWiFiSecurityWPA2 ;
        else if ( strstr(sec, "WPA") )
            results[i].xSecurity = eWiFiSecurityWPA;
        else
            results[i].xSecurity = eWiFiSecurityNotSupported;

#if 0
        printf("Scan results[%lu]: SSID:%s Channel[%d] RSSI[%d] Security[%d]\r\n",
                i, results[i].ucSSID, results[i].ucChannel, results[i].cRSSI, results[i].xSecurity);
#endif

NEXT_LOOP:
        ptr = end+1;
    }

    return FSP_SUCCESS;
}

fsp_err_t rm_wifi_da16600_connect(da16600_netinfo_t *netinfo, const char *ssid, WIFISecurity_t security, const char *passphrase)
{
    fsp_err_t          err;
    char               atcmd[256]={'\0'};

    if( !netinfo || !ssid || !passphrase )
        return FSP_ERR_INVALID_ARGUMENT;

    if( DISCONNECT == netinfo->status )
    {
        wifi_print("Start connect to Router \"%s\"...\r\n", ssid);

        snprintf(atcmd, sizeof(atcmd), "AT+WFJAP=%s,%d,%d,%s", ssid, security, TKIP_AES, passphrase);
        err = rm_atcmd_check_ok(atcmd, 3000);
        if( FSP_SUCCESS!= err )
        {
            return FSP_ERR_WIFI_AP_NOT_CONNECTED;
        }

        netinfo->status = CONNECTING;
    }

    rm_wifi_da16600_get_netinfo(netinfo);
    if( CONNECTED == netinfo->status)
    {
        return FSP_SUCCESS;
    }

    return FSP_ERR_WIFI_AP_NOT_CONNECTED;
}

fsp_err_t rm_wifi_da16600_disconnect(void)
{
    wifi_print("Disconnect DA16600 WiFi now.\r\n");
    return rm_atcmd_check_ok("AT+WFQAP", 3000);
}

/* DA16600 got disconnect by router */
void rm_wifi_da16600_check_disconnect(da16600_netinfo_t *netinfo)
{
    EventBits_t          evbits;

    evbits = xEventGroupWaitBits(g_atcmd_ctx.xEventGroup, EVENT_BIT_ASYNC, pdTRUE, pdFALSE, pdMS_TO_TICKS(10));
    if( evbits & EVENT_BIT_ASYNC )
    {
        if( check_event_flag(EVENT_SYS_DEAUTH) )
        {
            printf("DA16600 WiFi disconnect\r\n");
            clear_event_flag(EVENT_SYS_DEAUTH);
            memset(netinfo, 0, sizeof(*netinfo));
        }
    }
}

fsp_err_t rm_wifi_da16600_enable_dhcp(int enable)
{
    char               atcmd[256]={'\0'};

    wifi_print("%s DHCP Client\r\n", enable?"Enable":"Disable");
    snprintf(atcmd, sizeof(atcmd), "AT+NWDHC=%d", enable? 1 : 0);
    return rm_atcmd_check_ok(atcmd, 3000);
}

fsp_err_t rm_wifi_da16600_set_ipaddr(char *ipaddr, char *netmask, char *gateway)
{
    char               atcmd[256]={'\0'};

    snprintf(atcmd, sizeof(atcmd), "AT+NWIP=0,%s,%s,%s", ipaddr, netmask, gateway);
    return rm_atcmd_check_ok(atcmd, 3000);
}

fsp_err_t rm_wifi_da16600_get_ipaddr(char *ipaddr, char *netmask, char *gateway)
{
    fsp_err_t          err;
    char               buf[256]={'\0'};
    int                i;
    char              *ptr, *end;

    err = rm_atcmd_check_value("AT+NWIP=?", 3000, buf, sizeof(buf));
    if( err )
    {
        return err;
    }

    /* Buf: 0,192.168.2.158,255.255.255.0,192.168.2.1 */

    if( !(ptr=strchr(buf, ',')) )
        return FSP_ERR_NOT_FOUND;

    /* Parser IP address */
    ptr++;
    if( !(end=strchr(ptr, ',')) )
        return FSP_ERR_NOT_FOUND;

    if( ipaddr )
    {
        for(i=0; i<IPADDR_LEN-1; i++)
        {
            if(*ptr == ',' || *ptr == '\0')
                break;

            ipaddr[i] = *ptr++;
        }
        ipaddr[i] = '\0';
    }

    /* Parser Netmask */
    ptr = end+1;
    if( !(end=strchr(ptr, ',')) )
        return FSP_ERR_NOT_FOUND;

    if( netmask )
    {
        for(i=0; i<IPADDR_LEN-1; i++)
        {
            if(*ptr == ',' || *ptr == '\0')
                break;

            netmask[i] = *ptr++;
        }
        netmask[i] = '\0';
    }

    /* Parser Gateway */
    ptr = end+1;

    if( gateway )
    {
        for(i=0; i<IPADDR_LEN-1; i++)
        {
            if(*ptr == ',' || *ptr == '\0')
                break;

            gateway[i] = *ptr++;
        }
        gateway[i] = '\0';
    }

    return FSP_SUCCESS;
}


fsp_err_t rm_wifi_da16600_set_dns(char *dns1, char *dns2)
{
    fsp_err_t          err;
    char               atcmd[256]={'\0'};

    if( dns1 )
    {
        snprintf(atcmd, sizeof(atcmd), "AT+NWDNS=%s", dns1);
        err = rm_atcmd_check_ok(atcmd, 3000);
        if( FSP_SUCCESS!= err )
        {
            return err;
        }
    }

    if( dns2 )
    {
        snprintf(atcmd, sizeof(atcmd), "AT+NWDNS2=%s", dns2);
        err = rm_atcmd_check_ok(atcmd, 3000);
        if( FSP_SUCCESS!= err )
        {
            return err;
        }
    }

    return FSP_SUCCESS;
}

fsp_err_t rm_wifi_da16600_dns_query(const char *domain, char *ipaddr)
{
    fsp_err_t          err;
    char               buf[256]={'\0'};
    char               atcmd[256]={'\0'};

    snprintf(atcmd, sizeof(atcmd), "AT+NWHOST=%s", domain);
    err = rm_atcmd_check_value(atcmd, 5000, buf, sizeof(buf));
    if( FSP_SUCCESS!= err )
    {
        return err;
    }


    if(strlen(buf) < IPADDR_LEN )
        strcpy(ipaddr, buf);

    return FSP_SUCCESS;
}

fsp_err_t rm_wifi_da16600_ping(char *ipaddr, uint32_t count, uint32_t interval_ms)
{
    fsp_err_t          err;
    char               buf[256]={'\0'};
    char               atcmd[256]={'\0'};
    int                rv, sent_cnt, recv_cnt = 0;


    (void) interval_ms;

    if(!ipaddr )
        return FSP_ERR_INVALID_ARGUMENT;

    snprintf(atcmd, sizeof(atcmd), "AT+NWPING=0,%s,%lu", ipaddr, count);
    err = rm_atcmd_check_value(atcmd, 10000, buf, sizeof(buf));
    if( FSP_SUCCESS!= err )
    {
        return FSP_ERR_WIFI_TRANSMIT_FAILED;
    }

    rv = sscanf(buf, "%d,%d", &sent_cnt, &recv_cnt);
    if(2==rv && recv_cnt > 0 )
        return FSP_SUCCESS;

    return FSP_ERR_WIFI_FAILED;
}

fsp_err_t rm_wifi_da16600_tcp_listen(int port)
{
    fsp_err_t          err;
    char               atcmd[256]={'\0'};
    int                lport;

    err=rm_wifi_da16600_socket_info(CID_TCP_SERVER, &lport, NULL, NULL);
    if( FSP_SUCCESS == err )
    {
        if( lport == port )
        {
            printf("Socket server already listen on [%d]\r\n", lport);
            return FSP_SUCCESS;
        }
        else
        {
            printf("Socket server already listen on [%d], stop it now\r\n", lport);
            rm_wifi_da16600_socket_disconnect(CID_TCP_SERVER);
        }
    }

    snprintf(atcmd, sizeof(atcmd), "AT+TRTS=%d", port);
    return rm_atcmd_check_ok(atcmd, 3000);
}

fsp_err_t rm_wifi_da16600_tcp_connect(char *host, int port)
{
    fsp_err_t          err;
    char               atcmd[256]={'\0'};
    char               rIP[IPADDR_LEN]={0};
    int                rport = 0;

    err=rm_wifi_da16600_socket_info(CID_TCP_CLIENT, NULL, rIP, &rport);
    if( FSP_SUCCESS == err )
    {
        if( rport==port && strstr(rIP, host))
        {
            printf("Socket server already connect to [%s:%d]\r\n", rIP, rport);
            return FSP_SUCCESS;
        }
        else
        {
            printf("Socket server already connect to [%s:%d], disconnect it.\r\n", rIP, rport);
            rm_wifi_da16600_socket_disconnect(CID_TCP_SERVER);
        }
    }

    snprintf(atcmd, sizeof(atcmd), "AT+TRTC=%s,%d", host, port);
    return rm_atcmd_check_ok(atcmd, 3000);
}

fsp_err_t rm_wifi_da16600_socket_disconnect(cid_type_t cid)
{
    char               atcmd[256]={'\0'};

    snprintf(atcmd, sizeof(atcmd), "AT+TRTRM=%d", cid);
    return rm_atcmd_check_ok(atcmd, 3000);
}


fsp_err_t rm_wifi_da16600_socket_info(cid_type_t cid, int *lport, char *rIP, int *rport)
{
    fsp_err_t          err;
    char               atcmd[256]={'\0'};
    char               buf[256]={'\0'};
    int                tmp_cid;
    char               tmp_ip[IPADDR_LEN];
    int                tmp_rport;
    int                tmp_lport;
    int                rv;

    snprintf(atcmd, sizeof(atcmd), "AT+TRPRT=%d", cid);
    err = rm_atcmd_check_value(atcmd, 3000, buf, sizeof(buf));
    if( err )
        return err;

    /* buf: 1,TCP,192.168.2.58,12345,50401 */
    rv = sscanf(buf, "%d,%*[^,],%[^,],%d,%d", &tmp_cid, tmp_ip, &tmp_rport, &tmp_lport);
    if( rv == 4 && tmp_cid==cid )
    {
        if(lport)
            *lport = tmp_lport;

        if(rport)
            *rport = tmp_rport;

        if(rIP)
            strcpy(rIP, tmp_ip);

        return FSP_SUCCESS;
    }

    return FSP_ERR_NOT_FOUND;
}


int32_t rm_wifi_da16600_socket_send(cid_type_t cid, char *ipaddr, int port, uint8_t *data, uint32_t bytes)
{
    fsp_err_t          err;
    char               header[256]={0};

    if( CID_TCP_SERVER==cid && (!ipaddr || port<=0) )
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    header[0]=0x1B; /*ESC*/
    if( CID_TCP_SERVER == cid )
    {
        snprintf(&header[1], sizeof(header)-1, "S%d%lu,%s,%d,r,", cid, bytes, ipaddr, port);
    }
    else if( CID_TCP_CLIENT == cid )
    {
        snprintf(&header[1], sizeof(header)-1, "S%d%lu,0,0,r,", cid, bytes);
    }

    err = rm_data_send((uint8_t *)header, strlen(header));
    if( FSP_SUCCESS!= err)
    {
        return err;
    }

    vTaskDelay(5);
    err = rm_data_send(data, bytes);
    if( FSP_SUCCESS!= err)
    {
        return err;
    }

    return (int32_t)bytes;
}

