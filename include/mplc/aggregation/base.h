#pragma once
#include <boost/container/list.hpp>
#include "pin.h"
#include "filter.h"
#include "mplc/time_span.h"

namespace mplc { namespace aggregation {
    OpcUa_VariantHlp operator/(const OpcUa_VariantHlp& left, const OpcUa_VariantHlp& right);
    OpcUa_VariantHlp operator-(const OpcUa_VariantHlp& left, const OpcUa_VariantHlp& right);
    OpcUa_VariantHlp operator+(const OpcUa_VariantHlp& left, const OpcUa_VariantHlp& right);
    OpcUa_VariantHlp abs(const OpcUa_VariantHlp& value);
    bool operator>(const OpcUa_VariantHlp& left, const OpcUa_VariantHlp& right);
    inline bool operator<(const OpcUa_VariantHlp& left, const OpcUa_VariantHlp& right) {
        return !(left > right);
    }
    enum class UsePrevIntervalValue { Default, Yes, No };
    struct SampleInterval {
        FileTime t_start, t_end;
        int64_t deadband{};
        int64_t len;
        int64_t delta;
        uint8_t delta_month{};
        enum FormingMethod { OnChange = 0, Periodic = 1, Total = 2 } method;
        enum IntervalUnit {
            Raw = -1,
            Second = 0,
            Minute = 1,
            Hour = 2,
            Day = 3,
            Month = 4,
        } unit;
        SampleInterval(): len(1), delta(0), method(OnChange), unit(Raw) {}

        MPLC_AGGREAGATION_API OpcUa_StatusCode init(int formingMethod,
                                                    int intervalUnit,
                                                    FileTime startTime,
                                                    FileTime endTime,
                                                    double resampleInterval,
                                                    int64_t deadband_ft = 0);

        MPLC_AGGREAGATION_API FileTime ft_len(int64_t interval = 1) const;

        MPLC_AGGREAGATION_API int64_t get(FileTime time) const;
        MPLC_AGGREAGATION_API FileTime start(int64_t interval) const;

        FileTime end(int64_t interval) const {
            return start(interval + 1);
        }

        struct iterator {
            int64_t interval;

            iterator(int64_t interval_): interval(interval_) {}
            bool operator!=(const iterator& other) const {
                return other.interval != interval;
            }
            iterator& operator++() {
                ++interval;
                return *this;
            }
            int64_t operator*() const {
                return interval;
            }
        };

        MPLC_AGGREAGATION_API iterator begin() const;
        MPLC_AGGREAGATION_API iterator end() const;
    };

    struct Interface {
        typedef std::vector<std::pair<int64_t, Pin::ptr>> PinStorage;
        typedef PinStorage::iterator iterator;

        virtual void setUsePrevIntervalValue(int val) = 0;
        virtual void setInterval(const SampleInterval& interval) = 0;
        virtual void setFilter(uint32_t quality_filter) = 0;
        virtual int64_t addValue(const Pin::ptr& pin) = 0;
        virtual Pin::ptr getValue(int64_t interval_id) = 0;
        // virtual void setFinish(Pin::ptr &pin) = 0;
        virtual void finalize(const int64_t interval_id = 0) = 0;

        virtual Pin::ptr boundLeft() = 0;
        virtual Pin::ptr boundRight() = 0;
        virtual iterator begin() = 0;
        virtual iterator end() = 0;
        virtual size_t size() = 0;
        virtual ~Interface() {}
    };

    class Base : public Interface {
    public:
    protected:
        SampleInterval m_interval;
        PinStorage data;
        // PinList begin;
        // PinList *current;
        Filter filter;
        size_t out;
        // PinList *out;
        Pin::ptr last;
        Pin::ptr bound_left, bound_right;
        int64_t last_interval;
        bool use_prev_interval_val;
        virtual void add(const Pin::ptr& pin, int64_t interval) = 0;

        // int64_t interval_len;
        // int64_t delta;
        // int64_t getStartTime(int64_t interval) const { return interval_len * interval + delta; }
        // int64_t getEndTime(int64_t interval) const { return interval_len * (interval + 1) + delta; }
        virtual Pin::ptr get(int64_t interval) = 0;
        bool belongs_to(int64_t pin_interval, int64_t interval) const;

