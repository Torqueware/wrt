/******************************************************************************
 * main.cxx                                                                   *
 *                                                                            *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>               *
 *                                                                            *
 * This file is the main driver program for the WRT config system.            *
 * As of now its singular purpose is to push configuration settings           *
 * over SSH.                                                                  *
 *                                                                            *
 ******************************************************************************/

// SYSTEM LIBRARIES
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/wait.h>

// C LIBRARIES
#include <getopt.h>

// STL LIBRARIES
#include <fstream>
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
//#include <wrt_config.hxx>

using namespace wrt;

//typedef for hash list of APs known
//typedef std::unordered_map<std::string, AccessPoint> APList;

//Exit codes
const auto kExitSuccess            = EXIT_SUCCESS,
           kExitFailure            = EXIT_FAILURE,
           kForever                = 1;

//Config defaults
const auto kDefaultConfigFile("/etc/wrt/wrt.cfg");
const auto kDefaultConfigDirectory("/etc/wrt/");
const auto kDefaultRemoteConfigDirectory("/etc/");
const auto kDefaultCertDirectory("/etc/dropbear/");
const auto kDefaultKeyType("id_dsa");
const auto kDefaultInterface("eth0");

//Root-less configuration elements
const auto kVersion("Version");             //NEW!!!

//Root Configuration Elements - Categories!
const auto kConfigRoot("Configuration");    //NEW!!!
const auto kUsersRoot("Users");             //NEW!!!
const auto kPathsRoot("Paths");             //NEW!!!
const auto kLogRoot("Logs");                //NEW!!!
const auto kWireless("Wireless");           //NEW!!!

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

//Configuration Functions
static void ParseCommandLineOptions(int argc, char **argv);

static libconfig::Config &ReadConfigFile(std::string file = kDefaultConfigFile);
static void WriteConfigFile(libconfig::Config &settings,
                            std::string file = kDefaultConfigFile);

//Utility Functions
static APList &GetAPList(libconfig::Config &config);
static int ForkChild(int pipefd[] = NULL);
static int WaitForChild(int PID, int options = 0);

//Print command block
static void PrintAP(AccessPoint &AP, int index, int depth = 0);
static void NameAP(AccessPoint &AP, int index, int depth = 0);
static void ListAP(AccessPoint &AP, int depth = 0);

//Add command block
static void AddAPConfig(AccessPoint &AP);
static void AddAPKey(AccessPoint &AP);

//Remove command block
static void RemoveAPConfig(AccessPoint &AP);
static void RemoveAPKey(AccessPoint &AP);

//Push command block
static bool CheckConfig(AccessPoint &AP);
static void PushConfig(AccessPoint &AP);
static void PushWirelessConfig(AccessPoint &AP);
static void CommitConfig(AccessPoint &AP);

//Command line output functions / command blocks
static void Help();
static void Usage();
static void Version();

/******************************************************************************
 * PROGRAM MAIN                                                     [main-MA] *
 ******************************************************************************/

//WRT output stream
WRTout     wout,    //SLOPPY - refactor later
           werr,    //TODO
           wlog,    //TODO
           wsyslog; //TODO

//Global flags and functions that must never be used by others
namespace
{
APList PendingNodes;

auto ConfigFile(kDefaultConfigFile);
libconfig::Config State;              //make this extern later

auto    Push   = false,
        Force  = false,
        List   = false,
        Add    = false,
        Remove = false;

WRTout  out,
        err,
        log,
        sys;
}

/**
 * Where the computer magic occurs
 */
