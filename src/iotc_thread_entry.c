#include "iotc_thread.h"
#include "rm_wifi_da16600.h"
#include "console.h"
#include <rm_atcmd.h>
#include "cJSON.h"
#include "time.h"
#include "stdlib.h"
#include "string.h"
#include "queue.h"
#include "certs.h"
#include "iotc_thread_entry.h"
#include "stdbool.h"

#define IoTC_ENV    "poc"
#define MAX_RETRIES 5
#define OUTPUT_MQTT_DEBUG

#define MY_CHAR_ARRAY_SIZE 64
#define IDENTITY_URL_SIZE 128
#define JSON_STRING_SIZE 2048

// Local globals
char *buf;
char identityURL[IDENTITY_URL_SIZE];
char hostnameString[MY_CHAR_ARRAY_SIZE] = {'\0'};
char pubTopicString[MY_CHAR_ARRAY_SIZE] = {'\0'};
char subTopicString[MY_CHAR_ARRAY_SIZE] = {'\0'};

// Define the different states that implement the IoTConnect connection
enum IOTC_STATE {
    INIT_DA16600 = 0,
    LOAD_CERTS,
    SETUP_NETWORK,
    DISCOVERY,
    GET_IDENTITY,
    SETUP_MQTT,
    FAILURE_STATE,
    WAIT_FOR_TELEMETRY_DATA
};

// Define the state machine state variable
enum IOTC_STATE currentState = INIT_DA16600;

// Define the state machine functions
void init_da16600(void);
void setup_network(void);
void load_certs(void);
void run_discovery(void);
void get_identity(void);
void setup_mqtt(void);
void wait_for_telemetry(void);
void failure_state(void);

// Helper functions
void extractJSON(char*);
bool reestablish_mqtt_conn(void);

#define DEBUG_IOTC_PRINT
#ifdef  DEBUG_IOTC_PRINT
#define iotc_print(format,args...) printf(format, ##args)
#else
#define iotc_print(format,args...) do{} while(0);
#endif


/* IoTC Thread entry function */
void iotc_thread_entry(void *pvParameters)
{

    FSP_PARAMETER_NOT_USED (pvParameters);

    // Put the DA16600 into reset
    R_BSP_PinWrite(DA16600_RstPin, BSP_IO_LEVEL_LOW);

    // If we don't need the DA16600 at all then leave the device in reset and kill this task.
    if(((CLOUD_NONE == get_target_cloud()) && (BLE_DISABLE == get_ble_mode()))){

        vTaskDelete(NULL);
    }

    /* Wait for console thread initialization to complete */
    xSemaphoreTake( g_xInitialSemaphore, portMAX_DELAY );

    // The application is using the FreeRTOS heap4 heap management implementation.
    // Redefine the cJSON malloc and free calls to use the correct calls
    // to use heap4 memory.
    cJSON_Hooks heap4Hooks;
    heap4Hooks.malloc_fn = &pvPortMalloc;
    heap4Hooks.free_fn = &vPortFree;

    // Tell cJSON to use the heap4 management calls
    cJSON_InitHooks(&heap4Hooks);

    // The buf buffer is used by the atcmd interface to return AT command
    // results.  We allocate the memory here and leave it allocated until the
    // thread exits.
    buf = (char*)pvPortMalloc(ATBUF_SIZE);
    if(NULL == buf){
        iotc_print("Could not allocate buf memory, exiting thread!\n");
        vTaskDelete(NULL);
    }

    printf("IoT Connect on AWS (MQTT) Thread Running...\r\n");

    R_BSP_PinWrite(DA16600_RstPin, BSP_IO_LEVEL_HIGH);

    // Set the initial state
    currentState = INIT_DA16600;

    // Loop forever . . .
    while (true){

        // Process the current state
        switch (currentState){
            case (INIT_DA16600):
                init_da16600();
                break;
            case (LOAD_CERTS):
                load_certs();
                break;
            case (SETUP_NETWORK):
                setup_network();
                break;
            case (DISCOVERY):
                run_discovery();
                break;
            case (GET_IDENTITY):
                get_identity();
                break;
            case (SETUP_MQTT):
                setup_mqtt();
                break;
            case (WAIT_FOR_TELEMETRY_DATA):
                wait_for_telemetry();
                break;
            case FAILURE_STATE:
                if (buf) {
                    vPortFree(buf);
                }
                failure_state();
                break;
            default:
                break;
        }
    }

    // Free the buf memory
    if (buf){
        vPortFree(buf);
    }
    vTaskDelete(NULL);
}

