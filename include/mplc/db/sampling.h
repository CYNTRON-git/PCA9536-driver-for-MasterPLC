#pragma once
namespace mplc { namespace archive {
    class Sampling {
        PinList& m_data;
        const PinList::iterator it_to;
        PinValue::ptr p_min, p_max, last;
        int64_t next_interval;
        int64_t m_interval_len;
        double v_min, v_max, _new;
        OpcUa_StatusCode m_last_sc{};
        bool to_end;
        friend class Request;
        void erase();

    public:
        Sampling(const int64_t interval_len, PinList& data, PinList::iterator to);
        virtual ~Sampling();
        void push(const PinValue::ptr& pin);
        void flush();
        static void insert(PinList& data,
                           int64_t interval_len,
                           PinVector::const_iterator begin,
                           PinVector::const_iterator end);
        MPLC_DATABASE_API static void sampl(PinVector& data, int64_t interval_len);
    };
}}  // namespace mplc::archive
