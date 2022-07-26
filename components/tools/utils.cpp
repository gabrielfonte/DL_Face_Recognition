/*
 * utils.c
 *
 *  Created on: 19 de jan. de 2022
 *      Author: gabriel
 */

#include "utils.h"
#include "esp_log.h"
#include "esp_camera.h"
#include "dl_image.hpp"
#include <iostream>

static const char *TAG = "ai_utils";

#define R_BITS 0b1111100000000000
#define G_BITS 0b0000011111100000
#define B_BITS 0b0000000000011111

#define FB_SIZE sizeof(camera_fb_t)

rgb __convertRgb565ToRgb888(uint16_t pixel){
	rgb rgb_pixel;

	rgb_pixel.r = (pixel & R_BITS) >> 8;
	rgb_pixel.g = (pixel & G_BITS) >> 3;
	rgb_pixel.b = (pixel & B_BITS) << 3;

	return rgb_pixel;
}

uint8_t cvtImgRGB565ToRGB888(uint8_t *img_input, uint8_t *img_output, uint32_t size){
	uint32_t j = 0;
	for(uint32_t i = 0; i < size; i+=2, j+=3){
		uint16_t img_pixel = (img_input[i] << 8) | (img_input[i+1] << 0);
		rgb rgb_pixel = __convertRgb565ToRgb888(img_pixel);
		img_output[j] = rgb_pixel.r;
		img_output[j+1] = rgb_pixel.g;
		img_output[j+2] = rgb_pixel.b;
	}
	return 0;
}

uint8_t cvtImgRGB565ToRGB888_16(uint16_t *img_input, uint8_t *img_output, uint32_t size){
	printf("Started Conversion of RGB565 format (uint16_t[%u]) to RGB888 format (uint8_t[%u])\n", size, size*3);
	uint32_t j = 0;
	for(uint32_t i = 0; i < size; i++, j+=3){
		uint16_t img_pixel = img_input[i];
		rgb rgb_pixel = __convertRgb565ToRgb888(img_pixel);
		img_output[j] = rgb_pixel.r;
		img_output[j+1] = rgb_pixel.g;
		img_output[j+2] = rgb_pixel.b;
	}
	return 0;
}

uint8_t cvtImgRGB565ToBGR888(uint8_t *img_input, uint8_t *img_output, uint32_t size){
	uint32_t j = 0;
	for(uint32_t i = 0; i < size; i+=2, j+=3){
		uint16_t img_pixel = (img_input[i] << 8) | (img_input[i+1] << 0);
		rgb rgb_pixel = __convertRgb565ToRgb888(img_pixel);
		img_output[j] = rgb_pixel.b;
		img_output[j+1] = rgb_pixel.g;
		img_output[j+2] = rgb_pixel.r;
	}
	return 0;
}

uint8_t cvtfbRGB565TofbRGB888(camera_fb_t *fb_input, camera_fb_t *fb_output){
	printf("Started Conversion of RGB565 format (camera_fb_t*) to RGB888 format (camera_fb_t*)\n");
	assert(fb_input->len == (fb_input->width*fb_input->height*2));
	uint32_t j = 0;

	fb_output->len = 0;
	fb_output->height = fb_input->height;
	fb_output->width = fb_input->width;
	fb_output->timestamp = fb_input->timestamp;
	fb_output->format = PIXFORMAT_RGB888;

	for(uint32_t i = 0; i < fb_input->len; i+=2, j+=3){
		uint16_t img_pixel = (fb_input->buf[i] << 8) | (fb_input->buf[i+1] << 0);
		rgb rgb_pixel = __convertRgb565ToRgb888(img_pixel);
		fb_output->buf[j] = rgb_pixel.r;
		fb_output->buf[j+1] = rgb_pixel.g;
		fb_output->buf[j+2] = rgb_pixel.b;
		fb_output->len += 3;
	}
	return 0;
}

uint8_t cvtfbRGB565TofbBGR888(camera_fb_t *fb_input, camera_fb_t *fb_output){
	printf("Started Conversion of RGB565 format (camera_fb_t*) [%d] to BGR888 format (camera_fb_t*) [%d] \n", fb_input->len, fb_input->width*fb_input->height*3);
	assert(fb_input->len == (fb_input->width*fb_input->height*2));

	fb_output = (camera_fb_t*) heap_caps_malloc(FB_SIZE + (fb_input->width*fb_input->height*3), MALLOC_CAP_8BIT); //Don't forget to free

	uint32_t j = 0;

	fb_output->len = 0;
	fb_output->height = fb_input->height;
	fb_output->width = fb_input->width;
	fb_output->timestamp = fb_input->timestamp;
	fb_output->format = PIXFORMAT_RGB888;

	uint8_t img[fb_input->width*fb_input->height*3];

	printf("Started Image Conversion\n");

	for(uint32_t i = 0; i < fb_input->len; i+=2, j+=3){
		uint16_t img_pixel = (fb_input->buf[i] << 8) | (fb_input->buf[i+1] << 0);
		rgb rgb_pixel = __convertRgb565ToRgb888(img_pixel);
		img[j] = rgb_pixel.b;
		img[j+1] = rgb_pixel.g;
		img[j+2] = rgb_pixel.r;
	}

	fb_output->buf = img;
	fb_output->len = j;

	printf("End of Conversion\n");
	return 0;
}

static const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";


static inline bool is_base64(BYTE c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(BYTE const* buf, unsigned int bufLen) {
  std::string ret;
  int i = 0;
  int j = 0;
  BYTE char_array_3[3];
  BYTE char_array_4[4];

  while (bufLen--) {
    char_array_3[i++] = *(buf++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';
  }

  return ret;
}

std::vector<BYTE> base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  BYTE char_array_4[4], char_array_3[3];
  std::vector<BYTE> ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
          ret.push_back(char_array_3[i]);
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
  }

  return ret;
}
