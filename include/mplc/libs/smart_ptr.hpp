#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_unique.hpp>
#include <boost/smart_ptr/owner_less.hpp>
#include <boost/intrusive_ptr.hpp>
namespace mplc { namespace lib {

    using boost::enable_shared_from_this;
    using boost::make_shared;
    using boost::make_unique;
    using boost::scoped_ptr;
    using boost::static_pointer_cast;
    using std::unique_ptr;

    using boost::owner_less;
    using boost::shared_ptr;
    using boost::weak_ptr;
    using boost::intrusive_ptr;
    template<typename TO, typename FROM>
    unique_ptr<TO> static_pointer_cast(unique_ptr<FROM>&& old) {
        return unique_ptr<TO>{static_cast<TO*>(old.release())};
        // conversion: unique_ptr<FROM>->FROM*->TO*->unique_ptr<TO>
    }
    template<typename T, typename D>
    std::unique_ptr<T, D> make_handle(T* handle, D deleter) {
        return std::unique_ptr<T, D>{handle, deleter};
    }
    using boost::dynamic_pointer_cast;
}}  // namespace mplc::lib
