/******************************************************************************
 * main.cxx                                                                   *
 *                                                                            *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>               *
 *                                                                            *
 * This file is the main driver program for the WRT config system.            *
 * As of now its singular purpose is to push configuration settings           *
 * over SSH using libssh.                                                     *
 *                                                                            *
 ******************************************************************************/

#include <main.hxx>

using namespace wrt;

/******************************************************************************
 * CONFIGURATION FUNCTIONS                                          [main-CF] *
 ******************************************************************************/

/* Parse command line info */
libconfig::Setting& ParseCommandLineOptions(int argc, char* argv[]) {
  std::string failure_message = "ParseCommandLineOptions(int, char *) failed.";
  static libconfig::Config parsedData;

  try {

    libconfig::Setting &addList =
      parsedData.getRoot().add(kAddList, libconfig::Setting::TypeArray);
    libconfig::Setting &removeList =
      parsedData.getRoot().add(kRemoveList, libconfig::Setting::TypeArray);

    int command_line_option = 0,
        option_index        = 0;
    
    do {
      command_line_option = getopt_long(argc, argv, "lfpuvbhVc:a:r:",
                                        long_options, &option_index);

      switch(command_line_option) {
        case 0:
          break;

        case 'c':
          ConfigFile = std::string(optarg);
          break;

        case 'l':
          List = true;
          break;

        case 'a':
          Add = true;
          addList.add(libconfig::Setting::TypeString) = argv[optind - 1];
          if (optind == argc) { Usage(); }
          addList.add(libconfig::Setting::TypeString) = argv[optind];
          optind++;
          break;

        case 'r':
          Remove = true;
          removeList.add(libconfig::Setting::TypeString) = argv[optind - 1];
          break;

        case 'p':
          Push = true;
          break;

        case 'f':
          Force = true;
          break;

        case 'v':
          LogLevel++;
          break;

        case 'b':
          LogLevel--;
          break;

        case 'u':
          Usage(); //these functions cause immediate termination

        case 'h':
          Help(); //these functions cause immediate termination

        case 'V':
          Version(); //these functions cause immediate termination

        default:
          break;
      }

    } while (command_line_option != -1);

    if (LogLevel > kDefaultLogLevel) {
      if (LogLevel < kVeryVerboseLogLevel) {
        std::cout << "ParseCommandLineOptions:" << std::endl;

      } else {
        std::cout << "ParseCommandLineOptions(" << argc << ", [";
    
        for(int i = 1; i < argc; ++i) {
          std::cout << argv[i] << " ";
        }

        std::cout << "])" << std::endl << std::endl;
      }

      std::cout << "WRT Configuration:" << std::endl
                << "\tConfig File: "
                << ConfigFile << std::endl
                << "\tVerbosity: ";
      
      switch(LogLevel) {
        case kBriefLogLevel:
          std::cout << "brief" << std::endl;
          break;

        case kDefaultLogLevel:
          std::cout << "normal" << std::endl;
          break;

        case kVerboseLogLevel:
          std::cout << "verbose" << std::endl;
          break;

        case kVeryVerboseLogLevel:
          std::cout << "very verbose" << std::endl;
          break;

        default:
          if (LogLevel > kVeryVerboseLogLevel) {
            std::cout << "silly (" << LogLevel << ")" << std::endl;
          }
          break;
      }

      std::cout << "WRT Program State" << std::endl;
      std::cout << "\tPush   flag: " << Push << std::endl;
      std::cout << "\tForce  flag: " << Force << std::endl;
      std::cout << "\tList   flag: " << List << std::endl;
      std::cout << "\tAdd    flag: " << Add << std::endl;
      std::cout << "\tRemove flag: " << Remove << std::endl;
      std::cout << std::endl;
    }

  } catch (...) {
    std::throw_with_nested(std::runtime_error(failure_message));
  }

  return (parsedData.getRoot());
}

/* Open config file for later reading */
libconfig::Config& ReadConfigFile(std::string file) {
  std::string failure_message = "ReadConfigFile(std::string) failed.",
              read_error = " could not be read from disk.";
  static libconfig::Config config;

  if (!config.exists(kAPList)) {
    try {
      try {

        config.readFile(file.c_str());

      } catch (libconfig::FileIOException &e) {  
        std::throw_with_nested(std::runtime_error(ConfigFile + read_error));
      }

    } catch (...) {
        std::throw_with_nested(std::runtime_error(failure_message));
    }
  }
  
  return (config);
}

