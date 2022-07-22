#include "application.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"

using namespace std;

void captureTask(void *pvParameters){

	bool recognize = true;
	camera_fb_t *frame = NULL;
	int height = 0, width = 0;
	uint8_t *decoded_image = NULL;
	websocket_response_t response = {0};
	face_register_t new_face;
	bool register_face = false;

	register_camera(PIXFORMAT_YUV422, FRAMESIZE_CIF, 2, xQueueRecognition, xFrameSemaphore);

	while(1){
		bool received_frame = false;
		if(xQueueReceive(xQueueRecognition, &frame, portTICK_PERIOD_MS)){

			height = (int) frame->height, width = (int) frame->width;
			decoded_image = (uint8_t *) app_camera_decode(frame);

			if(frame->format != PIXFORMAT_RGB565){
				frame->format = PIXFORMAT_RGB888;
				frame->len = height*width*3;
				free(frame->buf);	
				frame->buf = (uint8_t*) decoded_image;
			}

			received_frame = true;
		}

		if(xQueueReceive(xQueueRegister, &new_face, portTICK_PERIOD_MS)){
			cout << "New register required for " << new_face.name << endl;
			if(new_face.command == CMD_REGISTER_FACE){
				register_face = true;
			}
		}

		if(received_frame){
		
			std::list<dl::detect::result_t> detect_results;
			uint32_t time_ms = 0;

			if(Inference(decoded_image, height, width, &detect_results)){
				if(recognize){
					if(register_face){
						response.command = new_face.command;
						response.name = new_face.name;
						if(EnrollIDtoFlash(decoded_image, height, width, detect_results, new_face.name) > 0){
							response.detect_result = detect_results;
							response.status = 200;
						}
						else{
							response.status = 507;
						}
						register_face = false;
					}
					else{
						response.detect_result = detect_results;
						response.recognition_result = Recognize(decoded_image, height, width, detect_results, &time_ms);
						if(response.recognition_result.id > 0){
							response.status = 200;
							fb_gfx_printf(frame, detect_results.front().box[0], detect_results.front().box[1]-30, 0xF800, "%s %.2f",
							response.recognition_result.name.c_str(), response.recognition_result.similarity); 
						}
						else{
							response.status = 404;
							fb_gfx_printf(frame, detect_results.front().box[0], detect_results.front().box[1]-30, 0x07E0, "%s %.2f", 
							response.recognition_result.name.c_str(), response.recognition_result.similarity);
						}
					}
					draw_detection_result(decoded_image, height, width, detect_results);
				}
				xQueueSend(xQueueWebsocket, &response, (TickType_t) 0);
			}

			if(is_http_streaming()){
				if(xQueueSend(xQueueHTTP, &frame, (TickType_t) 10)){
				}
				else{
					esp_camera_fb_return(frame);
				}
			}
			else{
				esp_camera_fb_return(frame);
			}

			decoded_image = NULL;
		}
	}
}