void init_da16600(void){

    char atcmd[64] = {'\0'};
    static int failCnt = 0;

    iotc_print("IoTConnect-state: INIT_DA16600\n");

    // Initialize the AT module UART and start the atcmd thread.
    rm_wifi_da16600_init();

    if(MAX_RETRIES == failCnt ){
        printf("ERROR: Could not initialize DA16600\n");
        currentState = FAILURE_STATE;
        return;
    }

    if( FSP_SUCCESS != rm_wifi_da16600_check_at(10)){
        printf("ERROR: DA16600 WiFi module not ready, trying again . . . \r\n");
        failCnt++;
        return;
    }

    if( FSP_SUCCESS != rm_wifi_da16600_check_at(10)){
        printf("ERROR: DA16600 WiFi module not ready, trying again . . . \r\n");
        failCnt++;
        return;
    }
    else
        iotc_print("INFO: DA16600 WiFi module ready!\r\n");

    // AT command initialize
    memset(buf, '\0', ATBUF_SIZE);
    rm_atcmd_send("ATZ",1000,buf,sizeof(buf));

    // Command Echo
    memset(buf, '\0', ATBUF_SIZE);
    rm_atcmd_send("ATE",1000,buf,sizeof(buf));

    // Check to see if we're configured to connect to one of the supported Cloud Solutions.
    // If not, then reset the DA16600 to factory defaults so any previous configurations are
    // removed from the device.
    if(CLOUD_NONE == get_target_cloud()){

        // Send the NVRAM clean command.  This resets the DA16600 to factory defaults.
        // Any MQTT configuration or certificates will be removed from the device.  This
        // command will also blow away our BLE advertisement setting, so we do this operation
        // before determining if we're going to set a custom advertisement name.
        memset(buf, '\0', ATBUF_SIZE);
        rm_atcmd_send("ATF", 5000,buf, sizeof(buf));

        // Delay to allow the DA16600 to reset
        vTaskDelay(1000);

    }

    // If BLE mode is enabled, then set the BLE advertisement name based on the config.ini file.
    // If the default name was not changed, then the DA16600 will generate a random name in the form
    // of DA16600-<wxyz>
    if(BLE_ENABLE == get_ble_mode()){

        if(0 != strncmp(BLE_DEFAULT_NAME, get_ble_name(), 32)){

            // Set the BLE advertisement name based on the config.ini entry
            memset(buf, '\0', ATBUF_SIZE);
            snprintf(atcmd, sizeof(atcmd), "AT+BLENAME=%s", get_ble_name());
            rm_atcmd_send(atcmd,5000,buf,sizeof(buf));
        }
    }
    else{
        // Disable all BLE advertising
        rm_atcmd_send("AT+ADVSTOP", 5000,buf, sizeof(buf));
    }

    // Check one more time if we have a cloud configuration.  If not, then
    // exit the thread.
    if(CLOUD_NONE == get_target_cloud()){
        printf("No cloud configuration found: Exiting cloud connectivity Thread\n");

        // Free the buf memory
        if (buf){
            vPortFree(buf);
        }

        vTaskDelete(NULL);
    }

    // Set the next state to load the certificates
    currentState = LOAD_CERTS;
}

