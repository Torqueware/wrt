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

#include <string>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace wrt
{

//Default Constants
const std::string kNoName("No Name");
const std::string kNoMAC("00:00:00:00:00:00");
const std::string kNoIPv4("0.0.0.0");
const std::string kNoIPv6("::0");

class AccessPoint
{
public:
  enum class Type
  {
    none,
    tl_wr703n,
    tl_mr3020,
    wrt54g,
    whr_hp_g300n,
  };

  AccessPoint() = default;
  AccessPoint(std::string MACAddress);
  AccessPoint(const char *MACAddress);
  AccessPoint(std::string Name, std::string MACAddress);
  AccessPoint(const char *Name, const char *MACAddress);
  AccessPoint(std::string Name, std::string MACAddress, std::string Type);
  AccessPoint(const char *Name, const char *MACAddress, const char *Type);

  //Comparator
  int compare(AccessPoint const &ap);

  //static helper functions
  static void FormatMAC(std::string &MACtoFormat);
  static void MACtoEUI64(std::string &MACtoMutate);
  static std::string TypeToString(AccessPoint::Type Type);
  static AccessPoint::Type StringToType(std::string type);

  //getter and setter inlines
  inline bool hasMAC()
  {
    return mac_address_ != kNoMAC;
  }

  inline bool hasName()
  {
    return ap_name_ != kNoName;
  }

  inline bool hasType()
  {
    return ap_type_ != Type::none;
  }

  inline bool hasIPv4()
  {
    return ipv4_address_ != kNoIPv4;
  }

  inline bool hasIPv6()
  {
    return ipv6_address_ != kNoIPv6;
  }

  inline bool hasLinkLocalIPv4()
  {
    return link_local_ipv4_address_ != kNoIPv4;
  }

  inline bool hasLinkLocalIPv6()
  {
    return link_local_ipv6_address_ != kNoIPv6;
  }

  inline bool hasAddress()
  {
    return hasIPv4() || hasIPv6() || hasLinkLocalIPv4() || hasLinkLocalIPv6();
  }

  inline std::string getName()
  {
    return ap_name_;
  }

  inline std::string getMAC()
  {
    return mac_address_;
  }

  inline std::string getType()
  {
    return TypeToString(ap_type_);
  }

  inline AccessPoint::Type getEnumType()
  {
    return ap_type_;
  }

  inline std::string getIPv4()
  {
    return ipv4_address_;
  }

  inline std::string getIPv6()
  {
    return ipv6_address_;
  }

  inline std::string getLinkLocalIPv4()
  {
    return link_local_ipv4_address_;
  }

  inline std::string getLinkLocalIPv6()
  {
    return link_local_ipv6_address_;
  }

  inline void setIPv4(std::string address)
  {
    ipv4_address_ = address;
  }

  inline void setIPv6(std::string address)
  {
    ipv6_address_ = address;
  }

  inline void setType(AccessPoint::Type type)
  {
    ap_type_ = type;
  }

  inline void setType(std::string type)
  {
    setType(StringToType(type));
  }

  bool operator == (AccessPoint const &ap)
  {
    return (bool) !compare(ap);
  }

  bool operator != (AccessPoint const &ap)
  {
    return (bool) compare(ap);
  }

  bool operator < (AccessPoint const &ap)
  {
    return (bool) compare(ap) < 0;
  }

  bool operator > (AccessPoint const &ap)
  {
    return (bool) compare(ap) > 0;
  }

private:
  Type ap_type_                        = Type::none;

  std::string ap_name_                 = kNoName;
  std::string mac_address_             = kNoMAC;
  std::string ipv4_address_            = kNoIPv4;
  std::string link_local_ipv4_address_ = kNoIPv4;
  std::string ipv6_address_            = kNoIPv6;
  std::string link_local_ipv6_address_ = kNoIPv6;
};

}

#endif
