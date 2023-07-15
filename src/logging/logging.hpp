// MIT License
//
// Copyright (c) 2022 Joe Roback <joe.roback@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once

#include "../racechrono-canbus.hpp"

#include <freertos/FreeRTOS.h>

#include <cstdio>
#include <utility>

namespace logging
{

/**
 * logging levels
 */
enum class log_level
{
    off,
    error,
    warn,
    info,
    verbose,
    debug,
};

/**
 * logger class
 */
class logger final
{
    CPP_NOCOPY(logger);
    CPP_NOMOVE(logger);

public:
    static logger& get() noexcept;

    ~logger() noexcept;

    /**
     * get current log level
     */
    log_level level() const noexcept { return _log_level; }

    /**
     * set current log level
     */
    void set_level(log_level level) noexcept { _log_level = level; }

    /**
     * log message using printf-style formatting
     */
    template <typename ...TArgs>
    void log(log_level level, const char* fmt, TArgs&& ...args) noexcept
    {
        if (level <= _log_level)
        {
            char buf[128];
            size_t len = snprintf(buf, sizeof(buf), fmt, std::forward<TArgs>(args)...);
            portENTER_CRITICAL(&_lock);
            Serial.write(buf, len);
            Serial.flush();  // when writing to Serial on different cores, flush() seems required
            portEXIT_CRITICAL(&_lock);
        }
    }

    /**
     * log message using printf-style formatting and append newline
     */
    template <typename ...TArgs>
    void logln(log_level level, const char* fmt, TArgs&& ...args) noexcept
    {
        if (level <= _log_level)
        {
            char buf[128];
            size_t len = snprintf(buf, sizeof(buf), fmt, std::forward<TArgs>(args)...);
            portENTER_CRITICAL(&_lock);
            Serial.write(buf, len);
            Serial.println();
            Serial.flush();  // when writing to Serial on different cores, flush() seems required
            portEXIT_CRITICAL(&_lock);
        }
    }

private:
    explicit logger() noexcept;

private:
    log_level _log_level;
    portMUX_TYPE _lock;
};

} // namespace logging

template <typename ...TArgs>
void error(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().log(logging::log_level::error, fmt, std::forward<TArgs>(args)...);
}

template <typename ...TArgs>
void errorln(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().logln(logging::log_level::error, fmt, std::forward<TArgs>(args)...);
}

template <typename ...TArgs>
void warn(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().log(logging::log_level::warn, fmt, std::forward<TArgs>(args)...);
}

template <typename ...TArgs>
void warnln(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().logln(logging::log_level::warn, fmt, std::forward<TArgs>(args)...);
}

#if defined(DEBUG)

template <typename ...TArgs>
void info(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().log(logging::log_level::info, fmt, std::forward<TArgs>(args)...);
}

template <typename ...TArgs>
void infoln(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().logln(logging::log_level::info, fmt, std::forward<TArgs>(args)...);
}

template <typename ...TArgs>
void verbose(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().log(logging::log_level::verbose, fmt, std::forward<TArgs>(args)...);
}

template <typename ...TArgs>
void verboseln(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().logln(logging::log_level::verbose, fmt, std::forward<TArgs>(args)...);
}

template <typename ...TArgs>
void debug(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().log(logging::log_level::debug, fmt, std::forward<TArgs>(args)...);
}

template <typename ...TArgs>
void debugln(const char* fmt, TArgs&& ...args) noexcept
{
    logging::logger::get().logln(logging::log_level::debug, fmt, std::forward<TArgs>(args)...);
}

#else

#define info(_fmt, ...)
#define infoln(_fmt, ...)
#define verbose(_fmt, ...)
#define verboseln(_fmt, ...)
#define debug(_fmt, ...)
#define debugln(_fmt, ...)

#endif
