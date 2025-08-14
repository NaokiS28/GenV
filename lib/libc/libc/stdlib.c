/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * stdlib.c - Created on 12-08-2025
 * 
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef STDLIB_C
#define STDLIB_C
#include "stdlib.h"
#include "stdio.h"

/* Abort functions */

void _assertAbort(const char *file, int line, const char *expr) {
#ifndef NDEBUG
	printf("%s:%d: assert(%s)\n", file, line, expr);
#endif

	for (;;)
		__asm__ volatile("");
}

void abort(void) {
#ifndef NDEBUG
	puts("abort()");
#endif

	for (;;)
		__asm__ volatile("");
}

void __cxa_pure_virtual(void) {
#ifndef NDEBUG
	puts("__cxa_pure_virtual()");
#endif

	for (;;)
		__asm__ volatile("");
}

#endif