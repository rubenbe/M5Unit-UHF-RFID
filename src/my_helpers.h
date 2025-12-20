# pragma once

#include <string>
#include <sstream>
#include <cstdint>

struct MyHelper {
    // Base case for single value
    template <typename T>
    static void append_csv_value(std::string &s, T value) {
        s += std::to_string(value);
    }

    // Specialization for const char* to avoid using std::to_string
    static void append_csv_value(std::string &s, const char* value) {
        s += value;
    }

    // Recursive case for multiple values
    template <typename T, typename... Args>
    static void append_csv_value(std::string &s, T value, Args... args) {
        append_csv_value(s, value);
        s += ',';
        append_csv_value(s, args...);
    }

    // Variadic CSV function
    template <typename... Args>
    static std::string format_csv(Args... args) {
        std::string result;
        append_csv_value(result, args...);
        return result;
    }
};
