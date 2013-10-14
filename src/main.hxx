/***********************************************************************
 * main.hxx                                                            *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is the header for the main file for the WRT system.
 *                                                                     *
 **********************************************************************/

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
#define CFG_


//Globals for driver program control / configuration file
int LogLevel           = DEFAULT_LOG_LEVEL;
std::string ConfigFile = std::string(WRT_CONFIG_DIR)
                       + std::string(WRT_CONFIG_FILE);

/* Command line options that wrt accepts - used by getopt */
static struct option long_options[] = {
  {"config",    required_argument,    0,  'c'},
  {"list",      no_argument,          0,  'l'},
  {"add",       required_argument,    0,  'a'},
  {"remove",    required_argument,    0,  'r'},
  {"push",      no_argument,          0,  'p'},
  {"usage",     no_argument,          0,  'u'},
  {"verbose",   no_argument,          0,  'v'},
  {"breif",     no_argument,          0,  'b'},
  {"help",      no_argument,          0,  'h'},
  {"version",   no_argument,          0,  'V'},
  {0, 0, 0, 0}
};

typedef std::unordered_map<std::string,bool> CLOptions;
typedef std::unordered_map<std::string,wrt::AccessPoint> APList;
typedef libconfig::Config  Configuration;

//Configuration function blocks
CLOptions& ParseCommandLineOptions(int argc, char* argv[]);
Configuration& ReadConfigFile(std::string file);
APList& ParseAPList(Configuration &settings);

//Main program function blocks 
void ListManagedAPs(APList &APs);
//void RemoveAPs(std::vector<std::string> &APs);
//void AddAPs(std::vector<std::string> &APs);
void PushConfiguration(Configuration &settings, wrt::AccessPoint &target);

//Command line prompt blocks
void Help();
void Usage();
void Version();

#endif
