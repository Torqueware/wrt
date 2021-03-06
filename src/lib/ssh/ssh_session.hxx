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

#include <ssh_exception.hxx>
#include <ssh_keys.hxx>

namespace ssh {

class Channel;

class Session {
  friend class Key;
  friend class Channel;

public:
  Session();
  ~Session();
 
  void setOption(enum ssh_options_e type, std::string option);
  void setOption(enum ssh_options_e type, const char *option); //TODO DEPRICATE
  void setOption(enum ssh_options_e type, long int option); //TODO DEPRICATE
  void setOption(enum ssh_options_e type, void *option); //TODO DEPRICATE
  
  void optionsCopy(const Session &source);
  void optionsParseConfig(const char *file);
 
  int getAuthList();

  int userauthPublickeyAuto();
  int userauthPassword(const char *password);
  int userauthTryPublickey(ssh_key pubkey);
  int userauthPublickey(ssh_key privkey);
  
  int isServerKnown();
  int getOpensshVersion();
  int getVersion();
  
  std::string getIssueBanner();
  
  const char *getDisconnectMessage();
  const char *getError();
  int getErrorCode();

  socket_t getSocket();

  void connect();
  void disconnect();
  void silentDisconnect();
  
  int writeKnownhost();

private:
  ssh_session c_session_;
  ssh_session getCSession();

  /* No copy constructor, no = operator */
  Session(const Session &);
  Session& operator = (const Session &);
}; //class Session

} //namespace ssh

#endif
