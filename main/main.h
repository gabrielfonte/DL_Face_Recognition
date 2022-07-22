/*
 * main.h
 *
 *  Created on: 1 de fev. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

#include <stdio.h>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"
#include "camera/app_camera_esp.h"
#include "websocket/websocket.h"
#include "wifi/wi-fi.h"
#include "esp_log.h"
#include "dl_image.hpp"
#include "fb_gfx.h"
#include "application/application.h"


/* ESP-DL Face Recognition Project */

TaskHandle_t xCameraTaskHandle;
QueueHandle_t xQueueRecognition;
QueueHandle_t xQueueHTTP;
QueueHandle_t xQueueRegister;
QueueHandle_t xQueueEvaluate;
QueueHandle_t xQueueWebsocket;
QueueHandle_t xWebsocketResponseQueue;
SemaphoreHandle_t xFrameSemaphore;

#endif
