/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "app_camera_esp.h"

static const char *TAG = "app_camera";
static QueueHandle_t xQueueFrameO = NULL;
static SemaphoreHandle_t xframeSemaphore;

static void task_process_handler(void *arg)
{
    while (true)
    {
	    camera_fb_t *frame = esp_camera_fb_get();
	    if (frame){
		xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
	    }
    }
}

void register_camera(const pixformat_t pixel_fromat,
                     const framesize_t frame_size,
                     const uint8_t fb_count,
                     const QueueHandle_t frame_o,
					 const SemaphoreHandle_t frameSemaphore)
{
#if CONFIG_CAMERA_MODEL_ESP_EYE
  /* IO13, IO14 is designed for JTAG by default,
   * to use it as generalized input,
   * firstly declair it as pullup input */
  gpio_config_t conf;
  conf.mode = GPIO_MODE_INPUT;
  conf.pull_up_en = GPIO_PULLUP_ENABLE;
  conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  conf.intr_type = GPIO_INTR_DISABLE;
  conf.pin_bit_mask = 1LL << 13;
  gpio_config(&conf);
  conf.pin_bit_mask = 1LL << 14;
  gpio_config(&conf);
#endif
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = XCLK_FREQ;
  config.pixel_format = pixel_fromat;
  config.frame_size = frame_size;
  config.jpeg_quality = 5;
  config.fb_count = fb_count;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.grab_mode = CAMERA_GRAB_LATEST;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
  }

  sensor_t *s = esp_camera_sensor_get();
  s->set_vflip(s, 1); //flip it back
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID)
  {
      s->set_brightness(s, 1);  //up the blightness just a bit
      s->set_saturation(s, -2); //lower the saturation
  }

  xQueueFrameO = frame_o;
  xframeSemaphore = frameSemaphore;
  xTaskCreatePinnedToCore(task_process_handler, TAG, 1 * 1024, NULL, 5, NULL, 1);
}
