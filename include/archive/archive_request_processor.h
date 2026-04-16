//#ifndef __ARCHIVE_REQUEST_PROCESSOR_H__
//#define __ARCHIVE_REQUEST_PROCESSOR_H__
//#include "share/request_processor.h"
//
//class ArchiveRequestProcessor : public CRequestProcessor {
//private:
//    static LPCSTR s_supportedMethods[];
//    int32_t _lastArchiveSubId;
//    typedef std::map<uint32_t, ArchiveSubscription> ArchiveSubscriptionMap;
//    ArchiveSubscriptionMap _archiveSubMap;
//    mplc::lib::mutex async_read;
//    struct AsyncRequestInfo {
//        int source_id;
//        boost::shared_ptr<ArchiveRequest> req;
//        int32_t req_id;
//    };
//    std::map<int32_t, AsyncRequestInfo> async_requests;
//
//public:
//#ifndef USE_NEW_DB_API
//    ADD_HTTP_METHODS(Methods,
//                     GetArchiveItems,
//                     HistoryReadRaw,
//                     HistoryReadRawAsync,
//                     PublishHistoryData,
//                     ReadNewRecords,
//                     // DeleteHistoryDataSubscription,
//                     // CreateHistoryDataSubscription,
//                     GetReportsData)
//#else
//    ADD_HTTP_METHODS(Methods, NO_METHODS)
//#endif
//    ArchiveRequestProcessor(): CRequestProcessor("ArchiveManager"), _lastArchiveSubId(0) {}
//
//    void Reset() {}
//
//    OpcUa_StatusCode CallMethod(const std::string& method,
//                                const std::string& source,
//                                const Document& request,
//                                ResponseWriter& writer,
//                                int nMaxResponseSize);
//    // OpcUa_StatusCode CallSubscriptionMethod(Methods::Name method, const Document & request, RequestWriter & writer,
//    // int nMaxResponseSize);
//
//    // OpcUa_StatusCode DeleteHistoryDataSubscription(const Document & request);
//
//private:
//    typedef std::map<int, ArchiveRequest> RequestsBySourceIndexMap;
//    OpcUa_StatusCode GetArchiveItems(const Document& request, ResponseWriter& writer);
//    OpcUa_StatusCode HistoryReadRaw(const Document& request, ResponseWriter& writer, int nMaxResponseSize) const;
//    OpcUa_StatusCode ReadNewRecords(const Document& request, ResponseWriter& writer, int nMaxResponseSize) const;
//    OpcUa_StatusCode HistoryReadRawAsync(const Document& request, ResponseWriter& writer, int nMaxResponseSize);
//    // OpcUa_StatusCode CreateHistoryDataSubscription(const Document& request, RequestWriter& writer, int
//    // nMaxResponseSize);
//    OpcUa_StatusCode PublishHistoryData(const Document& request, ResponseWriter& writer, int nMaxResponseSize);
//};
//
//#endif  //__ARCHIVE_REQUEST_PROCESSOR_H__
