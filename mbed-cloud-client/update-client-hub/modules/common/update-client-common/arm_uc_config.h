// ----------------------------------------------------------------------------
// Copyright 2016-2020 ARM Ltd.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------

#ifndef ARM_UPDATE_CONFIG_H
#define ARM_UPDATE_CONFIG_H

#ifdef MBED_CLOUD_CLIENT_USER_CONFIG_FILE
#include MBED_CLOUD_CLIENT_USER_CONFIG_FILE
#endif

#ifdef MBED_CLOUD_CLIENT_SUPPORT_UPDATE
// ARM_UC_ENABLE is an internal flag used by update client.
#define ARM_UC_ENABLE 1
#ifdef MBED_CLOUD_CLIENT_SUPPORT_MULTICAST_UPDATE
    #define ARM_UC_MULTICAST_ENABLE 1
    #define ARM_UC_FEATURE_DELTA_PAAL 1
    #define ARM_UC_FEATURE_DELTA_PAAL_NEWMANIFEST 1
    #ifdef MBED_CLOUD_CLIENT_MULTICAST_BORDER_ROUTER
        #define ARM_UC_MULTICAST_BORDER_ROUTER_MODE
    #else
        #define ARM_UC_MULTICAST_NODE_MODE
        #if !defined(MBED_CONF_UPDATE_CLIENT_DELTA_STORAGE_ADDRESS) || !defined (MBED_CONF_UPDATE_CLIENT_DELTA_STORAGE_SIZE)
            #error "Delta feature with multicast updates requires storage address and size"
        #endif
        #if MBED_CONF_UPDATE_CLIENT_STORAGE_LOCATIONS > 1
            #error "Multicast updates support only one update storage location (and separate delta storage location)"
        #endif
    #endif
#else
    #define ARM_UC_MULTICAST_ENABLE 0
#endif
#else
#define ARM_UC_ENABLE 0
#define ARM_UC_MULTICAST_ENABLE 0
#define ARM_UC_FEATURE_DELTA_PAAL 0
#endif

#define ARM_UC_DELTA_SLOT_ID 1


// Rest is only defined when update client is enabled
#if defined(ARM_UC_ENABLE) && (ARM_UC_ENABLE == 1)

#ifndef MAX_SOURCES
#define MAX_SOURCES 10
#endif

#ifndef ARM_UC_SOCKET_MAX_RETRY
#define ARM_UC_SOCKET_MAX_RETRY 3
#endif

/* PROFILE FLAGS
ARM_UC_PROFILE_MBED_CLIENT_LITE
ARM_UC_PROFILE_MBED_CLOUD_CLIENT
*/

/* FEATURE FLAGS
ARM_UC_FEATURE_PAL_BLOCKDEVICE
ARM_UC_FEATURE_PAL_FILESYSTEM
ARM_UC_FEATURE_PAL_FLASHIAP
ARM_UC_FEATURE_PAL_FLASHIAP_MCUBOOT
ARM_UC_FEATURE_PAL_LINUX
ARM_UC_FEATURE_PAL_RTL8195AM
ARM_UC_FEATURE_FW_SOURCE_HTTP
ARM_UC_FEATURE_FW_SOURCE_COAP
ARM_UC_FEATURE_FW_SOURCE_LOCAL_FILE
ARM_UC_FEATURE_RESUME_ENGINE
ARM_UC_FEATURE_MANIFEST_PUBKEY
ARM_UC_FEATURE_MANIFEST_PSK
ARM_UC_FEATURE_IDENTITY_NVSTORE
ARM_UC_FEATURE_IDENTITY_KCM
ARM_UC_FEATURE_IDENTITY_RAW_CONFIG
ARM_UC_FEATURE_CRYPTO_PAL
ARM_UC_FEATURE_CRYPTO_MBEDTLS
ARM_UC_FEATURE_CERT_STORE_KCM
ARM_UC_FEATURE_CERT_STORE_RAW
ARM_UC_FEATURE_PSK_STORE_NVSTORE
ARM_UC_FEATURE_PSK_STORE_RAW
ARM_UC_FEATURE_SIMPLE_COAP_SOURCE
*/

