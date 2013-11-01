/******************************************************************************
 * wrt_io.hxx                                                            *
 *                                                                            *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>               *
 *                                                                            *
 * This header describes the WRT Logging Subsystem. As of this moment, this   * 
 * object is planned to be a singleton that binds the unused clog iostream to *
 * a standard WRT logging file (based on initial WRT configuration).          *
 *                                                                            *
 ******************************************************************************/

#ifndef LIBWRT_IO_HXX_
#define LIBWRT_IO_HXX_

#include <syslog.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#include <string>
#include <iostream>
#include <streambuf>

namespace wrt {

class Output {
public:
  enum class Verbosity : int {
      kSquelch     = -1,
      kBrief       =  0,
      kDefault     =  1,
      kVerbose     =  2,
      kVeryVerbose =  3,
      kDebug       =  4,
      kDebug1      =  5,
      kDebug2      =  6,
      kDebug3      =  7,  
  };

  static std::string EnumToString(Verbosity v);
};

class Syslog : public std::basic_streambuf<char, std::char_traits<char>> {
public:
  enum class LogLevel : int {
      kSyslogPanic       = LOG_EMERG,
      kSyslogAlert       = LOG_ALERT,
      kSyslogCritical    = LOG_CRIT,
      kSyslogError       = LOG_ERR,
      kSyslogWarning     = LOG_WARNING,
      kSyslogNotice      = LOG_NOTICE,
      kSyslogInfo        = LOG_INFO,
      kSyslogDebug       = LOG_DEBUG,
  };

  explicit Syslog(std::string ident, int facility);

protected:
  int sync();
  int overflow(int c);

private:
  friend std::ostream& operator<< (std::ostream& os,
    const Syslog::LogLevel& l);

  LogLevel    syslog_priority_;
  int         syslog_facility_;
  char        syslog_ident_[50];
  std::string buffer_;
};

class FileLog : public std::streambuf {

};

extern Output::Verbosity OutputLevel;

class VerbosityBuffer : public std::streambuf {
private:
  Output::Verbosity buffer_verbosity_;
  std::string buffer_;
  
public:
  explicit VerbosityBuffer(Output::Verbosity v = Output::Verbosity::kDefault) {
    buffer_verbosity_ = v;
  }
  
  virtual ~VerbosityBuffer() {
    sync();
  }

protected:
  virtual int overflow(int c = EOF) {
    if (c != EOF)
    {
      buffer_ += static_cast<char>(c);
    }

    return c;
  }

  virtual int sync() {
    std::cout << std::flush;
    
    if (buffer_.length())
    {
      if (OutputLevel >= buffer_verbosity_)
      {
        fputs(buffer_.c_str(), stdout);
      }
      
      buffer_.erase();
    }

    return 0;
  }

private:
  friend std::ostream& operator<< (std::ostream& os,
    const Output::Verbosity& v);
};

class WRTout : public std::ostream {
public:
  WRTout() : std::ostream(new VerbosityBuffer()), std::ios(0) {}
  ~WRTout() { delete rdbuf(); }

};

std::ostream& operator<< (std::ostream& os, const Syslog::LogLevel&  l);
std::ostream& operator<< (std::ostream& os, const Output::Verbosity& v);

};

#endif
