#pragma once

#include <boost/function.hpp>
#include <boost/bind/bind.hpp>

namespace mplc { namespace lib {

    namespace placeholders = boost::placeholders;

    using boost::is_placeholder;
    // using boost::is_
    using boost::bind;
    using boost::function;

}}  // namespace mplc::lib
using namespace boost::placeholders;
