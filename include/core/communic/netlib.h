#ifndef NETLIB_H
#define NETLIB_H
// clang-format off
typedef DWORD RBOOLEAN;
#if defined(PC_MSDOS)
	#include "wattcp/tcp.h"
#elif defined(WIN32) || defined(WINCE)

#ifndef _WINSOCK2API_
	#define WIN32_LEAN_AND_MEAN 
	#include <windows.h>	
	#include <winsock2.h>
#endif

#elif defined(IPC_ANY)
	#include "ipc/tcpip.h"
#elif defined(LINUX)
    #include <string.h>
    #include <sys/select.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
#elif defined(ECOS)
    #include <string.h>
    #include <sys/select.h>
    #include <sys/types.h>
    #include <network.h>
#endif

BEGIN_EXTERN_C

typedef struct sockaddr_in NLAddress;

typedef struct NLIPAddressList
{
	DWORD AddrList[4];
	int nCurrent;
	int nCount;
	int nSuccessCount;
	DWORD dwLastChangeTime;
	int nCountPreffered;
} NLIPAddressList;

//приемный сокет и адрес
typedef struct NLSocket
{
    BYTE socket_is_open;
    BYTE open_was_called;
	WORD wPort;
	NLAddress Addr;
	NLIPAddressList AddressList;
#if defined(PC_MSDOS)
    udp_Socket* Socket;
#elif defined(PC_WIN32)
    SOCKET Socket;
	int socket_state;
	DWORD socket_timeout;
#elif defined(LINUX) || defined(ECOS) || defined(IPC_ANY)
	int Socket;
	int socket_state;
	int socket_timeout;
#endif
} NLSocket;

#define MAX_SUCCEES_ATTEMPTS_ON_OTHER_NETWORK	100

extern int NLGetBasePort();
extern void NLSocketsInit(void);

// wPort = 0 open socket without binding
extern int NLOpenUDPSocket(NLSocket* pSocket, WORD wPort);
extern int NLTryOpenUDPSocket(NLSocket* pSocket, WORD wPort, WORD wPortRange);
extern int NLTryReopenUDPSocket(NLSocket* pSocket);

extern int NLInitSocket(NLSocket* pSocket);
extern int NLInitSocketWithAddressList(NLSocket* pSocket, const char* szAddresses, WORD port);
extern void NLCloseSocket(NLSocket* pSocket);
extern void NLMakeAddressChIP(NLAddress* pAddress, const char* tcIPAddress, WORD wPort);
extern void NLMakeAddress(NLAddress* pAddress, DWORD dwIPAddress, WORD wPort);
extern void NLMakeAddressFromList(NLSocket* pSocket, WORD wPort);

extern void NLParseAddressList(NLIPAddressList* pAddressList, const char* addresses);
extern int NLSendTo(NLSocket* pSocket, NLAddress* pAddress, BYTE* pBuffer, int nSize);
extern int NLRecvFrom(NLSocket* pSocket, NLAddress* pAddress, BYTE* pBuffer, int nSize, int nTimeout);

extern void NLClear(NLSocket* pSocket, BYTE* pBuffer, int nSize);
extern int NLSendReceive(NLSocket* pSocket, NLAddress* pAddress, BYTE* pSendBuffer, int nSendSize, 
				  BYTE* pReceiveBuffer, int nReceiveSize, DWORD nTimeout, RBOOLEAN bUseCRC, int nRetryCount, int nSerialTaskIndex);
extern void NLChangeIPAddressInList(NLIPAddressList* pList);
extern void NLCheckResetIPAddressInList(NLIPAddressList* pList);

extern int NLTCPSend(NLSocket* pSocket, BYTE* pBuffer, int nSize);
extern int NLTCPRecv(NLSocket* pSocket, BYTE* pBuffer, int nSize, int nTimeout,BYTE nTaskID);
extern int NLTCPRecvStopByte(NLSocket* pSocket, BYTE *pBuffer, int nSize, int nTimeout, BYTE nTaskID, char* cFinishChars, int CountFinishChars, int CountAfterByte);
extern int NLOpenTCPSocket(NLSocket* pSocket);
extern int NLTryOpenTCPSocket(NLSocket* pSocket);
extern int NLTCPSendReceive(NLSocket* pSocket, BYTE* pSendBuffer, int nSendSize, BYTE* pReceiveBuffer, int nReceiveSize, DWORD nTimeout, int nSerialTaskIndex, int* pSize);
extern int NLTCPSendReceiveStopByte(NLSocket* pSocket, const BYTE* pSendBuffer, int nSendSize,BYTE* pReceiveBuffer, int nReceiveSize, DWORD nTimeout, int nSerialTaskIndex, int* pSize, char* cFinishChars, int CountFinishChars, int CountAfterByte);
extern int NLTCPSendReceiveDLMS(NLSocket* pSocket, BYTE* pSendBuffer, int nSendSize,BYTE* pReceiveBuffer, int nReceiveSize, DWORD nTimeout, int nSerialTaskIndex, int* pSize,BYTE StopByte, int CountAfterByte);
extern int NLTCPRecvDLMS(NLSocket* pSocket, BYTE *pBuffer, int nSize, int nTimeout,BYTE nTaskID,BYTE StopByte, int CountAfterByte);
extern int NLTCPCheck(NLSocket* pSocket);
extern void NLTCPGetInfo(char *BufInfo, int Size, const NLAddress *Addr);
extern DWORD NLResolveHostName(const char* host);

//Межконтроллерный обмен
extern int NLGetOtherControllerAddresses(int controllerIndex, NLIPAddressList* pList, int* controllerPort, RBOOLEAN* fault);

#define CLOSE_SOCK      0
#define CONNECTING_SOCK 1
#define CONNECTED_SOCK  2 //
#define TIMEOUT_SOCK    10 //

END_EXTERN_C

#endif // #ifndef NETLIB_H
