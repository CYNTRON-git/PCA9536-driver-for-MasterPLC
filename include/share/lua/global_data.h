#pragma once
#include <share/scada_types.h>
#include <mplc/libs/threads.hpp>
#include <msgpack/object_fwd.hpp>
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/container/flat_map.hpp>

namespace mplc { namespace glob_data {
    struct IData {
        static constexpr const char* lua_name = "LuaDataRef";
        bool m_lock;
        enum Type { none, object, array, _double, _integer, _boolean, datetime, string } type;
        IData(Type type): m_lock(false), type(type) {}
        virtual void lock() {
            m_lock = true;
        }
        virtual void unlock() {
            m_lock = false;
        }
        virtual void set(lua_State* L, bool deepCopy = true) const = 0;
        virtual void get(lua_State* L) = 0;
        virtual size_t size() const {
            return 0;
        }
        typedef IData* ptr;
        MPLCSHARE_API static IData* New(lua_State* L);
        virtual OpcUa_StatusCode to_json(JsonWrapper& json) const = 0;
        virtual OpcUa_StatusCode to_variant(OpcUa_VariantHlp& val) const = 0;
        virtual OpcUa_StatusCode from_variant(const OpcUa_VariantHlp& val) {
            return OpcUa_Bad;
        }
        virtual IData::ptr operator[](lib::string_view child) const = 0;
        // enum Type { Null, Array, Object, Time, Json } type;
        // IData(): type(Null) {}
        virtual ~IData() {}
        virtual void to_msgpack(msgpack::object::with_zone& o) = 0;
    };

    /// <summary>
    /// Тип данных для хранения глобальных переменных, в частностиглобальных параметров.
    ///  В lua метатаблице LuaDataRef хранятся ссылки (адреса?) нп объекты типа TableData
    /// </summary>
    struct TableData : IData {
        struct VarValues {
            virtual ~VarValues() = default;
            virtual int setI(lua_State* L) = 0;
            virtual int getI(lua_State* L) = 0;
        };

        virtual void clone(lua_State* L) const = 0;
        virtual void setI(lua_State* L) const = 0;
        virtual void getI(lua_State* L) = 0;
        MPLCSHARE_API virtual void getLenI(lua_State* L);
        static const char* name() {
            return "LuaDataRef";
        }

        static int GC(lua_State* L) {
            return 1;
        }

        // MPLCSHARE_API static TableData* GetThis(lua_State* L);
        MPLCSHARE_API static int RegInLua(lua_State* L);
        MPLCSHARE_API static int _clone(lua_State* L);
        MPLCSHARE_API static int newindex(lua_State* L);
        MPLCSHARE_API static int index(lua_State* L);
        MPLCSHARE_API static int getLen(lua_State* L);
        MPLCSHARE_API void set(lua_State* L, bool deepCopy = true) const override;
        MPLCSHARE_API OpcUa_StatusCode to_variant(OpcUa_VariantHlp& val) const override;
        MPLCSHARE_API virtual OpcUa_StatusCode to_data_value(OpcUa_VariantHlp& val) const;
        ~TableData() override {}
        TableData(Type type): IData(type) {}
        mutable lib::unordered_flat_map<lua_State*, int> ref;
        mutable lib::mutex _mtx;
        static int _lua_set(lua_State* L, const TableData* obj) {
            auto ud = lua_newuserdata(L, sizeof(obj));
            luaL_getmetatable(L, lua_name);
            lua_setmetatable(L, -2);
            memcpy(ud, &obj, sizeof(obj));
            return 1;
        }

    protected:
        static TableData* _this(lua_State* L) {
            auto* ptr = lua_touserdata(L, 1);
            return *static_cast<TableData**>(ptr);
        }
    };

    struct SimpleData final : IData {
        MPLCSHARE_API void set(lua_State* L, bool deepCopy = true) const override;
        MPLCSHARE_API void get(lua_State* L) override;

        MPLCSHARE_API OpcUa_StatusCode to_json(JsonWrapper& json) const override;
        MPLCSHARE_API OpcUa_StatusCode to_variant(OpcUa_VariantHlp& val) const override;
        MPLCSHARE_API OpcUa_StatusCode from_variant(const OpcUa_VariantHlp& val) override;
        MPLCSHARE_API void to_msgpack(msgpack::object::with_zone& o) override;
        SimpleData(): IData(_double) {}