#if !defined(__SXOS__)
#ifndef ARM_UC_FEATURE_DELTA_PAAL
// To Enable Delta support with latest manifest format
// define both ARM_UC_FEATURE_DELTA_PAAL and
// ARM_UC_FEATURE_DELTA_PAAL_NEWMANIFEST to 1
// If you define only first one the temporary
// manifest-format with delta-information encoded
// in "VendorInfo" -field will be supported
#define ARM_UC_FEATURE_DELTA_PAAL 1
#define ARM_UC_FEATURE_DELTA_PAAL_NEWMANIFEST 1
#endif
#endif

#ifdef TARGET_LIKE_MBED
#if defined(ARM_UC_FEATURE_DELTA_PAAL) && (ARM_UC_FEATURE_DELTA_PAAL == 1)
// Delta-PAAL directly accesses this macro. If Mbed CLI did not find the bootloader image, this macro will not be generated.
#ifndef APPLICATION_ADDR
#error "Missing APPLICATION_ADDR definition. Check configuration for bootloader binary."
#endif
#endif
#endif

#define MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL_COAP 1
#define MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL_HTTP 2

/* Map MCC Fetcher-flag to internal flag */
#if defined(MBED_CONF_MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL)
#if MBED_CONF_MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL == MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL_COAP
#define ARM_UC_FEATURE_FW_SOURCE_COAP 1
#define ARM_UC_FEATURE_FW_SOURCE_HTTP 0
#elif MBED_CONF_MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL == MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL_HTTP
#define ARM_UC_FEATURE_FW_SOURCE_COAP 0
#define ARM_UC_FEATURE_FW_SOURCE_HTTP 1
#else
#error "Invalid value for MBED_CONF_MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL. Must be either MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL_HTTP or MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL_COAP."
#endif
#endif

#if defined(MBED_CONF_MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL) && MBED_CONF_MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL == MBED_CLOUD_CLIENT_UPDATE_DOWNLOAD_PROTOCOL_HTTP
#define ARM_UC_FEATURE_FW_SOURCE_COAP 0
#define ARM_UC_FEATURE_FW_SOURCE_HTTP 1
#endif

// Override to use local source
#if defined(TEST_USING_LOCAL_SOURCES) && TEST_USING_LOCAL_SOURCES == 1
#define ARM_UC_FEATURE_FW_SOURCE_LOCAL_FILE 1
#define ARM_UC_FEATURE_FW_SOURCE_COAP 0
#define ARM_UC_FEATURE_FW_SOURCE_HTTP 0
#endif  // TEST_USING_LOCAL_SOURCES

/* Map Legacy flags to new flags */
#if defined(ARM_UC_USE_PAL_CRYPTO) && ARM_UC_USE_PAL_CRYPTO == 1
#warning "ARM_UC_USE_PAL_CRYPTO DEPRECATED, See arm_uc_config.h for new flags."
#define ARM_UC_FEATURE_CRYPTO_PAL 1
#endif

#if defined(ARM_UC_USE_KCM) && ARM_UC_USE_KCM == 1
#warning "ARM_UC_USE_KCM DEPRECATED, See arm_uc_config.h for new flags."
#define ARM_UC_FEATURE_IDENTITY_NVSTORE 0
#define ARM_UC_FEATURE_IDENTITY_KCM 1
#define ARM_UC_FEATURE_IDENTITY_RAW_CONFIG 0
#define ARM_UC_FEATURE_PSK_STORE_NVSTORE 0
#define ARM_UC_FEATURE_PSK_STORE_RAW 0
#define ARM_UC_FEATURE_CERT_STORE_KCM 1
#define ARM_UC_FEATURE_CERT_STORE_RAW 0
#endif

#if defined(ARM_UC_USE_CFSTORE) && ARM_UC_USE_CFSTORE != 0
#error "CFSTORE SUPPORT DEPRECATED!"
#endif

#if defined (ARM_UC_USE_RAW_CONFIG) && ARM_UC_USE_RAW_CONFIG == 1
#warning "ARM_UC_USE_RAW_CONFIG DEPRECATED, See arm_uc_config.h for new flags."
#define ARM_UC_FEATURE_IDENTITY_NVSTORE 0
#define ARM_UC_FEATURE_IDENTITY_KCM 0
#define ARM_UC_FEATURE_IDENTITY_RAW_CONFIG 1
#define ARM_UC_FEATURE_PSK_STORE_NVSTORE 0
#define ARM_UC_FEATURE_PSK_STORE_RAW 1
#define ARM_UC_FEATURE_CERT_STORE_KCM 0
#define ARM_UC_FEATURE_CERT_STORE_RAW 1
#endif