/* Writes current configuration to the config file */
void WriteConfigFile(libconfig::Config& file) {
  std::string failure_message = "WriteConfigFile(libconfig::Config &) failed.",
              write_error = " could not be written to disk.";

  try {
    try {

     file.writeFile(ConfigFile.c_str());

    } catch (libconfig::FileIOException &e) {
      std::throw_with_nested(std::runtime_error(ConfigFile + write_error));
    }

  } catch (...) {
    std::throw_with_nested(std::runtime_error(failure_message));
  }

  return;
}

/******************************************************************************
 * UTILITY FUNCTIONS                                                [main-UT] *
 ******************************************************************************/
libconfig::Setting& GetConfigSettings(void) {
  std::string failure_message = "GetConfigSettings(void) failed.";

  try {

    return(ReadConfigFile(ConfigFile).getRoot());

  } catch (...) {
    std::throw_with_nested(std::runtime_error(failure_message));
  }
}

APList& GetAddList(libconfig::Setting& parse) {
  std::string failure_message = "GetAddList(libconfig::Settings &) failed.";
  static APList pendingAdditions;

  if (pendingAdditions.empty()) {
    try {
      
      libconfig::Setting &list = parse[kAddList];

      for (int i = 0, size = list.getLength(); i < size; i++) {
        std::string name = list[i], mac = list[i];
         pendingAdditions[name] = AccessPoint(name, mac);
         i++;
      }

    } catch (...) {
      std::throw_with_nested(std::runtime_error(failure_message));
    }
  }

  return(pendingAdditions);
}

APList& GetRemoveList(libconfig::Setting& parse) {
  std::string failure_message = "GetAddList(libconfig::Settings &) failed.";
  static APList pendingRemovals;

  if(pendingRemovals.empty()) {
    try {

      libconfig::Setting &list = parse[kRemoveList];

      for (int i = 0, size = list.getLength(); i < size; i++) {
        std::string name = list[i];

        pendingRemovals[name] = AccessPoint(name);
      }

    } catch (...) {
      std::throw_with_nested(std::runtime_error(failure_message));
    }
  }

  return(pendingRemovals);
}

/* Parse list of APs managed from the config file */
APList& GetAPList(libconfig::Config &config) {
  std::string failure_message = "GetAPList(libconfig::Config &) failed.";
  static APList APs;
  
  if (APs.empty()) {
    try {

      libconfig::Setting &list = config.getRoot();
      int listCount = list[kAPList].getLength();
    
      for(int i = 0; i < listCount; ++i) {
        const libconfig::Setting &AP = list[kAPList][i];
        std::string name, mac;

        if (AP.lookupValue(kAPName, name) && AP.lookupValue(kAPMAC, mac)) {
          APs[name] = AccessPoint(name, mac);
        }
      }
  
    } catch(...) {
      std::throw_with_nested(std::runtime_error(failure_message));
    }
  }

  return(APs);
}

/******************************************************************************
 * DRIVER FUNCTIONS                                                 [main-DR] *
 ******************************************************************************/

/* Print list of managed APs from the current config */
void ListAPs(libconfig::Config& config) {
  std::string failure_message = "ListAPs(libconfig::Config &) failed.";
  
if (LogLevel >= kDefaultLogLevel) {
  std::cout << "wrt: Managed Access Points:" << std::endl;
}

  try {

    APList APs = GetAPList(config);
    int item = 1;

    for(auto AP : APs) {
      AccessPoint thisAP = AP.second;

      std::cout << item++ << ": " << thisAP.getName()
                << ": " << thisAP.getType() << std::endl
                << "\tMAC " << thisAP.getMAC() << std::endl
                << "\tIPv4 " << thisAP.autoIPv4() << std::endl
                << "\tIPv6 " << thisAP.autoIPv6() << std::endl;
    }
    std::cout << std::endl;
  
  } catch(...) {
    std::throw_with_nested(std::runtime_error(failure_message));
  }

  return;
}

/* Adds AP to the config file */
void AddAPConfig(libconfig::Config &config, AccessPoint &APInfo) {
  std::string already_exists = APInfo.getName() + " already exists!",
             failure_message = "AddAP(libconfig::Config &, "
                               "AccessPoint &APInfo *) failed.";

  if (LogLevel >= kDefaultLogLevel) {
    std::cout << "wrt: Adding " << APInfo.getName()
              << " to config file:" << std::endl;
  }

  try {

    libconfig::Setting &APList = config.lookup(kAPList);
    ssh::Session session;

    for (int i = 0, size = APList.getLength(); i < size; i++) {
      std::string name = APList[i][kAPName],
                  mac  = APList[i][kAPMAC];

      if (name == APInfo.getName() || mac == APInfo.getMAC()) {
        throw(std::runtime_error(already_exists));
      }
    }

    libconfig::Setting &AP = APList.add(libconfig::Setting::TypeGroup);
    std::string name = APInfo.getName(),
                mac  = APInfo.getMAC(),
                ipv4 = APInfo.autoIPv4(),
                ipv6 = APInfo.autoIPv6();

    AP.add(kAPName, libconfig::Setting::TypeString) = name;
    AP.add(kAPMAC,  libconfig::Setting::TypeString) = mac;
    AP.add(kAPIPv4, libconfig::Setting::TypeString) = ipv4;
    AP.add(kAPIPv6, libconfig::Setting::TypeString) = ipv6;

    WriteConfigFile(config);

  } catch(std::runtime_error const& e) {
    print_exception(e);
  } catch(...) {
    std::throw_with_nested(std::runtime_error(failure_message));
  }

  return;
}