fsp_err_t loadAWS_certificates(int certId)
{
    fsp_err_t err;

    const char* cert = NULL;
    char certData[2048] = {'\0'};

    switch(get_load_certificate_from()){
        case LOAD_CERTS_FROM_HEADER:

            // Find the correct certificate define based on the passed in cert ID
            switch(certId){
                case ROOT_CA:
                    cert = cert_aws_ca1;
                    break;
                case DEVICE_CERT:
                    cert = awsDevice_cert;
                    break;
                case DEVICE_PUBLIC_KEY:
                    cert = awsDevicePrivateKey_cert;
                    break;
            }

            // Make sure we found the certificate data
            if(NULL == cert){
                return -1;
            }

            err = rm_data_send((uint8_t*) "\x1b", (uint32_t) strlen("\x1b"));
            if ( FSP_SUCCESS != err )
                return err;

            // These delays are required to allow the certificate to be stored correctly
            vTaskDelay(500);

            err = rm_data_send((uint8_t*) cert, (uint32_t) strlen(cert));
            if ( FSP_SUCCESS != err )
                return err;

            vTaskDelay(500);

            err = rm_data_send((uint8_t*) "\x03",(uint32_t) strlen("\x03"));
            if ( FSP_SUCCESS != err )
                return err;

            vTaskDelay(500);

            break;

        case LOAD_CERTS_USE_DA16600_CERTS:
            
            // If we're using the certs previously loaded into the DA16600, then 
            // we're done.
                        
            break;

        case LOAD_CERTS_FROM_FILES:

            // Get the data from the specified file into the local array
            if(!get_certificate_data(get_certificate_file_name(certId), certId, certData)){

                printf("ERROR: Not able to read certificate file %s\n", get_certificate_file_name(certId));
                printf("Verify that the missing cert file in on the microSD card\n");
            }

            err = rm_data_send((uint8_t*) "\x1b", (uint32_t) strlen("\x1b"));
            if ( FSP_SUCCESS != err )
                return err;

            // These delays are required to allow the certificate to be stored correctly
            vTaskDelay(500);

            // Send the certificate data
            err = rm_data_send((uint8_t*)certData, strlen(certData));
            if ( FSP_SUCCESS != err )
                return err;

            vTaskDelay(500);

            err = rm_data_send((uint8_t*) "\x03",(uint32_t) strlen("\x03"));
            if ( FSP_SUCCESS != err )
                return err;

            vTaskDelay(500);
    }


    // iotc_print("Certificate Loaded %s\n",cert);

    return FSP_SUCCESS;
}

void load_certs(){

    static int failCnt = 0;

    iotc_print("IoTConnect-state: LOAD_CERTIFICATES\n");

    // If we're using the certificates on the DA16600, just move to the next state
    if(LOAD_CERTS_USE_DA16600_CERTS == get_load_certificate_from()){
        iotc_print("INFO: Assuming certificates are already loaded to the DA16600 memory!\n");
        currentState = SETUP_NETWORK;
        return;
    }

    // Check to see if we're stuck trying to delete the certificates on the DA16600
    if(MAX_RETRIES == failCnt){
        printf("ERROR: Not able to load AWS certificates to the DA16600\n");
        currentState = FAILURE_STATE;
        return;
    }

    // Delete all TLS certificates stored in the DA16600
    if(FSP_SUCCESS != rm_atcmd_check_ok("AT+NWDCRT",10000)){
        printf("ERROR: Not able to delete certificates from DA16600\n");
        failCnt++;
        return;
    }

    // Load the AWS Root CA certificate
    if(FSP_SUCCESS != loadAWS_certificates(ROOT_CA)){
        failCnt++;
        return;
    }

    // Load the Device Certificate
    if(FSP_SUCCESS != loadAWS_certificates(DEVICE_CERT)){
        failCnt++;
        return;
    }

    // Load the Device Private Key
    if(FSP_SUCCESS != loadAWS_certificates(DEVICE_PUBLIC_KEY)){
        failCnt++;
        return;
    }

    // Check the certificate status, should return 7
    memset(buf, '\0', ATBUF_SIZE);
    if(FSP_SUCCESS != rm_atcmd_check_value("AT+NWCCRT",5000,buf,sizeof(buf))){
        failCnt++;
        return;
    }

    // Verify that all three certificate bits have been set, if not
    // return to enter this state again
    if(buf[0] != '7'){
        printf("ERROR: AWS certificates not loaded, trying again . . . \n");
        failCnt++;
        return;
    }

    failCnt = 0;
    currentState = SETUP_NETWORK;
}

