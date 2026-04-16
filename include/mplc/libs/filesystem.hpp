#pragma once
#include <share/base_macros.h>
#include <boost/filesystem.hpp>

namespace mplc { namespace lib { namespace fs {
    using namespace boost::filesystem;
#if BOOST_VERSION > 108000
    inline auto copy_directory(const fs::path& from, const fs::path& to, boost::system::error_code& ec) {
        return fs::copy(from, to, fs::copy_options::directories_only, ec);
    }

    struct copy_option {
        static constexpr auto overwrite_if_exists = fs::copy_options::overwrite_existing;
        static constexpr auto fail_if_exists = fs::copy_options::none;
    };

#endif

}}}  // namespace mplc::lib::fs

#pragma comment(lib, "boost_filesystem.lib")
