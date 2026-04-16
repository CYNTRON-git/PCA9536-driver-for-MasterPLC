#pragma once
#include <queue>
#include <mplc/db/archive_proc.hpp>
#include <mplc/db/configs.h>
#include "events_archive_request.h"
#include "events/events_archive_rec.h"
#include <events/events_archive_share.h>

namespace mplc { namespace events {

    MPLC_EVENTS_API void ReserveEvent(const EventsArchiveRec* event_rec);
    template<class _ConnectionPool>
    class EventsArchive : public archive::IArchive<_ConnectionPool, archive::ArchiveCfg>, public IEventsArchiveSource {
    protected:
        mutable lib::mutex _mtx, req_mtx, rec_mtx;
        std::queue<EventsArchiveRequest::weak_ptr> requests;
        std::queue<DeleteEventsRequest::ptr> del_requests;
        std::vector<EventsArchiveRec> records;
        std::queue<db::CustomRequest::weak_ptr> custom_requests;
        EventsArchive() {}

    public:
        ~EventsArchive() override {}

        // OpcUa_StatusCode AddValue(const EventsArchiveRec& rec) override {
        //    lib::lock_guard<lib::mutex> lock(rec_mtx);
        //    static int recId = 0;
        //    ++recId;
        //    if (IsEnableUserTrace()) {
        //        std::stringstream ss;
        //        ss << "\n--- DB EventRec " << recId << '\n';
        //        ss << rec;
        //        std::string res = ss.str();
        //        PRINTLN(res.c_str());
        //    }
        //    records.push_back(rec);
        //   /* if (ReservEnabled() && !IsSystemInReserv())
        //        ReserveEvent(&rec);*/
        //    return OpcUa_Good;
        //}

        OpcUa_StatusCode AddRequest(lib::weak_ptr<EventsArchiveRequest> request) override {
            lib::lock_guard<lib::mutex> lock(req_mtx);
            requests.push(request);
            return OpcUa_Good;
        }

        OpcUa_StatusCode AddRequest(DeleteEventsRequest::ptr request) override {
            lib::lock_guard<lib::mutex> lock(req_mtx);
            del_requests.push(request);
            return OpcUa_Good;
        }

        OpcUa_StatusCode AddRequest(db::CustomRequest::ptr request) override {
            lib::lock_guard<lib::mutex> lock(req_mtx);
            custom_requests.push(request);
            return OpcUa_Good;
        }
    };
    struct IEventsArchiveProcFactory {
        virtual ~IEventsArchiveProcFactory() {}
        virtual IEventsArchiveProc::ptr make(const archive::ArchiveCfg& arch_cfg,
                                             const archive::ConnectionCfg& pool_cfg) = 0;
    };
}}  // namespace mplc::events
