/*
 * iotc_thread_entry.h
 *
 *  Created on: Aug 11, 2023
 *      Author: Brian Willess
 */

#ifndef IOTC_THREAD_ENTRY_H_
#define IOTC_THREAD_ENTRY_H_

// Define the struct we use to receive telemetry from the ndp thread
struct telemetryQueueMsg {
    char* msgPtr;
    int msgSize;
};

typedef struct telemetryQueueMsg telemetryQueueMsg_t;

#endif /* IOTC_THREAD_ENTRY_H_ */