int main(int argc, char *argv[])
{
  try { // <---- fucking disgusting - depricate this trash

    ParseCommandLineOptions(argc, argv);
    libconfig::Config &config = ReadConfigFile(ConfigFile);

    if (List) {
      int index = 1;

      wout << Output::Verbosity::kBrief
           << "WRT APs Known:"
           << std::endl;

      for (auto &AP : GetAPList(config)) {
        PrintAP(AP.second, index, 1);

        index++;
      }

      if (GetAPList(config).empty()) {
        wout << Output::Verbosity::kBrief
             << std::string(Output::kTabWidth, ' ')
             << "none" << std::endl;
      }

    } else if (Add) {
      int index = 1, child, status;

      wout << Output::Verbosity::kBrief
           << "Adding Host Information to System Config:"
           << std::endl;

      for (auto &AP : PendingNodes) {
        NameAP(AP.second, index, 1);

        if ((child = ForkChild())) { /* Parent */
          if ((status = WaitForChild(child))) {
            std::string failed_termination("Remove AP: Subprocess \"");
            failed_termination += child;
            failed_termination += "\" terminated with abnormal status (";
            failed_termination += status;
            failed_termination += "). Aborting.";

            throw std::runtime_error(failed_termination);

          } else {
            AddAPConfig(AP.second);
            WriteConfigFile(config, ConfigFile);
          }

        } else { /* Child */
          AddAPKey(AP.second);
        }

        index++;
      }

    } else if (Remove) {
      int index = 1, child, status;

      wout << Output::Verbosity::kBrief
           << "Removing Host Information from System Config:"
           << std::endl;

      for (auto &AP : PendingNodes) {
        NameAP(AP.second, index, 1);

        if ((child = ForkChild())) {
          if ((status = WaitForChild(child))) {
            std::string failed_termination("Remove AP: Subprocess \"");
            failed_termination += child;
            failed_termination += "\" terminated with abnormal status (";
            failed_termination += status;
            failed_termination += "). Aborting.";

            throw std::runtime_error(failed_termination);

          } else {
            RemoveAPConfig(AP.second);
            WriteConfigFile(config, ConfigFile);
          }

        } else {

          RemoveAPKey(AP.second);
        }

        index++;
      }

    } else if (Push) {
      int index = 1, child, status;

      wout << Output::Verbosity::kBrief
           << "Updating Managed Hosts:"
           << std::endl;

      for (auto &AP : GetAPList(config)) {
        NameAP(AP.second, index, 1);

        if (Force || CheckConfig(AP.second)) {

          if ((child = ForkChild())) {
            if ((status = WaitForChild(child))) {
              wout << Output::Verbosity::kDebug
                   << "Subprocess " << child << ": Exited with status "
                   << status << std::endl;
            }

          } else {
            PushConfig(AP.second);
          }

          if ((child = ForkChild())) {
            if ((status = WaitForChild(child))) {
              wout << Output::Verbosity::kDebug
                   << "Subprocess " << child << ": Exited with status "
                   << status << std::endl;
            }

          } else {
            PushWirelessConfig(AP.second);
          }

          if ((child = ForkChild())) {
            if ((status = WaitForChild(child))) {
              wout << Output::Verbosity::kDebug
                   << "Subprocess " << child << ": Exited with status "
                   << status << std::endl;
            }

          } else {
            CommitConfig(AP.second);
          }
        }

        index++;
      }
    }

  } catch (const std::exception &exception) {

    std::cerr << std::endl << "wrt: Operation unsuccessful!" << std::endl;

    PrintException(exception, 1);
    std::exit(kExitFailure);
  }

  wout << Output::Verbosity::kDefault
       << std::endl << "wrt: Operation completed successfully"
       << std::endl;
  std::exit(kExitSuccess);
}

/******************************************************************************
 * CONFIGURATION FUNCTIONS                                          [main-CF] *
 ******************************************************************************/

/**
 * Parses command line options and sets appropriate globals and program flags
 *
 * @method  ParseCommandLineOptions
 *
 * @param   argc                     Integer index of arguments in argv
 * @param   argv                     An array of char pointers
 *
 * @return                           libconfig setting which contains
 *                                   parsed data from the command line
 */
