#pragma once

#include "archive/mplc_archive.h"
#include "share/net_stream.h"
#include "mplc/database.h"

struct RemoteSourceInfo {
    int64_t remote_id, local_id;
    std::string local_path, remote_path;
    int64_t last_time;
    int32_t rand_id;
    int64_t archiveItemId;
    int32_t error_count;
    OpcUa_BuiltInTypeHlp type;
    mplc::PinCache::ptr pin;
    bool more_values;
    bool time_inited;

    RemoteSourceInfo()
        : remote_id(0), local_id(0), last_time(0), rand_id(rand()), more_values(false), archiveItemId(-1),
          error_count(0), time_inited(false) {}
    RemoteSourceInfo(const Value& info) {
        init(info);
    }
    FileTime addValue(const Value& rec) const;
    void init(const Value& info);
    bool initTime();
};

class RemoteArchiveSource;

class RemoteControllerArchive {
private:
    mplc::lib::unordered_map<int64_t, int> map_archive_id_to_index;
    std::vector<RemoteSourceInfo> requests;
    mplc::endpoint endpoint;
    mplc::socket m_socket;
    volatile bool _stop;
    bool need_get_items;
    bool need_subscription;
    int sub_id;
    int request_id;
    int request_last_index;
    int request_next_index;
    int last_error_count;
    int all_error_count;
    int count_more_values;

    int64_t last_stat_info;
    int64_t last_period;
    int64_t cycle_counts;
    int64_t last_read;
    int64_t controller_id;
    int64_t last_values_count;
    int64_t all_values_count;
    RemoteArchiveSource* host;
    mplc::lib::thread* thread;

    mplc::JsonRequest writer_get_items;
    mplc::JsonRequest writer_create_subscription;
    mplc::JsonRequest writer_read;
    mplc::JsonRequest writer_publish;

    void static ThreadFunc(void* arg);
    OpcUa_StatusCode Execute();
    OpcUa_StatusCode CreateSubscription();
    OpcUa_StatusCode GetItems();
    OpcUa_StatusCode CreateRequest();
    OpcUa_StatusCode PublishData(bool& no_response_prev, int count_iteration);

public:
    RemoteControllerArchive(RemoteArchiveSource* host, int64_t controller_id);
    OpcUa_StatusCode Start();
    OpcUa_StatusCode Stop();
    OpcUa_StatusCode AddRequest(const Value& item);
};

class RemoteArchiveSource {
public:
    OpcUa_StatusCode Init();
    OpcUa_StatusCode Start();
    OpcUa_StatusCode Stop();
    static RemoteArchiveSource& Instance();
    ~RemoteArchiveSource();

private:
    OpcUa_StatusCode AddRequest(const Value& item);
    RemoteArchiveSource();
    RemoteArchiveSource(const RemoteArchiveSource& ras);
    RemoteArchiveSource& operator=(const RemoteArchiveSource& ras);
    virtual OpcUa_StatusCode DestroySource();

    bool _started;
    std::map<int64_t, RemoteControllerArchive*> controllers;

public:
    int64_t request_period;
    int num_items_in_request;
    int _timeout;
    int _limit;
};
