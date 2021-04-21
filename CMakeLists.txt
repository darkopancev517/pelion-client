INCLUDE(CMakeForceCompiler)
# CROSS COMPILER SETTING
cmake_minimum_required (VERSION 3.5)
SET(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(mbedCloudClientExample)

add_definitions(-DNS_EXCLUDE_HIGHRES_TIMER)
add_definitions(-DNS_EVENTLOOP_USE_TICK_TIMER)
add_definitions(-DMBED_CONF_MBED_CLIENT_EVENT_LOOP_SIZE=8192)

# mbed-cloud-client-example
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/platform)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/include)

FILE(GLOB MBED_CLOUD_CLIENT_EXAMPLE_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"

if(PARSEC_TPM_SE_SUPPORT)
    "${CMAKE_CURRENT_SOURCE_DIR}/source/platform/secure_element/mcc_se_init.c"
endif()

    )

if(PARSEC_TPM_SE_SUPPORT)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/platform/secure_element/se_configs)
endif()

if(ENABLE_DEVICE_SENTRY)
# MCCE device sentry example enabled, so add appropriate include files directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-cloud-client/device-sentry-client/source/include)
endif()

# Contain developer credential information
set(MBED_CLIENT_COMMON_CONFIG_SOURCE
  "${CMAKE_CURRENT_SOURCE_DIR}/update_default_resources.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/mbed_cloud_dev_credentials.c"
)

add_subdirectory(source/platform ${CMAKE_CURRENT_BINARY_DIR}/source/platform)
add_subdirectory(mbed-cloud-client ${CMAKE_CURRENT_BINARY_DIR}/mbed-cloud-client)

add_library(mbedClientSharedObjects STATIC ${MBED_CLIENT_COMMON_CONFIG_SOURCE})

# Needed to link sources if there is cross-dependencies between application<->library.
set (SRC_LIBS ${SRC_LIBS} PARENT_SCOPE)

CREATE_TEST_LIBRARY(mbedCloudClientExample "${MBED_CLOUD_CLIENT_EXAMPLE_SRC}" "")

target_link_libraries(mbedCloudClientExample mbedCloudClient platformCommon mbedClientSharedObjects)