void ParseCommandLineOptions(int argc, char *argv[])
{
  int command_line_option = 0, option_index = 0;
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

  try {
    do {
      //TODO: Un-gnu this code - consider a wrt::Configuration library
      command_line_option = getopt_long(argc, argv, "lfpuvbhqVc:a:r:",
                                        long_options, &option_index);

      switch (command_line_option) {
      case 0:
        wout << Output::Verbosity::kDebug2
             << "Case Zero. Bad." << std::endl;
        //Forgot what this means
        break;

      case 'c':
        wout << Output::Verbosity::kDebug1
             << "Setting target configuration file to \""
             << optarg << "\"..." << std::endl;
        ConfigFile = optarg;
        break;

      case 'l':
        wout << Output::Verbosity::kDebug1
             << "Setting \"List\" operation flag..." << std::endl;
        List = true;
        break;

      case 'a':

        //If we are already removing nodes, or have run out of arguments
        //remind the user how to use the program
        if (Remove || optind == argc) {
          Usage();

          std::exit(kExitFailure);
        }

        wout << Output::Verbosity::kDebug1
             << "Queue \"" << argv[optind - 1]
             << "\" to operations list - pending addition..."
             << std::endl;

        Add = true;
        PendingNodes[argv[optind - 1]] =
          AccessPoint(argv[optind - 1], argv[optind]);
        optind++;
        break;

      case 'r':

        //If we are already adding nodes, or have run out of arguments
        //remind the user how to use the program
        if (Add || (optind - 1) == argc) {
          Usage();

          std::exit(kExitFailure);
        }

        wout << Output::Verbosity::kDebug1
             << "Queue \"" << argv[optind - 1]
             << "\" to operations list - pending removal..."
             << std::endl;

        Remove = true;
        PendingNodes[argv[optind - 1]] = AccessPoint(argv[optind - 1], "");
        break;

      case 'p':
        wout << Output::Verbosity::kDebug1
             << "Push flag set..."
             << std::endl;

        Push = true;
        break;

      case 'f':
        wout << Output::Verbosity::kDebug1
             << "Force flag set..."
             << std::endl;

        Force = true;
        break;

      case 'v':
        wout << Output::Verbosity::kVerbose
             << "Verbosity flag set...";
        wout << Output::Verbosity::kVeryVerbose
             << " I am now quite verbose."
             << std::endl;

        if (OutputLevel == Output::Verbosity::kVerbose) {
          wout << Output::Verbosity::kVerbose
               << std::endl;
        }

        wrt::OutputLevel = static_cast<Output::Verbosity>(
                             static_cast<int>(wrt::OutputLevel) + 1);
        break;

      case 'b':
      case 'q':
        wout << Output::Verbosity::kBrief
             << "Brevity flag set...";
        wout << Output::Verbosity::kVerbose
             << " reducing OutputLevel."
             << std::endl;

        if (OutputLevel == Output::Verbosity::kVerbose) {
          wout << Output::Verbosity::kVerbose
               << std::endl;
        }

        wrt::OutputLevel = static_cast<Output::Verbosity>(
                             static_cast<int>(wrt::OutputLevel) - 1);
        break;

      case 'u':
        Usage();
        std::exit(kExitSuccess);

      case 'h':
        Help();
        std::exit(kExitSuccess);

      case 'V':
        Version();
        std::exit(kExitSuccess);

      default:
        break;
      }

    } while (command_line_option != -1);

    wout << Output::Verbosity::kDebug
         << "ParseCommandLineOptions:"
         << std::endl;

    wout << Output::Verbosity::kDebug1
         << "ParseCommandLineOptions( "
         << argc << ", [";

    for (int i = 1; i < argc; ++i) {
      wout << Output::Verbosity::kDebug1
           << argv[i]
           << " ";
    }

    wout << Output::Verbosity::kDebug1
         << "] )"
         << std::endl;

    if (!Push && !Force && !List && !Add && !Remove) {
      Usage();

      std::exit(kExitFailure);
    }

    wout << Output::Verbosity::kVerbose
         << "WRT Configuration:"              << std::endl
         << "\tConfig File: " << ConfigFile   << std::endl
         << "\tVerbosity: "
         << Output::EnumToString(OutputLevel) << std::endl;

    wout << Output::Verbosity::kVeryVerbose << std::boolalpha
         << "Program State"   << std::endl
         << "\tPush   flag: " << Push   << std::endl
         << "\tForce  flag: " << Force  << std::endl
         << "\tList   flag: " << List   << std::endl
         << "\tAdd    flag: " << Add    << std::endl
         << "\tRemove flag: " << Remove << std::endl
         << std::noboolalpha            << std::endl;

  } catch (...) {
    std::throw_with_nested(std::runtime_error("ParseCommandLineOptions"
                           "(int, char *) failed."));
  }
}

