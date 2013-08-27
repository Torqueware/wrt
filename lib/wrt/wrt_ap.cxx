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

#include "wrt_ap"

namespace wrt {

class AccessPoint {

public:
  AccessPoint(std::string mac_address) {
    mac = mac_address;
      
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
    link_local_ipv6_address = "fe80::"
                              + mac.substr(0, 8)
                              + "ff:fe"
                              + mac.substr(8, 16);
  }

  /**
   * Returns the MAC address of the AP
   **/
  std::string getMAC() {
    return mac_address;
  }

  /**
   * Returns the IPv4 address of the AP (nope)
   **/
  std::string getIPv4Address() {
    if(ipv4_address == nullptr) {
      return link_local_ipv4_address;
    }
        
    return ipv4_address;
  }

  /**
   * Returns the IPv6 address of the AP
   **/
  std::string getIPv6Address() {
    if(ipv6_address == nullptr) {
      return link_local_ipv6_address;
    }
        
    return ipv6_address;
  }

private:
  std::string mac_address;

  std::string ipv4_address;
  std::string link_local_ipv4_address;
      
  std::string ipv6_address;
  std::string link_local_ipv6_address;

}; //class AccessPoint

} //namespace wrt

//EOF