#if defined (ARM_UC_USE_NVSTORE) && ARM_UC_USE_NVSTORE == 1
#warning "ARM_UC_USE_NVSTORE DEPRECATED, See arm_uc_config.h for new flags."
#define ARM_UC_FEATURE_IDENTITY_NVSTORE 1
#define ARM_UC_FEATURE_IDENTITY_KCM 0
#define ARM_UC_FEATURE_IDENTITY_RAW_CONFIG 0
#define ARM_UC_FEATURE_PSK_STORE_NVSTORE 1
#define ARM_UC_FEATURE_PSK_STORE_RAW 0
#define ARM_UC_FEATURE_CERT_STORE_KCM 0
#define ARM_UC_FEATURE_CERT_STORE_RAW 0
#endif

/* Profile feature modifiers */
#if defined(ARM_UC_PROFILE_MBED_CLIENT_LITE) && (ARM_UC_PROFILE_MBED_CLIENT_LITE == 1)

#ifndef ARM_UC_FEATURE_MANIFEST_PUBKEY
#define ARM_UC_FEATURE_MANIFEST_PUBKEY 0
#endif /* ARM_UC_FEATURE_MANIFEST_PUBKEY */

#ifndef ARM_UC_FEATURE_MANIFEST_PSK
#define ARM_UC_FEATURE_MANIFEST_PSK 1
#endif /* ARM_UC_FEATURE_MANIFEST_PSK */

#ifndef ARM_UC_FEATURE_FW_SOURCE_COAP
#define ARM_UC_FEATURE_FW_SOURCE_COAP 1
#endif /* ARM_UC_FEATURE_FW_SOURCE_COAP */

#ifndef ARM_UC_FEATURE_RESUME_ENGINE
#define ARM_UC_FEATURE_RESUME_ENGINE 0
#endif /* ARM_UC_FEATURE_RESUME_ENGINE */

#ifndef ARM_UC_FEATURE_FW_SOURCE_HTTP
#define ARM_UC_FEATURE_FW_SOURCE_HTTP 0
#endif /* ARM_UC_FEATURE_FW_SOURCE_HTTP */

#ifndef ARM_UC_FEATURE_FW_SOURCE_LOCAL_FILE
#define ARM_UC_FEATURE_FW_SOURCE_LOCAL_FILE 0
#endif /* ARM_UC_FEATURE_FW_SOURCE_LOCAL_FILE */

#ifndef ARM_UC_FEATURE_CRYPTO_MBEDTLS
#define ARM_UC_FEATURE_CRYPTO_MBEDTLS 1
#endif /* ARM_UC_FEATURE_CRYPTO_MBEDTLS */

#ifndef ARM_UC_FEATURE_IDENTITY_NVSTORE
#define ARM_UC_FEATURE_IDENTITY_NVSTORE 1
#endif /* ARM_UC_FEATURE_IDENTITY_NVSTORE */

#ifndef ARM_UC_FEATURE_CERT_STORE_KCM
#define ARM_UC_FEATURE_CERT_STORE_KCM 0
#endif /* ARM_UC_FEATURE_CERT_STORE_KCM */

#ifndef ARM_UC_FEATURE_CERT_STORE_RAW
#define ARM_UC_FEATURE_CERT_STORE_RAW 0
#endif /* ARM_UC_FEATURE_CERT_STORE_RAW */

#ifndef ARM_UC_FEATURE_PSK_STORE_NVSTORE
#define ARM_UC_FEATURE_PSK_STORE_NVSTORE 1
#endif /* ARM_UC_FEATURE_PSK_STORE_NVSTORE */

#ifndef ARM_UC_FEATURE_PSK_STORE_RAW
#define ARM_UC_FEATURE_PSK_STORE_RAW 0
#endif /* ARM_UC_FEATURE_PSK_STORE_RAW */

#ifndef ARM_UC_FEATURE_SIMPLE_COAP_SOURCE
#define ARM_UC_FEATURE_SIMPLE_COAP_SOURCE 1
#endif

#elif defined(ARM_UC_PROFILE_MBED_CLOUD_CLIENT) && (ARM_UC_PROFILE_MBED_CLOUD_CLIENT == 1)