/**
 * Reads a configuration file
 *
 * @param  file file to read, defaults to /etc/wrt/wrt.cfg
 *
 * @return libconfig::Configuration object containing parsed
 *         information from file
 */
libconfig::Config &ReadConfigFile(std::string file)
{
  if (!State.exists(kAPList)) {
    try {
      try {
        State.readFile(file.c_str());

      } catch (libconfig::FileIOException &e) {
        std::string read_error(1, '"');
        read_error += file;
        read_error += "\": could not be read from disk.";

        std::throw_with_nested(std::runtime_error(read_error));
      }

    } catch (...) {
      std::throw_with_nested(std::runtime_error("ReadConfigFile(std::string)"
                             " failed."));
    }
  }

  return State;
}

/**
 * Writes given settings to a configuration file
 *
 * @method  WriteConfigFile
 *
 * @param   settings         config to be written
 * @param   file             file to write to
 */
void WriteConfigFile(libconfig::Config &settings, std::string file)
{
  try {
    try {

      settings.writeFile(ConfigFile);

    } catch (libconfig::FileIOException &e) {
      std::string error = "\"" + file + "\": could not be written to disk.";

      std::throw_with_nested(std::runtime_error(error));
    }

  } catch (...) {
    std::throw_with_nested(std::runtime_error("WriteConfigFile"
                           "(libconfig::Config &)"
                           " failed."));
  }

  return;
}

/******************************************************************************
 * UTILITY FUNCTIONS                                                [main-UT] *
 ******************************************************************************/

std::string getTarget(AccessPoint &AP, bool brackets = false)
{
  std::string target;

  if (AP.hasIPv4()) {
    target += AP.getIPv4();

  } else if (AP.hasIPv6()) {
    if (brackets) {
      target += '[';
    }

    target += AP.getIPv6();

    if (brackets) {
      target += ']';
    }

  } else if (AP.hasLinkLocalIPv6()) {
    if (brackets) {
      target += '[';
    }

    target += AP.getLinkLocalIPv6();
    target += '%';
    target += kDefaultInterface;

    if (brackets) {
      target += ']';
    }
  }

  return target;
}

std::string SCPTarget(AccessPoint &AP)
{
  std::string target = getTarget(AP, true);

  target += ':';
  target += kDefaultRemoteConfigDirectory;

  return target;
}

APList &GetAPList(libconfig::Config &config)
{
  static APList APs;

  if (APs.empty()) {
    try {
      libconfig::Setting &list = State.getRoot()[kAPList];

      for (int i = 0; i < list.getLength(); ++i) {
        std::string name = list[i][kAPName],
                    type = list[i][kAPType],
                    mac  = list[i][kAPMAC],
                    ipv4 = list[i][kAPIPv4],
                    ipv6 = list[i][kAPIPv6];

        APs[name] = AccessPoint(name, mac, type);
        APs[name].setIPv4(ipv4);
        APs[name].setIPv6(ipv6);
      }

    } catch (...) {
      std::throw_with_nested(std::runtime_error("GetAPList"
                             "(libconfig::Config &) failed."));
    }
  }

  return (APs);
}

int ForkChild(int pipefd[])
{
  int child;

  if ((child = fork()) != -1) {  //Parent
    wout << Output::Verbosity::kDebug2
         << "Child process spawned... PID:"
         << child << std::endl;

  } else if (child) { //Child is -1
    throw std::runtime_error("fork(): returned -1");

  } else if (pipe) { //Replace stdin and stdout with given pipe
    close(STDERR_FILENO);

    if (dup2(STDIN_FILENO, pipefd[0]) || dup2(STDOUT_FILENO, pipefd[1])) {
      throw std::runtime_error("dup2(): returned -1");
    }

  } else { //Close all output streams
    close(STDOUT_FILENO);
    close(STDIN_FILENO);
    close(STDERR_FILENO);
  }

  return child;
}