void AddAPKey(libconfig::Config &config, AccessPoint &AP) {
  std::string failure_message = "AddAPKey(libconfig::Config"
                                " &, AccessPoint &) failed.";

  if (LogLevel >= kDefaultLogLevel) {
    std::cout << "wrt: Performing handshake with " << AP.getName()
              << " to update known_hosts file:" << std::endl;
  }

  try {
    ssh::Session KeyExchange;
    std::string  user = config.lookup(kRemoteUser),
                 cert = config.lookup(kCertificates);


    //SSH Session initial setup
    if(AP.getIPv4().empty()) { //TODO: make this default to 6 later
      KeyExchange.setOption(SSH_OPTIONS_HOST, AP.autoIPv6());
    } else { //should not execute right now
      KeyExchange.setOption(SSH_OPTIONS_HOST, AP.getIPv4());
    }
   
    KeyExchange.setOption(SSH_OPTIONS_LOG_VERBOSITY, &LogLevel);
    KeyExchange.setOption(SSH_OPTIONS_USER, user);
    KeyExchange.setOption(SSH_OPTIONS_SSH_DIR, cert);
    
    //SSH Session connect
    KeyExchange.connect();
    KeyExchange.userauthPublickeyAuto();

    switch (KeyExchange.isServerKnown()) {
      case SSH_SERVER_FILE_NOT_FOUND:
      /* Doesn't matter, we will make one */
      case SSH_SERVER_NOT_KNOWN:
        if(Force) {
          KeyExchange.writeKnownhost();
        } else {
          std::string input;

          ssh::Key k = ssh::Key(KeyExchange);
          std::cout << "wrt: '" << AP.getName() << "' public key hash: "
                    << k.getHash() << std::endl;
          do {
            std::cout << "Add key to known hosts [Y/n]? ";
            std::cin >> input;

            if (input == "y" || input == "Y") {
              KeyExchange.writeKnownhost();
              break;
            }
          } while ( !std::cin.fail() || input == "n" || input == "N");
        }

      case SSH_SERVER_KNOWN_OK:
        break; /* Why are we adding it in the first place? */ 

      case SSH_SERVER_KNOWN_CHANGED:
        throw std::runtime_error("Server previously known, key has changed. "
                                 "Remove the offending key and rerun wrt.");

      case SSH_SERVER_FOUND_OTHER:
        throw std::runtime_error("Remote host's public key not found, but "
                                 "another key exists! This is potentially "
                                 "a man-in-the-middle attack! Aborting for"
                                 "security reasons!");
      
      case SSH_SERVER_ERROR:
        throw std::runtime_error("Remote server error.");
      
      default:
        throw std::runtime_error(std::string("Out of Range: Error Exception! "
                                             "Something is really broke!"));
    }

    KeyExchange.disconnect();

  } catch (...) {
    std::throw_with_nested(std::runtime_error(failure_message));
  }

  return;
}

/* Removes AP from config file, and removes public key from knownhosts */
void RemoveAPConfig(libconfig::Config& config, AccessPoint &APInfo) {
  std::string error_message = "RemoveAPConfig() failed";

  if (LogLevel >= kDefaultLogLevel) {
    std::cout << "wrt: Removing " << APInfo.getName()
              << " from config file." << std::endl;
  }

  try {
    try {

      WriteConfigFile(config);

    } catch (...) {
      std::throw_with_nested(std::runtime_error(error_message));
    }
  } catch (const std::exception &exception) {
    print_exception(exception);
  }

  return;
}

void RemoveAPKey(libconfig::Config& config, AccessPoint &APInfo) {
  std::string error_message = "RemoveAPKey() failed";

  if (LogLevel >= kDefaultLogLevel) {
    std::cout << "wrt: Removing " << APInfo.getName()
              << " key from known_hosts file." << std::endl;
  }

  try {
    try {

    } catch (...) {
      std::throw_with_nested(std::runtime_error(error_message));
    }
  } catch (const std::exception &exception) {
    print_exception(exception);
  }

  return;
}


