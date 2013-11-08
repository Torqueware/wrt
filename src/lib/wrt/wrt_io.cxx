/******************************************************************************
 * wrt_io.cxx                                                             *
 *                                                                            *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>               *
 *                                                                            *
 * This header describes the WRT Logging Subsystem. As of this moment, this   * 
 * object is planned to be initialized and bound to the unused clog iostream. *
 * A primitive XML stream will be written to the standard WRT log file.       *
 *                                                                            *
 ******************************************************************************/

#include <wrt_io.hxx>

namespace wrt {

Output::Verbosity OutputLevel(Output::Verbosity::kDefault);

std::string Output::EnumToString(Output::Verbosity v) {
  switch(v) {
    case Verbosity::kSquelch:
      return std::string("Squelch");
    case Verbosity::kBrief:
      return std::string("Brief");
    case Verbosity::kDefault:
      return std::string("Default");
    case Verbosity::kVerbose:
      return std::string("Verbose");
    case Verbosity::kVeryVerbose:
      return std::string("Very Verbose");
    case Verbosity::kDebug:
      return std::string("Debug");
    case Verbosity::kDebug1:
      return std::string("Debug1");
    case Verbosity::kDebug2:
      return std::string("Debug2");
    case Verbosity::kDebug3:
      return std::string("Debug3");
    default:
      if (static_cast<int>(v) < static_cast<int>(Verbosity::kSquelch))
      {
        return std::string("...");
      } else {
        return std::string("Absurd");
      }
  }
  
  return std::string("Default case! IMPOSSIBLE!");
}

Syslog::Syslog(std::string ident, int facility) {
  syslog_facility_ = facility;
  syslog_priority_ = Syslog::LogLevel::kSyslogInfo;


  strncpy(syslog_ident_, ident.c_str(), sizeof(syslog_ident_));
  syslog_ident_[sizeof(syslog_ident_) - 1] = '\0';

  openlog(syslog_ident_, LOG_PID, syslog_facility_);
}

int Syslog::sync() {
  if (buffer_.length()) {
    syslog((int)syslog_priority_, buffer_.c_str());
    buffer_.erase();

    // default to debug for each message
    syslog_priority_ = LogLevel::kSyslogInfo;
  }
  
  return 0;
}

int Syslog::overflow(int c = EOF) {
  if (c != EOF) {
    buffer_ += static_cast<char>(c);
  } else {
    sync();
  }
    
  return c;
}

std::ostream& operator<< (std::ostream& os, const Syslog::LogLevel& l) {
  static_cast<Syslog *>(os.rdbuf())->syslog_priority_ = l;

  return os;
}

std::ostream& operator<< (std::ostream& os, const Output::Verbosity& v) {
  static_cast<VerbosityBuffer *>(os.rdbuf())->buffer_verbosity_ = v;

  return os;
}

Output::Verbosity operator-- (Output::Verbosity& v, int) {
  Output::Verbosity old = v;
  const int i = (int) v;
  v = (Output::Verbosity)(i - 1);

  return old;
}

Output::Verbosity operator++ (Output::Verbosity& v, int) {
  Output::Verbosity old = v;
  const int i = (int) v;
  v = (Output::Verbosity)(i + 1);

  return old;
}

}