#ifndef ARM_UC_FEATURE_MANIFEST_PUBKEY
#define ARM_UC_FEATURE_MANIFEST_PUBKEY 1
#endif /* ARM_UC_FEATURE_MANIFEST_PUBKEY */

#ifndef ARM_UC_FEATURE_MANIFEST_PSK
#define ARM_UC_FEATURE_MANIFEST_PSK 0
#endif /* ARM_UC_FEATURE_MANIFEST_PSK */

#ifndef ARM_UC_FEATURE_FW_SOURCE_COAP
#define ARM_UC_FEATURE_FW_SOURCE_COAP 0
#endif /* ARM_UC_FEATURE_FW_SOURCE_COAP */

#ifndef ARM_UC_FEATURE_FW_SOURCE_HTTP
#define ARM_UC_FEATURE_FW_SOURCE_HTTP 1
#endif /* ARM_UC_FEATURE_FW_SOURCE_HTTP */

#ifndef ARM_UC_FEATURE_FW_SOURCE_LOCAL_FILE
#define ARM_UC_FEATURE_FW_SOURCE_LOCAL_FILE 0
#endif /* ARM_UC_FEATURE_FW_SOURCE_LOCAL_FILE */

#ifndef ARM_UC_FEATURE_RESUME_ENGINE
#define ARM_UC_FEATURE_RESUME_ENGINE 1
#endif /* ARM_UC_FEATURE_RESUME_ENGINE */

#ifndef ARM_UC_FEATURE_IDENTITY_KCM
#define ARM_UC_FEATURE_IDENTITY_KCM 1
#endif /* ARM_UC_FEATURE_IDENTITY_RAW_CONFIG */

#ifndef ARM_UC_FEATURE_CRYPTO_PAL
#define ARM_UC_FEATURE_CRYPTO_PAL 1
#endif /* ARM_UC_FEATURE_CRYPTO_PAL */

#ifndef ARM_UC_FEATURE_CERT_STORE_KCM
#define ARM_UC_FEATURE_CERT_STORE_KCM 1
#endif /* ARM_UC_FEATURE_CERT_STORE_KCM */

#ifndef ARM_UC_FEATURE_CERT_STORE_RAW
#define ARM_UC_FEATURE_CERT_STORE_RAW 0
#endif /* ARM_UC_FEATURE_CERT_STORE_RAW */

#ifndef ARM_UC_FEATURE_PSK_STORE_NVSTORE
#define ARM_UC_FEATURE_PSK_STORE_NVSTORE 0
#endif /* ARM_UC_FEATURE_PSK_STORE_NVSTORE */

#ifndef ARM_UC_FEATURE_PSK_STORE_RAW
#define ARM_UC_FEATURE_PSK_STORE_RAW 0
#endif /* ARM_UC_FEATURE_PSK_STORE_RAW */

#ifndef ARM_UC_FEATURE_SIMPLE_COAP_SOURCE
#define ARM_UC_FEATURE_SIMPLE_COAP_SOURCE 1
#endif

#else

#error "Please define macro ARM_UC_PROFILE_MBED_CLIENT_LITE=1 or ARM_UC_PROFILE_MBED_CLOUD_CLIENT=1."

#endif /* ARM_UC_PROFILE_MBED_CLIENT_LITE */

/* Always have only one PAAL implementation */
#if defined(UPDATE_CLIENT_STORAGE_FLASHIAP_BLOCKDEVICE)

#define ARM_UC_FEATURE_PAL_FLASHIAP 1
#define ARM_UC_FEATURE_PAL_BLOCKDEVICE 1
#define ARM_UC_FEATURE_PAL_FLASHIAP_MCUBOOT 0
#define ARM_UC_FEATURE_PAL_FILESYSTEM 0
#define ARM_UC_FEATURE_PAL_LINUX 0
#define ARM_UC_FEATURE_PAL_RTL8195AM 0

#elif defined(UPDATE_CLIENT_STORAGE_FLASHIAP)

#define ARM_UC_FEATURE_PAL_FLASHIAP 1
#define ARM_UC_FEATURE_PAL_FLASHIAP_MCUBOOT 0
#define ARM_UC_FEATURE_PAL_BLOCKDEVICE 0
#define ARM_UC_FEATURE_PAL_FILESYSTEM 0
#define ARM_UC_FEATURE_PAL_LINUX 0
#define ARM_UC_FEATURE_PAL_RTL8195AM 0

