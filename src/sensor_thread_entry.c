#include "sensor_thread.h"
#include <stdio.h>

/* TODO: Enable if you want to open I2C bus */
void g_comms_i2c_bus0_quick_setup(void);

/* Quick setup for g_comms_i2c_bus0. */
void g_comms_i2c_bus0_quick_setup(void)
{
    fsp_err_t err;
    i2c_master_instance_t * p_driver_instance = (i2c_master_instance_t *) g_comms_i2c_bus0_extended_cfg.p_driver_instance;

    /* Open I2C driver, this must be done before calling any COMMS API */
    err = p_driver_instance->p_api->open(p_driver_instance->p_ctrl, p_driver_instance->p_cfg);
    assert(FSP_SUCCESS == err);

#if BSP_CFG_RTOS
    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore)
    {
#if BSP_CFG_RTOS == 1 // AzureOS
        tx_semaphore_create(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle,
                            g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_name,
                            (ULONG) 0);
#elif BSP_CFG_RTOS == 2 // FreeRTOS
        *(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle)
            = xSemaphoreCreateCountingStatic((UBaseType_t) 1, (UBaseType_t) 0, g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_memory);
#endif
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex)
    {
#if BSP_CFG_RTOS == 1 // AzureOS
        tx_mutex_create(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle,
                        g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_name,
                        TX_INHERIT);
#elif BSP_CFG_RTOS == 2 // FreeRTOS
        *(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle)
            = xSemaphoreCreateRecursiveMutexStatic(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_memory);
#endif
    }
#endif
}


/* TODO: Enable if you want to open I2C Communications Device */
#define G_COMMS_I2C_DEVICE0_NON_BLOCKING (0)

#if G_COMMS_I2C_DEVICE0_NON_BLOCKING
volatile bool g_i2c_completed = false;
#endif

/* TODO: Enable if you want to use a callback */
#define G_COMMS_I2C_DEVICE0_CALLBACK_ENABLE (0)
#if G_COMMS_I2C_DEVICE0_CALLBACK_ENABLE
void rm_hs300x_callback(rm_comms_callback_args_t * p_args)
{
#if G_COMMS_I2C_DEVICE0_NON_BLOCKING
    if (RM_COMMS_EVENT_OPERATION_COMPLETE == p_args->event)
    {
        g_i2c_completed = true;
    }
#else
    FSP_PARAMETER_NOT_USED(p_args);
#endif
}
#endif

/* Quick setup for g_comms_i2c_device0.
 * - g_comms_i2c_bus0 must be setup before calling this function
 *     (See Developer Assistance -> g_comms_i2c_device0 -> g_comms_i2c_bus0 -> Quick Setup).
 */
void g_comms_i2c_device0_quick_setup(void);

/* Quick setup for g_comms_i2c_device0. */
void g_comms_i2c_device0_quick_setup(void)
{
    fsp_err_t err;

    /* Open I2C Communications device instance, this must be done before calling any COMMS_I2C API */
    err = g_comms_i2c_device0.p_api->open(g_comms_i2c_device0.p_ctrl, g_comms_i2c_device0.p_cfg);
    assert(FSP_SUCCESS == err);

#if G_COMMS_I2C_DEVICE0_NON_BLOCKING
    while (!g_i2c_completed)
    {
        ;
    }
#endif
}


/* Quick getting humidity and temperature values for g_hs300x_sensor0.
 * - g_hs300x_sensor0 must be setup before calling this function.
 */
void g_hs300x_sensor0_quick_getting_humidity_and_temperature(rm_hs300x_data_t * p_data);

/* Quick getting humidity and temperature for g_hs300x_sensor0. */
void g_hs300x_sensor0_quick_getting_humidity_and_temperature(rm_hs300x_data_t * p_data)
{
    fsp_err_t            err;
    rm_hs300x_raw_data_t hs300x_raw_data;
    bool is_valid_data = false;

    /* Start the measurement */
    err = g_hs300x_sensor0.p_api->measurementStart(g_hs300x_sensor0.p_ctrl);
    assert(FSP_SUCCESS == err);
#if G_HS300X_SENSOR0_NON_BLOCKING
    while (!g_hs300x_completed)
    {
        ;
    }
    g_hs300x_completed = false;
#endif

    do
    {
        /* Read ADC data from HS300X sensor */
        err = g_hs300x_sensor0.p_api->read(g_hs300x_sensor0.p_ctrl, &hs300x_raw_data);
        assert(FSP_SUCCESS == err);
#if G_HS300X_SENSOR0_NON_BLOCKING
        while (!g_hs300x_completed)
        {
            ;
        }
        g_hs300x_completed = false;
#endif

        /* Calculate humidity and temperature values from ADC data */
        err = g_hs300x_sensor0.p_api->dataCalculate(g_hs300x_sensor0.p_ctrl, &hs300x_raw_data, p_data);
        if (FSP_SUCCESS == err)
        {
            is_valid_data = true;
        }
        else if (FSP_ERR_SENSOR_INVALID_DATA == err)
        {
            is_valid_data = false;
        }
        else
        {
            assert(false);
        }
    }
    while (false == is_valid_data);
}


/* TODO: Enable if you want to open HS300X */
#define G_HS300X_SENSOR0_NON_BLOCKING (1)

