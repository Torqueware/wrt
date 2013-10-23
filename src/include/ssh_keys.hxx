/***********************************************************************
 * keys.hpp                                                            *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is the header for the keys lib.                           *
 *                                                                     *
 **********************************************************************/

#ifndef LIBSSH_KEYS_HPP_
#define LIBSSH_KEYS_HPP_

#define LIBSSH_LEGACY_0_4_

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <cstdlib>
#include <iostream>
#include <cstring>

#include <ssh_session.hxx>
#include <ssh_exception.hxx>

namespace ssh {

class Key {

public:
  Key() = delete;
  Key(Session &session);
  Key(ssh_session c_session);
  ~Key();

  std::string getHash();

private:
  ssh_key        c_key;
  unsigned char *c_hash;
  int            c_hash_length;
      
  // Key(const Key &);
  // Key& operator = (const Key &);
};

}

#endif