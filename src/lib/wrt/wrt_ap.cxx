/******************************************************************************
 * wrt_ap.cxx                                                                 *
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

#include <wrt_ap.hxx>

namespace wrt
{

AccessPoint::AccessPoint(std::string MACAddress)
{
  ap_type_     = Type::none;

  mac_address_ = MACAddress;
  FormatMAC(MACAddress);

  link_local_ipv6_address_ = mac_address_;
  MACtoEUI64(link_local_ipv6_address_);
}

AccessPoint::AccessPoint(const char *MACAddress)
{
  ap_type_     = Type::none;

  mac_address_ = std::string(MACAddress);
  FormatMAC(mac_address_);

  link_local_ipv6_address_ = mac_address_;
  MACtoEUI64(link_local_ipv6_address_);
}

AccessPoint::AccessPoint(std::string Name, std::string MACAddress)
{
  ap_type_     = Type::none;
  ap_name_     = Name;

  mac_address_ = MACAddress;
  FormatMAC(mac_address_);

  link_local_ipv6_address_ = mac_address_;
  MACtoEUI64(link_local_ipv6_address_);
}

AccessPoint::AccessPoint(const char *Name, const char *MACAddress)
{
  ap_type_     = Type::none;
  ap_name_     = std::string(Name);

  mac_address_ = std::string(MACAddress);
  FormatMAC(mac_address_);

  link_local_ipv6_address_ = mac_address_;
  MACtoEUI64(link_local_ipv6_address_);
}

AccessPoint::AccessPoint(std::string Name,
                         std::string MACAddress,
                         AccessPoint::Type Type)
{
  ap_type_     = Type;
  ap_name_     = Name;

  mac_address_ = MACAddress;
  FormatMAC(mac_address_);

  link_local_ipv6_address_ = mac_address_;
  MACtoEUI64(link_local_ipv6_address_);
}

AccessPoint::AccessPoint(const char *Name,
                         const char *MACAddress,
                         AccessPoint::Type Type)
{
  ap_type_     = Type;
  ap_name_     = std::string(Name);

  mac_address_ = std::string(MACAddress);
  FormatMAC(mac_address_);

  link_local_ipv6_address_ = mac_address_;
  MACtoEUI64(link_local_ipv6_address_);
}

AccessPoint::AccessPoint(std::string Name,
                         std::string MACAddress,
                         std::string Type)
{
  ap_type_     = StringToType(Type);
  ap_name_     = Name;

  mac_address_ = MACAddress;
  FormatMAC(mac_address_);

  link_local_ipv6_address_ = mac_address_;
  MACtoEUI64(link_local_ipv6_address_);
}

AccessPoint::AccessPoint(const char *Name,
                         const char *MACAddress,
                         const char *Type)
{
  ap_type_     = StringToType(std::string(Type));
  ap_name_     = std::string(Name);

  mac_address_ = std::string(MACAddress);
  FormatMAC(mac_address_);

  link_local_ipv6_address_ = mac_address_;
  MACtoEUI64(link_local_ipv6_address_);
}


bool AccessPoint::hasName()
{
  return !ap_name_.empty();
}

bool AccessPoint::hasType()
{
  return ap_type_ != AccessPoint::Type::none;
}

bool AccessPoint::hasMAC()
{
  return mac_address_.empty() && mac_address_ != "00:00:00:00:00:00";
}

bool AccessPoint::hasIPv4()
{
  return !ipv4_address_.empty() && ipv4_address_ != "0.0.0.0";
}

bool AccessPoint::hasIPv6()
{
  return !ipv6_address_.empty() && ipv6_address_ != "::0";
}

bool AccessPoint::hasLinkLocalIPv4()
{
  return !link_local_ipv4_address_.empty()
         && link_local_ipv4_address_ != "0.0.0.0";
}

bool AccessPoint::hasLinkLocalIPv6()
{
  return !link_local_ipv6_address_.empty()
         && link_local_ipv6_address_ != "::0";
}

AccessPoint::Type AccessPoint::getEnumType()
{
  return ap_type_;
}

AccessPoint::Type AccessPoint::getRawType()
{
  return ap_type_;
}

std::string AccessPoint::getName()
{
  return ap_name_;
}
/**
 * Returns the MAC address of the AP
 **/
std::string AccessPoint::getMAC()
{
  return mac_address_;
}

std::string AccessPoint::getType()
{
  return TypeToString(ap_type_);
}

std::string AccessPoint::autoIPv4()
{
  if (ipv4_address_.empty()) {
    return link_local_ipv4_address_;
  }

  return ipv4_address_;
}

std::string AccessPoint::autoIPv6()
{
  if (ipv6_address_.empty()) {
    return link_local_ipv6_address_;
  }

  return ipv4_address_;
}

std::string AccessPoint::getIPv4()
{
  return ipv4_address_;
}

std::string AccessPoint::getIPv6()
{
  return ipv6_address_;
}

