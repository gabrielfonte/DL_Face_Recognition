#
# Camera functions CMakeLists
#

idf_component_register(
    SRCS "utils.cpp"
    PRIV_REQUIRES console esp-dl esp32-camera spi_flash fb_gfx
    INCLUDE_DIRS "include")
