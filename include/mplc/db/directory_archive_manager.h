#pragma once
#include <share/mplcshare.h>
#include <mplc/database.h>
#include "directory_request.h"
#include "directory_archive.h"

namespace mplc {
    class DirectoryArchiveManager final : boost::noncopyable {
        typedef std::list<std::pair<int, DirectoryRequest::weak_ptr> > RequestQueue;
        typedef std::map<std::string, IDirectoryArchiveFactory*> FactoryMap;
        FactoryMap factories;
        std::map<int, DirectoryArchive::ptr> archives;
        lib::mutex _mutex;
        int64_t updateDirParamsPeriod;
        //RequestQueue requests;
        /*Document cfg;
        cache::Cache cache;
        mplc::lib::mutex _mutex;
        mplc::lib::mutex req_mutex;
        bool stop;
        mplc::lib::thread request_process;*/
        int _default;
        //void requestProc();
        DirectoryArchiveManager();

    public:
        ~DirectoryArchiveManager();
        OpcUa_StatusCode init();
        void addArchive(int db_id, const Value& config);
        void clear();
        //void Stop() { stop = true; }
       
        MPLC_DATABASE_API OpcUa_StatusCode exec(const DirectoryRequest::ptr& req);
        MPLC_DATABASE_API int64_t GetUpdatePeriod() { return updateDirParamsPeriod; };

        MPLC_DATABASE_API void regFactory(const std::string& name, IDirectoryArchiveFactory* factory);
        MPLC_DATABASE_API static DirectoryArchiveManager& instance();
    };

}
