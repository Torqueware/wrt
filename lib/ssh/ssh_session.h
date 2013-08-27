/***********************************************************************
 * ssh_session.hpp                                                     *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is a header file for a library that is a wrapper for the  *
 * library libssh. This library's purpose is to wrap C code for clean  *
 * use as objects in C++                                               *
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

#ifndef LIBSSH_SESSION_HPP_
#define LIBSSH_SESSION_HPP_

/* avoid using deprecated functions */
#define LIBSSH_LEGACY_0_4

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <cstdlib>
#include <iostream>

#include "ssh_exception"

namespace ssh {

class Session {
  friend class Key;
  friend class Channel;

public:
  Session();
  ~Session();
 
  void setOption(enum ssh_options_e type, std::string option);
  void setOption(enum ssh_options_e type, const char *option);
  void setOption(enum ssh_options_e type, long int option); 
  void setOption(enum ssh_options_e type, void *option); 
  void connect();
  int userauthPublickeyAuto();
  int userauthPassword(const char *password);
  int userauthTryPublickey(ssh_key pubkey);
  int userauthPublickey(ssh_key privkey);
  int getAuthList();
  void disconnect();
  const char *getDisconnectMessage();
  const char *getError();
  int getErrorCode();
  socket_t getSocket();
  std::string getIssueBanner();
  int getOpensshVersion();
  int getVersion();
  int isServerKnown();
  void optionsCopy(const Session &source);
  void optionsParseConfig(const char *file);
  void silentDisconnect();
  int writeKnownhost();

private:
  ssh_session c_session;
  ssh_session getCSession();

  /* No copy constructor, no = operator */
  Session(const Session &);
  Session& operator = (const Session &);
}; //class Session

} //namespace ssh

#endif
