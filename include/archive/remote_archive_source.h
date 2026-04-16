#ifndef __REMOTE_ARCHIVE_SOURCE_H__
#define __REMOTE_ARCHIVE_SOURCE_H__

//#include <string>
//#include <vector>
//#include <map>
//#include "share/addins_share.h"
//#include "archive/archive_source.h"
//#include "share/base_source.h"
//#include "archive/archive_share.h"
#include "communic/netlib.h"
#include "addincmn.h"
#include "share/lua/lua_addins.h"
#include "archive/mplc_archive.h"
#include "share/request_processor.h"

#define RECV_BUFF_SIZE 64000
#define SEND_BUFF_SIZE 2048
#define REQUEST_TIMEOUT 60000

struct RemoteSourceInfo
{
	RemoteSourceInfo() : ControllerIndex(0), ArchiveId(0), remoteID(0), localID(0), LastTime(0),
		ArchiveSource(NULL)
	{
	}
	int ControllerIndex;
	int  ArchiveId;
	
	int64_t  remoteID, localID;
	std::string Path;
	int64_t LastTime;
	class ArchiveSource* ArchiveSource;
};
class RemoteArchiveSource : public BaseSource
{
public:
	RemoteArchiveSource();
	virtual ~RemoteArchiveSource() {
		StopThread();
	}

	OpcUa_StatusCode Start();
	OpcUa_StatusCode Stop();
    MPLC_ARCHIVE_API OpcUa_StatusCode AddRequest(const RemoteSourceInfo &info);
private:
	RemoteArchiveSource(const RemoteArchiveSource& ras);
	RemoteArchiveSource& operator=(const RemoteArchiveSource& ras);
	OpcUa_StatusCode Execute();
	virtual OpcUa_StatusCode DestroySource();
	int _jsonPos;
	void static ThreadFunc(void * arg);
	struct NLSocket _mySocket;
	struct NLIPAddressList _addressList;
	int _controllerPort;
	BYTE pSendBuffer[SEND_BUFF_SIZE];
	BYTE pRecvBuffer[RECV_BUFF_SIZE];
	WORD _transId;
	CCriticalSection _sec, startSec;
	bool _started;
	std::map<int, std::vector<RemoteSourceInfo> > requests;

};

#endif //__REMOTE_ARCHIVE_SOURCE_H__
