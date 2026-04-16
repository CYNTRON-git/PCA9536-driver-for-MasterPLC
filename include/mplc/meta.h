#pragma once

#include <share/config.h>
#include "meta/detail/property.h"
#include "meta/detail/type.h"
#include "meta/detail/type_list.h"
#include "meta/object.h"
#include "meta/property.h"
#include "meta/enumeration.h"
#include "meta/type.h"
#include "meta/macros.h"
#include "meta/array.h"
#include "meta_fwd.h"

//template<class T>
//struct msgpack::Deserializer<T, boost::enable_if_t<mplc::meta::detail::Has<mplc::meta::Object::Holder<T>>::Table::value>> {
//    static void call(const object& src, T& dst) {
//        mplc::meta::Object::from<T>().deserialize(src, &dst);
//    }
//};
//
//template<class T>
//struct msgpack::Serializer<T, boost::enable_if_t<mplc::meta::detail::Has<mplc::meta::Object::Holder<T>>::Table::value>> {
//    static void call(const T& src, object_handle& dst) {
//        mplc::meta::Object::from<T>().serialize(&src, dst);
//    }
//};
