/***********************************************************************
 * keys.cxx                                                            *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is an object designed to abstract libssh key management   *
 * (which is C code) into a CPP wrapper class.                         *
 *                                                                     *
 **********************************************************************/

#include "keys.hpp"

namespace ssh {
  class Key {
    public:
      Key() {
        c_key = ssh_key_new();
      }

      ~Key() {
        ssh_key_clean(c_key);
        c_key = nullptr;
      }

    protected:
      ssh_key c_key;

      /* No copy constructor -> Override the = operator. */
      Key(const Key &);
      Key& operator = (const Key &);
  };

  class PrivateKey : public Key {
    public:

      /* TODO: ERROR CHECKING */
      void Import(Session session,
       std::string file, std::string passphrase) {
        ssh_key_import_private(c_key, session.getCSession(),
         file.c_str(), passphrase.c_str());
      }
    
      ssh_key getKey() {
        return(c_key);
      }
    }
  };

} //namespace ssh
