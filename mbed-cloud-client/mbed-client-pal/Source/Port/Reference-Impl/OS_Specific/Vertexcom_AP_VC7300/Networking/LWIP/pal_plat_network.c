#include "pal.h"
#include "pal_plat_network.h"

#define TRACE_GROUP "PAL"

palStatus_t pal_plat_socketsInit(void* context)
{
    (void)context; // parameter not used in this case - this avoids the warning
    return PAL_SUCCESS;
}

palStatus_t pal_plat_registerNetworkInterface(void* context, uint32_t* interfaceIndex)
{
    palStatus_t result = PAL_SUCCESS;
    return result;
}

palStatus_t pal_plat_unregisterNetworkInterface(uint32_t interfaceIndex)
{
    return PAL_SUCCESS;
}

palStatus_t pal_plat_socketsTerminate(void* context)
{
    (void)context; // replace with macro
    // clean up static sockets array ? (close all sockets?)
    return PAL_SUCCESS;
}

palStatus_t pal_plat_setSocketOptions(palSocket_t socket, int optionName, const void* optionValue, palSocketLength_t optionLength)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_setSocketOptionsWithLevel(palSocket_t socket, palSocketOptionLevelName_t optionLevel, int optionName, const void* optionValue, palSocketLength_t optionLength)
{
    (void)socket;
    (void)optionLevel;
    (void)optionName;
    (void)optionValue;
    (void)optionLength;

    return PAL_ERR_NOT_SUPPORTED;
}

palStatus_t pal_plat_isNonBlocking(palSocket_t socket, bool* isNonBlocking)
{
    return PAL_SUCCESS;
}

palStatus_t pal_plat_bind(palSocket_t socket, palSocketAddress_t* myAddress, palSocketLength_t addressLength)
{
    return PAL_SUCCESS;
}

palStatus_t pal_plat_receiveFrom(palSocket_t socket, void* buffer, size_t length, palSocketAddress_t* from, palSocketLength_t* fromLength, size_t* bytesReceived)
{
    return PAL_SUCCESS;
}

palStatus_t pal_plat_sendTo(palSocket_t socket, const void* buffer, size_t length, const palSocketAddress_t* to, palSocketLength_t toLength, size_t* bytesSent)
{
    return PAL_SUCCESS;
}

palStatus_t pal_plat_close(palSocket_t* socket)
{
    return PAL_SUCCESS;
}

palStatus_t pal_plat_getNumberOfNetInterfaces( uint32_t* numInterfaces)
{
    return PAL_SUCCESS;
}

palStatus_t pal_plat_getNetInterfaceInfo(uint32_t interfaceNum, palNetInterfaceInfo_t * interfaceInfo)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

#if PAL_NET_TCP_AND_TLS_SUPPORT // functionality below supported only in case TCP is supported.

#if PAL_NET_SERVER_SOCKET_API
palStatus_t pal_plat_listen(palSocket_t socket, int backlog)
{
    palStatus_t status = PAL_SUCCESS;
    return status;
}

palStatus_t pal_plat_accept(palSocket_t socket, palSocketAddress_t * address, palSocketLength_t* addressLen, palSocket_t* acceptedSocket, palAsyncSocketCallback_t callback, void* callbackArgument)
{
    palStatus_t result = PAL_SUCCESS;
    return result;
}

#endif // PAL_NET_SERVER_SOCKET_API

palStatus_t pal_plat_connect(palSocket_t socket, const palSocketAddress_t* address, palSocketLength_t addressLen)
{
    return PAL_SUCCESS;
}

palStatus_t pal_plat_recv(palSocket_t socket, void *buf, size_t len, size_t* recievedDataSize)
{
    return PAL_SUCCESS;
}

palStatus_t pal_plat_send(palSocket_t socket, const void *buf, size_t len, size_t* sentDataSize)
{
    return PAL_SUCCESS;
}

#endif // PAL_NET_TCP_AND_TLS_SUPPORT

palStatus_t pal_plat_asynchronousSocket(palSocketDomain_t domain, palSocketType_t type, bool nonBlockingSocket, uint32_t interfaceNum, palAsyncSocketCallback_t callback, void* callbackArgument, palSocket_t* socket)
{
    return PAL_SUCCESS;
}

#if PAL_NET_DNS_SUPPORT

palStatus_t pal_plat_getAddressInfo(const char *hostname, palSocketAddress_t *address, palSocketLength_t* length)
{
    palStatus_t result = PAL_SUCCESS;
    return result;
}

#endif // PAL_NET_DNS_SUPPORT

palStatus_t pal_plat_setConnectionStatusCallback(uint32_t interfaceIndex, connectionStatusCallback callback, void *arg)
{
    (void)interfaceIndex;
    (void)callback;
    (void)arg;

    return PAL_ERR_NOT_SUPPORTED;
}

uint8_t pal_plat_getRttEstimate()
{
    return PAL_DEFAULT_RTT_ESTIMATE;
}

uint16_t pal_plat_getStaggerEstimate(uint16_t data_amount)
{
    (void) data_amount;
    return PAL_DEFAULT_STAGGER_ESTIMATE;
}