#elif defined(UPDATE_CLIENT_STORAGE_FLASHIAP_MCUBOOT)

#define ARM_UC_FEATURE_PAL_FLASHIAP_MCUBOOT 1
#define ARM_UC_FEATURE_PAL_FLASHIAP 0
#define ARM_UC_FEATURE_PAL_BLOCKDEVICE 0
#define ARM_UC_FEATURE_PAL_FILESYSTEM 0
#define ARM_UC_FEATURE_PAL_LINUX 0
#define ARM_UC_FEATURE_PAL_RTL8195AM 0

#elif defined(UPDATE_CLIENT_STORAGE_FILESYSTEM)

#define ARM_UC_FEATURE_PAL_FLASHIAP 1
#define ARM_UC_FEATURE_PAL_FLASHIAP_MCUBOOT 0
#define ARM_UC_FEATURE_PAL_BLOCKDEVICE 0
#define ARM_UC_FEATURE_PAL_FILESYSTEM 1
#define ARM_UC_FEATURE_PAL_LINUX 0
#define ARM_UC_FEATURE_PAL_RTL8195AM 0

#elif defined(UPDATE_CLIENT_STORAGE_LINUX_GENERIC)

#define ARM_UC_FEATURE_PAL_FLASHIAP 0
#define ARM_UC_FEATURE_PAL_FLASHIAP_MCUBOOT 0
#define ARM_UC_FEATURE_PAL_BLOCKDEVICE 0
#define ARM_UC_FEATURE_PAL_FILESYSTEM 0
#define ARM_UC_FEATURE_PAL_LINUX 1
#define ARM_UC_FEATURE_PAL_RTL8195AM 0

#elif defined(UPDATE_CLIENT_STORAGE_REALTEK_RTL8195AM)

#define ARM_UC_FEATURE_PAL_FLASHIAP 0
#define ARM_UC_FEATURE_PAL_FLASHIAP_MCUBOOT 0
#define ARM_UC_FEATURE_PAL_BLOCKDEVICE 0
#define ARM_UC_FEATURE_PAL_FILESYSTEM 0
#define ARM_UC_FEATURE_PAL_LINUX 0
#define ARM_UC_FEATURE_PAL_RTL8195AM 1

#else /* support legacy configuration method for storage */

#if defined(ARM_UC_USE_PAL_BLOCKDEVICE) && (ARM_UC_USE_PAL_BLOCKDEVICE == 1)
#define ARM_UC_FEATURE_PAL_BLOCKDEVICE 1
#endif

#if defined(TARGET_LIKE_MBED)
#if !defined (ARM_UC_FEATURE_PAL_FLASHIAP_MCUBOOT) || (ARM_UC_FEATURE_PAL_FLASHIAP_MCUBOOT == 0)
#define ARM_UC_FEATURE_PAL_FLASHIAP 1
#endif
#endif

#if defined(ARM_UC_PROFILE_MBED_CLOUD_CLIENT) && (ARM_UC_PROFILE_MBED_CLOUD_CLIENT == 1)
/* turn on all possible implementations */
#define ARM_UC_FEATURE_PAL_FILESYSTEM 1
#define ARM_UC_FEATURE_PAL_LINUX 1
#define ARM_UC_FEATURE_PAL_RTL8195AM 1

#elif defined(ARM_UC_PROFILE_MBED_CLIENT_LITE) && (ARM_UC_PROFILE_MBED_CLIENT_LITE == 1)
/* turn on only pal flashiap */
#define ARM_UC_FEATURE_PAL_FILESYSTEM 0
#define ARM_UC_FEATURE_PAL_LINUX 0
#define ARM_UC_FEATURE_PAL_RTL8195AM 0

#endif /* ARM_UC_PROFILE_MBED_CLOUD_CLIENT */

#endif

