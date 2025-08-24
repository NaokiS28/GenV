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
#include "common/util/misc.hpp"
#include "common/util/time.hpp"
#include "common/services/system/system.hpp"

namespace Logs
{

	/* Logging framework */

	Logger logger;

	void LogBuffer::clear(void)
	{
		for (auto line : _lines)
			line[0] = 0;
	}

	char *LogBuffer::allocateLine(void)
	{
		size_t tail = _tail;
		_tail = (tail + 1) % MAX_LOG_LINES;

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
				puts("\r\n");
			}
		}
		else if (_enableSyslog)
		{
			va_start(ap, format);
			vprintf(format, ap);
			va_end(ap);
			puts("\r\n");
		}
	}

	void Logger::logT(const char *type, const char *format, const char *func, const int linenum, ...)
	{
		// CriticalSection sec;

		tm time;
		char *str = new char[MAX_LOG_LINE_LENGTH];
		if(!str)
			return;
		memset(str, 0, MAX_LOG_LINE_LENGTH);

		char *t_str = new char[16];
		if(!t_str)
			return;
		memset(t_str, 0, 16);

		System::getTime(time);
		Time::getTimeString(time, t_str);
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
				puts("\r\n");
			}
		}
		else if (_enableSyslog)
		{
			va_start(ap, linenum);
			vprintf(str, ap);
			va_end(ap);
			puts("\r\n");
		}
	}

}
