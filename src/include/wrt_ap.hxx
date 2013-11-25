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

/**
 * Default constants to initialize the AccessPoint class
 *
 * kNoName - Default name
 * kNoMAC  - Default MAC address
 * kNoIPv4 - Default IPv4 address
 * kNoIPv6 - Default IPv6 address
 */
const std::string kNoName("No Name");
const std::string kNoMAC("00:00:00:00:00:00");
const std::string kNoIPv4("0.0.0.0");
const std::string kNoIPv6("::0");

class AccessPoint
{
public:
  /**
   * Enum class to uniquely identify AP type
   */
  enum class Type
  {
    none,
    tl_wr703n,
    tl_mr3020,
    wrt54g,
    whr_hp_g300n,
  };

  /**
   * Constructors for AccessPoint
   */
  AccessPoint() = default;
  AccessPoint(std::string MACAddress);
  AccessPoint(const char *MACAddress);
  AccessPoint(std::string Name, std::string MACAddress);
  AccessPoint(const char *Name, const char *MACAddress);
  AccessPoint(std::string Name, std::string MACAddress, std::string Type);
  AccessPoint(const char *Name, const char *MACAddress, const char *Type);

  /**
   * Comparator function for the AccessPoint class
   *
   * @method  compare
   *
   * @param   ap       The other AP to compare this AP to
   *
   * @return           indicates whether this AP is greater than (>0),
   *                   less than (<0), or equal to (0) the given AP
   */
  int compare(AccessPoint const &ap);

  //static helper functions
  static void FormatMAC(std::string &MACtoFormat);
  static void MACtoEUI64(std::string &MACtoMutate);
  static std::string TypeToString(AccessPoint::Type Type);
  static AccessPoint::Type StringToType(std::string type);

  /**
   * Getter and setter inline functions
   */

  /**
   * Returns whether the object has a specified MAC address
   *
   * @method  hasMAC
   *
   * @return  true if specified, else false
   */
  inline bool hasMAC()
  {
    return mac_address_ != kNoMAC;
  }

  /**
   * Returns whether the object has a specified name
   *
   * @method  hasName
   *
   * @return  true if specified, else false
   */
  inline bool hasName()
  {
    return ap_name_ != kNoName;
  }

  /**
   * Returns whether the object has a specified type
   *
   * @method  hasType
   *
   * @return  true if specified, else false
   */
  inline bool hasType()
  {
    return ap_type_ != Type::none;
  }

  /**
   * [hasIPv4 description]
   *
   * @method  hasIPv4
   *
   * @return  true if specified, else false
   */
  inline bool hasIPv4()
  {
    return ipv4_address_ != kNoIPv4;
  }

  /**
   * [hasIPv6 description]
   *
   * @method  hasIPv6
   *
   * @return  true if specified, else false
   */
  inline bool hasIPv6()
  {
    return ipv6_address_ != kNoIPv6;
  }

  /**
   * [hasLinkLocalIPv4 description]
   *
   * @method  hasLinkLocalIPv4
   *
   * @return  true if specified, else false
   */
  inline bool hasLinkLocalIPv4()
  {
    return link_local_ipv4_address_ != kNoIPv4;
  }

  /**
   * [hasLinkLocalIPv6 description]
   *
   * @method  hasLinkLocalIPv6
   *
   * @return  true if specified, else false
   */
  inline bool hasLinkLocalIPv6()
  {
    return link_local_ipv6_address_ != kNoIPv6;
  }

  /**
   * [hasAddress description]
   *
   * @method  hasAddress
   *
   * @return  true if specified, else false
   */
  inline bool hasAddress()
  {
    return hasIPv4() || hasIPv6() || hasLinkLocalIPv4() || hasLinkLocalIPv6();
  }

  /**
   * [getName description]
   *
   * @method  getName
   *
   * @return  [description]
   */
  inline std::string getName()
  {
    return ap_name_;
  }

  /**
   * [getMAC description]
   *
   * @method  getMAC
   *
   * @return  [description]
   */
  inline std::string getMAC()
  {
    return mac_address_;
  }

  /**
   * [getType description]
   *
   * @method  getType
   *
   * @return  [description]
   */
  inline std::string getType()
  {
    return TypeToString(ap_type_);
  }

  /**
   * [getEnumType description]
   *
   * @method  getEnumType
   *
   * @return  [description]
   */
  inline AccessPoint::Type getEnumType()
  {
    return ap_type_;
  }

  /**
   * [getIPv4 description]
   *
   * @method  getIPv4
   *
   * @return  [description]
   */
  inline std::string getIPv4()
  {
    return ipv4_address_;
  }

  /**
   * [getIPv6 description]
   *
   * @method  getIPv6
   *
   * @return  [description]
   */
  inline std::string getIPv6()
  {
    return ipv6_address_;
  }

  /**
   * [getLinkLocalIPv4 description]
   *
   * @method  getLinkLocalIPv4
   *
   * @return  [description]
   */
  inline std::string getLinkLocalIPv4()
  {
    return link_local_ipv4_address_;
  }

  /**
   * [getLinkLocalIPv6 description]
   *
   * @method  getLinkLocalIPv6
   *
   * @return  [description]
   */
  inline std::string getLinkLocalIPv6()
  {
    return link_local_ipv6_address_;
  }

  /**
   * [setIPv4 description]
   *
   * @method  setIPv4
   *
   * @param   address  [description]
   */
  inline void setIPv4(std::string address)
  {
    ipv4_address_ = address;
  }

  /**
   * [setIPv6 description]
   *
   * @method  setIPv6
   *
   * @param   address  [description]
   */
  inline void setIPv6(std::string address)
  {
    ipv6_address_ = address;
  }

  /**
   * [setType description]
   *
   * @method  setType
   *
   * @param   type     [description]
   */
  inline void setType(AccessPoint::Type type)
  {
    ap_type_ = type;
  }

  /**
   * [setType description]
   *
   * @method  setType
   *
   * @param   type     [description]
   */
  inline void setType(std::string type)
  {
    setType(StringToType(type));
  }

  /**
   * Overloaded == operator to use comparator
   */
  bool operator == (AccessPoint const &ap)
  {
    return (bool) !compare(ap);
  }

  /**
   * Overloaded != operator to use comparator
   */
  bool operator != (AccessPoint const &ap)
  {
    return (bool) compare(ap);
  }

  /**
   * Override < operator to use comparator
   */
  bool operator < (AccessPoint const &ap)
  {
    return (bool) compare(ap) < 0;
  }

  /**
   * Override > operator to use comparator
   */
  bool operator > (AccessPoint const &ap)
  {
    return (bool) compare(ap) > 0;
  }

private:
  /**
   * AccessPoint type enum for current object
   */
  Type ap_type_                        = Type::none;

  /**
   * AccessPoint internal string - AP's name
   */
  std::string ap_name_                 = kNoName;
  /**
   * AccessPoint internal string - MAC address
   */
  std::string mac_address_             = kNoMAC;
  /**
   *
   */
  std::string ipv4_address_            = kNoIPv4;
  std::string link_local_ipv4_address_ = kNoIPv4;

  std::string ipv6_address_            = kNoIPv6;
  std::string link_local_ipv6_address_ = kNoIPv6;
};

}

#endif
