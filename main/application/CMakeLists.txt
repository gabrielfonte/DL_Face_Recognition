#
# Main component of TF Micro project 'person_detection'.
#

idf_component_register(
    SRCS
        "face_recognition.cpp"
        "recognition_evaluation.cpp"
    PRIV_REQUIRES esp-dl camera spi_flash json websocket nvs_flash freertos esp_websocket_client esp_wifi fb_gfx esp_http_server mdns
    INCLUDE_DIRS "")