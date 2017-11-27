#ifndef LOGGING_H
#define LOGGING_H

#include <inttypes.h>
#include <stdarg.h>
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define LOG_LEVEL_NOOUTPUT 0
#define LOG_LEVEL_ERRORS 1
#define LOG_LEVEL_WARNINGS 2
#define LOG_LEVEL_INFOS 3
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_VERBOSE 5

// default loglevel if nothing is set from user
#define LOGLEVEL LOG_LEVEL_DEBUG


#define CR "\r\n"
#define LOGGING_VERSION 1

typedef void (*char_printer_fn_t)(const char c);
class LogPrinter : public Print {
  char_printer_fn_t _printer;
public:
  LogPrinter(char_printer_fn_t printer) {
    _printer = printer;
  }
  virtual size_t write(uint8_t c) {
    _printer(c);
    return 1;
  }
};

typedef void (*string_printer_fn_t)(const char *s);
class LogBufferedPrinter : public Print {
  string_printer_fn_t _printer;
	char *_buffer;
	size_t _size;
	size_t _index;
public:
  LogBufferedPrinter(string_printer_fn_t printer, char *buffer, size_t size) {
		// assert(printer!=NULL);
		// assert(buffer!=NULL);
		// assert(size>1);
    _printer = printer;
		_buffer = buffer;
		_size = size;
  }
  virtual size_t write(uint8_t c) {
		_buffer[_index++] = c;
		if (c=='\n' || (_index+1)==_size) {
			_buffer[_index] = 0;
			_printer(_buffer);
			_index = 0;
		}
    return 1;
  }
};


/*!
* Logging is a helper class to output informations over
* RS232. If you know log4j or log4net, this logging class
* is more or less similar ;-) <br>
* Different loglevels can be used to extend or reduce output
* All methods are able to handle any number of output parameters.
* All methods print out a formated string (like printf).<br>
* To reduce output and program size, reduce loglevel.
* <br>
* Output format string can contain below wildcards. Every wildcard
* must be start with percent sign (\%)
*
* <b>Depending on loglevel, source code is excluded from compile !</b><br>
* <br>
* <b>Wildcards</b><br>
* <ul>
* <li><b>\%s</b>	replace with an string (char*)</li>
* <li><b>\%c</b>	replace with an character</li>
* <li><b>\%d</b>	replace with an integer value</li>
* <li><b>\%l</b>	replace with an long value</li>
* <li><b>\%x</b>	replace and convert integer value into hex</li>
* <li><b>\%X</b>	like %x but combine with <b>0x</b>123AB</li>
* <li><b>\%b</b>	replace and convert integer value into binary</li>
* <li><b>\%B</b>	like %x but combine with <b>0b</b>10100011</li>
* <li><b>\%t</b>	replace and convert boolean value into <b>"t"</b> or <b>"f"</b></li>
* <li><b>\%T</b>	like %t but convert into <b>"true"</b> or <b>"false"</b></li>
* <li><b>\%f</b>    replace with a double value</li>
* <li><b>\\%</b>    to escape an % char</li>
* </ul><br>
* <b>Loglevels</b><br>
* <table border="0">
* <tr><td>0</td><td>LOG_LEVEL_NOOUTPUT</td><td>no output </td></tr>
* <tr><td>1</td><td>LOG_LEVEL_ERRORS</td><td>only errors </td></tr>
* <tr><td>2</td><td>LOG_LEVEL_INFOS</td><td>errors and info </td></tr>
* <tr><td>3</td><td>LOG_LEVEL_DEBUG</td><td>errors, info and debug </td></tr>
* <tr><td>4</td><td>LOG_LEVEL_VERBOSE</td><td>all </td></tr>
* </table>
* <br>
* <h1>History</h1><br>
* <table border="0">
* <tr><td>01 FEB 2012</td><td>initial release</td></tr>
* <tr><td>06 MAR 2012</td><td>implement a preinstanciated object (like in Wire, ...)</td></tr>
* <tr><td></td><td>method init get now loglevel and baud parameter</td></tr>
*/
class Logging {
private:
    int _level;
    Print *_printer;
public:
    /*!
	 * default Constructor
	 */
    Logging(){} ;

	/**
	* Initializing, must be called as first.
	* \param level the log level
	* \param printer the function called to print a character, instead of Serial
	*/
	void Init(int level, Print &printer);

/**
	* Output an error message. Output message contains
	* ERROR: followed by original msg
	* Error messages are printed out, at every loglevel
	* except 0 ;-)
	* \param msg format string to output
	* \param ... any number of variables
	* \return void
	*/

	template <class T> void Error(T msg, ...){
		if (LOG_LEVEL_ERRORS <= _level) {
			va_list args;
			va_start(args, msg);
			print(F("ERROR: "),args); // args unused
			print(msg,args);
			va_end(args);
		}
	}


/**
	* Output a warning message. Output message contains
	* WARNING: followed by original msg
	* Warning messages are printed out at l
	* loglevels >= LOG_LEVEL_WARNINGS
	* \param msg format string to output
	* \param ... any number of variables
	* \return void
	*/

	template <class T> void Warn(T msg, ...){
		if (LOG_LEVEL_WARNINGS <= _level) {
			va_list args;
			va_start(args, msg);
			print(F("WARNING: "),args); // args unused
			print(msg,args);
			va_end(args);
		}
	}


    /**
	* Output an info message. Output message contains
	* Info messages are printed out at l
	* loglevels >= LOG_LEVEL_INFOS
	*
	* \param msg format string to output
	* \param ... any number of variables
	* \return void
	*/

  template <class T> void Info(T msg, ...){
    if (LOG_LEVEL_INFOS <= _level) {
      va_list args;
      va_start(args, msg);
      print(msg,args);
      va_end(args);
    }
  }

    /**
	* Output an debug message. Output message contains
	* Debug messages are printed out at l
	* loglevels >= LOG_LEVEL_DEBUG
	*
	* \param msg format string to output
	* \param ... any number of variables
	* \return void
	*/


  template <class T> void Debug(T msg, ...){
    if (LOG_LEVEL_DEBUG <= _level) {
      va_list args;
      va_start(args, msg);
      print(F("debug: "),args); // args unused
      print(msg,args);
      va_end(args);
    }
  }
	// Allows continued debug messages with no "DEBUG:" prefix
	template <class T> void Debug_(T msg, ...){
		if (LOG_LEVEL_DEBUG <= _level) {
      va_list args;
      va_start(args, msg);
      print(msg,args);
      va_end(args);
    }
  }


    /**
	* Output an verbose message. Output message contains
	* Debug messages are printed out at l
	* loglevels >= LOG_LEVEL_VERBOSE
	*
	* \param msg format string to output
	* \param ... any number of variables
	* \return void
	*/

  template <class T> void Verbose(T msg, ...){
    if (LOG_LEVEL_VERBOSE <= _level) {
      va_list args;
      va_start(args, msg);
      print (F("verbose: "),args); // args unused
      print(msg,args);
      va_end(args);
    }
  }


private:
    void print(const char *format, va_list args);
    #if !defined(PLATFORM_NATIVE)
    void print(const __FlashStringHelper *format, va_list args);
    #endif
    void printFormat(const char format, va_list *args);
};

extern Logging Log;
#endif
