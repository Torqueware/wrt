/***********************************************************************
 * keys.cxx                                                            *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is an object designed to abstract libssh key management   *
 * (which is C code) into a CPP wrapper class.                         *
 *                                                                     *
 **********************************************************************/

#include <ssh_keys.hxx>

namespace ssh {
Key::Key(Session &session) {
  ssh_get_publickey(session.c_session_, &c_key);
  c_hash_length = ssh_get_pubkey_hash(session.c_session_, &c_hash);
}

Key::Key(ssh_session c_session) {
  ssh_get_publickey(c_session, &c_key);
  ssh_get_pubkey_hash(c_session, &c_hash);    
}

Key::~Key() {
  ssh_key_free(c_key);
  c_key = nullptr;

  free(c_hash);
  c_hash = nullptr;
}

std::string Key::getHash() {
  std::string CPPhexa = nullptr;

  if (c_hash) {
    const char *hexa = ssh_get_hexa(c_hash, c_hash_length);
    CPPhexa = std::string(hexa);
    free((void *)hexa);
  }

  return CPPhexa;
}

} //namespace ssh
