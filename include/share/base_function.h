#pragma once
// #include <mplc_stdint.h>
#ifdef _WIN32
#    include <tchar.h>
#endif

#include <opcua.h>
#include <share/config.h>
#include <rapidjson/document.h>
#include <mplc/libs/system.hpp>
#include <mplc/libs/string_view.hpp>
#include <mplc/libs/containers.hpp>
namespace mplc {
    MPLCSHARE_API lib::error_code sys_copy_files(const std::string& source_path,
                                                 const std::string& target_path,
                                                 const std::string& wildcard = "",
                                                 const bool overwrite = true,
                                                 const bool recursive = true,
                                                 int* count_files = nullptr,
                                                 int* count_folders = nullptr) noexcept;

    class File {
        FILE* ptr;

    public:
        MPLCSHARE_API File(const std::string& utf8_path, const char* mode);
        File(): ptr(nullptr) {}
        MPLCSHARE_API int open(const std::string& utf8_path, const char* mode);
        MPLCSHARE_API void close();
        MPLCSHARE_API ~File();

        template<class T>
        size_t read(T* buffer, int size) {
            if (!ptr)
                return 0;
            return fread(buffer, sizeof(T), size, ptr);
        }
        template<class T>
        size_t write(T* buffer, int size) {
            if (!ptr)
                return 0;
            return fwrite(buffer, sizeof(T), size, ptr);
        }
        size_t write(const char* str) {
            if (!ptr)
                return 0;
            return fwrite(str, sizeof(1), strlen(str), ptr);
        }
        size_t write(const std::string& str) {
            if (!ptr)
                return 0;
            return fwrite(str.c_str(), sizeof(1), str.size(), ptr);
        }
        operator bool() const {
            return ptr != nullptr;
        }
    };

    MPLCSHARE_API const std::string& get_cfg_dir();
    MPLCSHARE_API const std::string& get_webdir_utf8();
    MPLCSHARE_API const std::string& get_cfg_dir_utf8();
    MPLCSHARE_API const std::string& get_project_dir_utf8();
    MPLCSHARE_API const std::string& bin_path_utf8();

    MPLCSHARE_API std::string translite_cp1251(const std::string&);
    MPLCSHARE_API std::string cp1251_to_utf8(lib::string_view source);
    MPLCSHARE_API std::string utf8_to_cp1251(const std::string& source);
    MPLCSHARE_API std::string cp866_to_cp1251(const std::string& s);
    MPLCSHARE_API std::string project_name();
    MPLCSHARE_API std::string project_id();

    MPLCSHARE_API double to_double(const char* str, size_t len);
    MPLCSHARE_API int64_t to_int64(const char* str, size_t len, bool* ok = nullptr);
    MPLCSHARE_API bool to_int64(lib::string_view str, int64_t& res);

    inline double to_double(lib::string_view v) {
        return to_double(v.data(), v.size());
    }
    inline int64_t to_int64(lib::string_view v) {
        return to_int64(v.data(), v.size());
    }

