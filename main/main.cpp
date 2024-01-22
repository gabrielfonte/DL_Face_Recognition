#include "main.h"

using namespace std;
static const char *TAG = "Main";
static websocket_client_config_t config;

static void queue_initialize(){
	xQueueWebsocket = xQueueCreate(1, sizeof(websocket_response_t));
	xWebsocketResponseQueue = xQueueCreate(1, sizeof(bool));
    	xQueueRecognition = xQueueCreate(1, sizeof(camera_fb_t *));
	xQueueHTTP = xQueueCreate(1, sizeof(camera_fb_t *));
	xQueueRegister = xQueueCreate(1, sizeof(face_register_t));
	xQueueEvaluate = xQueueCreate(1, sizeof(face_register_t));
    	xFrameSemaphore = xSemaphoreCreateBinary();
    	xSemaphoreGive(xFrameSemaphore);
}

extern "C" void app_main(void)
{
	wifi_init();
    	app_mdns_main();
	queue_initialize();

	config.uri = "ws://node-server-tcc.herokuapp.com";
	config.transport = WEBSOCKET_TRANSPORT_OVER_TCP;
	config.send_timeout = portMAX_DELAY;

	websocket_init(&config);
	websocket_client_start();

    	recognizer_init();
	start_http_stream_server(xQueueHTTP, NULL, true);

	xTaskCreate(captureTask, "Capture", 4*1024, NULL, tskIDLE_PRIORITY, &xCameraTaskHandle);

	while(1){
		vTaskDelay(8000/portTICK_PERIOD_MS);
	}
}