void setup_network(void){

    size_t nwipReturnStringLen = 0;
    static int failCnt = 0;
    static int level2FailCnt = 0;
    int timeCheck = 0;
    char atcmd[256]={'\0'};

    iotc_print("IoTConnect-state: SETUP_NETWORK\n");

    if(USE_CONFIG_WIFI_SETTINGS == get_wifi_config()){

        // Check to see if we're stuck trying to connect to the network
        if(MAX_RETRIES <= failCnt){

            // Check to see if we've tried to call rm_wifi_da16600_init already while trying to recover the
            // network.  If not, give it a try before giving up.
            if( 1 > level2FailCnt++){

                // Lets try one last thing before we give up on the network connection
                rm_wifi_da16600_init();
                currentState = INIT_DA16600;
                failCnt = 0;
                return;
            }

            printf("ERROR: Did not connect to the network, verify your network credentials\n");
            currentState = FAILURE_STATE;
            return;
        }

        // Set WiFi mode to Station mode
        memset(buf, '\0', ATBUF_SIZE);
        rm_atcmd_send("AT+WFMODE=0",1000,buf,sizeof(buf));

        // Read the wifi mode to verify it's set to station (0)
        memset(buf, '\0', ATBUF_SIZE);
        rm_atcmd_send("AT+WFMODE",1000,buf,sizeof(buf));

        // Set the two letter Wi-Fi country code: https://www.iso.org/obp/ui/#search
        memset(buf, '\0', ATBUF_SIZE);
        snprintf(atcmd, sizeof(atcmd), "AT+WFCC=%s", get_wifi_cc());
        rm_atcmd_send(atcmd,1000,buf,sizeof(buf));

        // Verify that the AP SSID is configured before trying to connect to the network
        if(strcmp(get_wifi_ap(), "WiFi AP Name Undefined") == 0){
            printf("ERROR: WiFi credentials are not defined.  Edit config.ini file to add WiFi credentials\n");
            currentState = FAILURE_STATE;
            return;
        }

        // Construct the "Connect to AP" command using
        // * SSID from configuration
        // * 4 == WPA+WPA2
        // * 1 == Key Index
        // * SSID password from configuration
        snprintf(atcmd, sizeof(atcmd), "AT+WFJAP=%s,%d,%d,%s", get_wifi_ap(), 4, 1, get_wifi_pw());
        rm_atcmd_check_ok(atcmd, 10000);

        // Start the DHCP Client
        memset(buf, '\0', ATBUF_SIZE);
        rm_atcmd_send("AT+NWDHC=1",5000,buf,sizeof(buf));

        // Get the WiFi configuration from the DA16600
        memset(buf, '\0', ATBUF_SIZE);
        if(FSP_SUCCESS != rm_atcmd_check_value("AT+WFSTAT",5000,buf,sizeof(buf))){
            failCnt++;
            return;
        }

        memset(buf, '\0', ATBUF_SIZE);
        if(FSP_SUCCESS != rm_atcmd_check_value("AT+NWIP",5000,buf,sizeof(buf))){
            // 0,192.168.0.143,255.255.255.0,192.168.0.1",
            failCnt++;
            return;
        }

        // Calculate the length of a return string where no address' are defined
        // We'll use this to determine if we have an IP address
        nwipReturnStringLen = strlen("0,0.0.0.0,0.0.0.0,0.0.0.0");

        // Check the size of the string returned, if we did not get an ip address and
        // gateway address, output a message and try again
        if(strlen(buf) <= nwipReturnStringLen ){

            printf("\nERROR: DA16600 did not connect to a Wi-Fi Access Point, verify your Access Point Credentials, trying again . . . \n");
            failCnt++;
            return;
        }

        // Set the time server FQDN based on the config.ini entry
        memset(atcmd, '\0', ATCMD_SIZE);
        snprintf(atcmd, sizeof(atcmd), "AT+NWSNTP=1,%s,60", get_ntp_time_server());

        // Start the SNTP client
        rm_atcmd_check_ok(atcmd,2000);

    } // End !USE_RENESAS_TOOL_FOR_CONFIG
    // Make sure we get a valid time from the time server before moving on . . .
    do {
        // Get the current GMT time from the time server
        memset(buf, '\0', ATBUF_SIZE);
        if(FSP_SUCCESS != rm_atcmd_check_value("AT+TIME=?",1000,buf,sizeof(buf))){
            failCnt++;
            return;
        }

        vTaskDelay(1000);

        if(USE_RENESAS_TOOL_FOR_CONFIG == get_wifi_config() && 10 < timeCheck++ ){

            printf("\n\nWaiting for a connection to the internet, verify your Wi-Fi network is setup\n");
            printf("using the Renesas Wi-Fi Provisioning Tool from your device's App store\n");
            timeCheck = 0;
        }

      // Loop until the time returned is not the default 1/1/1970
    } while (0 == strncmp(buf, "1970-01-01",strlen("1970-01-01")));

    // We got through each step without errors, clear the static error count
    // and set the next state;
    failCnt = 0;
    currentState = DISCOVERY;
}