    MPLCSHARE_API bool is_abs_path(const std::string& path);
    MPLCSHARE_API OpcUa_StatusCode parse_json_insitu(char* json_str, rapidjson::Document& json);
    MPLCSHARE_API OpcUa_StatusCode parse_json(lib::string_view str, rapidjson::Document& json);
    inline OpcUa_StatusCode parse_json(const char* json_str, rapidjson::Document& json) {
        if (!json_str) {
            return OpcUa_BadSyntaxError;
        }
        return parse_json(lib::string_view(json_str), json);
    }
    MPLCSHARE_API OpcUa_StatusCode file_size(TCHAR* path, size_t& size);
    MPLCSHARE_API TCHAR* tchar_from_utf8(const std::string& utf8_str);
    MPLCSHARE_API OpcUa_StatusCode mkdir(const std::string& path_utf8);
    MPLCSHARE_API void print_json(const rapidjson::Value& json, const std::string& name);
    MPLCSHARE_API OpcUa_StatusCode read_file(const std::string& utf8_path, char** buf, int& size, int64_t& error_code);
    MPLCSHARE_API FILE* file_open(const std::string& utf8_path, const char* mode);
    MPLCSHARE_API OpcUa_StatusCode write_file(const std::string& utf8_path,
                                              const char* content,
                                              size_t size,
                                              int64_t& error_code);
    MPLCSHARE_API bool f_exists(const std::string& utf8_path);
    MPLCSHARE_API int f_remove(const std::string& utf8_path);
    MPLCSHARE_API int f_rename(const std::string& utf8_path_old, const std::string& utf8_path_new);
    MPLCSHARE_API char* to_string(int32_t i32, char* first, char* end);
    MPLCSHARE_API char* to_string(uint32_t u32, char* first, char* end);
    MPLCSHARE_API char* to_string(int64_t i64, char* first, char* end);
    MPLCSHARE_API char* to_string(uint64_t u64, char* first, char* end);
    MPLCSHARE_API char* to_string(double v, char* first, char* end);
    MPLCSHARE_API std::string md5_string(lib::string_view str);
    MPLCSHARE_API void hex_dump(const void* data, size_t len);
    MPLCSHARE_API void hex_format(std::ostream& os, const void* ptr, size_t len);
    MPLCSHARE_API std::string hex_string(const void* ptr, size_t len);
    MPLCSHARE_API uint64_t hex_to_uint64(lib::string_view str_hex, bool* ok = nullptr);

    MPLCSHARE_API std::string to_string(uint32_t v);
    MPLCSHARE_API std::string to_string(int32_t v);
    MPLCSHARE_API std::string to_string(uint64_t v);
    MPLCSHARE_API std::string to_string(int64_t v);
    MPLCSHARE_API std::string to_string(double v);
    MPLCSHARE_API std::string to_string(float v);

    template<class T, class ContainerT>
    void tokenize(ContainerT& tokens, const T& str, const T& delimiters = " ", bool trim_empty = false) {
        typedef typename ContainerT::value_type value_type;
        typedef typename ContainerT::size_type size_type;
        typename T::size_type last_pos = 0;
        while (last_pos < str.size() + 1) {
            typename T::size_type pos = str.find_first_of(delimiters, last_pos);
            if (pos == T::npos)
                pos = str.size();

            if (pos != last_pos)
                tokens.insert(tokens.end(), value_type(str.substr(last_pos, pos - last_pos)));
            else if (!trim_empty)
                tokens.insert(tokens.end(), T());

            last_pos = pos + 1;
        }
    }
    template<class T, class D, class ContainerT>
    void split(ContainerT& tokens, const T& str, const D& delimiters = " ", bool trim_empty = false) {
        tokenize<T, ContainerT>(tokens, str, T(delimiters), trim_empty);
    }
    inline void print_json(const rapidjson::Value& json) {
        print_json(json, "json");
    }

    MPLCSHARE_API void to_base64(const void* data, size_t data_size, size_t b64_size, char* out);
    MPLCSHARE_API void from_base64(const char* b64, size_t b64_size, size_t data_size, void* out);
    MPLCSHARE_API void fix_byte_order(const uint8_t* data, size_t data_size, uint8_t* out);

    template<class T>
    T fix_byte_order(T& data) {
        const uint8_t* ref = reinterpret_cast<const uint8_t*>(&data);
        uint8_t buf[sizeof(T)] = {0};
        fix_byte_order(ref, sizeof(T), buf);
        return *reinterpret_cast<T*>(&buf);
    }

