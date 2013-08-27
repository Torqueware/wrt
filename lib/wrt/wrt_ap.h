/***********************************************************************
 * ap.hpp                                                              *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is an object designed to organize a single remote access  *
 * point's info to be used for later use - namely:                     *
 *   o. To organize individual access point information by object.     *
 *   o. To use these individual listings to establish ssh sessions.    *
 *   o. To keep a history of individual statistics and logs.           *
 *                                                                     *
 **********************************************************************/

#ifndef LIBWRT_AP_H_
#define LIBWRT_AP_H_

#include <cstdlib>
#include <iostream>
#include <cstring>

namespace wrt {

class AccessPoint {

public:
  AccessPoint(std::string mac_address);

  std::string getMAC();
  std::string getIPv4Address();
  std::string getIPv6Address();

private:
  std::string mac_address;

  std::string ipv4_address;
  std::string link_local_ipv4_address;
      
  std::string ipv6_address;
  std::string link_local_ipv6_address;

} //class AccessPoint

} //namespace wrt

#endif