        void SetBool(bool v) {
            data.b = v;
            type = _boolean;
        }
        void SetInt64(int64_t v) {
            data.i64 = v;
            type = _integer;
        }
        void SetDouble(double v) {
            data.f64 = v;
            type = _double;
        }
        bool GetBool() const {
            switch (type) {
            case _double:
                return data.f64;
            case _integer:
                return data.i64;
            case _boolean:
                return data.b;
            default:
                return false;
            }
        }
        int64_t GetInt64() const {
            switch (type) {
            case _double:
                return data.f64;
            case _integer:
                return data.i64;
            case _boolean:
                return data.b;
            default:
                return 0;
            }
        }
        double GetDouble() const {
            switch (type) {
            case _double:
                return data.f64;
            case _integer:
                return data.i64;
            case _boolean:
                return data.b;
            default:
                return 0.0;
            }
        }
        IData::ptr operator[](lib::string_view name) const override {
            return nullptr;
        }
        union Data {
            double f64;
            int64_t i64;
            bool b;
        } data{};
    };
    struct TimeData : IData {
        MPLCSHARE_API void set(lua_State* L, bool deepCopy = true) const override {
            set_lua_value(data, L);
        }
        MPLCSHARE_API void get(lua_State* L) override {
            get_lua_value(data, L);
        }
        MPLCSHARE_API OpcUa_StatusCode to_variant(OpcUa_VariantHlp& val) const override {
            return val.SetTime(data.dt());
        }
        MPLCSHARE_API void to_msgpack(msgpack::object::with_zone& o) override;
        MPLCSHARE_API OpcUa_StatusCode to_json(JsonWrapper& json) const override {
            lib::lock_guard<lib::mutex> lock(_mtx);
            set_json_data(json, data);
            return OpcUa_Good;
        }
        IData::ptr operator[](lib::string_view name) const override {
            return nullptr;
        }
        TimeData(): IData(datetime) {}
        FileTime data;
        mutable lib::mutex _mtx;
    };
    struct StringData : IData {
        StringData(): IData(string) {}
        MPLCSHARE_API void set(lua_State* L, bool deepCopy = true) const override;
        MPLCSHARE_API void get(lua_State* L) override;
        MPLCSHARE_API OpcUa_StatusCode to_variant(OpcUa_VariantHlp& val) const override;

        size_t size() const override {
            lib::lock_guard<lib::mutex> lock(_mtx);
            return data.size();
        }
        MPLCSHARE_API OpcUa_StatusCode to_json(JsonWrapper& json) const override;
        MPLCSHARE_API void to_msgpack(msgpack::object::with_zone& o) override;

        IData::ptr operator[](lib::string_view name) const override {
            return nullptr;
        }
        std::string data;
        mutable lib::mutex _mtx;
    };

    struct ArrData : TableData {
        MPLCSHARE_API void setI(lua_State* L) const override;

        MPLCSHARE_API void getI(lua_State* L) override;
        MPLCSHARE_API void getLenI(lua_State* L) override;

        MPLCSHARE_API void get(lua_State* L) override;

        MPLCSHARE_API void clone(lua_State* L) const override;

        size_t size() const override {
            return m_size;
        }

        MPLCSHARE_API IData::ptr operator[](lib::string_view id) const override;
        MPLCSHARE_API OpcUa_StatusCode to_json(JsonWrapper& json) const override;

        MPLCSHARE_API void to_msgpack(msgpack::object::with_zone& o) override;
        MPLCSHARE_API void lock() override;
        MPLCSHARE_API void unlock() override;

        template<typename T>
        OpcUa_StatusCode to_pure_variant(OpcUa_Variant& val) const {
            lib::lock_guard<lib::mutex> lock(_mtx);
            val.Value.Array.Value.Array = OpcUa_ReAlloc(val.Value.Array.Value.Array, size() * sizeof(T));
            val.Value.Array.Length = size();
            size_t i = 0;
            for (const auto& it: data) {
                if (i >= size()) {
                    break;
                }
                OpcUa_VariantHlp vh;
                it.second->to_variant(vh);
                static_cast<T*>(val.Value.Array.Value.Array)[i++] = vh.Get<T>();
            }
            return OpcUa_Good;
        }

        ArrData(): TableData(array), m_size(0) {}
        ~ArrData() override;
        int m_size;
        std::map<int, ptr> data;
    };

    template<>
    inline OpcUa_StatusCode ArrData::to_pure_variant<OpcUa_DateTime>(OpcUa_Variant& val) const {
        lib::lock_guard<lib::mutex> lock(_mtx);
        val.Value.Array.Value.Array = (OpcUa_DateTime*)OpcUa_ReAlloc(val.Value.Array.Value.Array,
                                                                     size() * sizeof(OpcUa_DateTime));
        val.Value.Array.Length = size();
        size_t i = 0;
        for (const auto& it: data) {
            if (i >= size()) {
                break;
            }
            OpcUa_VariantHlp vh;
            it.second->to_variant(vh);
            vh.GetTime(static_cast<OpcUa_DateTime*>(val.Value.Array.Value.Array)[i]);
            ++i;
        }
        return OpcUa_Good;
    }

