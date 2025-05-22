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
#include "common/util/log.hpp"
#include "common/util/misc.hpp"

namespace util {

/* Logging framework */

Logger logger;

void LogBuffer::clear(void) {
	for (auto line : _lines)
		line[0] = 0;
}

char *LogBuffer::allocateLine(void) {
	size_t tail = _tail;
	_tail       = (tail + 1) % MAX_LOG_LINES;

	return _lines[tail];
}

void Logger::setLogBuffer(LogBuffer *buffer) {
	CriticalSection sec;

	_buffer = buffer;
}

void Logger::log(const char *format, ...) {
	CriticalSection sec;

	va_list ap;

	if (_buffer) {
		auto line = _buffer->allocateLine();

		va_start(ap, format);
		vsnprintf(line, MAX_LOG_LINE_LENGTH, format, ap);
		va_end(ap);

		if (_enableSyslog)
			puts(line);
	} else if (_enableSyslog) {
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);

		putchar('\n');
	}
}

}
