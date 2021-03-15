#################################################################################
#  Copyright 2016-2020 ARM Ltd.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#################################################################################

# CROSS COMPILER SETTINGS
cmake_minimum_required (VERSION 2.8)
SET(CMAKE_SYSTEM_NAME Generic)

project(vertexcomFreeRTOS)

set(PDMC_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
set(MBEDTLS_SOURCE_DIR ${FREERTOS_FOLDER}/libraries/3rdparty/mbedtls/library)
set(PORTING_SRC_FOLDER ${PDMC_FOLDER}/source/platform/Vertexcom_AP_VC7300)
set(VCDRIVER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vcdrivers/source)

ADD_GLOBALDIR(${CMAKE_CURRENT_SOURCE_DIR})
ADD_GLOBALDIR(${FREERTOS_FOLDER}/libraries/3rdparty/mbedtls/include/)
ADD_GLOBALDIR(${FREERTOS_FOLDER}/libraries/3rdparty/mbedtls/include/mbedtls)
ADD_GLOBALDIR(${FREERTOS_FOLDER}/libraries/abstractions/pkcs11/mbedtls)
ADD_GLOBALDIR(${PDMC_FOLDER}/mbed-cloud-client/mbed-client-pal/Source/Port/Reference-Impl/OS_Specific/Vertexcom_AP_VC7300/Board_Specific/TARGET_VC7300)
ADD_GLOBALDIR(${CMAKE_CURRENT_SOURCE_DIR}/vcdrivers/include)
ADD_GLOBALDIR(${CMAKE_CURRENT_SOURCE_DIR}/vcdrivers/include/cmsis)
ADD_GLOBALDIR(${CMAKE_CURRENT_SOURCE_DIR}/vcdrivers/source/sirius)

set(VCDRIVER_SOURCES
    ${VCDRIVER_SOURCE_DIR}/assert_failure.c
    ${VCDRIVER_SOURCE_DIR}/cortexm.c
    ${VCDRIVER_SOURCE_DIR}/stdiobase.c
    ${VCDRIVER_SOURCE_DIR}/syscalls.c

    ${VCDRIVER_SOURCE_DIR}/sirius/vectors.c
    ${VCDRIVER_SOURCE_DIR}/sirius/cpu.c

    ${VCDRIVER_SOURCE_DIR}/sirius/periph/gpio.c
    ${VCDRIVER_SOURCE_DIR}/sirius/periph/irq.c
    ${VCDRIVER_SOURCE_DIR}/sirius/periph/tim.c
    ${VCDRIVER_SOURCE_DIR}/sirius/periph/uart.c
)

set(MBEDCRYPTO_SOURCES
    ${MBEDTLS_SOURCE_DIR}/aes.c
    ${MBEDTLS_SOURCE_DIR}/aesni.c
    ${MBEDTLS_SOURCE_DIR}/arc4.c
    ${MBEDTLS_SOURCE_DIR}/asn1parse.c
    ${MBEDTLS_SOURCE_DIR}/asn1write.c
    ${MBEDTLS_SOURCE_DIR}/base64.c
    ${MBEDTLS_SOURCE_DIR}/bignum.c
    ${MBEDTLS_SOURCE_DIR}/blowfish.c
    ${MBEDTLS_SOURCE_DIR}/camellia.c
    ${MBEDTLS_SOURCE_DIR}/ccm.c
    ${MBEDTLS_SOURCE_DIR}/cipher.c
    ${MBEDTLS_SOURCE_DIR}/cipher_wrap.c
    ${MBEDTLS_SOURCE_DIR}/cmac.c
    ${MBEDTLS_SOURCE_DIR}/ctr_drbg.c
    ${MBEDTLS_SOURCE_DIR}/des.c
    ${MBEDTLS_SOURCE_DIR}/dhm.c
    ${MBEDTLS_SOURCE_DIR}/ecdh.c
    ${MBEDTLS_SOURCE_DIR}/ecdsa.c
    ${MBEDTLS_SOURCE_DIR}/ecjpake.c
    ${MBEDTLS_SOURCE_DIR}/ecp.c
    ${MBEDTLS_SOURCE_DIR}/ecp_curves.c
    ${MBEDTLS_SOURCE_DIR}/entropy.c
    ${MBEDTLS_SOURCE_DIR}/entropy_poll.c
    ${MBEDTLS_SOURCE_DIR}/error.c
    ${MBEDTLS_SOURCE_DIR}/gcm.c
    ${MBEDTLS_SOURCE_DIR}/havege.c
    ${MBEDTLS_SOURCE_DIR}/hmac_drbg.c
    ${MBEDTLS_SOURCE_DIR}/md.c
    ${MBEDTLS_SOURCE_DIR}/md2.c
    ${MBEDTLS_SOURCE_DIR}/md4.c
    ${MBEDTLS_SOURCE_DIR}/md5.c
    ${MBEDTLS_SOURCE_DIR}/md_wrap.c
    ${MBEDTLS_SOURCE_DIR}/memory_buffer_alloc.c
    ${MBEDTLS_SOURCE_DIR}/oid.c
    ${MBEDTLS_SOURCE_DIR}/padlock.c
    ${MBEDTLS_SOURCE_DIR}/pem.c
    ${MBEDTLS_SOURCE_DIR}/pk.c
    ${MBEDTLS_SOURCE_DIR}/pk_wrap.c
    ${MBEDTLS_SOURCE_DIR}/pkcs12.c
    ${MBEDTLS_SOURCE_DIR}/pkcs5.c
    ${MBEDTLS_SOURCE_DIR}/pkparse.c
    ${MBEDTLS_SOURCE_DIR}/pkwrite.c
    ${MBEDTLS_SOURCE_DIR}/platform.c
    ${MBEDTLS_SOURCE_DIR}/platform_util.c
    ${MBEDTLS_SOURCE_DIR}/ripemd160.c
    ${MBEDTLS_SOURCE_DIR}/rsa.c
    ${MBEDTLS_SOURCE_DIR}/rsa_internal.c
    ${MBEDTLS_SOURCE_DIR}/sha1.c
    ${MBEDTLS_SOURCE_DIR}/sha256.c
    ${MBEDTLS_SOURCE_DIR}/sha512.c
    ${MBEDTLS_SOURCE_DIR}/threading.c
    ${MBEDTLS_SOURCE_DIR}/timing.c
    ${MBEDTLS_SOURCE_DIR}/version.c
    ${MBEDTLS_SOURCE_DIR}/version_features.c
    ${MBEDTLS_SOURCE_DIR}/xtea.c
)

set(MBEDTLS_SOURCES
    ${MBEDTLS_SOURCE_DIR}/debug.c
    ${MBEDTLS_SOURCE_DIR}/net_sockets.c
    ${MBEDTLS_SOURCE_DIR}/ssl_cache.c
    ${MBEDTLS_SOURCE_DIR}/ssl_ciphersuites.c
    ${MBEDTLS_SOURCE_DIR}/ssl_cli.c
    ${MBEDTLS_SOURCE_DIR}/ssl_cookie.c
    ${MBEDTLS_SOURCE_DIR}/ssl_srv.c
    ${MBEDTLS_SOURCE_DIR}/ssl_ticket.c
    ${MBEDTLS_SOURCE_DIR}/ssl_tls.c
)

# Some files are commented out due to not available in the current AWS FreeRTOS repo.
set(MBEDX509_SOURCES
    ${MBEDTLS_SOURCE_DIR}/certs.c
    ${MBEDTLS_SOURCE_DIR}/x509.c
    ${MBEDTLS_SOURCE_DIR}/x509_create.c
    ${MBEDTLS_SOURCE_DIR}/x509_crl.c
    ${MBEDTLS_SOURCE_DIR}/x509_crt.c
    ${MBEDTLS_SOURCE_DIR}/x509_csr.c
    ${MBEDTLS_SOURCE_DIR}/x509write_crt.c
    ${MBEDTLS_SOURCE_DIR}/x509write_csr.c
)

set(FLASH_DRIVER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/FlashIAP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flash_api.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port_storage.c
)

set(PLATFORM_LIBS_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/dummy.c
)

add_library(mbedcrypto STATIC ${MBEDCRYPTO_SOURCES})
add_library(mbedtls STATIC ${MBEDTLS_SOURCES})
add_library(mbedx509 STATIC ${MBEDX509_SOURCES})
add_library(vcdriver STATIC ${VCDRIVER_SOURCES})
add_library(kvstore_flash_impl STATIC ${FLASH_DRIVER_SOURCES})
add_library(PLATFORM_LIBS STATIC ${PLATFORM_LIBS_SRCS})

# Make sure dependent libraries are built before this target.
SET(PLATFORM_LIBS_list vcdriver kvstore_flash_impl mbedcrypto mbedtls mbedx509)
add_dependencies(PLATFORM_LIBS ${PLATFORM_LIBS_list})
# This is the one-for-all library.
# Please note that it's important to put '--whole-archive' scope in order to resolve
# weak/strong definitions within a static library.
target_link_libraries(PLATFORM_LIBS -Wl,--whole-archive ${PLATFORM_LIBS_list} -Wl,--no-whole-archive)
