/******************************************************************************
 * main.hxx                                                                   *
 *                                                                            *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>               *
 *                                                                            *
 * This file is the header for the main file for the WRT system.              *
 *                                                                            *
 ******************************************************************************/

//Not really needed, but prudent!
#ifndef WRT_MAIN_H_
#define WRT_MAIN_H_

// SYSTEM LIBRARIES
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/wait.h>

// C LIBRARIES
#include <getopt.h>

// STL LIBRARIES
#include <iostream> 
#include <cstdlib>
#include <cerrno>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <unordered_map>

// LIBCONFIG DEPENDENCY
#include <libconfig.h++>

// WRT OBJECTS
#include <wrt_ap.hxx>
#include <wrt_io.hxx>
#include <wrt_exception.hxx>

using namespace wrt;

/**
 * struct for use by gnu clo parsing
 */
static struct option long_options[] = {
  {"config",  required_argument, 0, 'c'},
  {"list",    no_argument,       0, 'l'},
  {"add",     required_argument, 0, 'a'},
  {"remove",  required_argument, 0, 'r'},
  {"push",    no_argument,       0, 'p'},
  {"force",   no_argument,       0, 'f'},
  {"usage",   no_argument,       0, 'u'},
  {"verbose", no_argument,       0, 'v'},
  {"brief",   no_argument,       0, 'q'},
  {"help",    no_argument,       0, 'h'},
  {"version", no_argument,       0, 'V'},
  {0, 0, 0, 0}
};

//typedef for hash list of APs known
typedef std::unordered_map<std::string,AccessPoint> APList;

                  //Exit codes
const auto kExitSuccess            = EXIT_SUCCESS,
           kExitFailure            = EXIT_FAILURE,

                  //Magic numbers
           kForever                = 1;

                  //Config defaults
const auto kDefaultConfigFile("/etc/wrt/wrt.cfg");
const auto kDefaultConfigDirectory("/etc/wrt/");
           
//Root-less configuration elements
const auto kVersion("Version");   //NEW!!!

//Root Configuration Elements - Categories!
const auto kConfigRoot("Configuration");  //NEW!!!
const auto kUsersRoot("Users");        //NEW!!!
const auto kPathsRoot("Paths");        //NEW!!!
const auto kLogRoot("Logs");      //NEW!!!
const auto kWireless("Wireless");       //NEW!!!
           
const auto kLocalUser("Local_User");
const auto kRemoteUser("Remote_User");
const auto kCertificates("Cert_Dir");
const auto kConfigDirectory("Config_Dir");
const auto kConfigurationFile("Config_File");
const auto kLogDirectory("Log_Dir");
const auto kLogLevel("Log_Level");
const auto kPIDFile("PID_File");
const auto kSSID("SSID");
const auto kCrypto("Encryption");
const auto kPassword("Wifi_Password");
const auto kAPList("Access_Points");
const auto kAPName("Name");
const auto kAPType("Type");
const auto kAPMAC("MAC");
const auto kAPIPv6("IPv6");
const auto kAPIPv4("IPv4");
 
//WRT output stream
WRTout     wout,    //SLOPPY - refactor later
           werr,    //TODO
           wlog,    //TODO
           wsyslog; //TODO

//Global flags for program control - only EVER set when parsing command line
auto ConfigFile(kDefaultConfigFile); //make this an extern also
libconfig::Config& PresentState(); //make this extern later

//Global flags and functions that must never be used by others

APList PendingNodes;

auto Push   = false,
     Force  = false,
     List   = false,
     Add    = false,
     Remove = false;

//Utility function to parse command line configuration
void ParseCommandLineOptions(int argc, char **argv);
//Print command block
void PrintAP(AccessPoint& AP);
//Add command block
void AddAPConfig(AccessPoint& AP);
void AddAPKey(AccessPoint& AP);
//Remove command block
void RemoveAPConfig(AccessPoint& AP);
void RemoveAPKey(AccessPoint& AP);
//Push command block
bool CheckConfig(AccessPoint& AP);
void PushConfig(AccessPoint& AP);
//Command line output functions / command blocks
void Help();
void Usage();
void Version();

#endif
