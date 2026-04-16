#pragma once
#include <mplc/libs/string_view.hpp>
#include <mplc/libs/optional.hpp>

namespace mplc {

    template<typename T, typename R = T>
    class StringSwitch {
        /// The string we are matching.
        const lib::string_view Str;

        /// The pointer to the result of this switch statement, once known,
        /// null before that.
        lib::optional<T> Result;

    public:
        explicit StringSwitch(lib::string_view S): Str(S), Result() {}

        // StringSwitch is not copyable.
        StringSwitch(const StringSwitch&) = delete;

        // StringSwitch is not assignable due to 'Str' being 'const'.
        void operator=(const StringSwitch&) = delete;
        void operator=(StringSwitch&& other) = delete;

        StringSwitch(StringSwitch&& other) noexcept: Str(other.Str), Result(std::move(other.Result)) {}

        ~StringSwitch() = default;

        // Case-sensitive case matchers
        StringSwitch& Case(lib::string_view S, T Value) {
            if (!Result && Str == S) {
                Result = std::move(Value);
            }
            return *this;
        }

        StringSwitch& Cases(lib::string_view S0, lib::string_view S1, T Value) {
            return Case(S0, Value).Case(S1, Value);
        }

        StringSwitch& Cases(lib::string_view S0, lib::string_view S1, lib::string_view S2, T Value) {
            return Case(S0, Value).Cases(S1, S2, Value);
        }

        StringSwitch& Cases(lib::string_view S0,
                            lib::string_view S1,
                            lib::string_view S2,
                            lib::string_view S3,
                            T Value) {
            return Case(S0, Value).Cases(S1, S2, S3, Value);
        }

        StringSwitch& Cases(lib::string_view S0,
                            lib::string_view S1,
                            lib::string_view S2,
                            lib::string_view S3,
                            lib::string_view S4,
                            T Value) {
            return Case(S0, Value).Cases(S1, S2, S3, S4, Value);
        }

        StringSwitch& Cases(lib::string_view S0,
                            lib::string_view S1,
                            lib::string_view S2,
                            lib::string_view S3,
                            lib::string_view S4,
                            lib::string_view S5,
                            T Value) {
            return Case(S0, Value).Cases(S1, S2, S3, S4, S5, Value);
        }

        StringSwitch& Cases(lib::string_view S0,
                            lib::string_view S1,
                            lib::string_view S2,
                            lib::string_view S3,
                            lib::string_view S4,
                            lib::string_view S5,
                            lib::string_view S6,
                            T Value) {
            return Case(S0, Value).Cases(S1, S2, S3, S4, S5, S6, Value);
        }

        StringSwitch& Cases(lib::string_view S0,
                            lib::string_view S1,
                            lib::string_view S2,
                            lib::string_view S3,
                            lib::string_view S4,
                            lib::string_view S5,
                            lib::string_view S6,
                            lib::string_view S7,
                            T Value) {
            return Case(S0, Value).Cases(S1, S2, S3, S4, S5, S6, S7, Value);
        }

        StringSwitch& Cases(lib::string_view S0,
                            lib::string_view S1,
                            lib::string_view S2,
                            lib::string_view S3,
                            lib::string_view S4,
                            lib::string_view S5,
                            lib::string_view S6,
                            lib::string_view S7,
                            lib::string_view S8,
                            T Value) {
            return Case(S0, Value).Cases(S1, S2, S3, S4, S5, S6, S7, S8, Value);
        }

        StringSwitch& Cases(lib::string_view S0,
                            lib::string_view S1,
                            lib::string_view S2,
                            lib::string_view S3,
                            lib::string_view S4,
                            lib::string_view S5,
                            lib::string_view S6,
                            lib::string_view S7,
                            lib::string_view S8,
                            lib::string_view S9,
                            T Value) {
            return Case(S0, Value).Cases(S1, S2, S3, S4, S5, S6, S7, S8, S9, Value);
        }

        [[nodiscard]] R Default(T Value) {
            if (Result)
                return std::move(*Result);
            return Value;
        }

        [[nodiscard]] operator R() {
            assert(Result && "Fell off the end of a string-switch");
            return std::move(*Result);
        }
    };

}  // namespace mplc
