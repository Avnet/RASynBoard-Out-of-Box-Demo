#include "telemetryTiming.h"

// This thread is responsible for timing between the application sending an inference
// event i.e., Rolling and sending the idle telemetry message that will replace the 
// event gif i.e., Rolling with the idle graphic.

bool supressNdp120Events = false;

/* Telemetry Timing entry function */
/* pvParameters contains TaskHandle_t */
void telemetryTiming_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    while (1)
    {

        // If this flag is set then we just send a telemetry event
        if(supressNdp120Events){
            
            // Delay for 4 seconds to allow the IoTConnect Gif to play
            vTaskDelay (pdMS_TO_TICKS(get_puck_idle_delay()));

            // Send the Idle telemetry to put the idle graphic up on IoTConnect
            enqueInferenceData(IDLE_NETWORK_NUM, IDLE_INFERENCE_INDEX);
            supressNdp120Events = false;
        }

        // Sleep to release the mcu
        vTaskDelay (pdMS_TO_TICKS(1000UL));
    }
}

void suppressNdp120Events(void){

    supressNdp120Events = true;

}
bool getSupressNdp120Event(void){

    return supressNdp120Events;
}
