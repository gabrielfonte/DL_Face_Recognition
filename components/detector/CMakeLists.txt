#
# Detection CMakeLists
#

idf_component_register(
    SRCS
        "detection.cpp"
    PRIV_REQUIRES console esp32-camera esp-dl spi_flash nvs_flash
    INCLUDE_DIRS "include")