/* Always have only one IDENTITY implementation */
#if defined(ARM_UC_FEATURE_IDENTITY_NVSTORE) && (ARM_UC_FEATURE_IDENTITY_NVSTORE == 1)
#define ARM_UC_FEATURE_IDENTITY_KCM 0
#define ARM_UC_FEATURE_IDENTITY_RAW_CONFIG 0
#elif defined(ARM_UC_FEATURE_IDENTITY_KCM) && (ARM_UC_FEATURE_IDENTITY_KCM == 1)
#define ARM_UC_FEATURE_IDENTITY_NVSTORE 0
#define ARM_UC_FEATURE_IDENTITY_RAW_CONFIG 0
#elif defined(ARM_UC_FEATURE_IDENTITY_RAW_CONFIG) && (ARM_UC_FEATURE_IDENTITY_RAW_CONFIG == 1)
#define ARM_UC_FEATURE_IDENTITY_NVSTORE 0
#define ARM_UC_FEATURE_IDENTITY_KCM 0
#endif

/* Always have only one CRYPTO implementation */
#if defined(ARM_UC_FEATURE_CRYPTO_PAL) && (ARM_UC_FEATURE_CRYPTO_PAL == 1)
#define ARM_UC_FEATURE_CRYPTO_MBEDTLS 0
#elif defined(ARM_UC_FEATURE_CRYPTO_MBEDTLS) && (ARM_UC_FEATURE_CRYPTO_MBEDTLS == 1)
#define ARM_UC_FEATURE_CRYPTO_PAL 0
#endif

/* Total memory allocated for download buffers.
   For HTTP sources, this number cannot be below 1 KiB.
*/
#ifdef MBED_CLOUD_CLIENT_UPDATE_BUFFER
#if MBED_CLOUD_CLIENT_UPDATE_BUFFER < 1024
#error MBED_CLOUD_CLIENT_UPDATE_BUFFER must be 1024 bytes or more
#else
#define ARM_UC_BUFFER_SIZE MBED_CLOUD_CLIENT_UPDATE_BUFFER
#endif
#endif

#ifndef ARM_UC_BUFFER_SIZE
#define ARM_UC_BUFFER_SIZE 1024
#endif

#define MBED_CLOUD_CLIENT_UPDATE_CERTIFICATE_PREFIX "mbed.UpdateAuthCert."
#define MBED_CLOUD_CLIENT_UPDATE_CERTIFICATE_DEFAULT "mbed.UpdateAuthCert"
#define MBED_CLOUD_SHA256_BYTES (256/8)
#define MBED_CLOUD_BASE64_SIZE(X) (((X + 2)/3)*4)
#define MBED_CLOUD_CLIENT_UPDATE_CERTIFICATE_NAME_SIZE (MBED_CLOUD_BASE64_SIZE(MBED_CLOUD_SHA256_BYTES) + sizeof(MBED_CLOUD_CLIENT_UPDATE_CERTIFICATE_PREFIX))


// NOTE: The charset must be sorted except for the trailing character which is used as a padding character.
#define MBED_CLOUD_UPDATE_BASE64_CHARSET "0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz-"

#ifndef ARM_UC_SCHEDULER_STORAGE_POOL_SIZE
#define ARM_UC_SCHEDULER_STORAGE_POOL_SIZE 32
#endif

#if defined(ARM_UC_FEATURE_ROOTLESS_STAGE_1) && (ARM_UC_FEATURE_ROOTLESS_STAGE_1 == 1) && !defined(TARGET_IS_PC_LINUX)
#error The rootless update feature can be enabled only for Linux builds.
#endif
#if !defined(ARM_UC_DELTAPAAL_WRITE_BUF_SIZE)
#ifndef SD
// Use temp definition. This way wont overwrite if SD is defined
#define SD_TEMP_DEFINE
#define SD 1
#endif
// MBED_CONF_STORAGE_FILESYSTEM_BLOCKDEVICE macro can contain macro SD.
// If device used SD block device, page size is hard coded as 512
#if (MBED_CONF_STORAGE_FILESYSTEM_BLOCKDEVICE == 1)
#define ARM_UC_DELTAPAAL_WRITE_BUF_SIZE 512
#else
// ToDo: should this be other than hard coded value
#define ARM_UC_DELTAPAAL_WRITE_BUF_SIZE 128
#endif
#ifdef SD_TEMP_DEFINE
#undef SD
#endif
#endif

#ifndef BS_PATCH_COMPILE_TIME_MEMORY_ALLOC
#define BS_PATCH_COMPILE_TIME_MEMORY_ALLOC 1024
#endif

#endif // ARM_UC_ENABLE
#endif // ARM_UPDATE_CONFIG_H