std::string AccessPoint::getLinkLocalIPv4()
{
  return link_local_ipv4_address_;
}

std::string AccessPoint::getLinkLocalIPv6()
{
  return link_local_ipv6_address_;
}

void AccessPoint::setIPv4(std::string address)
{
  if (address.empty()) {
    return;
  }

  ipv4_address_ = address;
}

void AccessPoint::setIPv6(std::string address)
{
  if (address.empty()) {
    return;
  }

  ipv6_address_ = address;
}

void AccessPoint::setType(std::string type)
{
  if (type.empty()) {
    return;
  }

  ap_type_ = StringToType(type);
}

void AccessPoint::setType(AccessPoint::Type type)
{
  ap_type_ = type;
}

int AccessPoint::compare(AccessPoint const &ap)
{
  if (ap_name_ == mac_address_) {
    return mac_address_.compare(ap.mac_address_);
  }

  return ap_name_.compare(ap.ap_name_);
}

void AccessPoint::FormatMAC(std::string &MACtoFormat)
{
  if (MACtoFormat.empty()) {
    MACtoFormat = std::string("00:00:00:00:00:00");

  } else {

    //this is where the fun stops
    std::stringstream ss(std::ios_base::in |
                         std::ios_base::out | std::ios_base::ate);

    ss << std::uppercase << MACtoFormat;
    ss >> MACtoFormat;
  }

  return;
}

void AccessPoint::MACtoEUI64(std::string &MACtoMutate)
{
  if (MACtoMutate.empty()) {
    MACtoMutate = "fe80::0200:00ff:fe00:0000";
  }

  else {
    //this is where the fun stops
    std::stringstream ss(std::ios_base::in |
                         std::ios_base::out | std::ios_base::ate);

    /**
     * What is being done here: the construction of a link-local
     * IPv6 address from the MAC address of a given access point.
     *
     * To better understand stateless IPv6 link-local addressing,
     * please read RFC 4291.
     *
     * These addresses are ideal because they cannot leave the local
     * network - assuming sanity (RFC conformance).
     **/

    ss << "fe80::" << MACtoMutate.at(0) << std::hex
       << (std::strtoul(MACtoMutate.c_str() + 1, NULL, 16) | 0x2)
       << std::dec << std::nouppercase << MACtoMutate.substr(3, 5)
       << "ff:fe" << MACtoMutate.substr(9, 5)
       << MACtoMutate.substr(15, 2);

    ss >> MACtoMutate;

    /* Convert all letters to lower case in the
     * ipv6 link local address */
    std::transform(MACtoMutate.begin(),
                   MACtoMutate.end(),
                   MACtoMutate.begin(),
                   (int (*)(int))std::tolower);
  }

  return;
}

std::string AccessPoint::TypeToString(AccessPoint::Type Type)
{
  switch (Type) {
  case AccessPoint::Type::none:
    return "none";

  case AccessPoint::Type::tl_wr703n:
    return "TL-WR703N";

  case AccessPoint::Type::tl_mr3020:
    return "TL-MR3020";

  case AccessPoint::Type::wrt54g:
    return "WRT54G";

  case AccessPoint::Type::whr_hp_g300n:
    return "WHR-HP-G300N";

  default:
    return "unknown";
  }
}

AccessPoint::Type AccessPoint::StringToType(std::string type)
{
  static std::unordered_map<std::string, AccessPoint::Type> getEnum = {
    {"none",         AccessPoint::Type::none},
    {"TL-WR703N",    AccessPoint::Type::tl_wr703n},
    {"tl-wr703n",    AccessPoint::Type::tl_wr703n},
    {"TL_WR703N",    AccessPoint::Type::tl_wr703n},
    {"tl_wr703n",    AccessPoint::Type::tl_wr703n},
    {"wr703n",       AccessPoint::Type::tl_wr703n},
    {"TL-MR3020",    AccessPoint::Type::tl_mr3020},
    {"TL_MR3020",    AccessPoint::Type::tl_mr3020},
    {"tl_mr3020",    AccessPoint::Type::tl_mr3020},
    {"mr3020",       AccessPoint::Type::tl_mr3020},
    {"WHR-HP-G300N", AccessPoint::Type::whr_hp_g300n},
    {"whr-hp-g300n", AccessPoint::Type::whr_hp_g300n},
    {"WHR_HP_G300N", AccessPoint::Type::whr_hp_g300n},
    {"whr_hp_g300n", AccessPoint::Type::whr_hp_g300n},
    {"hp_g300n",     AccessPoint::Type::whr_hp_g300n},
    {"hp_g300n",     AccessPoint::Type::whr_hp_g300n},
    {"WRT54G",       AccessPoint::Type::wrt54g},
    {"wrt54g",       AccessPoint::Type::wrt54g}
  };

  return getEnum[type];
}

} //namespace wrt

//EOF