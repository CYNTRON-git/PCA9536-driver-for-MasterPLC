#pragma once

// The following ifdef block is the standard way of creating macros which make
// exporting from a DLL simpler. All files within this DLL are compiled with the
// MPLCSHARE_EXPORTS symbol defined on the command line. This symbol should not
// be defined on any project that uses this DLL. This way any other project
// whose source files include this file see MPLCSHARE_API functions as being
// imported from a DLL, whereas this DLL sees symbols defined with this macro as
// being exported.

#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <mplc/vm/vminfo.h>
#include "share/config.h"
#include "addins_share.h"
#include "addincmn.h"
#include "share/request_processor.h"
#include "share/task_manager.h"
#include "share/addin_base.h"
#include "share/devices.h"
#include "share/string_conv.h"
#include "lua/lua_util.h"
#include "base_macros.h"
#include "base_function.h"
#include "share/jsoninfo.h"
#include "share/projinfo.h"
#include "template_string.h"
#include "filetime.h"

namespace mplc {
    struct OperatorSession;

    template<class _First, class _Second, class _Third>
    struct triple {
        _First first;
        _Second second;
        _Third third;
        triple(_First first, _Second second, _Third third): first(first), second(second), third(third) {}
    };
    template<class _First, class _Second, class _Third>
    bool operator<(const triple<_First, _Second, _Third>& lhs, const triple<_First, _Second, _Third>& rhs) {
        return (lhs.first < rhs.first || (!(rhs.first < lhs.first) && lhs.second < rhs.second) ||
                (!(rhs.first < lhs.first) && !(rhs.second < lhs.second) && lhs.third < rhs.third));
    }
    template<class _First, class _Second, class _Third>
    triple<_First, _Second, _Third> make_triple(_First first, _Second second, _Third third) {
        return triple<_First, _Second, _Third>(first, second, third);
    }
    struct CGIJsonRequest {
        rapidjson::Document json_data;
        lib::shared_ptr<OperatorSession> session;
        lib::string_view method;
        lib::string_view login;
        lib::string_view host;
    };
}  // namespace mplc
