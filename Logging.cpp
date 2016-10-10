#include "Logging.h"

void Logging::Init(int level, long baud){
  _level = constrain(level,LOG_LEVEL_NOOUTPUT,LOG_LEVEL_VERBOSE);
  _baud = baud;
  Serial.begin(_baud);
  _printer = &Serial;
}

void Logging::Init(int level, Print &printer){
  _level = constrain(level,LOG_LEVEL_NOOUTPUT,LOG_LEVEL_VERBOSE);
  _baud = 0;
  _printer = &printer;
}

void Logging::print(const __FlashStringHelper *format, va_list args) {
  PGM_P p = reinterpret_cast<PGM_P>(format);
  char c = pgm_read_byte(p++);
  for(;c != 0; c = pgm_read_byte(p++)){
    if (c == '%') {
      c = pgm_read_byte(p++);
      printFormat(c, &args);
    } else {
      if (c == '\\') {
        c = pgm_read_byte(p++);
      }
      _printer->print(c);
    }
  }
}

void Logging::print(const char *format, va_list args) {
  for (; *format != 0; ++format) {
    if (*format == '%') {
      ++format;
      printFormat(*format, &args);
    } else {
      if (*format == '\\') {
        ++format;
      }
      _printer->print(*format);
    }
  }
}

void Logging::printFormat(const char format, va_list *args) {
  if (format == '\0') return;

  if (format == '%') {
    _printer->print(format);
    return;
  }

  if( format == 's' ) {
    register char *s = (char *)va_arg( *args, int );
    _printer->print(s);
    return;
  }

  if( format == 'd' || format == 'i') {
    _printer->print(va_arg( *args, int ),DEC);
    return;
  }

  if( format == 'x' ) {
    _printer->print(va_arg( *args, int ),HEX);
    return;
  }

  if( format == 'X' ) {
    _printer->print("0x");
    _printer->print(va_arg( *args, int ),HEX);
    return;
  }

  if( format == 'b' ) {
    _printer->print(va_arg( *args, int ),BIN);
    return;
  }

  if( format == 'B' ) {
    _printer->print("0b");
    _printer->print(va_arg( *args, int ),BIN);
    return;
  }

  if( format == 'l' ) {
    _printer->print(va_arg( *args, long ),DEC);
    return;
  }

  if( format == 'c' ) {
    _printer->print(va_arg( *args, int ));
    return;
  }

  if( format == 't' ) {
    if (va_arg( *args, int ) == 1) {
      _printer->print("T");
    }
    else {
      _printer->print("F");
    }
    return;
  }

  if( format == 'T' ) {
    if (va_arg( *args, int ) == 1) {
      _printer->print(F("true"));
    }
    else {
      _printer->print(F("false"));
    }
    return;
  }

  if ( format == 'f' ) {
    register double d = va_arg( *args, double );
    _printer->print(String(d));
  }

}

 Logging Log = Logging();
