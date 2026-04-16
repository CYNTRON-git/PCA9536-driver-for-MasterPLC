#pragma once

#include "base.h"

namespace mplc { namespace aggregation {

    ///////////////////////////////////////////////////////////////////
    class LastValue : public Base {
    public:
        MPLC_AGGREAGATION_API LastValue();
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };
    ///////////////////////////////////////////////////////////////////
    class LastBoundValue : public Base {
    public:
        MPLC_AGGREAGATION_API LastBoundValue();
        // MPLC_AGGREAGATION_API void finalize(const int64_t interval_id) override;
        MPLC_AGGREAGATION_API int64_t addValue(const Pin::ptr& pin) override;
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };
    ///////////////////////////////////////////////////////////////////
    class FirstValue final : public Base {
    public:
        MPLC_AGGREAGATION_API FirstValue();
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };

    ///////////////////////////////////////////////////////////////////

    // !TODO: Не берёт значение в нулевой точке интервала
    class FirstBoundValue final : public LastBoundValue {
    public:
        MPLC_AGGREAGATION_API FirstBoundValue();
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };
    ///////////////////////////////////////////////////////////////////
    class Average final : public Base {
        AggregateQuality qAggregator;
        OpcUa_BuiltInType type;
        size_t count;
        double sum;

    public:
        MPLC_AGGREAGATION_API Average();
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };

    ///////////////////////////////////////////////////////////////////
    class Count final : public Base {
        AggregateQuality qAggregator;
        int64_t count;
        Pin::ptr empty_pin;

    public:
        MPLC_AGGREAGATION_API Count();
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };

    ///////////////////////////////////////////////////////////////////
    class Sum final : public Base {
        AggregateQuality qAggregator;
        double sum;

    public:
        // MPLC_AGGREAGATION_API void finalize(const int64_t interval_id) override;
        MPLC_AGGREAGATION_API Sum();
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };

    ///////////////////////////////////////////////////////////////////
    class Interpolation : public Base {
        AggregateQuality qAggregator;
        Pin::ptr calc(const Pin::ptr& first, const Pin::ptr& last, int64_t interval);

    public:
        MPLC_AGGREAGATION_API Interpolation();
        MPLC_AGGREAGATION_API static double linear(const Pin::ptr& first, FileTime time, const Pin::ptr& last);

        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };

    ///////////////////////////////////////////////////////////////////
    class CountSwitchingOn : public Base {
        int count;
        bool last_state;
        AggregateQuality qAggregate;

    public:
        MPLC_AGGREAGATION_API CountSwitchingOn();
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };

    ///////////////////////////////////////////////////////////////////
    class ExactValue : public Base {
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };

    ///////////////////////////////////////////////////////////////////
    class Delta final : public Base {
        AggregateQuality qAggregator;

    public:
        MPLC_AGGREAGATION_API Delta();
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };
    ///////////////////////////////////////////////////////////////////
    class DeltaBound final : public LastBoundValue {
        AggregateQuality qAggregator;

    public:
        MPLC_AGGREAGATION_API DeltaBound();
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };
    ///////////////////////////////////////////////////////////////////
    template<class Compare = Min>
    class Comparator final : public Base {
    public:
        Comparator() {}
        void add(const Pin::ptr& pin, int64_t interval) override {
            if (data.empty() || data.back().first < interval) {
                data.push_back(std::make_pair(interval, pin));
                return;
            }
            std::pair<int64_t, Pin::ptr>& tmp = data.back();
            if (tmp.first == interval) {
                tmp.second = Compare::get(pin, tmp.second);
            }
        }
        Pin::ptr get(int64_t interval) override {
            if (data.empty())
                return Pin::ptr();
            size_t next = out;
            while (++next < data.size() && data[next].first <= interval)
                ++out;
            return data[out].first == interval ? data[out].second : Pin::ptr();
        }
    };

    ///////////////////////////////////////////////////////////////////
    class Sempling final : public Base {
        Pin::ptr first, min, max;
        void flush();

    public:
        MPLC_AGGREAGATION_API void finalize(const int64_t interval = 0) override;
        MPLC_AGGREAGATION_API Sempling();
        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };

    ///////////////////////////////////////////////////////////////////
    class Integral : public Base {
    protected:
        struct Section {
            Section(int64_t from, int64_t to, std::pair<int64_t, Pin::ptr> pos): from(from), to(to), pair(pos) {}
            FileTime from, to;
            std::pair<int64_t, Pin::ptr> pair;
        };
        static double tropecia(double y0, double y1, double x0, double x1) {
            return ((y0 + y1) / 2 * (x1 - x0)) / 1000;  // result in seconds
        }
        static double leftRectangle(double y0, double y1, double x0, double x1) {
            return ((x1 - x0) * y0) / 1000;  // result in seconds
        }
        double integral(double y0, double y1, double x0, double x1) {
            return type == SubType::LeftRectangle ? leftRectangle(y0, y1, x0, x1) : tropecia(y0, y1, x0, x1);
        }
        AggregateQuality qAggregator;
        FileTime lastTime;
        Pin::ptr lastPeeked;
        IntegralArea lastArea;
        // double area;
        std::vector<Section> lostSections;

        void loadLostSections(const Pin::ptr& pin);

        void updateArea(const Pin::ptr& pin, bool peek_pin, bool peek_area);

        void updatePin(const std::pair<int64_t, Pin::ptr>& pair, const double add_area) const;

    public:
        SubType type;

        Integral(SubType type): type(type) {
            // area = 0;
            lastArea = IntegralArea::Nothing;
            lastTime = 0;
        }

        MPLC_AGGREAGATION_API void add(const Pin::ptr& pin, int64_t interval) override;

        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };

    class IntegralAverage final : public Integral {
    public:
        IntegralAverage(SubType type): Integral(type) {}
        MPLC_AGGREAGATION_API Pin::ptr get(int64_t interaval) override;
    };
}}  // namespace mplc::aggregation