void run_discovery(void)
{
    EventBits_t   evbits;

    static const char discoveryString[] = {"AT+NWHTCH=https://awsdiscovery.iotconnect.io/api/v2.1/dsdk/cpId/%s/env/%s,get"};
    char jsonString[JSON_STRING_SIZE]= {'\0'};

    iotc_print("IoTConnect-state: RUN_DISCOVERY\n");

    // Build the discovery command using the configured IoTConnect CPID and env strings
    memset(httpsBuffer,'\0',sizeof(httpsBuffer));
    snprintf(jsonString, JSON_STRING_SIZE, discoveryString,  get_iotc_cpid(), get_iotc_env());

    memset(buf, '\0', ATBUF_SIZE);
    if(FSP_SUCCESS != rm_atcmd_send(jsonString, 5000, buf, sizeof(buf))){
        return;
    }

    // Wait here for the https response to trigger the event bit, we
    // set a 10 second timeout in case we never get a response
    evbits = xEventGroupWaitBits(g_https_extended_msg_event_group, EVENT_BIT_EXTENDED_MSG, pdTRUE, pdFALSE , 10000);
    if(pdFALSE == (evbits & EVENT_BIT_EXTENDED_MSG)){

        // If we timed out but did not hit the max retries, just return.  The state machine will run this state again.
        iotc_print("WARNING: Timeout waiting for https response from IoTConnect, trying again . . . \n");
        return;
    }

    // Pull the JSON from httpsBuffer
    extractJSON(jsonString);

/*
    The return JSON should look similar to . . .
    {
      "d": {
        "ec": 0,
        "bu": "https://awspocdi.iotconnect.io/api/2.1/agent/device-identity/cg/55555550-2854-4a77-8f3b-ca1696401e08",
        "log:mqtt": {
          "hn": "",
          "un": "",
          "pwd": "",
          "topic": ""
        },
        "pf": "aws",
        "dip": 1
      },
      "status": 200,
      "message": "Success"
    }
*/

    // Pass the JSON to the parser, if the JSON in invalid, this call returns NULL
    cJSON *root = cJSON_Parse(jsonString);
    if (root == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                printf("ERROR: JSON parse error\n");
            }

            cJSON_Delete(root);
            return;
        }

    // Get pointers to the d and bu objects
    cJSON *data = cJSON_GetObjectItem(root, "d");
    cJSON *bu = cJSON_GetObjectItem(data, "bu");

    // Pull the base URL from the JSON
    char *baseURL = bu->valuestring;
    cJSON_Delete(root);

    //iotc_print("BASE URL: %s\n\r",baseURL);

    // Use the base URL and the device ID (UID) to construct the Identity URL
    sprintf(identityURL, "%s%s%s", baseURL,"/uid/", get_iotc_uid());

    //iotc_print("IDENTITY URL: %s\n\r",identityURL);

    // We got through each step without errors set the next state;
    currentState = GET_IDENTITY;
    return;
}

