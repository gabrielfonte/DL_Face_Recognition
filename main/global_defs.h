#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"
#include "detection.h"
#include "recognizer.h"

extern TaskHandle_t xCameraTaskHandle;
extern QueueHandle_t xQueueRecognition;
extern QueueHandle_t xQueueHTTP;
extern QueueHandle_t xQueueRegister;
extern QueueHandle_t xQueueEvaluate;
extern QueueHandle_t xQueueWebsocket;
extern QueueHandle_t xWebsocketResponseQueue;
extern SemaphoreHandle_t xFrameSemaphore;