if (MBED_CLOUD_CLIENT_SDK MATCHES Vertexcom_WISUN_VC7300)

    if (NOT CONFIG_BUILD_WITH_VCRTOS)
        message(FATAL_ERROR "CONFIG_BUILD_WITH_VCRTOS must be enabled!")
    endif()

    if (NOT CONFIG_MCU_vc7300a_X_PSRAM)
        message(FATAL_ERROR "CONFIG_MCU_vc7300a_X_PSRAM must be enabled!")
    endif()

    include(${CMAKE_CURRENT_SOURCE_DIR}/WISUN-flags.cmake)

    set(VERTEXCOM_WISUN_BOOTLOADER_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/source/platform/Vertexcom_WISUN_VC7300/wisun/dummy/bootloader.c)

    FILE(GLOB VERTEXCOM_WISUN_FIRMWARE_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/source/platform/Vertexcom_WISUN_VC7300/wisun/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/platform/Vertexcom_WISUN_VC7300/wisun/*.cpp"
    )

    set(VERTEXCOM_WISUN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pal-platform/SDK/Vertexcom_WISUN_VC7300/wisun)
    set(VERTEXCOM_WISUN_BIN_DIR ${PROJECT_BINARY_DIR}/../${CMAKE_BUILD_TYPE})

    add_definitions(-DVCRTOS_PROJECT_CONFIG_FILE="vcrtos-project-config.h")
    add_definitions(-DVCRTOS_VERSION="1.2.2")

    CREATE_TEST_LIBRARY(vertexcom_wisun_bootloader "${VERTEXCOM_WISUN_BOOTLOADER_SRCS}" "")

    # wisun bootloader linker script preprocessing
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/vertexcom_wisun_bootloader.ld
        COMMAND ${CMAKE_C_COMPILER}
        -E -P ${VERTEXCOM_WISUN_DIR}/mcu/vc7300a/ARM/linker/vc7300a_bootloader.ld.S
        -include ${VERTEXCOM_WISUN_DIR}/include/autoconf.h
        -include ${VERTEXCOM_WISUN_DIR}/include/param.h
        -o ${PROJECT_BINARY_DIR}/vertexcom_wisun_bootloader.ld
        VERBATIM
    )
    add_custom_target(vertexcom_wisun_bootloader_linker ALL DEPENDS ${PROJECT_BINARY_DIR}/vertexcom_wisun_bootloader.ld)
    add_dependencies(vertexcom_wisun_bootloader PLATFORM_BOOTLOADER_LIBS vertexcom_wisun_bootloader_linker)
    target_link_libraries(vertexcom_wisun_bootloader PLATFORM_BOOTLOADER_LIBS)

    CREATE_TEST_LIBRARY(vertexcom_wisun_firmware "${VERTEXCOM_WISUN_FIRMWARE_SRCS}" "")
    set_target_properties(vertexcom_wisun_firmware PROPERTIES COMPILE_FLAGS ${WISUN_MAIN_CFLAGS})

    # wisun firmware linker script preprocessing
    add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/vertexcom_wisun_firmware.ld
        COMMAND ${CMAKE_C_COMPILER}
        -E -P ${VERTEXCOM_WISUN_DIR}/mcu/vc7300a/ARM/linker/vc7300a_main_vcrtos_psram.ld.S
        -include ${VERTEXCOM_WISUN_DIR}/include/autoconf.h
        -include ${VERTEXCOM_WISUN_DIR}/include/param.h
        -I${VERTEXCOM_WISUN_DIR}
        -o ${PROJECT_BINARY_DIR}/vertexcom_wisun_firmware.ld
        VERBATIM
    )
    add_custom_target(vertexcom_wisun_firmware_linker ALL DEPENDS ${PROJECT_BINARY_DIR}/vertexcom_wisun_firmware.ld)
    add_dependencies(vertexcom_wisun_firmware mbedCloudClient platformCommon mbedClientSharedObjects vertexcom_wisun_firmware_linker)
    target_link_libraries(vertexcom_wisun_firmware PLATFORM_LIBS)

    add_dependencies(mbedCloudClientExample vertexcom_wisun_firmware_linker)

    if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
        set(ELZMA_PATH ${VERTEXCOM_WISUN_DIR}/tools/easylzma/Darwin)
    else()
        set(ELZMA_PATH ${VERTEXCOM_WISUN_DIR}/tools/easylzma/Linux/x86_64)
    endif()

    # create compressed version of vertexcom_wisun_firmware.bin
    add_custom_command(OUTPUT ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.bin.lzma
        COMMAND ${ELZMA_PATH}/elzma -k -s 65536 -v -f ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.bin && mv ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.bin.lzma ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.tmp && touch ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.magic && printf "vtxc" >> ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.magic && cat ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.magic ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.tmp > ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.bin.lzma && rm ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.magic ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.tmp
        COMMENT "create compressed version of vertexcom_wisun_firmware.bin"
        VERBATIM
    )
    add_custom_target(vertexcom_wisun_firmware_compressed ALL DEPENDS
        ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.bin.lzma
    )

    # padding vertexcom_wisun_bootloader.bin to 16K
    add_custom_command(OUTPUT ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader_16K.bin
        COMMAND ${VERTEXCOM_WISUN_DIR}/tools/scripts/padding_bootloader.sh ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader.bin ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader_padding.bin && cat ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader.bin ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader_padding.bin > ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader_16K.bin && rm ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader_padding.bin
        COMMENT "padding vertexcom_wisun_bootloader.bin to 16KB"
        VERBATIM
    )
    add_custom_target(vertexcom_wisun_bootloader_16K ALL DEPENDS
        ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader_16K.bin
    )

    # combine vertexcom_wisun_bootloader_16K.bin with vertexcom_wisun_firmware.bin

    if (CONFIG_WISUN_ROOT)
        set(WISUN_DEVICE_ROLE ROOT)
    else()
        set(WISUN_DEVICE_ROLE NODE)
    endif()

    add_custom_command(OUTPUT ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_${WISUN_DEVICE_ROLE}.bin
        COMMAND cat ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader_16K.bin ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.bin > ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_${WISUN_DEVICE_ROLE}.bin
        COMMENT "combine bootloader and main image as vertexcom_wisun_${WISUN_DEVICE_ROLE}.bin"
        VERBATIM
    )
    add_custom_target(vertexcom_wisun_${WISUN_DEVICE_ROLE} ALL DEPENDS
        ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader_16K.bin
        ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_${WISUN_DEVICE_ROLE}.bin
    )

    add_dependencies(vertexcom_wisun_${WISUN_DEVICE_ROLE} vertexcom_wisun_bootloader_16K vertexcom_wisun_firmware_compressed)

    # WISUN firmware upgrade image --------------------------------------------

    # build WISUN ota header tool
    add_custom_command(OUTPUT ${VERTEXCOM_WISUN_DIR}/tools/upgrade/crc16_ccitt
        COMMAND gcc ${VERTEXCOM_WISUN_DIR}/tools/upgrade/crc16_ccitt.c -o ${VERTEXCOM_WISUN_DIR}/tools/upgrade/crc16_ccitt
        COMMENT "build WISUN ota header tool"
        VERBATIM
    )
    add_custom_target(vertexcom_ota_header_tool ALL DEPENDS
        ${VERTEXCOM_WISUN_DIR}/tools/upgrade/crc16_ccitt
    )

    # build WISUN ota config file
    add_custom_command(OUTPUT ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config
        COMMAND touch ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config && echo "ver=1" >> ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config && echo "spread=1" >> ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config && echo "auto_reboot=${CONFIG_OTA_AUTO_REBOOT}" >> ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config && echo "mcu=${CONFIG_OTA_MCU_ID}" >> ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config && echo "board=${CONFIG_OTA_BOARD_ID}" >> ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config && echo "role=${CONFIG_OTA_RPL_ROLE}" >> ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config && echo "auto_reboot_delay=${CONFIG_OTA_AUTO_REBOOT_DELAY}" >> ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config && echo "coap_send_interval=${CONFIG_OTA_COAP_SEND_INTERVAL}" >> ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config
        COMMENT "build WISUN ota config file"
        VERBATIM
    )
    add_custom_target(vertexcom_wisun_ota_config ALL DEPENDS
        ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config
    )

    # build WISUN ota image
    add_custom_command(OUTPUT ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_${WISUN_DEVICE_ROLE}.bin
        COMMAND ${VERTEXCOM_WISUN_DIR}/tools/upgrade/gen_ota_image.sh ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_config ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_firmware.bin.lzma && mv ${PROJECT_BINARY_DIR}/image_for_upgrade.bin ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_${WISUN_DEVICE_ROLE}.bin
        COMMENT "build WISUN ota image"
        VERBATIM
    )
    add_custom_target(vertexcom_wisun_ota_image ALL DEPENDS
        ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_${WISUN_DEVICE_ROLE}.bin
    )

    add_dependencies(vertexcom_wisun_ota_image vertexcom_wisun_ota_config vertexcom_ota_header_tool vertexcom_wisun_${WISUN_DEVICE_ROLE})

    # build WISUN PSRAM image
    add_custom_command(OUTPUT ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_PSRAM_${WISUN_DEVICE_ROLE}.bin
        COMMAND cat ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_bootloader_16K.bin ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_ota_${WISUN_DEVICE_ROLE}.bin > ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_PSRAM_${WISUN_DEVICE_ROLE}.bin
        COMMENT "build WISUN PSRAM image"
        VERBATIM
    )
    add_custom_target(vertexcom_wisun_PSRAM_image ALL DEPENDS
        ${VERTEXCOM_WISUN_BIN_DIR}/vertexcom_wisun_PSRAM_${WISUN_DEVICE_ROLE}.bin
    )

    add_dependencies(vertexcom_wisun_PSRAM_image vertexcom_wisun_bootloader_16K vertexcom_wisun_ota_image)

    # -------------------------------------------------------------------------

endif() #if (MBED_CLOUD_CLIENT_SDK MATCHES Vertexcom_WISUN_VC7300)