void get_identity(void)
{

#define FINAL_IDENTITY_SIZE 256
    char finalIdentityURL[FINAL_IDENTITY_SIZE] = {'\0'};
    char jsonString[JSON_STRING_SIZE] = {'\0'};
    EventBits_t evbits;

    iotc_print("IoTConnect-state: GET_IDENTITY\n");

    // Catch the case where we entered this state but we don't
    // have a valid identityURL string.  Fall back to the Discovery
    // state to correct the issue.
    if(0 == strlen(identityURL)){
        currentState = DISCOVERY;
        return;
    }

    // Update the identify URL to include the get command
    memset(httpsBuffer,0,sizeof(httpsBuffer));
    sprintf(finalIdentityURL, "%s%s%s", "AT+NWHTCH=",identityURL,",get");

    // Send the updated identityURL message up to IoTConnect
    memset(buf, '\0', ATBUF_SIZE);
    if (FSP_SUCCESS != rm_atcmd_send(finalIdentityURL, 5000,buf, sizeof(buf)))
    {

        iotc_print("ERROR: Failed to pull Identity from IoTConnect, trying again . . .\n");
        return;
    }


    // Wait here for the https response to trigger the event bit, we 
    // set a 5 second timeout in case we never get a response
    evbits = xEventGroupWaitBits(g_https_extended_msg_event_group, EVENT_BIT_EXTENDED_MSG, pdTRUE, pdFALSE , 5000);
    if(pdFALSE == (evbits & EVENT_BIT_EXTENDED_MSG)){

        iotc_print("WARNING: Timeout waiting for identity response from IoTConnect, retrying . . .\n");
        return;
//        currentState = SETUP_NETWORK;
    }

    // Pull the JSON from httpsBuffer
    extractJSON(jsonString);
/*
    JSON should be similar to . . .
    {
       "d": {
           "ec": 0,
           "ct": 200,
           "meta": {
               "at": 3,
               "df": 15,
               "cd": "XG83K89",
               "gtw": null,
               "edge": 0,
               "pf": 0,
               "hwv": "",
               "swv": "",
               "v": 2.1
           },
           "has": {
               "d": 0,
               "attr": 1,
               "set": 0,
               "r": 0,
               "ota": 0
           },
           "p": {
               "n": "mqtt",
               "h": "a3ace4e144syja-ats.iot.us-east-1.amazonaws.com",
               "p": 8883,
               "id": "RASynBoardBW",
               "un": "a3ace4419usyja-ats.iot.us-east-1.amazonaws.com/RASynBoardBW",
               "topics": {
                   "rpt": "$aws/rules/msg_d2c_rpt/RASynBoardBW/XG83K89/2.1/0",
                   "flt": "$aws/rules/msg_d2c_flt/RASynBoardBW/XG83K89/2.1/3",
                   "od": "$aws/rules/msg_d2c_od/RASynBoardBW/XG83K89/2.1/4",
                   "hb": "$aws/rules/msg_d2c_hb/RASynBoardBW/XG83K89/2.1/5",
                   "ack": "$aws/rules/msg_d2c_ack/RASynBoardBW/XG83K89/2.1/6",
                   "dl": "$aws/rules/msg_d2c_dl/RASynBoardBW/XG83K89/2.1/7",
                   "di": "$aws/rules/msg_d2c_di/RASynBoardBW/XG83K89/2.1/1",
                   "c2d": "iot/RASynBoardBW/cmd"
               }
           },
           "dt": "2023-07-25T17:13:55.764Z"
       },
       "status": 200,
       "message": "Device info loaded successfully."
    }
*/
    // Verify we have a valid JSON document, if not the call returns NULL
    cJSON *root = cJSON_Parse(jsonString);
    if (root == NULL) {
            cJSON_Delete(root);
            return;
        }

    // Pull all the pieces we need to send telemetry data to IoTConnect from the JSON document
    cJSON *data = cJSON_GetObjectItem(root, "d");
    cJSON *p = cJSON_GetObjectItem(data, "p");
    cJSON *h = cJSON_GetObjectItem(p, "h");
    cJSON *topics = cJSON_GetObjectItem(p, "topics");
    cJSON *rpt = cJSON_GetObjectItem(topics, "rpt");
    cJSON *c2d = cJSON_GetObjectItem(topics, "c2d");

    // Update the global strings with values pulled from the JSON. We'll use these each time we send telemetry.
    strncpy(hostnameString, h->valuestring, MY_CHAR_ARRAY_SIZE-1);
    strncpy(pubTopicString, rpt->valuestring, MY_CHAR_ARRAY_SIZE-1);
    strncpy(subTopicString, c2d->valuestring, MY_CHAR_ARRAY_SIZE-1);

    iotc_print("  Host: %s\n",hostnameString);
    iotc_print("  PUBT: %s\n",pubTopicString);
    iotc_print("  SUBT: %s\n",subTopicString);

    // Free the memory consumed by cJSON
    cJSON_Delete(root);

    currentState = SETUP_MQTT;
    return;

}

