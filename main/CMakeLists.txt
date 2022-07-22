#
# Main component of TF Micro project 'person_detection'.
#

idf_component_register(
    SRCS
        "main.cpp"
        "application/face_recognition.cpp"
        "application/recognition_evaluation.cpp"
        "camera/app_camera_esp.c"
        "websocket/websocket.cc"
        "wifi/wi-fi.c"
        "http/app_mdns.c"
        "http/http_stream.cpp"
    PRIV_REQUIRES esp32-camera detector recognizer tools esp-dl spi_flash json nvs_flash freertos esp_websocket_client esp_wifi fb_gfx esp_http_server mdns 
    INCLUDE_DIRS "")