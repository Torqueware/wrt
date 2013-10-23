/***********************************************************************
 * wrt_ap.hxx                                                          *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is an object designed to organize a single remote access  *
 * point's info to be used for later use - namely:                     *
 *   o. To organize individual access point information by object.     *
 *   o. To use these individual listings to establish ssh sessions.    *
 *   o. To keep a history of individual statistics and logs.           *
 *                                                                     *
 * TODO: PASS STRING REFERENCES                                        *
 *                                                                     *
 **********************************************************************/

#ifndef LIBWRT_ACCESS_POINT_H_
#define LIBWRT_ACCESS_POINT_H_

#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>

namespace wrt {

class AccessPoint {

public:
  AccessPoint() = default;
  AccessPoint(std::string MACAddress);

  std::string& MAC();
  //DEPRICATE
  std::string getMAC();

  std::string& Name();

  std::string& IPv4Address();
  //DEPRICATE
  std::string getIPv4Address();
  
  std::string& IPv6Address();
  //DEPRICATE
  std::string getIPv6Address();

  static std::string FormatMAC(std::string MACAddress);
  static std::string MACtoEUI64(std::string MACAddress);

private:
  std::string ap_name;
  std::string mac_address;

  std::string ipv4_address;
  std::string link_local_ipv4_address;
      
  std::string ipv6_address;
  std::string link_local_ipv6_address;

};

}

#endif