    public:
        void setUsePrevIntervalValue(int val) override {
            switch ((UsePrevIntervalValue)val) {
            case UsePrevIntervalValue::Yes:
            case UsePrevIntervalValue::Default:
                use_prev_interval_val = true;
                break;
            case UsePrevIntervalValue::No:
            default:
                use_prev_interval_val = false;
                break;
            }
        }
        Pin::ptr boundLeft() override {
            return bound_left;
        }
        Pin::ptr boundRight() override {
            return bound_right;
        }
        size_t size() override {
            return data.size();
        }
        iterator begin() override {
            return data.begin();
        }
        iterator end() override {
            return data.end();
        }
        MPLC_AGGREAGATION_API Pin::ptr getValue(int64_t interval) override;
        MPLC_AGGREAGATION_API Base();
        // MPLC_AGGREAGATION_API virtual void setFinish(Pin::ptr &pin) override;
        // MPLC_AGGREAGATION_API virtual void setStart(Pin::ptr &pin, int64_t) override;
        MPLC_AGGREAGATION_API void finalize(const int64_t final_interval = 0) override;
        MPLC_AGGREAGATION_API void setInterval(const SampleInterval& interval) override;
        MPLC_AGGREAGATION_API void setFilter(uint32_t qualityFilter) override;
        MPLC_AGGREAGATION_API int64_t addValue(const Pin::ptr& pin) override;
        // int64_t toInterval(FileTime ft) const { return (ft.dt() - delta) / interval_len; }
        virtual ~Base();
    };

    struct Min {
        static Pin::ptr get(const Pin::ptr& left, const Pin::ptr& right) {
            return (left->value > right->value) ? right : left;
        }
    };

    struct Max {
        static Pin::ptr get(const Pin::ptr& left, const Pin::ptr& right) {
            return (left->value < right->value) ? right : left;
        }
    };

    struct LeftRectangle {
        static double integral(double y0, double y1, double x0, double x1) {
            return ((x1 - x0) * y0) / 1000;
        }  // result in seconds
    };

    struct Tropecia {
        static double integral(double y0, double y1, double x0, double x1) {
            return ((y0 + y1) / 2 * (x1 - x0)) / 1000;  // result in seconds
        }
    };

    // inline double get_value(const Pin::ptr& v) {
    //    double val;
    //    v->value.GetDouble(val);
    //    return val;
    //}
    // inline double get_time(const Pin::ptr& v) {
    //    return v->time;
    //}
    // inline double interpolation(const Pin::ptr& first, int64_t time, const Pin::ptr& last) {
    //    double y0 = get_value(first);
    //    double y1 = get_value(last);
    //    return y0 + (time - first->time) * (y1 - y0) / (last->time - first->time);
    //}
    ////template<class Method = Tropecia>
    // struct Out {
    //    Out& operator << (const Pin::ptr& pin) {
    //        return *this;
    //    }
    //    Out& operator << (double val) {
    //        return *this;
    //    }
    //};

    // struct Integral1 {
    // protected:
    //    Filter filter;
    //    //Out& out;
    //    Pin::ptr last;
    //    std::vector<std::pair<int64_t, int64_t>> times;
    //    Pin::ptr last_rp;
    //    double res;
    // public:
    //    //Integral1(Out& out) : out(out), res(0) {}
    //    Integral1() : res(0) {}
    //    void calc(const Pin::ptr& pin) {
    //        for(size_t i =0; i < times.size(); ++i) {
    //            double y0 = interpolation(last_rp, times[i].first, pin);
    //            double y1 = interpolation(last_rp, times[i].second, pin);
    //            res += Tropecia::integral(y0, y1, times[i].first, times[i].second);
    //        }
    //        times.clear();
    //    }

    //    Integral1& operator << (const Pin::ptr& pin) {
    //        int l_opt = filter.getOptions(last->quality);
    //        int r_opt = filter.getOptions(pin->quality);
    //        bool use_interval = l_opt & IntegralArea::After && r_opt & IntegralArea::Before;
    //        if(filter.isIgnore(pin->quality)) {
    //            if(use_interval) {
    //                times.push_back(std::make_pair(last->time, pin->time));
    //            }
    //        } else {
    //            if(!times.empty()) {
    //                calc(pin);
    //                if(use_interval) {
    //                    double y0 = interpolation(last_rp, get_time(last), pin);
    //                    res += Tropecia::integral(y0, get_value(pin), get_time(last), get_time(pin));
    //                }
    //            } else {
    //                res += Tropecia::integral(get_value(last), get_value(pin), get_time(last),get_time(pin));
    //            }
    //            last_rp = pin;
    //        }
    //        last = pin;
    //        return *this;
    //    }
    //    Out& operator>>(Out& out) {
    //        out << res;
    //    }
    //};

    // struct Part {
    //    Filter filter;
    //    int64_t interval;
    //    int64_t current_interval; Integral1 proc;
    //    Part& operator << (const Pin::ptr& pin) {
    //        if(pin->time / interval == current_interval) {
    //            proc << pin;
    //        }
    //    }
    //};
    // void test() {
    //    Integral1 calc;
    //    Pin::ptr pin;
    //    calc << pin << pin;
    //    Out o;
    //    calc >> o;

    //}
}}  // namespace mplc::aggregation
