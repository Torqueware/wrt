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

#include <ssh.hpp> //DEPRICATE THIIIIIIS
#include <libconfig.h++>

#include <wrt_ap.hxx>
#include <ssh_exception.hxx>
#include <ssh_session.hxx>

#define BRIEF_LOG_LEVEL        0
#define DEFAULT_LOG_LEVEL      1
#define VERBOSE_LOG_LEVEL      2
#define VERY_VERBOSE_LOG_LEVEL 3

#define WRT_CONFIG_DIR         "/etc/wrt/"
#define WRT_CONFIG_FILE        "wrt.cfg"
#define WRT_REMOTE_USER        "root"
#define WRT_CERT_DIR           "/etc/wrt/"

#define CFG_LOCAL_USER         "local_user"
#define CFG_REMOTE_USER        "remote_user"
#define CFG_CERT_DIR           "cert_dir"
#define CFG_SSID               "SSID"
#define CFG_CRYPTO             "encryption"
#define CFG_PASSWORD           "secret"
#define CFG_AP_LIST            "Access_Points"
#define CFG_AP_LIST_NAME       "Name"
#define CFG_AP_LIST_MAC        "MAC"

//Constants
                  //Exit codes
const int         kExitSuccess         = 0,
                  kExitFailure         = 1,

                  //Magic numbers
                  kForever             = 1,
                  
                  //Log levels
                  kBriefLogLevel       = 0,
                  kDefaultLogLevel     = 1,
                  kVerboseLogLevel     = 2,
                  kVeryVerboseLogLevel = 3;

                  //Config defaults
const std::string kConfigDirectory     = "/etc/wrt/",
                  kConfigFile          = "wrt.cfg";

const char        kAddList[]           = "add",
                  kRemoveList[]        = "remove";

                  //Config file field tags, and structure tags
const char        kLocalUser[]         = "local_user",
                  kRemoteUser[]        = "remote_user",
                  kSSID[]              = "SSID",
                  kCrypto[]            = "encryption",
                  kPassword[]          = "secret",
                  kAPList[]            = "Access_Points",
                  kAPName[]            = "Name",
                  kAPMAC[]             = "MAC",
                  kAPIPv6[]            = "IPv6",
                  kAPIPv4[]            = "IPv4";

//Global flags for program control - only EVER set when parsing command line
std::string ConfigFile  = kConfigDirectory + kConfigFile;

int  LogLevel           = kDefaultLogLevel,
     
     AddIndex           = 0,
     RemoveIndex        = 0;

bool Push               = false,
     Force              = false,
     List               = false,
     Add                = false,
     Remove             = false;

/* Command line options that wrt accepts - used by getopt */
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

//typedef for hash list of APs known
typedef std::unordered_map<std::string,wrt::AccessPoint> APList;


//Configuration function block

//Config file reading / writing
libconfig::Config& ReadConfigFile(std::string file);
void               WriteConfigFile(libconfig::Config& file);

/* The only function permitted to set global variables. Normally, globals are
 * bad practice, but this function depends on no program variables, and it's
 * only input is the command line arguments of the program. It's only output is
 * a libconfig root setting of items to add / remove. */
libconfig::Setting& ParseCommandLineOptions(int argc, char **argv);

//Utility function block
APList&            GetAPList(libconfig::Config &settings);

//Main program function block 
void ListAPs(libconfig::Config& config);
void AddAP(libconfig::Config& config, char *name);
void RemoveAP(libconfig::Config& config, char *name);
void PushConfig(libconfig::Config& config, wrt::AccessPoint& AP);

//Command line prompt block
void Help();
void Usage();
void Version();

#endif