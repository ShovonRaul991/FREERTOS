#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

// Define task priorities
#define SOIL_MOISTURE_TASK_PRIORITY   (tskIDLE_PRIORITY + 2)
#define WEATHER_TASK_PRIORITY         (tskIDLE_PRIORITY + 2)
#define LOGGING_TASK_PRIORITY         (tskIDLE_PRIORITY + 1)
#define LOW_POWER_TASK_PRIORITY       (tskIDLE_PRIORITY + 3)
#define IRRIGATION_TASK_PRIORITY      (tskIDLE_PRIORITY + 2)

// Define task stack sizes
#define TASK_STACK_SIZE               1024

// Define threshold values
#define SOIL_MOISTURE_THRESHOLD      30.0f // Example threshold for soil moisture
#define TEMPERATURE_THRESHOLD        25.0f // Example threshold for temperature
#define HUMIDITY_THRESHOLD           70.0f // Example threshold for humidity
#define PRECIPITATION_THRESHOLD      5.0f  //Example threshold for precipitation

// Function prototypes
void soilMoistureTask(void *pvParameters);
void weatherTask(void *pvParameters);
void loggingTask(void *pvParameters);
void lowPowerTask(void *pvParameters);
void irrigationTask(void *pvParameters);
void vApplicationIdleHook(void);

// Simulated data reading functions
float readSoilMoistureFromTextFile();
void readWeatherDataFromTextFile(float temperature, float humidity, float precipitation);

// Global variables for sensor data
float soilMoisture = 0.0f;
float temperature = 0.0f;
float humidity = 0.0f;
float precipitation = 0.0f;
uint32_t irrigationCount = 0;

// Task handles
TaskHandle_t xSoilMoistureTaskHandle = NULL;
TaskHandle_t xWeatherTaskHandle = NULL;
TaskHandle_t xLoggingTaskHandle = NULL;
TaskHandle_t xLowPowerTaskHandle = NULL;
TaskHandle_t xIrrigationTaskHandle = NULL;

int main(void) {
    // Create tasks
    xTaskCreate(soilMoistureTask, "Soil Moisture Task", TASK_STACK_SIZE, NULL, SOIL_MOISTURE_TASK_PRIORITY, &xSoilMoistureTaskHandle);
    xTaskCreate(weatherTask, "Weather Task", TASK_STACK_SIZE, NULL, WEATHER_TASK_PRIORITY, &xWeatherTaskHandle);
    xTaskCreate(loggingTask, "Logging Task", TASK_STACK_SIZE, NULL, LOGGING_TASK_PRIORITY, &xLoggingTaskHandle);
    xTaskCreate(lowPowerTask, "Low Power Task", TASK_STACK_SIZE, NULL, LOW_POWER_TASK_PRIORITY, &xLowPowerTaskHandle);
    xTaskCreate(irrigationTask, "Irrigation Task", TASK_STACK_SIZE, NULL, IRRIGATION_TASK_PRIORITY, &xIrrigationTaskHandle);

    // Start the scheduler
    vTaskStartScheduler();

    // Should not reach here
    return 0;
}

// Task to monitor soil moisture levels
void soilMoistureTask(void *pvParameters) {
    uint32_t pipe_no = 10; //for soil moisture control
    while (1) {
        printf("\nSoil moisture checking......\t");
        // Read soil moisture from text file
        soilMoisture = readSoilMoistureFromTextFile();
        printf("\t\tSoil moisture is %f\n",soilMoisture);

        // Check if soil moisture is below threshold
        if (soilMoisture < SOIL_MOISTURE_THRESHOLD) {
            // Call irrigation control task
            printf("\nIrrigation Required for soil moisture....\n");
            // xTaskNotifyGive(xIrrigationTaskHandle);
            xTaskNotify(xIrrigationTaskHandle,pipe_no,eSetBits);
            irrigationCount++; // Increment irrigation count

        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}

// Task to monitor weather conditions
void weatherTask(void *pvParameters) {
    uint32_t pipe_no = 15; //for weather condition i.e two different pipe numbers for two different task
    while (1) {
        // Read weather data from text file
        printf("\nWeather checking......\t");
        readWeatherDataFromTextFile(temperature, humidity, precipitation);

        // Check if temperature or humidity is above threshold
        if (temperature > TEMPERATURE_THRESHOLD || humidity < HUMIDITY_THRESHOLD || precipitation < PRECIPITATION_THRESHOLD) {
            // Call irrigation control task
            printf("\nIrrigation Required for temperature condition....\n");
            // xTaskNotifyGive(xIrrigationTaskHandle);
            
            xTaskNotify(xIrrigationTaskHandle,pipe_no,eSetBits);
            irrigationCount++; // Increment irrigation count
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Delay for 2 seconds
    }
}

// Task for logging sensor data
void loggingTask(void *pvParameters) {
    while (1) {
        printf("\nlogs are checking......\n");

        vTaskDelay(pdMS_TO_TICKS(5000)); // Delay for 5 seconds
    }
}

// Task for low-power management
void lowPowerTask(void *pvParameters) {
    while (1) {
        printf("\nlow power requirement is checking......\n");

        vTaskDelay(pdMS_TO_TICKS(10000)); // Delay for 10 seconds
    }
}

// Task for irrigation control
void irrigationTask(void *pvParameters) {
    uint32_t notificationValue;
    while (1) {
        
        if(xTaskNotifyWait(0xFFFFFFFF,ULONG_MAX,&notificationValue,portMAX_DELAY) == pdTRUE)
        {
            printf("\nIrrigation should be performed.. Pipe no. A%d should be started\n", notificationValue);
        }
    }
}

// Simulated function to read soil moisture from text file
float readSoilMoistureFromTextFile() {
    // Simulated reading from text file
    return (float)(rand() % 100); // Random value between 0 and 100
}

// Simulated function to read weather data from text file
void readWeatherDataFromTextFile(float temperature, float humidity, float precipitation) {
    // Simulated reading from text file
    temperature = (float)(rand() % 50); // Random value between 0 and 50
    humidity = (float)(rand() % 100); // Random value between 0 and 100
    precipitation = (float)(rand() % 20); // Random value between 0 and 20
    printf("\t\tTemperature is %f, \tHumidity is %f\t, precipitation is %f\n",temperature,humidity,precipitation);
}

// Idle hook function to perform power-saving measures during idle periods
void vApplicationIdleHook(void) {
   
    if (irrigationCount >= 5) {
        // Run only the low power task for 30 seconds
        printf('\n\t\t\t\t\t\tLow power state.....');
        vTaskDelay(pdMS_TO_TICKS(30000));
        irrigationCount = 0; // Reset irrigation count after 30 seconds
    }
}