    template<class T>
    std::string to_base64(const T& _data) {
        static const size_t b64_size = 4 * ((sizeof(T) + 2) / 3);
        static const size_t data_size = sizeof(T);
        char out[4 * ((sizeof(T) + 2) / 3)] = {0};
        const uint8_t* ref = reinterpret_cast<const uint8_t*>(&_data);
        uint8_t buf[sizeof(T)] = {0};
        fix_byte_order(ref, data_size, buf);
        to_base64(buf, data_size, b64_size, out);
        return std::string(out, b64_size);
    }
    inline std::string to_base64(const void* data, size_t data_size) {
        const size_t b64_size = 4 * ((data_size + 2) / 3);
        std::string out(b64_size, 0);
        const uint8_t* ref = static_cast<const uint8_t*>(data);
        to_base64(ref, data_size, b64_size, &out[0]);
        return out;
    }
    inline std::string to_base64(lib::string_view _data) {
        return to_base64(_data.data(), _data.size());
    }
    inline std::string to_base64(const std::string& _data) {
        return to_base64(_data.data(), _data.size());
    }
    template<class T>
    T from_base64(const std::string& b64) {
        static const size_t b64_size = 4 * ((sizeof(T) + 2) / 3);
        static const size_t data_size = sizeof(T);
        uint8_t decoded_data[sizeof(T)] = {0};
        if (b64.size() != b64_size)
            OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR, "Error parse base64 %s \n", b64.c_str());  //-V111
        else {
            from_base64(b64.c_str(), b64_size, data_size, decoded_data);
            fix_byte_order(decoded_data, data_size, decoded_data);
        }
        return *reinterpret_cast<T*>(&decoded_data);
    }
    template<class T = std::string>
    class splitter {
    public:
        class iterator {
            void next() {
                if (end == T::npos) {
                    start = end;
                    return;
                }
                end = ref.data.find_first_of(ref.delimiters, start);
                while (ref.trim && end == start) {
                    start = end + 1;
                    end = ref.data.find_first_of(ref.delimiters, start);
                }
                if (start == ref.data.size())
                    start = T::npos;
                if (start == T::npos)
                    return;
                _val = end == T::npos ? ref.data.substr(start) : ref.data.substr(start, end - start);
                start = end + 1;
            }

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using reference = T&;
            bool operator!=(const typename T::const_iterator& it) const {
                return !(*this == it);
            }
            bool operator==(const typename T::const_iterator& it) const {
                return (it == ref.data.end() && start == T::npos) || it == ref.data.begin() + start;
            }
            bool operator!=(const iterator& it) const {
                return !(*this == it);
            }
            bool operator==(const iterator& it) const {
                return &ref.data == &it.ref.data && start == it.start && end == it.end;
            }
            T& operator*() {
                return _val;
            }
            bool hasValue() {
                return start != T::npos;
            }
            T* operator->() {
                return &_val;
            }
            iterator& operator++() {
                next();
                return *this;
            }
            iterator operator++(int) {
                iterator it(*this);
                next();
                return it;
            }
            iterator(const iterator& it): _val(it._val), ref(it.ref), start(it.start), end(it.end) {}

        private:
            friend class splitter;
            iterator(const splitter& data, typename T::size_type start = 0, typename T::size_type end = 0)
                : ref(data), start(start), end(end) {
                next();
            }
            T _val;
            const splitter& ref;
            typename T::size_type start, end;
        };
        const T& value() const {
            return data;
        }
        T substr(const iterator& from) const {
            return (from.start < data.size()) ? data.substr(from.start) : T();
        }
        T substr(const iterator& from, const iterator& to) const {
            if (from.start >= data.size())
                return T();
            if (to.start == T::npos)
                return data.substr(from.start);
            return data.substr(from.start, to.start - from.start);
        }
        iterator begin() const {
            return iterator(*this);
        }
        iterator end() const {
            return iterator(*this, T::npos, T::npos);
        }
        splitter(const char* data, size_t size, const T& delimiters, bool trim = true)
            : data(data, size), delimiters(delimiters), trim(trim) {}
        splitter(const T& data, const T& delimiters, bool trim = true)
            : data(data), delimiters(delimiters), trim(trim) {}
        const T data;
        const T delimiters;  // fix for splitter(std::string, ";")
        bool trim;
    };

    template<class T>
    splitter<T> make_split(const T& str, const T& delim, bool trim = true) {
        return splitter<T>(str, delim, trim);
    }
}  // namespace mplc
