/*
 * BemaniUX - Copyright (C) 2022-2024 spicyjpeg, NaokiS
 *
 * BemaniUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * BemaniUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * BemaniUX. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#include "log.hpp"
#include "common/services/services.hpp"
#include "common/services/system/iface_system.hpp"
#include "common/util/time.hpp"

namespace Logs
{

    /* Logging framework */
    Logger logger;

    inline void putEndLine()
    {
        // Different consoles handle line endings differently.
#if defined(__APPLE__) || defined(__unix__)
        puts("\r");
#else
        puts("\r\n");
#endif
    }

    void LogBuffer::clear(void)
    {
        for (auto line : _lines)
            line[0] = 0;
    }

    char *LogBuffer::allocateLine(void)
    {
        size_t tail = _tail;
        _tail       = (tail + 1) % MAX_LOG_LINES;

        return _lines[tail];
    }

    void Logger::setLogBuffer(LogBuffer *buffer)
    {
        // CriticalSection sec;

        _buffer = buffer;
    }

    void Logger::log(const char *format, ...)
    {
        // CriticalSection sec;

        va_list ap;

        if (_buffer)
        {
            auto line = _buffer->allocateLine();

            va_start(ap, format);
            vsnprintf(line, MAX_LOG_LINE_LENGTH, format, ap);
            va_end(ap);

            if (_enableSyslog)
            {
                puts(line);
                putEndline();
            }
        }
        else if (_enableSyslog)
        {
            va_start(ap, format);
            vprintf(format, ap);
            va_end(ap);
            putEndline();
        }
    }

    void Logger::logT(const char *type, const char *format, const char *func, const int linenum, ...)
    {
        // CriticalSection sec;

        tm time = {0};
        char str[MAX_LOG_LINE_LENGTH];
        memset(str, 0, MAX_LOG_LINE_LENGTH);

        char t_str[20];
        memset(t_str, 0, sizeof(t_str));

        static System::ISystem *sys = getServiceManager()->getSystem();
        sys->getTime(time);
        Time::getTimeStringMillis(time, sys->millis(), t_str, sizeof(t_str));
        va_list ap;

        snprintf(str, MAX_LOG_LINE_LENGTH, "<%s> %s,%s(%d): %s", t_str, type, func, linenum, format);

        if (_buffer)
        {
            auto line = _buffer->allocateLine();

            va_start(ap, linenum);
            vsnprintf(line, MAX_LOG_LINE_LENGTH, str, ap);
            va_end(ap);

            if (_enableSyslog)
            {
                puts(line);
                putEndline();
            }
        }
        else if (_enableSyslog)
        {
            va_start(ap, linenum);
            vprintf(str, ap);
            va_end(ap);
            putEndline();
        }
    }

} // namespace Logs
