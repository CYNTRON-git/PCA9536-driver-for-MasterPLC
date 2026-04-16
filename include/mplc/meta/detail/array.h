#pragma once
#include "type.h"
#include "table.h"
#include "utility.h"
#include "type_list.h"

namespace mplc { namespace meta { namespace adapter {
    template<class T, class = void>
    struct Array;

    template<class T>
    struct Array<T, boost::enable_if_t<detail::is_array_t<T>::value>> {
        using type = T;
        using value_type = typename T::value_type;
        static const void* get(const void* obj, size_t pos) {
            if (!obj)
                return nullptr;
            auto& arr = *static_cast<T*>(const_cast<void*>(obj));
            if (arr.size() > pos) {
                return &arr[pos];
            }
            return nullptr;
        }
        static size_t size(const void* obj) {
            if (!obj)
                return 0;
            auto& arr = *static_cast<const T*>(obj);
            return arr.size();
        }
    };
}}}  // namespace mplc::meta::adapter
namespace mplc { namespace meta { namespace detail {
    // struct PropertyTable;

    template<typename, typename = void>
    struct has_array_adapter : boost::false_type {};
    template<typename T>
    struct has_array_adapter<T, boost::void_t<typename adapter::Array<T>::type>> : boost::true_type {};
    using GetArrayElem = const void* (*)(const void* /*src*/, size_t /*pos*/);
    using GetArraySize = size_t (*)(const void* /*src*/);
    struct ArrayTable : TypeTable {
        const TypeTable* value_type;
        GetArrayElem at;
        GetArraySize size;
        ArrayTable(Name type_name,
                   Create create,
                   Construct construct,
                   Destroy destroy,
                   Destruct destruct,
                   Clone clone,
                   Move move,
                   Serialize serialize,
                   Deserialize deserialize,
                   unsigned size,
                   const TypeTable* value_type,
                   GetArrayElem array_at,
                   GetArraySize array_size)
            : TypeTable{type_name,
                        create,
                        construct,
                        destroy,
                        destruct,
                        clone,
                        move,
                        serialize,
                        deserialize,
                        size,
                        MetaType::Array},
              value_type(value_type), at(array_at), size(array_size) {}
    };

    template<class T>
    struct TypeInfo<T, boost::enable_if_t<has_array_adapter<T>::value>> {
        using Storage = typename Type<T>::Storage;
        // using Small = typename Type<T>::Small;
        using _TypeFuncs = TypeFuncs<Storage>;
        using ArrayAdapter = adapter::Array<Storage>;
        static const ArrayTable& table() {
            static ArrayTable tbl{
                _TypeFuncs::name,
                CreateFunc<Storage>::call,
                ConstructFunc<Storage>::call,
                _TypeFuncs::destroy,
                _TypeFuncs::destruct,
                CloneFunc<Storage>::call,
                MoveFunc<Storage>::call,
                SerializeFunc<Storage>::call,
                DeserializeFunc<Storage>::call,
                sizeof(Storage),
                &TypeInfo<typename ArrayAdapter::value_type>::table(),
                ArrayAdapter::get,
                ArrayAdapter::size,
            };
            return tbl;
        }
    };

}}}  // namespace mplc::meta::detail
