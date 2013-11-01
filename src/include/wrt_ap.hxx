/******************************************************************************
 * wrt_ap.hxx                                                                 *
 *                                                                            *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>               *
 *                                                                            *
 * This file is an object designed to organize a single remote access         *
 * point's info to be used for later use - namely:                            *
 *   o. To organize individual access point information by object.            *
 *   o. To use these individual listings to establish ssh sessions.           *
 *   o. To keep a history of individual logs. (FUTURE)                        *
 *   o. To create a record of based on individual statistics (SUPER FUTURE)   *
 *                                                                            *
 ******************************************************************************/

#ifndef LIBWRT_ACCESS_POINT_H_
#define LIBWRT_ACCESS_POINT_H_

#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unordered_map>


namespace wrt {

class AccessPoint {
public:
  enum class Type { none,
                    tl_wr703n,
                    tl_mr3020,
                    wrt54g,
                    whr_hp_g300n, };

  AccessPoint() = default;
  AccessPoint(std::string Name, std::string MACAddress);
  AccessPoint(const char* Name, const char* MACAddress);
  AccessPoint(std::string Name, std::string MACAddress, AccessPoint::Type Type);
  AccessPoint(const char* Name, const char* MACAddress, AccessPoint::Type Type);
  AccessPoint(std::string MACAddress);
  AccessPoint(const char* MACAddress);

  bool hasName();
  bool hasType();
  bool hasIPv4();
  bool hasIPv6();
  bool hasLinkLocalIPv4();
  bool hasLinkLocalIPv6();

  std::string getName();
  std::string getMAC();

  std::string getType();
  AccessPoint::Type getEnumType();


  std::string autoIPv4(); //DEPRICATE
  std::string autoIPv6(); //DEPRICATE

  std::string getIPv4();
  std::string getIPv6();
  std::string getLinkLocalIPv4();
  std::string getLinkLocalIPv6();

  AccessPoint::Type getRawType(); //TODO: DEPRICATE


  void        setIPv4(std::string address);
  void        setIPv6(std::string address);

  void        setType(AccessPoint::Type type);
  void        setType(std::string type);

  //DEPRICATE
  std::string IPv4Address();
  std::string IPv6Address();

  //Comparator
  int compare(AccessPoint const& ap);

  //Static utility functions
  static void FormatMAC(std::string& MACtoFormat);
  static void MACtoEUI64(std::string& MACtoMutate);
  static std::string TypeToString(AccessPoint::Type Type);
  static AccessPoint::Type StringToType(std::string type);

  bool operator == (AccessPoint const& ap) {
    return (bool) !compare(ap);
  }

  bool operator != (AccessPoint const& ap) {
    return (bool) compare(ap);
  }

  bool operator < (AccessPoint const& ap) {
    return (bool) compare(ap) < 0;
  }

  bool operator > (AccessPoint const& ap) {
    return (bool) compare(ap) > 0;
  }

private:
  AccessPoint::Type ap_type_                  = Type::none;
  std::string       ap_name_                  = "";
  std::string       mac_address_              = "00:00:00:00:00:00";
  std::string       ipv4_address_             = "0.0.0.0";
  std::string       link_local_ipv4_address_  = "0.0.0.0";
  std::string       ipv6_address_             = "::0";
  std::string       link_local_ipv6_address_  = "::0";
};

}

#endif