int WaitForChild(int PID, int options)
{
  int wait_pid, status;

  wout << Output::Verbosity::kDebug2
       << "Waiting for child \"" << PID << "\""
       << std::endl;

  wait_pid = waitpid(PID, &status, options);

  if (wait_pid == PID) {
    wout << Output::Verbosity::kDebug2
         << "Child process termination."
         << std::endl;

  } else {
    throw std::runtime_error("WaitForChild(int, int *, int) failed.");
  }

  wout << Output::Verbosity::kDebug3
       << "Child process exit status: "
       << WEXITSTATUS(status)
       << std::endl << std::flush;

  return WEXITSTATUS(status);
}

/******************************************************************************
 * DRIVER FUNCTIONS                                                 [main-DR] *
 ******************************************************************************/

/**
 * Function that prints all features of a  single AP, it's meant to
 * write out a detailled illustration of an AP.
 *
 * @method  PrintAP
 *
 * @param   AP       AccessPoint object to list data for
 * @param   index    Index to print. An index of 0 is omitted
 *                   (remember: index, not offset)
 * @param   depth    Depth to begin indenting at
 */
void PrintAP(AccessPoint &AP, int index, int depth)
{
  NameAP(AP, index, depth);
  ListAP(AP, depth + 1);
}

/**
 * [NameAP description]
 * @param AP    [description]
 * @param index [description]
 * @param depth [description]
 */
void NameAP(AccessPoint &AP, int index, int depth)
{
  wout << Output::Verbosity::kBrief
       << std::string(Output::kTabWidth * depth, ' ')
       << std::flush;

  if (index) {
    wout << Output::Verbosity::kSquelch
         << index << ": "
         << std::flush;
  }

  wout << Output::Verbosity::kSquelch
       << AP.getName() << ": " << AP.getType()
       << std::endl;
}

/**
 * [ListAP description]
 * @param AP    [description]
 * @param index [description]
 * @param depth [description]
 */
void ListAP(AccessPoint &AP, int depth)
{
  wout << Output::Verbosity::kBrief
       << std::string(Output::kTabWidth * depth, ' ')
       << "MAC  " << AP.getMAC()
       << std::endl;


  if (AP.hasIPv4() || OutputLevel > Output::Verbosity::kVeryVerbose) {
    wout << Output::Verbosity::kDefault
         << std::string(Output::kTabWidth * depth, ' ')
         << "IPv4 " << AP.getIPv4()
         << std::endl;
  }

  if (AP.hasLinkLocalIPv4() || OutputLevel > Output::Verbosity::kVeryVerbose) {
    wout << Output::Verbosity::kDefault
         << std::string(Output::kTabWidth * depth, ' ')
         << std::flush;

    switch (OutputLevel) {
    case Output::Verbosity::kDebug:
    case Output::Verbosity::kVeryVerbose:
      wout << Output::Verbosity::kVeryVerbose
           << "Link Local IPv4 " << AP.getLinkLocalIPv4()
           << std::endl;
      break;

    case Output::Verbosity::kVerbose:
      wout << Output::Verbosity::kVerbose
           << "L.L. IPv4 " << AP.getLinkLocalIPv4()
           << std::endl;
      break;

    default:
      wout << Output::Verbosity::kDefault
           << "L.L. " << AP.getLinkLocalIPv4()
           << std::endl;

      break;
    }
  }

  if (AP.hasIPv6() || OutputLevel > Output::Verbosity::kVeryVerbose) {
    wout << Output::Verbosity::kDefault
         << std::string(Output::kTabWidth * depth, ' ')
         << "IPv6 " << AP.getIPv6()
         << std::endl;
  }


  if (AP.hasLinkLocalIPv6() || OutputLevel > Output::Verbosity::kVeryVerbose) {
    wout << Output::Verbosity::kDefault
         << std::string(Output::kTabWidth * depth, ' ')
         << std::flush;

    switch (OutputLevel) {
    case Output::Verbosity::kDebug:
    case Output::Verbosity::kVeryVerbose:
      wout << Output::Verbosity::kVeryVerbose
           << "Link Local IPv6 " << AP.getLinkLocalIPv6()
           << std::endl;
      break;

    case Output::Verbosity::kVerbose:
      wout << Output::Verbosity::kVerbose
           << "L.L. IPv6 " << AP.getLinkLocalIPv6()
           << std::endl;
      break;

    default:
      wout << Output::Verbosity::kDefault
           << "L.L. " << AP.getLinkLocalIPv6()
           << std::endl;

      break;
    }
  }
}