void setup_mqtt(void)
{

    char atCmdBuffer[256] = {'\0'};
#define MAX_TIMEOUTS 10
    int timeoutCnt = 0;

    iotc_print("IoTConnect-state: SETUP_MQTT\n");

    // Disable the MQTT client while we setup the connection details
    memset(buf, '\0', ATBUF_SIZE);
    if(FSP_SUCCESS != rm_atcmd_send("AT+NWMQCL=0", 1000,buf, sizeof(buf))){
        return;
    }

    // Set the MQTT Client ID
    memset(buf, '\0', ATBUF_SIZE);
    memset(atCmdBuffer,0,sizeof(atCmdBuffer));
    snprintf(atCmdBuffer, sizeof(atCmdBuffer), "AT+NWMQCID=%s",get_iotc_uid());
    if(FSP_SUCCESS != rm_atcmd_send(atCmdBuffer, 1000,buf, sizeof(buf))){
        return;
    }

    // Set the MQTT hostname and port
    memset(buf, '\0', ATBUF_SIZE);
    memset(atCmdBuffer,0,sizeof(atCmdBuffer));
    snprintf(atCmdBuffer, sizeof(atCmdBuffer), "AT+NWMQBR=%s,8883",hostnameString);

    if(FSP_SUCCESS != rm_atcmd_send(atCmdBuffer, 1000,buf, sizeof(buf))){
        return;
    }

    // Set the MQTT subscriber topic
    memset(buf, '\0', ATBUF_SIZE);
    memset(atCmdBuffer,0,sizeof(atCmdBuffer));
    snprintf(atCmdBuffer, sizeof(atCmdBuffer), "AT+NWMQTS=1,%s",subTopicString);

    if(FSP_SUCCESS != rm_atcmd_send(atCmdBuffer, 1000,buf, sizeof(buf))){
        return;
    }

    // Set the MQTT publisher topic
    memset(buf, '\0', ATBUF_SIZE);
    memset(atCmdBuffer,0,sizeof(atCmdBuffer));
    snprintf(atCmdBuffer, sizeof(atCmdBuffer), "AT+NWMQTP=%s",pubTopicString);

    if(FSP_SUCCESS != rm_atcmd_send(atCmdBuffer, 1000,buf, sizeof(buf))){
        return;
    }

    // Enable the MQTT over TLS function
    memset(buf, '\0', ATBUF_SIZE);
    if(FSP_SUCCESS != rm_atcmd_send("AT+NWMQTLS=1", 1000,buf, sizeof(buf))){
        return;
    }

    // All the MQTT configuration items have been sent, enable the MQTT client!
    memset(buf, '\0', ATBUF_SIZE);
    if(FSP_SUCCESS != rm_atcmd_send("AT+NWMQCL=1", 1000,buf, sizeof(buf))){
        return;
    }

    // Wait here until the MQTT connection is active
    while(true){
        memset(buf, '\0', ATBUF_SIZE);
        rm_atcmd_check_value("AT+NWMQCL",5000,buf,sizeof(buf));
        if(buf[0] == '1'){
            break;
        }

        // If we have to wait too long for the MQTT connect, exit to try again.
        if(MAX_TIMEOUTS == ++timeoutCnt){
            return;
        }

        vTaskDelay(4000);
    }

    iotc_print("INFO: MQTT Connection established to IoTConnect on AWS!\n");
    currentState = WAIT_FOR_TELEMETRY_DATA;
    return;
}

void printHeapSize(const char* ref)
{
    HeapStats_t myHeap;
    vPortGetHeapStats(&myHeap);
    printf("%s: %d\n",ref, myHeap.xAvailableHeapSpaceInBytes);
}

void buildAWSTelemetry(char* newTelemetry, char* awsTelemetry)
{

#define TIME_BUF_SIZE 32
    char timeBuf[TIME_BUF_SIZE]={'\0'};
    int position = 0;
    fsp_err_t err;

    // Verify we have a valid JSON document, if not the call returns NULL
    cJSON *userTelemetry = cJSON_Parse(newTelemetry);
    if (userTelemetry == NULL) {
                iotc_print("ERROR: Not able to parse passed in JSON: %s\n", newTelemetry);
                cJSON_Delete(userTelemetry);
                return;
    }
    iotc_print("%s\n", newTelemetry);

    // Get the current time to include in the telemetry message
    do
    {
        memset(buf, '\0', ATBUF_SIZE);
        err = rm_atcmd_check_value("AT+TIME=?",1000,buf,sizeof(buf));
        //iotc_print("TIMEIS: %s\n",buf);

    } while ( err != FSP_SUCCESS );

    for ( int i=0; i <= (int)strlen(buf); i++)
        if (buf[1] == ':')
        {
         position = i;
         break;
        }

    for ( int j = position ; j <= (int)strlen(buf);j++)
        timeBuf[j]=buf[j];

    //iotc_print("TIMEBUF: %s\n", timeBuf);

    for  ( int i=0; i <= (int)strlen(timeBuf);i++)
        if (timeBuf[i] == ',')
            timeBuf[i] = 'T';

    strcat(timeBuf, ".000Z");

    cJSON *root;
    cJSON *fmt;
    cJSON *thm;

    root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "dt", timeBuf);
    cJSON_AddItemToObject(root, "d", fmt = cJSON_CreateArray());
    cJSON_AddItemToArray(fmt, thm = cJSON_CreateObject());
    cJSON_AddStringToObject(thm, "dt", timeBuf);
    cJSON_AddItemToObject(thm, "d", userTelemetry);
    char *JSONString =  cJSON_PrintUnformatted(root);
    strcpy(awsTelemetry, JSONString);
    vPortFree(JSONString);
    cJSON_Delete(root);
}

