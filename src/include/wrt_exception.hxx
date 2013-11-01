/******************************************************************************
 * ssh_session.hxx                                                            *
 *                                                                            *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>               *
 *                                                                            *
 * This header describes a WRT Exception object. This object can be           * 
 * thrown by several WRT functions that interact with the network,            *
 * and may fail because of socket, protocol or memory errors.                 *
 *                                                                            *
 ******************************************************************************/

#ifndef LIBWRT_EXCEPTION_HPP_
#define LIBWRT_EXCEPTION_HPP_

#include <iostream>
#include <exception>
#include <stdexcept>

namespace wrt {

/**
 * 
 */
const int kIndentWidth = 2;

/**
 * 
 */
class WRTException : public std::runtime_error {
public:
  WRTException(const char* message)
    : std::runtime_error(message) {}
  WRTException(std::string &message)
    : std::runtime_error(message) {}

  int getCode();
  std::string getError();

private:
  int code_;
  std::string thrown_by_;
  std::string description_;

};

/**
 * [PrintException description]
 *
 * @method  PrintException
 *
 * @param   exception       [description]
 * @param   depth           [description]
 */
void PrintException(const std::exception& exception, int depth = 0) {
    std::cerr << std::string(kIndentWidth * depth, ' ')
              << "Exception: "
              << exception.what()
              << std::endl;
  
  try {
    std::rethrow_if_nested(exception);
  } catch(const std::exception& e) {
    PrintException(e, depth+1);
  } catch(...) {}

  return;
}

//DEPRICATE
void print_exception(const std::exception& exception, int depth = 0) {
  PrintException(exception, depth);
}

}

#endif
