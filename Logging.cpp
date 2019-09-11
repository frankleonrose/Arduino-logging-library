#include "Logging.h"

void Logging::Init(int level, Print &printer){
  _level = constrain(level,LOG_LEVEL_NOOUTPUT,LOG_LEVEL_VERBOSE);
  _printer = &printer;
}

#if !defined(PLATFORM_NATIVE)
void Logging::print(const __FlashStringHelper *format, va_list args) {
  PGM_P p = reinterpret_cast<PGM_P>(format);
  char c = pgm_read_byte(p++);
  for(;c != 0; c = pgm_read_byte(p++)){
    if (c == '%') {
      c = pgm_read_byte(p++);
      int len = 0;
      if (c == '*') {
        len = va_arg( args, int );
        c = pgm_read_byte(p++);
      }
      else if (c == 'l') {
        len = -1;
        c = pgm_read_byte(p++);
      }
      else {
        while (c >= '0' && c <= '9') {
          len = 10 * len + (c - '0');
          c = pgm_read_byte(p++);
        }
      }
      printFormat(c, &args, len);
    } else {
      if (c == '\\') {
        c = pgm_read_byte(p++);
      }
      _printer->print(c);
    }
  }
}
#endif

void Logging::print(const char *format, va_list args_in) {
  #if defined(PLATFORM_NATIVE)
  va_list args;
  va_copy(args, args_in);
  #else
  va_list &args = args_in;
  #endif
  for (; *format != 0; ++format) {
    if (*format == '%') {
      ++format;
      int len = 0;
      if (*format == '*') {
        len = va_arg( args, int );
        ++format;
      }
      else if (*format == 'l') {
        len = -1;
        ++format;
      }
      else {
        while (*format >= '0' && *format <= '9') {
          len = 10 * len + (*format - '0');
          ++format;
        }
      }
      printFormat(*format, &args, len);
    } else {
      if (*format == '\\') {
        ++format;
      }
      _printer->print(*format);
    }
  }
  #if defined(PLATFORM_NATIVE)
  va_end(args);
  #endif
}

void Logging::printFormat(const char format, va_list *args, const int len) {
  if (format == '\0') return;

  if (format == '%') {
    _printer->print(format);
    return;
  }

  if( format == 's' ) {
    char *s = (char *)va_arg( *args, char * );
    if (s!=NULL) {
      // Pad string if necessary.
      const int slen = strlen(s);
      if (slen<len) {
        for (int i=0; i<(len-slen); ++i) {
          _printer->print(" ");
        }
      }
    }
    _printer->print(s);
    return;
  }

  if( format == 'd' || format == 'i') {
    if (len==-1) {
      _printer->print(va_arg( *args, long int ),DEC);
    }
    else {
      _printer->print(va_arg( *args, int ),DEC);
    }
    return;
  }

  if( format == 'u' || format == 'i') {
    if (len==-1) {
      _printer->print(va_arg( *args, unsigned long int ),DEC);
    }
    else {
      _printer->print(va_arg( *args, unsigned int ),DEC);
    }
    return;
  }

  if( format == 'm' || format == 'M' ) {
    // Print a hex string of len bytes
    uint8_t *p = va_arg( *args, uint8_t * );
    if( format == 'M' ) {
      // Prefix hex bytes with address
      _printer->print((unsigned int)((size_t)p), HEX);
      _printer->print(": ");
    }
    for (int i = 0; i<len; ++i) {
      if (p[i]<16) {
        _printer->print('0');
      }
      _printer->print(p[i], HEX);
    }
    return;
  }

  if( format == 'x' ) {
    if (len==-1) {
      _printer->print(va_arg( *args, long int ),HEX);
    }
    else {
      _printer->print(va_arg( *args, int ),HEX);
    }
    return;
  }

  if( format == 'X' ) {
    _printer->print("0x");
    if (len==-1) {
      _printer->print(va_arg( *args, long int ),HEX);
    }
    else {
      _printer->print(va_arg( *args, int ),HEX);
    }
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

  if( format == 'c' ) {
    _printer->print(va_arg( *args, int ));
    return;
  }

  if( format == 'p' ) {
    _printer->print((unsigned long)va_arg( *args, void * ),HEX);
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
    double d = va_arg( *args, double );
  #if defined(PLATFORM_NATIVE)
    _printer->print(std::to_string(d).c_str());
  #else
    _printer->print(String(d));
  #endif
  }

}

 Logging Log = Logging();
