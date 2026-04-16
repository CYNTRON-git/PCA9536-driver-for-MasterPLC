#pragma once
#include <boost/preprocessor.hpp>
#include "addins_share.h"

#define ADD_HTTP_METHODS_1(n, base, Method) {BOOST_PP_STRINGIZE(Method), Method},

//#define ADD_HTTP_METHODS(Class, ...)                                                                                   \
//    struct Class {                                                                                                     \
//        enum Name { __VA_ARGS__ };                                                                                     \
//        static const std::map<std::string, Name>& names() {                                                            \
//            static std::map<std::string, Name> _map;                                                                   \
//            if(_map.empty()) { BOOST_PP_SEQ_FOR_EACH(ADD_HTTP_METHODS_1, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) }   \
//            return _map;                                                                                               \
//        }                                                                                                              \
//        static Name get(const std::string& method) {                                                                   \
//            const std::map<std::string, Name>& tmp = names();                                                          \
//            const std::map<std::string, Name>::const_iterator it = tmp.find(method);                                   \
//            if(it == tmp.end()) return static_cast<Name>(0);                                                           \
//            return it->second;                                                                                         \
//        }                                                                                                              \
//        static bool has(const std::string& method) {                                                                   \
//            const std::map<std::string, Name>& tmp = names();                                                          \
//            const std::map<std::string, Name>::const_iterator it = tmp.find(method);                                   \
//            return it != tmp.end();                                                                                    \
//        }                                                                                                              \
//    };                                                                                                                 \
//    bool hasMethod(const std::string& method) override { return Class::has(method); }

#define ADD_HTTP_METHODS(Class, ...)                                                                                   \
    struct Class {                                                                                                     \
        enum Name { __VA_ARGS__ };                                                                                     \
        static const Url* urls() {                                                                                     \
            static const Url _map[] = {                                                                                \
                BOOST_PP_SEQ_FOR_EACH(ADD_HTTP_METHODS_1, _, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)){nullptr, -1}};     \
            return _map;                                                                                               \
        }                                                                                                              \
    };

#define GETLOGINDATA_REQUEST_STR "GetLoginData"
#define GETSTATE_REQUEST_STR "GetState"

// template<class T>
class CRequestProcessor {
public:
    struct Url {
        const char* path;
        int id;
    };

protected:
    CCriticalSection _sec;
    const Url* methods;
    MPLCSHARE_API CRequestProcessor(const char* name, const Url* urls);
    MPLCSHARE_API virtual ~CRequestProcessor();
    virtual OpcUa_StatusCode CallMethod(int method,
                                        const std::string& source,
                                        const Document& request,
                                        ResponseWriter& writer,
                                        int nMaxResponseSize) = 0;
    MPLCSHARE_API static const char* GetErrorDescription(int code);

private:
    friend class RequestProcessorManager;
};
