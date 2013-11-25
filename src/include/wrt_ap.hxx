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
 * Default constant strings to initialize the AccessPoint class with
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
   * Constructors for AccessPoint - taking various parameters
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

  /**
   * Formats a MAC address to be properly formatted - mutates string given
   *
   * @method  FormatMAC
   *
   * @param   MACtoFormat  MAC string to format
   */
  static void FormatMAC(std::string &MACtoFormat);

  /**
   * Formats a MAC address into a EUI64 compliant IPv6 stateless address
   * - mutates given string
   *
   * @method  MACtoEUI64
   *
   * @param   MACtoMutate  MAC string to format
   */
  static void MACtoEUI64(std::string &MACtoMutate);

  /**
   * Returns a AccessPoint::Type in string form
   *
   * @method  TypeToString
   *
   * @param   Type          Enum to return in string form
   *
   * @return                String form of enum given
   */
  static std::string TypeToString(AccessPoint::Type Type);

  /**
   * Returns a AccessPoint::Type in enum form
   *
   * @method  StringToType
   *
   * @param   type          String type to return in enum form
   *
   * @return                Enum form of given string
   */
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
   * Returns whether the object has a IPv4 address
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
   * Returns whether the object has a IPv6 address
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
   * Returns whether the object has a link local IPv4 address
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
   * Returns whether the object has a link local IPv6 address
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
   * Returns whether the object has a network address
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
   * Accessor method to get the AP name
   *
   * @method  getName
   *
   * @return  AP name in string format
   */
  inline std::string getName()
  {
    return ap_name_;
  }

  /**
   * Accessor to get the MAC address
   *
   * @method  getMAC
   *
   * @return  MAC address in string format
   */
  inline std::string getMAC()
  {
    return mac_address_;
  }

  /**
   * Accessor to get the string type
   *
   * @method  getType
   *
   * @return  the type in string format
   */
  inline std::string getType()
  {
    return TypeToString(ap_type_);
  }

  /**
   * Accessor to get the enum type
   *
   * @method  getEnumType
   *
   * @return  the type in enum format
   */
  inline AccessPoint::Type getEnumType()
  {
    return ap_type_;
  }

  /**
   * Accessor to get the IPv4 address for a given AP
   *
   * @method  getIPv4
   *
   * @return  the IPv4 in string format
   */
  inline std::string getIPv4()
  {
    return ipv4_address_;
  }

  /**
   * Accessor to get the IPv6 address for a given AP
   *
   * @method  getIPv6
   *
   * @return  the IPv6 in string format
   */
  inline std::string getIPv6()
  {
    return ipv6_address_;
  }

  /**
   * Accessor to get the link local IPv4 for a given AP
   *
   * @method  getLinkLocalIPv4
   *
   * @return  the link local IPv4 in string format
   */
  inline std::string getLinkLocalIPv4()
  {
    return link_local_ipv4_address_;
  }

  /**
   * Accessor to get the link local IPv6 for a given AP
   *
   * @method  getLinkLocalIPv6
   *
   * @return  the link local IPv6 in string format
   */
  inline std::string getLinkLocalIPv6()
  {
    return link_local_ipv6_address_;
  }

  /**
   * AP mutator to set the IPv4 address by string
   *
   * @method  setIPv4
   *
   * @param   address  the string to set as the IPv4 address
   */
  inline void setIPv4(std::string address)
  {
    ipv4_address_ = address;
  }

  /**
   * AP mutator to set the IPv6 address by string
   *
   * @method  setIPv6
   *
   * @param   address  the string to set as the IPv6 address
   */
  inline void setIPv6(std::string address)
  {
    ipv6_address_ = address;
  }

  /**
   * AP mutator to set the type by AccessPoint::Type enum
   *
   * @method  setType
   *
   * @param   type     AccessPoint::Type to set as the type
   */
  inline void setType(AccessPoint::Type type)
  {
    ap_type_ = type;
  }

  /**
   * AP mutator to set the type by string
   *
   * @method  setType
   *
   * @param   type     string to set as the type
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
   * AccessPoint internal enum - AP's type
   */
  Type ap_type_ = Type::none;

  /**
   * AccessPoint internal string - AP's name
   */
  std::string ap_name_ = kNoName;
  /**
   * AccessPoint internal string - AP's MAC address
   */
  std::string mac_address_ = kNoMAC;

  /**
   * AccessPoint internal string - AP's IPv4 addresses
   */
  std::string ipv4_address_            = kNoIPv4;
  std::string link_local_ipv4_address_ = kNoIPv4;

  /**
   * AccessPoint internal string - AP's IPv6 addresses
   */
  std::string ipv6_address_            = kNoIPv6;
  std::string link_local_ipv6_address_ = kNoIPv6;
};

}

#endif
