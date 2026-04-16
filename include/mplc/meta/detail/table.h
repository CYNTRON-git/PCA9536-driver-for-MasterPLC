#pragma once
#include "../meta_config.h"

namespace mplc { namespace meta {
    class Property;
    class Object;
    enum class MetaType : uint32_t { None, Base, Small, Object, Array };
    namespace detail {
        using CallMember = void (*)(const void*, const void*, const void**);
        using GetMember = void (*)(const void*, const void*);
        using SetMember = void (*)(const void*, const void*);
        using GetMemberPtr = void* (*)(const void*);

        using Name = const char* (*)();
        using Create = void (*)(void**);
        using Construct = void (*)(void**);
        using Destroy = void (*)(void**);
        using Destruct = void (*)(void**);
        using Clone = void (*)(void* const*, void**);
        using Move = void (*)(void* const*, void**);
        using Convert = void (*)(void* const*, void**);
        using Serialize = void (*)(const void*, msgpack::object::with_zone&);
        using Deserialize = void (*)(const void*, const msgpack::object&);

        // using SerializeProp = void (*)(const void*, Serialize);
        // using DeserializeProp = void (*)(const void*, Deserialize);
        //  Table with basic functions

        struct TypeTable {
            Name type_name;
            Create create;
            Construct construct;
            Destroy destroy;
            Destruct destruct;
            Clone clone;
            Move move;
            Serialize serialize;
            Deserialize deserialize;
            size_t size;
            MetaType meta_type;
        };

        struct EnumeratorTable {
            lib::string_view name;
            const unsigned int value;
        };

        struct EnumerationTable {
            lib::string_view name;
            const unsigned int count;
            const EnumeratorTable* table;
        };

        struct MethodTable {
            lib::string_view name;
            CallMember invoker;
            const unsigned int argc;
            const TypeTable** types;
        };

        struct PropertyTable {
            lib::string_view name;
            const TypeTable* type;
            GetMemberPtr ptr;
            GetMember reader;
            Serialize serialize;
            SetMember writer;
            Deserialize deserialize;
        };

    }  // namespace detail
}}     // namespace mplc::meta
