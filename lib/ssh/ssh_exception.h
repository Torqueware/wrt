/***********************************************************************
 * ssh_session.hpp                                                     *
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

namespace ssh {

Class SshException {

public:
  SshException(ssh_session csession);
  SshException(const SshException &e);
  int getCode();
  std::string getError();

private:
  int code;
  std::string description;

};
}
#endif
