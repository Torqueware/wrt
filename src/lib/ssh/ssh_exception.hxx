/***********************************************************************
 * ssh_session.hxx                                                     *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This header describes a SSH Exception object. This object can be    * 
 * thrown by several SSH functions that interact with the network,     *
 * and may fail because of socket, protocol or memory errors.          *
 *                                                                     *
 * This file was created out of dissatisfaction (upon looking into the *
 * C++ wrapper from libssh). I feel that optimization that compromises *
 * readability is silly in the presense of no profiling.               *
 *                                                                     *
 * Therefore: I rewrote the header wrapper, and omitted functionality  *
 * I felt was unnecessary - thus I don't believe this code is covered  *
 * under the GPL.                                                      *
 *                                                                     *
 * HOWEVER: I AM NOT AN EXPERT ON THE GPL!!!                           *
 *                                                                     *
 * I am not adverse to licensing my code under the GPL, but would like *
 * to retain the freedom to choose my licensing later. If this is not  *
 * possible, so be it (I will happily correct the issue). Otherwise,   *
 * I am naming this file my own!                                       *
 *                                                                     *
 **********************************************************************/

#ifndef LIBSSH_EXCEPTION_HPP_
#define LIBSSH_EXCEPTION_HPP_

/* Avoid depracated features */
#define LIBSSH_LEGACY_0_4_

#include <libssh/libssh.h>
#include <libssh/server.h>

#include <iostream>
#include <exception>
#include <stdexcept>

namespace ssh {

class Session;
//class SFTPSession;
//class SCPSession;

class SshException : public std::runtime_error {
public:
  SshException(const char* msg)
    : std::runtime_error(msg) {}
  SshException(std::string &msg)
    : std::runtime_error(msg) {}
  
  SshException(ssh_session c_session)
    : std::runtime_error(ssh_get_error(c_session)) {
        
    _code        = ssh_get_error_code(c_session);
    _description = std::string(ssh_get_error(c_session));
  }

//SshException(ssh_scp c_session)
//: std::runtime_error(ssh_get_error_code( ... )) {};
//SshException(sftp_session c_session)
//: std::runtime_error(ssh_get_error_code( ... )) {};
  
  SshException(const SshException &e)
    : std::runtime_error(e.what()) {}

  int getCode();
  std::string getError();

private:
  int _code;
  std::string _description;

};

}

void print_exception(const std::exception& exception, int depth = 0);

#endif
