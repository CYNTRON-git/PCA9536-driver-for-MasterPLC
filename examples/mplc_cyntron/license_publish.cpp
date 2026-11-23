#include <mplc/api.h>
#include "core/main_imp.h"
#include "license_publish.h"

#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

namespace mplc::cyntron {
namespace {
constexpr char license_path[] = "/run/sa02m-mplc-license.json";
bool publish(const char* data, size_t size) {
    char temp[] = "/run/sa02m-mplc-license.json.XXXXXX";
    int fd = mkstemp(temp);
    if (fd < 0) return false;
    size_t done = 0;
    while (done < size) {
        ssize_t n = write(fd, data + done, size - done);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) break;
        done += static_cast<size_t>(n);
    }
    bool ok = done == size && fchmod(fd, 0644) == 0;
    if (close(fd) < 0) ok = false;
    if (ok && rename(temp, license_path) == 0) return true;
    unlink(temp);
    return false;
}

class Publisher {
public:
    void start() {
        std::lock_guard<std::mutex> guard(mutex);
        if (worker.joinable()) return;
        unlink(license_path);  // An earlier runtime may have used a different key.
        stopping = false;
        worker = std::thread(&Publisher::run, this);
    }
    void stop() {
        std::lock_guard<std::mutex> guard(mutex);
        stopping = true;
        if (worker.joinable()) worker.join();
    }
    ~Publisher() { stop(); }
private:
    void run() {
        using namespace std::chrono_literals;
        // Protection initializes after addins. Never publish the demo quotas.
        for (unsigned attempt = 1; attempt <= 44; ++attempt) {
            std::this_thread::sleep_for(250ms);
            if (stopping) return;
            int error = -1;
            int64_t remaining = 0;
            GetProtectInfo(&error, &remaining);
            auto number = GetFeatureParameter(fpLicNumber);
            char json[256];
            int length;
            if (error == 0 && number > 0) {
                length = std::snprintf(json, sizeof(json),
                    "{\"activated\":true,\"lic_number\":%u,\"points\":%u,\"clients\":%u,\"instances\":%u,\"time_left_ms\":%lld}\n",
                    unsigned(number), unsigned(GetFeatureParameter(fpPLCConnectionsLimit)),
                    unsigned(GetFeatureParameter(fpSessionsLimit)), unsigned(GetFeatureParameter(fpInstancesLimit)),
                    static_cast<long long>(remaining));
            } else {
                if (attempt < 40) continue;
                length = std::snprintf(json, sizeof(json), "{\"activated\":false,\"key_error\":%d}\n", error);
            }
            if (length > 0 && size_t(length) < sizeof(json) && publish(json, size_t(length))) return;
        }
        std::fputs("mplc_cyntron: cannot publish runtime license\n", stderr);
    }
    std::mutex mutex;
    std::atomic<bool> stopping{false};
    std::thread worker;
};
Publisher& publisher() {
    static Publisher instance;
    return instance;
}
}
void startLicensePublisher() { publisher().start(); }
void stopLicensePublisher() { publisher().stop(); }
}