    template<>
    inline OpcUa_StatusCode ArrData::to_pure_variant<OpcUa_String>(OpcUa_Variant& val) const {
        lib::lock_guard<lib::mutex> lock(_mtx);
        val.Value.Array.Value.StringArray = (OpcUa_String*)OpcUa_ReAlloc(val.Value.Array.Value.StringArray,
                                                                         size() * sizeof(OpcUa_String));
        val.Value.Array.Length = size();

        auto it = data.begin();
        for (int i = 0; i < m_size && it != data.end(); ++i, ++it) {
            OpcUa_VariantHlp vh;
            it->second->to_variant(vh);
            OpcUa_String_StrCpy((val.Value.Array.Value.StringArray + i), &vh.Value.String);
        }
        return OpcUa_Good;
    }

    template<>
    inline OpcUa_StatusCode ArrData::to_pure_variant<OpcUa_Byte>(OpcUa_Variant& val) const {
        lib::lock_guard<lib::mutex> lock(_mtx);
        if (val.Datatype != OpcUaType_ByteString) {
            OpcUa_Variant_Clear(&val);
            val.Datatype = OpcUaType_ByteString;
            val.Value.ByteString.Data = nullptr;
        }
        val.Value.ByteString.Data = (OpcUa_Byte*)OpcUa_ReAlloc(val.Value.ByteString.Data, size() * sizeof(OpcUa_Byte));
        val.Value.ByteString.Length = size();
        size_t i = 0;
        for (const auto& it: data) {
            if (i >= size()) {
                break;
            }
            OpcUa_VariantHlp vh;
            it.second->to_variant(vh);
            if (OpcUa_IsGood(vh.ChangeType(OpcUaType_Byte)))
                val.Value.ByteString.Data[i] = vh.Value.Byte;
            else
                val.Value.ByteString.Data[i] = 0;

            ++i;
        }
        return OpcUa_Good;
    }

    template<>
    inline OpcUa_StatusCode ArrData::to_pure_variant<bool>(OpcUa_Variant& val) const {
        lib::lock_guard<lib::mutex> lock(_mtx);
        val.Value.Array.Value.BooleanArray = (OpcUa_Boolean*)OpcUa_ReAlloc(val.Value.Array.Value.BooleanArray,
                                                                           size() * sizeof(OpcUa_Boolean));
        val.Value.Array.Length = size();
        auto it = data.begin();
        for (int i = 0; i < m_size && it != data.end(); ++i, ++it) {
            OpcUa_VariantHlp vh;
            it->second->to_variant(vh);
            bool b;
            if (OpcUa_IsGood(vh.GetBool(b)))
                val.Value.Array.Value.BooleanArray[i] = b ? 1 : 0;
            else
                val.Value.Array.Value.BooleanArray[i] = 0;
        }
        return OpcUa_Good;
    }

    struct ObjData : TableData {
    protected:
        using Storage = lib::unordered_flat_map<std::string, ptr, lib::string_hash, lib::string_equal_to>;
        Storage data;

        // std::vector<char*> fields;  // Store string for lib::string_view in data
    public:
        MPLCSHARE_API void setI(lua_State* L) const override;

        MPLCSHARE_API void getI(lua_State* L) override;

        MPLCSHARE_API void get(lua_State* L) override;

        MPLCSHARE_API IData::ptr& get(lib::string_view name);
        MPLCSHARE_API IData::ptr find(lib::string_view name) const;
        MPLCSHARE_API void clone(lua_State* L) const override;

        MPLCSHARE_API OpcUa_StatusCode to_json(JsonWrapper& json) const override;

        MPLCSHARE_API OpcUa_StatusCode to_data_value(OpcUa_VariantHlp& val) const override;

        MPLCSHARE_API void to_msgpack(msgpack::object::with_zone& o) override;
        MPLCSHARE_API void lock() override;
        MPLCSHARE_API IData::ptr operator[](lib::string_view name) const override;
        MPLCSHARE_API void unlock() override;
        MPLCSHARE_API void clear();

        size_t size() const override {
            return data.size();
        }
        ObjData(): TableData(object) {}
        ~ObjData() override {
            clear();
        }
        auto begin() -> decltype(data.begin()) {
            return data.begin();
        }
        auto end() -> decltype(data.end()) {
            return data.end();
        }
    };
    // struct ProxyData;

    struct GlobalDataRef {
        typedef lib::shared_ptr<GlobalDataRef> ptr;
        typedef lib::weak_ptr<GlobalDataRef> weak_ptr;