void wait_for_telemetry(void){

    telemetryQueueMsg_t newMsg;

    // Declare the message buffer that will contain the AT command + the
    // full JSON message
#define MQTT_JSON_SIZE 256
#define MQTT_MSG_SIZE 512
    char mqttPublishMessage[MQTT_MSG_SIZE] = {'\0'};
    char mqttJson[MQTT_JSON_SIZE] = {'\0'};

    iotc_print("Waiting for Telemery data!\n");

    while(pdTRUE){

        xQueueReceive(g_telemetry_queue, &newMsg, portMAX_DELAY);
        // printf("RX Telemetry from Queue: %d bytes--> %s\n", newMsg.msgSize, newMsg.msgPtr);

        // Verify we have a valid MQTT connection before sending telemetry
        memset(buf, '\0', ATBUF_SIZE);
        rm_atcmd_check_value("AT+NWMQCL",5000,buf,sizeof(buf));
        if(buf[0] != '1'){
            if(!reestablish_mqtt_conn()){
                return;
            }
        }

        // Construct the IoTConnect telemetry
        memset(mqttPublishMessage,'\0' ,MQTT_MSG_SIZE);
        memset(mqttJson, '\0', MQTT_JSON_SIZE);
        buildAWSTelemetry(newMsg.msgPtr, mqttJson);
        snprintf(mqttPublishMessage, sizeof(mqttPublishMessage), "AT+NWMQMSG='%s'",mqttJson);

        // Send the message
        memset(buf, '\0', ATBUF_SIZE);
        if(FSP_SUCCESS != rm_atcmd_send(mqttPublishMessage, 2000,buf, sizeof(buf))){
            currentState = DISCOVERY;
            return;
        }

        // Delay for a short period in case we found the queue with many messages
        vTaskDelay(100);

        // Free the memory that held the incoming JSON.
        vPortFree((void*) newMsg.msgPtr);
    }
}

// We call this function if we had previously established the TLS MQTT connection, then we lost it.
// Maybe we lost the wifi connection, maybe we put the device into a low power state.  Anyway, the DA16600 has
// all the details it needs to reestablish the network and mqtt connection, we just need to let it do its thing.
bool reestablish_mqtt_conn(void){

    iotc_print("Reestablishing MQTT connection\n");
    int timeoutCnt = 0;

    // AT command initialize
    memset(buf, '\0', ATBUF_SIZE);
    rm_atcmd_send("ATZ",1000,buf,sizeof(buf));

    // Command Echo
    memset(buf, '\0', ATBUF_SIZE);
    rm_atcmd_send("ATE",1000,buf,sizeof(buf));

    // spin here until we have a valid MQTT connection
    do{

        memset(buf, '\0', ATBUF_SIZE);
        rm_atcmd_check_value("AT+NWMQCL",2000,buf,sizeof(buf));

//        iotc_print("Check for MQTT connection\n");
        vTaskDelay(1000);

        // If we checked for an mqtt connection 25 times, then kick
        // the mqtt connection again.
        if(25 < timeoutCnt++){

//            iotc_print("Giving up, change to the SETUP_MQTT state to start the connection atain\n");
            currentState = SETUP_MQTT;
            return false;
        }

    }
    while(buf[0] != '1');

    iotc_print("MQTT connection ready!\n");
    currentState = WAIT_FOR_TELEMETRY_DATA;
    return true;
}

void failure_state(void){

    iotc_print("IoTConnect: Enter FAILURE_STATE\n");
    printf("ERROR: Fatal error encountered, stopping thread!\n");

    // We tried all the tricks we know to get the IoTConnect connection up.  If we're in
    // this state, then all attempts failed.  Kill this thread!
    vTaskDelete(NULL);
    return;
}

void extractJSON(char* tempString){

    int firstCurly = 0;
    int lastCurly = 0;
    int i;

    // Pull the return JSON message from the httpsBuffer that contains the IoTConnect response.

    // Find the fist '{' in the string to find the beginning of the JSON message
    for ( i = 0; i <= (int)(strlen(httpsBuffer)); i++)
        if (httpsBuffer[i] == '{')
        {
            firstCurly = i;
            break;
        }

    // Find the last '}' to identify the end of the JSON message
    for ( i = 0; i <= (int)strlen(httpsBuffer); i++)
        if (httpsBuffer[i] == '}')
        {
            lastCurly = i;
        }

    // Use the indexes of the beginning and end of the JSON, copy the JSON message into the tempString char array
    for (i = firstCurly; i <= lastCurly; i++)
        {
        tempString[i-firstCurly] = (char)httpsBuffer[i];
        }

    // Null terminate the string
    tempString[i-firstCurly] = '\0';
}
