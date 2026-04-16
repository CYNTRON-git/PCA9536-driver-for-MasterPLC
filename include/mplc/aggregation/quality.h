#pragma once
#include <share/mplcshare.h>

namespace mplc { namespace aggregation {
    struct Quality {
        Quality(): status_code(OpcUa_Good), m_enum(Good) {}
        enum Type { Good = 0, Stop = 1, Bad = 2, Uncertain = 3 };

        static Type FromStatusCode(OpcUa_StatusCode status_code) {
            return OpcUa_IsBad(status_code)               ? Bad :
                   OpcUa_IsUncertain(status_code)         ? Uncertain :
                   status_code == OpcUa_GoodShutdownEvent ? Stop :
                                                            Good;
        }
        static Type FromInt(uint32_t id) {
            return id > Uncertain ? Uncertain : static_cast<Type>(id);
        }
        static OpcUa_StatusCode FromType(const Type t) {
            switch (t) {
            case Good:
                return OpcUa_Good;
            case Stop:
                return OpcUa_GoodShutdownEvent;
            case Bad:
                return OpcUa_Bad;
            case Uncertain:
                return OpcUa_Uncertain;
            default:
                return OpcUa_Good;
            }
        }
        std::string getName() const {
            switch (m_enum) {
            case Good:
                return std::string("Good");
            case Stop:
                return std::string("Stop");
            case Bad:
                return std::string("Bad");
            case Uncertain:
                return std::string("Uncertain");
            default:
                return std::string("Good");
            }
        }
        Quality(const Quality& copy) {
            *this = copy;
        }
        explicit Quality(const OpcUa_StatusCode sc) {
            m_enum = FromStatusCode(sc);
            status_code = sc;
        }
        explicit Quality(const Type _enum) {
            m_enum = _enum;
            status_code = FromType(_enum);
        }
        // operator Type() const { return m_enum; }
        Type type() const {
            return m_enum;
        }
        OpcUa_StatusCode sc() const {
            return status_code;
        }
        // operator OpcUa_StatusCode() const { return status_code; }

        Quality& operator=(const OpcUa_StatusCode sc) {
            m_enum = FromStatusCode(sc);
            status_code = sc;
            return *this;
        }

        Quality& operator=(const Quality& copy) {
            m_enum = copy.m_enum;
            status_code = copy.status_code;
            return *this;
        }
        Quality& operator=(const Type& t) {
            m_enum = t;
            status_code = FromType(t);
            return *this;
        }

        friend const Value& operator>>(const Value& json, Quality& v) {
            v = (OpcUa_StatusCode)json.GetUint();
            return json;
        }

    private:
        OpcUa_StatusCode status_code;
        Type m_enum;
    };

    struct AdditionalQuality {
        enum Type { Cero = 0, Uno = 1, Dos = 2, Tres = 3 };
    };

    struct IntegralArea {
        enum Area {
            Nothing = AdditionalQuality::Cero,
            After = AdditionalQuality::Uno,
            Before = AdditionalQuality::Dos,
            All = AdditionalQuality::Tres
        };
        AddBaseEnumFunctions(IntegralArea, Area, Nothing)
    };

    class AggregateQuality {
        int32_t m_quality[4];

    public:
        AggregateQuality() {
            clean();
        }
        void clean() {
            m_quality[Quality::Bad] = 0;
            m_quality[Quality::Good] = 0;
            m_quality[Quality::Stop] = 0;
            m_quality[Quality::Uncertain] = 0;
        }
        AggregateQuality& operator<<(const Quality& quality) {
            m_quality[quality.type()]++;
            return *this;
        }
        Quality operator()() const {
            return Quality(m_quality[Quality::Bad]       ? Quality::Bad :
                           m_quality[Quality::Uncertain] ? Quality::Uncertain :
                           m_quality[Quality::Stop]      ? Quality::Stop :
                                                           Quality::Good);
        }
    };
}}  // namespace mplc::aggregation