#if G_HS300X_SENSOR0_NON_BLOCKING
volatile bool g_hs300x_completed = false;
#endif

#if RM_HS300X_CFG_PROGRAMMING_MODE
uint32_t g_hs300x_sensor_id;
#endif

/* TODO: Enable if you want to use a callback */
#define G_HS300X_SENSOR0_CALLBACK_ENABLE (1)
#if G_HS300X_SENSOR0_CALLBACK_ENABLE
void hs300x_callback(rm_hs300x_callback_args_t * p_args)
{
#if G_HS300X_SENSOR0_NON_BLOCKING
    if (RM_HS300X_EVENT_SUCCESS == p_args->event)
    {
        g_hs300x_completed = true;
    }
#else
    FSP_PARAMETER_NOT_USED(p_args);
#endif
}
#endif

/* Quick setup for g_hs300x_sensor0.
 * - g_comms_i2c_bus0 must be setup before calling this function
 *     (See Developer Assistance -> g_hs300x_sensor0 -> g_comms_i2c_device0 -> g_comms_i2c_bus0 -> Quick Setup).
 */
void g_hs300x_sensor0_quick_setup(void);

/* Quick setup for g_hs300x_sensor0. */
void g_hs300x_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Open HS300X sensor instance, this must be done before calling any HS300X API */
    err = g_hs300x_sensor0.p_api->open(g_hs300x_sensor0.p_ctrl, g_hs300x_sensor0.p_cfg);
    assert(FSP_SUCCESS == err);

#if RM_HS300X_CFG_PROGRAMMING_MODE
    /* Enter the programming mode. This must be called within 10ms after applying power. */
    err = g_hs300x_sensor0.p_api->programmingModeEnter(g_hs300x_sensor0.p_ctrl);
    assert(FSP_SUCCESS == err);

#if G_HS300X_SENSOR0_NON_BLOCKING
    while (!g_hs300x_completed)
    {
        ;
    }
    g_hs300x_completed = false;
#endif

    /* Delay 120us. Entering the programming mode takes 120us. */
    R_BSP_SoftwareDelay(120, BSP_DELAY_UNITS_MICROSECONDS);

    /* Get the sensor ID */
    err = g_hs300x_sensor0.p_api->sensorIdGet(g_hs300x_sensor0.p_ctrl, (uint32_t *)&g_hs300x_sensor_id);
    assert(FSP_SUCCESS == err);

    /* Change the humidity resolution to 8 bit */
    err = g_hs300x_sensor0.p_api->resolutionChange(g_hs300x_sensor0.p_ctrl, RM_HS300X_HUMIDITY_DATA, RM_HS300X_RESOLUTION_8BIT);
    assert(FSP_SUCCESS == err);

#if G_HS300X_SENSOR0_NON_BLOCKING
    while (!g_hs300x_completed)
    {
        ;
    }
    g_hs300x_completed = false;
#endif

    /* Delay 14ms. Failure to comply with these times may result in data corruption and introduce errors in sensor measurements. */
    R_BSP_SoftwareDelay(14, BSP_DELAY_UNITS_MILLISECONDS);

    /* Change the temperature resolution to 8 bit */
    err = g_hs300x_sensor0.p_api->resolutionChange(g_hs300x_sensor0.p_ctrl, RM_HS300X_TEMPERATURE_DATA, RM_HS300X_RESOLUTION_8BIT);
    assert(FSP_SUCCESS == err);

#if G_HS300X_SENSOR0_NON_BLOCKING
    while (!g_hs300x_completed)
    {
        ;
    }
    g_hs300x_completed = false;
#endif

    /* Delay 14ms. Failure to comply with these times may result in data corruption and introduce errors in sensor measurements. */
    R_BSP_SoftwareDelay(14, BSP_DELAY_UNITS_MILLISECONDS);

    /* Exit the programming mode */
    err = g_hs300x_sensor0.p_api->programmingModeExit(g_hs300x_sensor0.p_ctrl);
    assert(FSP_SUCCESS == err);

#if G_HS300X_SENSOR0_NON_BLOCKING
    while (!g_hs300x_completed)
    {
        ;
    }
    g_hs300x_completed = false;
#endif
#endif
}


/* Sensor Thread entry function */
void sensor_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    rm_hs300x_data_t ht_data;
    float g_temp = 0.0f;
    float g_hum = 0.0f;

    g_comms_i2c_bus0_quick_setup();
    g_comms_i2c_device0_quick_setup();
    g_hs300x_sensor0_quick_setup();

    /* TODO: add your own code here */
    while (1)
    {

        g_hs300x_sensor0_quick_getting_humidity_and_temperature(&ht_data);
        g_temp = (float)ht_data.temperature.integer_part + (float)ht_data.temperature.decimal_part / 100.0f;
        g_hum = (float)ht_data.humidity.integer_part + (float)ht_data.humidity.decimal_part / 100.0f;

        // Output the sensor readings.  Note we do not include support for printing
        // floats by default.
        printf("Temp: %d.%dC, Hum: %d.%d%%\n", ht_data.temperature.integer_part,
                                                ht_data.temperature.decimal_part,
                                                ht_data.humidity.integer_part,
                                                ht_data.humidity.decimal_part);

        vTaskDelay (1000);
    }
}
