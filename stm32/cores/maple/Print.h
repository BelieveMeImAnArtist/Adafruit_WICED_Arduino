/*
 * Print.h - Base class that provides print() and println()
 * Copyright (c) 2008 David A. Mellis.  All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 * Modified 12 April 2011 by Marti Bolivar <mbolivar@leaflabs.com>
 */

#ifndef _WIRISH_PRINT_H_
#define _WIRISH_PRINT_H_

#include <libmaple/libmaple_types.h>
#include "WString.h"
#include "Printable.h"

enum {
    BYTE = 0,
    BIN  = 2,
    OCT  = 8,
    DEC  = 10,
    HEX  = 16
};

class Print {
public:
  virtual size_t write(uint8_t ch) = 0;
  virtual size_t write(const uint8_t *buf, size_t len);

  size_t write(const char *buffer, size_t size)
  {
    return write((const uint8_t *)buffer, size);
  }

  size_t write(const char *str)
  {
    if (str == NULL) return 0;
    return write((const uint8_t *)str, strlen(str));
  }

  size_t print(const String &);
  size_t print(char);
  size_t print(const char[]);
  size_t print(uint8_t, int=DEC);
  size_t print(int, int=DEC);
  size_t print(unsigned int, int=DEC);
  size_t print(long, int=DEC);
  size_t print(unsigned long, int=DEC);
  size_t print(long long, int=DEC);
  size_t print(unsigned long long, int=DEC);
  size_t print(double, int=2);
  size_t print(const Printable&);
  size_t println(const Printable&);
  size_t println(void);
  size_t println(const String &s);
  size_t println(char);
  size_t println(const char[]);
  size_t println(uint8_t, int=DEC);
  size_t println(int, int=DEC);
  size_t println(unsigned int, int=DEC);
  size_t println(long, int=DEC);
  size_t println(unsigned long, int=DEC);
  size_t println(long long, int=DEC);
  size_t println(unsigned long long, int=DEC);
  size_t println(double, int=2);

  size_t printf(const char * format, ...);

  size_t printBuffer(uint8_t const[], int, char='-');
  size_t printBuffer(char const buffer[], int size, char delim='-')
  {
    return printBuffer((uint8_t const*) buffer, size, delim);
  }

  Print() : write_error(0) {}

  int getWriteError() { return write_error; }
  void clearWriteError() { setWriteError(0); }

protected:
  void setWriteError(int err = 1) { write_error = err; }

private:
  int write_error;
  size_t printNumber(unsigned long long, uint8_t);
  size_t printFloat(double, uint8_t);
};

#endif
