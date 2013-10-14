/***********************************************************************
 * keys.hpp                                                            *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is the header for the keys lib.                           *
 *                                                                     *
 **********************************************************************/

#ifndef LIBKEYS_HPP_
#define LIBKEYS_HPP_

#include <cstdlib>
#include <iostream>
#include <cstring>

namespace ssh {
class Key {
  public:
    Key();
    ~Key();

  protected:
    ssh_key c_key;
      
    Key(const Key &);
    Key& operator = (const Key &);
};
    
class PrivateKey : public Key {
  public:
    void Import(Session session,
     std::string file, std::string passphrase);
  
    ssh_key getKey();
};

}
#endif