/**
 * Adds AP to the config file
 *
 * @method  AddAPConfig
 *
 * @param   AP           AP information to add
 */
void AddAPConfig(AccessPoint &AP)
{
  wout << Output::Verbosity::kDefault
       << "wrt: Adding \"" << AP.getName()
       << "\" to config file:" << std::endl;

  try {
    libconfig::Setting &APList = State.lookup(kAPList);

    for (int i = 0, size = APList.getLength(); i < size; i++) {
      std::string name = APList[i][kAPName],
                  mac  = APList[i][kAPMAC];

      if (mac == AP.getMAC() || name == AP.getName()) {
        std::string already_exists(1, '"');
        already_exists += AP.getName();
        already_exists += "\" already exists!";

        throw std::runtime_error(already_exists);
      }
    }

    libconfig::Setting &NewEntry = APList.add(libconfig::Setting::TypeGroup);

    NewEntry.add(kAPName, libconfig::Setting::TypeString) = AP.getName();
    NewEntry.add(kAPType, libconfig::Setting::TypeString) = AP.getType();
    NewEntry.add(kAPMAC,  libconfig::Setting::TypeString) = AP.getMAC();
    NewEntry.add(kAPIPv4, libconfig::Setting::TypeString) = AP.getIPv4();
    NewEntry.add(kAPIPv6, libconfig::Setting::TypeString) = AP.getIPv6();

    WriteConfigFile(State);

  } catch (std::runtime_error const &e) {
    print_exception(e);

  } catch (...) {
    std::throw_with_nested(std::runtime_error("AddAPConfig"
                           "(libconfig::Config &, AccessPoint &AP) failed."));
  }

  return;
}

/**
 * Adds AP key to known_hosts
 *
 * @method  AddAPKey
 *
 * @param   AP        AP information to get ssh pubkey from
 */
void AddAPKey(AccessPoint &AP)
{
  std::string failure_message  =  "AddAPKey(AccessPoint &) failed.";
  std::string known_hosts_path =  ReadConfigFile().lookup(kConfigDirectory);
  known_hosts_path += std::string("known_hosts");

  int child = 0, status, known_hosts;

  wout << Output::Verbosity::kDebug1
       << "AddAPKey(AccessPoint &) called." << std::endl;

  wout << Output::Verbosity::kDebug2
       << "Opening file \"" << known_hosts_path << "\" for child process"
       << std::endl;

  known_hosts = open(known_hosts_path.c_str(),
                     O_WRONLY | O_APPEND | O_CREAT,
                     S_IRUSR | S_IWUSR);

  if (known_hosts != -1) {
    wout << Output::Verbosity::kDebug2
         << "File opened successfully!" << std::endl;

    wout << Output::Verbosity::kDebug3
         << "File \"" << known_hosts_path
         << "\" opened as fd " << known_hosts << std::endl;

    if ((child = fork()) != -1) {
      if (child) {
        wout << Output::Verbosity::kDebug2
             << "Child process spawned - closing known_hosts file"
             << " and waiting for child." << std::endl;

        close(known_hosts);
        waitpid(child, &status, 0);

        wout << Output::Verbosity::kDebug2
             << "Child process termination."
             << std::endl << std::flush;

        wout << Output::Verbosity::kDebug3
             << "Child process exit status: "
             << WEXITSTATUS(status)
             << std::endl << std::flush;

      } else {
        dup2(known_hosts, STDOUT_FILENO);
        close(STDIN_FILENO);
        close(STDERR_FILENO);

        execlp("ssh-keyscan",
               "ssh-keyscan",
               getTarget(AP).c_str(),
               (char *)NULL);

        std::exit(kExitFailure);
      }
    }
  }

  if (known_hosts == -1 || child == -1) {
    try {
      std::string error;

      if (known_hosts == -1) {
        error = "open(): cannot open file \"" + known_hosts_path + "\"";
      }

      else {
        error = "fork(): returned -1";
      }

      throw std::runtime_error(error);
    }

    catch (...) {
      std::throw_with_nested(std::runtime_error(failure_message));
    }
  }

  return;
}

