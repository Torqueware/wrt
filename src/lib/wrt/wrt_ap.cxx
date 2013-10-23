/***********************************************************************
 * wrt_ap.cxx                                                          *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is an object designed to organize a single remote access  *
 * point's info to be used for later use - namely:                     *
 *   o. To organize individual access point information by object.     *
 *   o. To use these listings to establish individual ssh sessions.    *
 *   o. To keep a history of individual statistics and logs.           *
 *                                                                     *
 **********************************************************************/

#include <wrt_ap.hxx>

namespace wrt {

AccessPoint::AccessPoint(std::string MACAddress) {
  if(!MACAddress.empty()) {
    this->mac_address = FormatMAC(MACAddress);
    this->ap_name     = this->mac_address;

    this->link_local_ipv6_address = MACtoEUI64(this->mac_address);
    // //this is where the fun stops
    // std::stringstream ss(std::ios_base::in |
    //                       std::ios_base::out |
    //                        std::ios_base::ate);
    
    // ss << std::uppercase << mac_address;
    // ss >> this->mac_address;

    // //clear the stream
    // ss.str(std::string(""));
    // ss.clear();
    
    // *
    //  * What is being done here: the construction of a link-local
    //  * IPv6 address from the MAC address of a given access point.
    //  *
    //  * To better understand stateless IPv6 link-local addressing,
    //  * please read RFC 4291.
    //  *
    //  * These addresses are ideal because they cannot leave the local
    //  * network - assuming sanity (RFC conformance).
    //  *
    // ss << "fe80::" << mac_address.at(0) << std::hex
    //    << (std::strtoul(mac_address.c_str() + 1, NULL, 16) | 0x2)
    //    << std::dec << std::nouppercase << mac_address.substr(3, 5)
    //    << "ff:fe" << mac_address.substr(9, 5)
    //    << mac_address.substr(15, 2);
    
    // ss >> this->link_local_ipv6_address;
    
    // /* Convert all letters to lower case in the
    //  * ipv6 link local address */
    // std::transform(this->link_local_ipv6_address.begin(),
    //                this->link_local_ipv6_address.end(),
    //                this->link_local_ipv6_address.begin(),
    //                (int (*)(int))std::tolower);
  }
}

  /**
   * Returns the MAC address of the AP
   **/
std::string AccessPoint::getMAC() {
  return this->mac_address;
}

std::string& AccessPoint::MAC() {
  return this->mac_address;
}

std::string& AccessPoint::Name() {
  return this->ap_name;
}

  /**
   * Returns the IPv4 address of the AP (nope)
   **/
std::string AccessPoint::getIPv4Address() {
  if(this->ipv4_address.empty()) {
    return this->link_local_ipv4_address;
  }
        
  return this->ipv4_address;
}

std::string& AccessPoint::IPv4Address() {
  if(this->ipv4_address.empty()) {
    return this->link_local_ipv4_address;
  }
        
  return this->ipv4_address;
}

  /**
   * Returns the IPv6 address of the AP
   **/
std::string AccessPoint::getIPv6Address() {
  if(this->ipv6_address.empty()) {
    return this->link_local_ipv6_address;
  }
        
  return this->ipv6_address;
}


std::string& AccessPoint::IPv6Address() {
  if(this->ipv6_address.empty()) {
    return this->link_local_ipv6_address;
  }
        
  return this->ipv6_address;
}

std::string AccessPoint::FormatMAC(std::string MACAddress) {
  std::string MAC;

  if (MACAddress.empty()) {
    return MAC;
  }

  //this is where the fun stops
  std::stringstream ss(std::ios_base::in | 
    std::ios_base::out | std::ios_base::ate);
    
  ss << std::uppercase << MACAddress;
  ss >> MAC;

  // //clear the stream
  // ss.str(std::string(""));
  // ss.clear();

  return MAC;
}

std::string AccessPoint::MACtoEUI64(std::string MACAddress) {
  std::string EUI64;

  if(MACAddress.empty()) {
    return EUI64;
  }

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
  ss << "fe80::" << MACAddress.at(0) << std::hex
     << (std::strtoul(MACAddress.c_str() + 1, NULL, 16) | 0x2)
     << std::dec << std::nouppercase << MACAddress.substr(3, 5)
     << "ff:fe" << MACAddress.substr(9, 5)
     << MACAddress.substr(15, 2);
  
  ss >> EUI64;
  
  /* Convert all letters to lower case in the
   * ipv6 link local address */
  std::transform(EUI64.begin(),
                 EUI64.end(),
                 EUI64.begin(),
                 (int (*)(int))std::tolower);

  return EUI64;
}

} //namespace wrt

//EOF