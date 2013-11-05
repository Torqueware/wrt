/******************************************************************************
 * main.hxx                                                                   *
 *                                                                            *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>               *
 *                                                                            *
 * This file is the header for the main file for the WRT system.              *
 *                                                                            *
 ******************************************************************************/

#ifndef WRT_MAIN_H_
#define WRT_MAIN_H_

#include <unistd.h>
#include <getopt.h>

#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <unordered_map>

#include <libconfig.h++>

#include <wrt_ap.hxx>
#include <wrt_io.hxx>
#include <wrt_exception.hxx>

namespace wrt {

//Constants
                  //Exit codes
const int         kExitSuccess            = EXIT_SUCCESS,
                  kExitFailure            = EXIT_FAILURE,

                  //Magic numbers
                  kForever                = 1;

                  //Config defaults
const std::string kDefaultConfigDirectory = "/etc/wrt/",
                  kDefaultConfigFileName  = "wrt.cfg";
                  
                  //Internal command line parser labels
const char        kPushConfiguration[]    = "push",
                  kForceOperation[]       = "force",
                  kListAPs[]              = "list",
                  kAddAPs[]               = "add",
                  kRemoveAPs[]            = "remove",
                  kAddList[]              = "addlist",
                  kRemoveList[]           = "removelist";

                  //Config file field tags, and structure tags
const char        kLocalUser[]            = "Local_User",
                  kRemoteUser[]           = "Remote_User",
                  kCertificates[]         = "Cert_Dir",
                  kConfigDirectory[]      = "Config_Dir",
                  kConfigurationFile[]    = "Config_File",
                  kLogDirectory[]         = "Log_Dir",
                  kLogLevel[]             = "Log_Level",
                  kPIDFile[]              = "PID_File",
                  kSSID[]                 = "SSID",
                  kCrypto[]               = "Encryption",
                  kPassword[]             = "Wifi_Password",
                  kAPList[]               = "Access_Points",
                  kAPName[]               = "Name",
                  kAPType[]               = "Type",
                  kAPMAC[]                = "MAC",
                  kAPIPv6[]               = "IPv6",
                  kAPIPv4[]               = "IPv4";

//WRT output stream
WRTout wout;
WRTout werr;

//Global flags for program control - only EVER set when parsing command line
std::string ConfigFile  = kDefaultConfigDirectory + kDefaultConfigFileName;

Output::Verbosity OutputLevel = Output::Verbosity::kDefault;
//extern std::stream ConfigFile = ConfigFile::kDefault;

bool Push               = false,
     Force              = false,
     List               = false,
     Add                = false,
     Remove             = false;

/**
 * struct for use by gnu clo parsing
 */
static struct option long_options[] = {
  {"config",    required_argument,    0,  'c'},
  {"list",      no_argument,          0,  'l'},
  {"add",       required_argument,    0,  'a'},
  {"remove",    required_argument,    0,  'r'},
  {"push",      no_argument,          0,  'p'},
  {"force",     no_argument,          0,  'f'},
  {"usage",     no_argument,          0,  'u'},
  {"verbose",   no_argument,          0,  'v'},
  {"breif",     no_argument,          0,  'b'},
  {"help",      no_argument,          0,  'h'},
  {"version",   no_argument,          0,  'V'},
  {0, 0, 0, 0}
};

}

//typedef for hash list of APs known
typedef std::unordered_map<std::string,wrt::AccessPoint> APList;

//Utility functions to read / write config files
libconfig::Config& ReadConfigFile(std::string file =
  wrt::kDefaultConfigDirectory + wrt::kDefaultConfigFileName);
void WriteConfigFile(libconfig::Config& settings, std::string file =
  wrt::kDefaultConfigDirectory + wrt::kDefaultConfigFileName);

//Utility function to parse command line configuration
libconfig::Setting& ParseCommandLineOptions(int argc, char **argv);

//Driver function command blocks
void PrintAP(wrt::AccessPoint& AP, int index = 0, int depth = 0);
void AddAPConfig(wrt::AccessPoint& AP);
void AddAPKey(wrt::AccessPoint& AP);
void RemoveAPConfig(wrt::AccessPoint& AP);
void RemoveAPKey(wrt::AccessPoint& AP);
void PushConfig(wrt::AccessPoint& AP);

//Command line output functions / command blocks
void Help();
void Usage();
void Version();

#endif