/**
 * Removes AP key from config file
 *
 * @method  RemoveAPConfig
 *
 * @param   AP              [description]
 */
void RemoveAPConfig(AccessPoint &AP)
{
  std::string error_message = "RemoveAPConfig() failed";

  wout << "wrt: Removing \"" << AP.getName()
       << "\" from config file." << std::endl;

  try {
    libconfig::Config &config = ReadConfigFile();
    libconfig::Setting &APList = config.lookup(kAPList);

    for (int i = 0, size = APList.getLength(); i < size; i++) {
      std::string name = APList[i][kAPName],
                  mac  = APList[i][kAPMAC];

      if (name == AP.getName() || mac == AP.getMAC()) {
        wout << Output::Verbosity::kDebug1
             << "Removing kAPList index " << i << std::endl;

        APList.remove(i);
      }
    }

  } catch (const std::exception &exception) {
    print_exception(exception);

  } catch (...) {
    std::throw_with_nested(std::runtime_error(error_message));
  }

  return;
}

/**
 * Removes AP key from known_hosts
 *
 * @method  RemoveAPKey
 *
 * @param   APInfo       [description]
 */
void RemoveAPKey(AccessPoint &AP)
{
  std::string known_hosts_path = ReadConfigFile().lookup(kConfigDirectory);
  known_hosts_path += "known_hosts";

  if (!std::ifstream(known_hosts_path.c_str())) {
    std::exit(kExitSuccess);
  }

  if (AP.hasIPv4()) {  /* Remove IPv4 from known_hosts */
    int child = 0;

    if ((child = fork()) != -1) {
      if (child) { /* Parent */
        int status = 0;
        waitpid(child, &status, 0);

      } else { /* Child */
        execlp("ssh-keygen",
               "ssh-keygen", "-q",
               "-R", AP.getIPv4().c_str(),
               "-f", known_hosts_path.c_str(),
               NULL);

        std::exit(kExitFailure);
      }
    }
  }

  if (AP.hasIPv6()) {  /* Remove IPv6 from known_hosts */
    int child = 0;

    if ((child = fork()) != -1) {
      if (child) { /* Parent */
        int status = 0;
        waitpid(child, &status, 0);

      } else { /* Child */
        execlp("ssh-keygen",
               "ssh-keygen", "-q",
               "-R", AP.getIPv6().c_str(),
               "-f", known_hosts_path.c_str(),
               NULL);

        std::exit(kExitFailure);
      }
    }
  }

  if (AP.hasLinkLocalIPv6()) {
    execlp("ssh-keygen",
           "ssh-keygen", "-q",
           "-R", AP.getLinkLocalIPv6().append("\%eth0").c_str(),
           "-f", known_hosts_path.c_str(),
           NULL);

    std::exit(kExitFailure);
  }

  std::exit(kExitFailure);
}

/**
 * [CheckConfig description]
 *
 * @method  CheckConfig
 *
 * @param   AP           [description]
 *
 * @return               [description]
 */
bool CheckConfig(AccessPoint &AP)
{
  return true;
}

/**
 * Push configuration using fork, scp, and ssh
 *
 * @method  PushConfig
 *
 * @param   AP          [description]
 */
void PushConfig(AccessPoint &AP)
{
  std::string localConfig = State.lookup(kConfigDirectory);
  localConfig += "config";

  std::cout << localConfig;

  execlp("scp",
         "scp",
         "-F",
         "/etc/wrt/ssh_config",
         "-pr",
         localConfig.c_str(),
         SCPTarget(AP).c_str(),
         (char *)NULL);

  std::exit(kExitFailure);
}