/* Push configuration */
void PushConfig(libconfig::Config& config, AccessPoint& AP) {
  std::string error_message     = "PushConfiguration() failed";

  ssh::Session session;
  std::string  no_ip_addr       = "No configured IPv4 or IPv6 address",
               host_key_changed = "Remote host key has changed, "
                                  "please remove the offending key",
               found_other_host = "Remote host public key not found"
                                  ", but another key exists!",
               remote_error     = "Remote server error";

  try {

  } catch (const std::runtime_error &exception) {
    print_exception(exception);

  } catch (...) {
    std::throw_with_nested(std::runtime_error(error_message));
  }

  return;
}

/******************************************************************************
 * CONSOLE OUTPUT                                                   [main-CO] *
 ******************************************************************************/

/* Prints the command line help prompt */
void Help() {
  std::cout << "Usage: wrt [OPTION...]"
            << std::endl;

  std::cout << "  -c <FILE>\t--config <FILE>\t" 
            << "Manually specify configuration file." 
            << std::endl;
  
  std::cout << "  -l\t\t--list\t\t"
            << "List managed access points."
            << std::endl;
  
  std::cout << "  -a <AP>\t--add <AP>\t"
            << "Add an AP for WRT to manage." 
            << std::endl;
  
  std::cout << "  -r <AP>\t--remove <AP>\t"
            << "Remove an AP from managed devices" 
            << std::endl;
  
  std::cout << "  -p\t\t--push\t\t"
            << "Update configs on managed access points." 
            << std::endl;
  
  std::cout << "  -u\t\t--usage\t\t"
            << "Give a short usage message" 
            << std::endl;
 
  std::cout << "  -v\t\t--verbose\t"
            << "Request detailed program responses" 
            << std::endl;

  std::cout << "  -b\t\t--brief\t\t"
            << "Request concise program responses" 
            << std::endl;

  std::cout << "  -h\t\t--help\t\t"
            << "Give this help list" 
            << std::endl;

  std::cout << "  -V\t\t--version\t"
            << "Print program version" 
            << std::endl;
  
  std::exit(EXIT_SUCCESS);
}

/* Prints the command line usage prompt */
void Usage() {
  std::cout << "Usage: wrt\t[-lpuvbhV]"
            << std::endl;
  
  std::cout << "\t\t[-c <CONFIG FILE>] [-a <AP NAME>] [-r <AP_NAME>]"
            << std::endl;

  std::cout << "\t\t[--config <CONFIG FILE> [--add <AP NAME>] "
            << "[--remove <AP_NAME>]"
            << std::endl;
 
  std::cout << "\t\t[--list] [--push] [--verbose] [--brief] [--usage] "
            << "[--help]" 
            << std::endl;
  
 
  std::exit(EXIT_SUCCESS);
}

/* Prints the program version */
void Version() {
  std::cout << "WRT 0.1" << std::endl;
  std::cout << "Copyright 2013"
            << " William Patrick Millard <wmillard1@gmail.com>" 
            << std::endl;
  
  std::exit(EXIT_SUCCESS);
}

/******************************************************************************
 * PROGRAM MAIN                                                     [main-MA] *
 ******************************************************************************/

int main(int argc, char* argv[]) {
  ssh_init();
  std::atexit((void (*)())ssh_finalize);


  try {
    libconfig::Setting &pendingNodes = ParseCommandLineOptions(argc, argv);
    libconfig::Config  &config       = ReadConfigFile(ConfigFile);

    if (List) {
      ListAPs(config);
    
    } else if (Add) {
      for (auto AP : GetAddList(pendingNodes)) {
        AddAPConfig(config, AP.second);
        AddAPKey(config, AP.second);
      }

    } else if (Remove) {
      for (auto AP : GetRemoveList(pendingNodes)) {
        RemoveAPConfig(config, AP.second);
        RemoveAPKey(config, AP.second);
      }
    
    } else if (Push) {
      if (LogLevel >= kDefaultLogLevel) {
        if (Force) {
          std::cout << "wrt: Forcing update on all managed APs" << std::endl;
        } else {
          std::cout << "wrt: Updating APs not up to current" <<std::endl;
        }
      }

      for (auto AP : GetAPList(config)) {
        PushConfig(config, AP.second);
      }

    } else {
      Usage();
    }

    if (LogLevel >= kVerboseLogLevel) {
      std::cout << "wrt: Operation completed successfully." << std::endl;
    }
  
  } catch (const std::exception &exception) {
    print_exception(exception);
    
    if (LogLevel >= kVerboseLogLevel) {
      std::cout << "wrt: Operation unsuccessful!" << std::endl;
    }

    std::exit(kExitFailure);
  }

  std::exit(kExitSuccess);
}
