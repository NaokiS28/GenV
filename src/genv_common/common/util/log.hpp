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

#pragma once

#include <stddef.h>

namespace util {

/* Logging framework */

static constexpr int MAX_LOG_LINE_LENGTH = 128;
static constexpr int MAX_LOG_LINES       = 64;

class LogBuffer {
private:
	char _lines[MAX_LOG_LINES][MAX_LOG_LINE_LENGTH];
	int  _tail;

public:
	inline LogBuffer(void)
	: _tail(0) {
		clear();
	}

	// 0 = last line, 1 = second to last, etc.
	inline const char *getLine(int line) const {
		return _lines[size_t(_tail - (line + 1)) % MAX_LOG_LINES];
	}

	void clear(void);
	char *allocateLine(void);
};

class Logger {
private:
	LogBuffer *_buffer;
	bool      _enableSyslog;

public:
	inline Logger(void)
	: _buffer(nullptr), _enableSyslog(true) {}

	void setLogBuffer(LogBuffer *buffer);
	void log(const char *format, ...);
};

extern Logger logger;

}

/* Logging macros */

#define LOG(type, fmt, ...) \
	util::logger.log( \
		type ",%s(%d): " fmt, __func__, __LINE__ __VA_OPT__(,) __VA_ARGS__ \
	)

#ifdef ENABLE_APP_LOGGING
#define LOG_APP(fmt, ...) LOG("app", fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_APP(fmt, ...)
#endif

#ifdef ENABLE_FS_LOGGING
#define LOG_FS(fmt, ...) LOG("fs", fmt __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_FS(fmt, ...)
#endif
