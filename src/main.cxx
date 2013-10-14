/***********************************************************************
 * main.cxx                                                            *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is the main driver program for the WRT config system.     *
 * As of now its singular purpose is to push configuration settings    *
 * over SSH using libssh.                                              *
 *                                                                     *
 **********************************************************************/

#include <main.hxx>

using namespace wrt;

int main(int argc, char* argv[]) {
  try {
    CLOptions     &flags    = ParseCommandLineOptions(argc, argv);
    Configuration &settings = ReadConfigFile(ConfigFile);
    APList        &APs      = ParseAPList(settings);

    if (flags["list"]) {
      if (LogLevel >= DEFAULT_LOG_LEVEL) {
        std::cout << *argv
                  << ": Managed Access Points:"
                  << std::endl;
      }

      ListManagedAPs(APs);
    
    } else if (flags["add"] | flags["remove"]) {
      std::cout << "TODO - far off" << std::endl;
    
    } else {
      if (LogLevel >= DEFAULT_LOG_LEVEL) {
        if (flags["push"]) {
          std::cout << *argv
                    << ": Overwrite configuration on managed nodes."
                    << std::endl;
        } else {
          std::cout << *argv
                    << ": Updating configuration on managed nodes."
                    << std::endl;
        }
      }

      for (auto AP : APs) {
        if (LogLevel >= DEFAULT_LOG_LEVEL) {
          std::cout << "\t" << AP.first << std::endl;
        }

        PushConfiguration(settings, AP.second);
      }
    
    }
  
  } catch (const std::exception &exception) {
    print_exception(exception);
    
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

/***********************************************************************
 * CONFIGURATION FUNCTIONS                                   [main-CF] *
 **********************************************************************/

/* Parse command line info */
CLOptions& ParseCommandLineOptions(int argc, char* argv[]) {
  static CLOptions Flags;
  int    command_line_option = 0;
  int    option_index        = 0;

  do {
    command_line_option = getopt_long(argc, argv, "lpuvbhVc:a:r:",
                                      long_options, &option_index);

    switch(command_line_option) {
      case 0:
        //TODO
        break;

      case 'c':
        ConfigFile = std::string(optarg);
        break;

      case 'l':
        Flags[std::string("list")] = true;
        break;

      case 'a':
        Flags[std::string("add")] = true;
        //TODO
        break;

      case 'r':
        Flags[std::string("remove")] = true;
        //TODO
        break;

      case 'p':
        Flags[std::string("push")] = true;
        break;

      case 'u':
        Usage();

      case 'v':
        LogLevel++;
        break;

      case 'b':
        LogLevel--;
        break;

      case 'h':
        Help();

      case 'V':
        Version();

      default:
        //TODO
        break;
    
    }
  
  } while (command_line_option != -1);

  if (LogLevel > DEFAULT_LOG_LEVEL) {
    if (LogLevel > VERBOSE_LOG_LEVEL) {
      std::cout << "ParseCommandLineOptions(" << argc << ", [";
    
      for(int i = 1; i < argc; ++i) {
        std::cout << argv[i] << " ";
      }

      std::cout << "])" << std::endl << std::endl;
    
    }

    std::cout << "WRT Configuration:" << std::endl
              << "\tFlag: config_file, Value: "
              << ConfigFile << std::endl
              << "\tFlag:   verbosity, Value: "
              << LogLevel << std::endl;

    for (auto &flag : Flags) {
      std::cout << "\tFlag:"
                << std::setw(12) << flag.first
                << std::setw(0)  << ", Value: " << flag.second
                << std::endl;
    
    }

    std::cout << std::endl;
  }

  return(Flags);
}

/* Open config file for later reading */
Configuration& ReadConfigFile(std::string file) {
         std::string       error_message = "ReadConfigFile() failed";
  static libconfig::Config config;

  try {
    config.readFile(file.c_str());
  
  } catch(...) {
      std::throw_with_nested(std::runtime_error(error_message));
  }
  
  return(config);
}

/* Parse list of APs managed from the config file */
APList& ParseAPList(Configuration &settings) {
  std::string error_message = "ParseAPList() failed";
  libconfig::Setting &root  = settings.getRoot(); 
  static APList APs;

  try {
    int count = root[CFG_AP_LIST].getLength();
    
  for(int i = 0; i < count; ++i) {
      const libconfig::Setting &AP = root[CFG_AP_LIST][i];
      std::string name, mac;

      if (AP.lookupValue(CFG_AP_LIST_NAME, name)
          && AP.lookupValue(CFG_AP_LIST_MAC, mac)) {
          APs[name] = AccessPoint(mac);
      }
    }
  
  } catch(...) {
      std::throw_with_nested(std::runtime_error(error_message));
  }

  return(APs);
}

/***********************************************************************
 * DRIVER ROUTINES                                           [main-DR] *
 **********************************************************************/

/* Print list of managed APs from the current config */
void ListManagedAPs(APList &APs) {
  int item = 1;
  
  try {
    for(auto AP : APs) {
      std::cout << item++ << ": " << AP.first << ": <TODO type> MAC "
                << AP.second.getMAC() << std::endl
                << "\tIPv4 " << AP.second.getIPv4Address() << std::endl
                << "\tIPv6 " << AP.second.getIPv6Address() << std::endl;
    }

    std::cout << std::endl;
  
  } catch(...) {
    std::string error_message = "ListManagedAPs(APList &) failed";
    std::throw_with_nested(std::runtime_error(error_message));
  }

  return;
}

/* Push configuration */
void PushConfiguration(Configuration &settings, AccessPoint &target) {
  ssh::Session session;
  std::string  no_ip_addr       = "No configured IPv4 or IPv6 address",
               host_key_changed = "Remote host key has changed, "
                                  "please remove the offending key",
               found_other_host = "Remote host public key not found"
                                  ", but another key exists!",
               remote_error     = "Remote server error",
               error_message    = "PushConfiguration() failed";
  
  try {
    std::string  remote_user    = settings.lookup(CFG_REMOTE_USER),
                 cert_directory = settings.lookup(CFG_CERT_DIR),
                 ip4            = target.IPv4Address(),
                 ip6            = target.IPv6Address();
    
    if (ip4.empty() && ip6.empty()) {
      throw std::runtime_error(no_ip_addr);
    }

    //SSH Session initial setup
    if(ip4.empty()) { //TODO: make this default to 6 later
      session.setOption(SSH_OPTIONS_HOST, target.IPv6Address().c_str());
    } else { //should not execute right now
      session.setOption(SSH_OPTIONS_HOST, ip4.c_str());
    }
   
    session.setOption(SSH_OPTIONS_LOG_VERBOSITY, &LogLevel);
    session.setOption(SSH_OPTIONS_USER, remote_user.c_str());
    session.setOption(SSH_OPTIONS_SSH_DIR, cert_directory.c_str());
    
    //SSH Session connect
    session.connect();
    session.userauthPublickeyAuto();

    switch (session.isServerKnown()) {
      case SSH_SERVER_FILE_NOT_FOUND:
      case SSH_SERVER_NOT_KNOWN:
        session.writeKnownhost();
      case SSH_SERVER_KNOWN_OK:
        break; 

      case SSH_SERVER_KNOWN_CHANGED:
        throw std::runtime_error(host_key_changed);

      case SSH_SERVER_FOUND_OTHER:
        throw std::runtime_error(found_other_host);
      
      case SSH_SERVER_ERROR:
        throw std::runtime_error(remote_error);
      
      default:
        throw std::runtime_error(std::string("default"));
    }

    session.disconnect();

  } catch (const std::runtime_error &exception) {
    print_exception(exception);
  } catch (...) {
    std::throw_with_nested(std::runtime_error(error_message));
  }

  return;
}

/***********************************************************************
 * CONSOLE OUTPUT                                            [main-CO] *
 **********************************************************************/

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
  
  exit(EXIT_SUCCESS);
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
  
 
  exit(EXIT_SUCCESS);
}

/* Prints the program version */
void Version() {
  std::cout << "WRT 0.1" << std::endl;
  std::cout << "Copyright 2013"
            << " William Patrick Millard <wmillard1@gmail.com>" 
            << std::endl;
  
  exit(EXIT_SUCCESS);
}
