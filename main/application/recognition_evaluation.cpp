#include "application.h"

using namespace std;

void evaluationTask(void *pvParameters){

	camera_fb_t *frame = NULL;
	int height = 0, width = 0, channels = 0;
	websocket_response_t response;
	face_register_t new_face;

	while(1){
		if(xQueueReceive(xQueueEvaluate, &new_face, portMAX_DELAY)){

			cout << "Received by xQueueEvaluate" << endl;
			vTaskSuspend(xCameraTaskHandle);

			width = new_face.img.shape[0]; height = new_face.img.shape[1], channels = new_face.img.shape[2];
			printf("Received image from queue with size {%d, %d, %d}, size %d and address: %p \n", width, height, channels, new_face.img.size, new_face.img.data);

			uint8_t *decoded_image = (uint8_t *) malloc(width*height*channels);

			if(fmt2rgb888((uint8_t*) new_face.img.data, new_face.img.size, PIXFORMAT_JPEG, decoded_image)){
				ESP_LOGI("CONVERSION", "END OF JPEG->RGB CONVERSION");
			}
			else{
				ESP_LOGE("CONVERSION", "CONVERSION FAILED");
			}

			std::list<dl::detect::result_t> detect_results;
			uint32_t time_ms = 0;

			if(Inference(decoded_image, height, width, &detect_results)){
				if(new_face.command == CMD_EVALUATE_REG_FACE){
					response.command = new_face.command;
					response.name = new_face.name;
					if(EnrollIDtoFlash(decoded_image, height, width, detect_results, new_face.name) > 0){
						response.detect_result = detect_results;
						response.status = 200;
					}
					else{
						response.status = 507;
					}
				}
				else{
					response.detect_result = detect_results;
					response.command = new_face.command;
					response.recognition_result = Recognize(decoded_image, height, width, detect_results, &time_ms);
					if(response.recognition_result.id > 0){
						response.status = 200;

					}
					else{
						response.status = 404;
					}
					response.name = new_face.name;
					response.time_ms = (int) time_ms;
				}
				xQueueSend(xQueueWebsocket, &response, (TickType_t) 0);
				draw_detection_result(decoded_image, height, width, detect_results);
				free(decoded_image);
			}
			else{
				response.command = new_face.command;
				response.name = new_face.name;
				response.status = 404;
				xQueueSend(xQueueWebsocket, &response, (TickType_t) 0);
			}
			free(new_face.img.data);
			new_face.img.data = NULL;
		}
	}
}