void PushWirelessConfig(AccessPoint &AP)
{
  std::string command("uci set system.hostname="),
      target(getTarget(AP)),
      ssid   = State.lookup(kSSID),
      crypto = State.lookup(kCrypto),
      secret = State.lookup(kPassword);

  if (AP.hasName()) {
    command += AP.getName();
    command += ';';
  } else {
    command.clear();
  }

  command += "uci set wireless.@wifi-device[0].disabled=0";
  command += ";uci set wireless.@wifi-iface[0].ssid=";
  command += ssid;
  command += ";uci set wireless.@wifi-iface[0].encryption=";
  command += crypto;
  command += ";uci set wireless.@wifi-iface[0].key=";
  command += secret;


  execlp("ssh",
         "ssh",
         "-F",
         "/etc/wrt/ssh_config",
         target.c_str(),
         command.c_str(),
         (char *)NULL);

  std::exit(kExitSuccess);
}

void CommitConfig(AccessPoint &AP)
{
  std::string target(getTarget(AP));
  auto command = "uci commit dhcp;"
                 "uci commit 6relayd;"
                 "uci commit dropbear;"
                 "uci commit firewall;"
                 "uci commit network;"
                 "uci commit ubootenv;"
                 "uci commit wireless;"
                 "wifi down;"
                 "wifi up";
  execlp("ssh",
         "ssh",
         "-F",
         "/etc/wrt/ssh_config",
         target.c_str(),
         command,
         (char *)NULL);

  std::exit(kExitSuccess);
}

/******************************************************************************
 * CONSOLE OUTPUT                                                   [main-CO] *
 ******************************************************************************/

/**
 * Prints the command line help prompt
 *
 * @method  Help
 */
void Help()
{
  std::cout << "Usage: wrt [OPTION...]"
            << std::endl << std::endl;

  std::cout << "  -c <FILE>"
            << "\t--config <FILE>"
            << "\tManually specify configuration file."
            << std::endl << std::endl;

  std::cout << "  -l"
            << "\t\t--list"
            << "\t\tList managed access points."
            << std::endl << std::endl;

  std::cout << "  -a <AP Name> <AP MAC>" << std::endl;
  std::cout << "  --add <AP Name> <AP MAC>"
            << "\tAdd an AP for WRT to manage."
            << std::endl << std::endl;

  std::cout << "  -r <AP Name> | <AP MAC>" << std::endl;
  std::cout << "  --remove <AP Name> | <AP MAC>"
            << "\tRemove an AP from managed devices"
            << std::endl << std::endl;

  std::cout << "  -p"
            << "\t\t--push"
            << "\t\tUpdate configs on managed access points."
            << std::endl << std::endl;

  std::cout << "  -u"
            << "\t\t--usage"
            << "\t\tGive a short usage message"
            << std::endl << std::endl;

  std::cout << "  -v"
            << "\t\t--verbose"
            << "\tRequest detailed program responses"
            << std::endl << std::endl;

  std::cout << "  -b"
            << "\t\t--brief"
            << "\t\tRequest concise program responses"
            << std::endl << std::endl;

  std::cout << "  -h"
            << "\t\t--help"
            << "\t\tGive this help list"
            << std::endl << std::endl;

  std::cout << "  -V"
            << "\t\t--version"
            << "\tPrint program version"
            << std::endl << std::endl;

  std::exit(kExitSuccess);
}

/**
 * Prints the command line usage prompt
 *
 * @method  Usage
 */
void Usage()
{
  std::cout << "Usage: wrt\t[-lpuvbhV]" << std::endl;
  std::cout << "\t\t[--list] [--push] [--usage] [--verbose]" << std::endl;
  std::cout << "\t\t[--brief] [--help] [--version]" << std::endl;
  std::cout << "\t\t[-c <CONFIG FILE>] [--config <CONFIG FILE>]" << std::endl;
  std::cout << "\t\t[-a <AP NAME> <AP MAC>]"
            << " [--add <AP NAME> <AP MAC>]" << std::endl;
  std::cout << "\t\t[-r <AP NAME> | <AP MAC>]"
            << " [--remove <AP NAME> | <AP MAC>]" << std::endl;

  std::exit(kExitSuccess);
}

/**
 * Prints the program version
 *
 * @method  Version
 */
void Version()
{
  std::cout << "WRT 0.2-beta" << std::endl;
  std::cout << "Copyright 2013"
            << " William Patrick Millard <wmillard1@gmail.com>"
            << std::endl;

  std::exit(kExitSuccess);
}