        void lock() const {
            mtx.lock();
        }
        void unlock() const {
            mtx.unlock();
        }
        IData& operator->() const {
            return *m_ref;
        }
        template<class T>
        T get() const;
        template<class T>
        T safe_get() {
            lib::lock_guard<lib::mutex> lock(mtx);
            return get<T>();
        }
        GlobalDataRef(): m_ref(nullptr) {}
        IData* raw() const {
            return m_ref;
        }

        MPLCSHARE_API ~GlobalDataRef();
        BOOST_DELETED_FUNCTION(GlobalDataRef(const GlobalDataRef&))
        BOOST_DELETED_FUNCTION(GlobalDataRef& operator=(const GlobalDataRef&))
    private:
        IData::ptr m_ref;
        mutable lib::mutex mtx;
        MPLCSHARE_API friend GlobalDataRef::ptr get_shared_data_ref(lib::string_view name);
        MPLCSHARE_API static ptr make(IData::ptr ref);
    };
    MPLCSHARE_API GlobalDataRef::ptr get_shared_data_ref(lib::string_view name);
    template<>
    inline double GlobalDataRef::get<double>() const {
        return dynamic_cast<SimpleData*>(m_ref)->GetDouble();
    }
    template<>
    inline bool GlobalDataRef::get<bool>() const {
        return dynamic_cast<SimpleData*>(m_ref)->GetBool();
    }
    template<>
    inline int64_t GlobalDataRef::get<int64_t>() const {
        return dynamic_cast<SimpleData*>(m_ref)->GetInt64();
    }

    // MPLCSHARE_API GlobalDataRef::ptr get_shared_data_ref(const std::string& name);
    MPLCSHARE_API IData::ptr get_data_ref(lib::string_view name);

    struct GlobalDataFB {
        static constexpr const char* name = "GlobalData";
        static int ConfigProcessorCallback(void* data, ConfigProcessorMode mode, ControllerConfig* config);
        static int RegInLua(lua_State* L);

        static IData::ptr GetParam(lua_State* L);
        static int LockParam(lua_State* L);
        static int UnlockParam(lua_State* L);
        static int TestParam(lua_State* L);
        static int New(lua_State* L);
        static int newindex(lua_State* L);
        static int index(lua_State* L);
        static int unsafe_newindex(lua_State* L);
        static int unsafe_index(lua_State* L);
        static int GC(lua_State* L);
        static GlobalDataFB& instance();

    private:
        struct TLockTable : TableData {
            GlobalDataFB& gd;
            TLockTable(GlobalDataFB& gd): TableData(object), gd(gd) {}
            void setI(lua_State* L) const override {
                lock_guard lock(gd);
                return gd.storage.setI(L);
            }
            void getI(lua_State* L) override;
            void get(lua_State* L) override {
                lock_guard lock(gd);
                gd.storage.get(L);
            }
            OpcUa_StatusCode to_json(JsonWrapper& json) const override {
                lock_guard lock(gd);
                return gd.storage.to_json(json);
            }
            IData::ptr operator[](lib::string_view child) const override {
                lock_guard lock(gd);
                return gd.storage.operator[](child);
            }
            void to_msgpack(msgpack::object::with_zone& o) override {
                lock_guard lock(gd);
                gd.storage.to_msgpack(o);
            }
            void clone(lua_State* L) const override {
                lock_guard lock(gd);
                gd.storage.clone(L);
            }
        };
        // void create(lua_State* L);
        struct lock_guard {
            lib::lock_guard<lib::mutex> lock;
            lock_guard(const GlobalDataFB& obj): lock(obj.mtx) {}
        };

        GlobalDataFB();
        GlobalDataFB(const GlobalDataFB&) = delete;
        GlobalDataFB& operator=(const GlobalDataFB&) = delete;
        GlobalDataFB(GlobalDataFB&&) noexcept = delete;
        GlobalDataFB& operator=(GlobalDataFB&&) noexcept = delete;

        void clear();
        //bool is_locked(lua_State* L, int key_pos);

        friend void add_ref(GlobalDataFB* x);
        friend void release(GlobalDataFB* x);

        MPLCSHARE_API friend GlobalDataRef::ptr get_shared_data_ref(lib::string_view name);
        MPLCSHARE_API friend IData::ptr get_data_ref(lib::string_view name);

        //lib::unordered_map<lua_State*, FBData> data;
        lib::unordered_map<IData::ptr, GlobalDataRef::weak_ptr> shared_data;
        ObjData storage;
        TLockTable* __VarValues;
        mutable lib::mutex mtx;
        mutable lib::atomic<int> refcount_;
    };

    MPLCSHARE_API void gdata_to_msgpack(IData* ref, msgpack::object::with_zone& o);
}}  // namespace mplc::glob_data

namespace mplc {
    inline void set_json_data(JsonWrapper& json, glob_data::IData::ptr val) {
        if (val) {
            val->to_json(json);
        }
    }
}  // namespace mplc
