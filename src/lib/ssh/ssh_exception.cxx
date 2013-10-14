/***********************************************************************
 * ssh_exception.hxx                                                   *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This header descibes the SSH library wrapper exceptions, as well as * 
 * related methods used as output functions
 *
 **********************************************************************/

#include <ssh_exception.hxx>

//Unrolls exceptions - WARNING: RECURSIVE
void print_exception(const std::exception& exception, int depth) {
  std::cerr << std::string(depth, ' ')
            << "exception: "
            << exception.what()
            << std::endl;
  
  try {
    std::rethrow_if_nested(exception);
  } catch(const std::exception& e) {
    print_exception(e, depth+1);
  } catch(...) {}

  return;
}
