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

#include "main.hxx"

using namespace wrt;

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
          OutputLevel = (Output::Verbosity) (((int)OutputLevel) + 1);
          break;

        case 'b':
          OutputLevel = (Output::Verbosity) (((int)OutputLevel) - 1);
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

    wout << Output::Verbosity::kDebug
         << "ParseCommandLineOptions:" << std::endl;


    wout << Output::Verbosity::kDebug1
         << "ParseCommandLineOptions( " << argc << ", [";

    for(int i = 1; i < argc; ++i) {
      wout << Output::Verbosity::kDebug1
           << argv[i] << " ";
    }

    wout << Output::Verbosity::kDebug1
         << "] )" << std::endl;

    wout << Output::Verbosity::kVerbose 
         << "WRT Configuration:"              << std::endl
         << "\tConfig File: " << ConfigFile   << std::endl
         << "\tVerbosity: "
         << Output::EnumToString(OutputLevel) << std::endl;
    
    wout << Output::Verbosity::kVeryVerbose
         << "WRT Program State"         << std::endl
         << "\tPush   flag: " << Push   << std::endl
         << "\tForce  flag: " << Force  << std::endl
         << "\tList   flag: " << List   << std::endl
         << "\tAdd    flag: " << Add    << std::endl
         << "\tRemove flag: " << Remove << std::endl
                                        << std::endl;
    }

  catch (...) 
  {
    std::throw_with_nested(std::runtime_error(failure_message));
  }

  return (parsedData.getRoot());
}

/**
 * Reads a configuration file
 *
 * @param  file file to read, defaults to /etc/wrt/wrt.cfg
 *
 * @return libconfig::Configuration object containing parsed 
 *         information from file
 */
libconfig::Config& ReadConfigFile(std::string file) {
  std::string failure_message = "ReadConfigFile(std::string) failed.",
              read_error = "\"" + file +"\": could not be read from disk.";
  static libconfig::Config config;

  if (!config.exists(kAPList)) {
    try {
      try {

        config.readFile(file.c_str());

      } catch (libconfig::FileIOException &e) {  
        std::throw_with_nested(std::runtime_error(read_error));
      }

    } catch (...) {
        std::throw_with_nested(std::runtime_error(failure_message));
    }
  }
  
  return (config);
}

/**
 * Writes given settings to a configuration file
 *
 * @method  WriteConfigFile
 *
 * @param   settings         config to be written
 * @param   file             file to write to
 */
void WriteConfigFile(libconfig::Config& settings, std::string file) {
  std::string failure_message = "WriteConfigFile(libconfig::Config &) failed.",
              write_error = "\"" + file + "\": could not be written to disk.";

  try
  {
    try
    {
     settings.writeFile(ConfigFile.c_str());

    }

    catch (libconfig::FileIOException &e)
    {
      std::throw_with_nested(std::runtime_error(write_error));
    }

  }

  catch (...)
  {
    std::throw_with_nested(std::runtime_error(failure_message));
  }

  return;
}

/******************************************************************************
 * UTILITY FUNCTIONS                                                [main-UT] *
 ******************************************************************************/

APList& GetAddList(libconfig::Setting& parse) {
  std::string failure_message = "GetAddList(libconfig::Settings &) failed.";
  static APList pendingAdditions;

  if (pendingAdditions.empty()) {
    try {
      
      libconfig::Setting &list = parse[kAddList];

      for (int i = 0, size = list.getLength(); i < size - 1; i++) {
        std::string name = list[i], mac = list[i+1];
        
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
  std::string failure_message = "GetRemoveList(libconfig::Settings &) failed.";
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

APList& GetAPList(libconfig::Config &config) {
  std::string failure_message = "GetAPList(libconfig::Config &) failed.";
  static APList APs;
  
  if (APs.empty()) {
    try {
      libconfig::Setting &list = config.getRoot()[kAPList];

      for(int i = 0; i < list.getLength(); ++i) {
        std::string name = list[i][kAPName],
                    type = list[i][kAPType],
                    mac  = list[i][kAPMAC],
                    ipv4 = list[i][kAPIPv4],
                    ipv6 = list[i][kAPIPv6];

          APs[name] = AccessPoint(name, mac);
          APs[name].setType(type);
          APs[name].setIPv4(ipv4);
          APs[name].setIPv6(ipv6);
      }
    }

    catch (...)
    {
      std::throw_with_nested(std::runtime_error(failure_message));
    }
  }

  return(APs);
}

/******************************************************************************
 * DRIVER FUNCTIONS                                                 [main-DR] *
 ******************************************************************************/

/**
 * Function that prints a single AP, meant to be used to list them
 *
 * @method  PrintAP
 *
 * @param   AP       AccessPoint object to list data for
 * @param   index    Index to print. An index of 0 is omitted
 *                   (remember: index, not offset)
 * @param   depth    Depth to begin indenting at
 */
void PrintAP(AccessPoint& AP, int index, int depth) {
  int upper = Output::kTabWidth * depth,
      lower = Output::kTabWidth * (depth + 1);

    wout << Output::Verbosity::kBrief
         << std::string(upper, ' ');

  if (index)
  {
    wout << Output::Verbosity::kBrief
         << index << ": ";
  }

  wout << Output::Verbosity::kBrief
       << AP.getName() << ": " << AP.getType() << std::endl
       << std::string(lower, ' ')
       << "MAC " << AP.getMAC() << std::endl;
       
  wout << Output::Verbosity::kDefault
       << std::string(lower, ' ')
       << "IPv4 " << AP.getIPv4() << std::endl;

  if (AP.hasLinkLocalIPv4())
  {
    wout << Output::Verbosity::kVerbose
         << std::string(lower, ' ')
         << "Link Local IPv4 " << AP.getLinkLocalIPv4() << std::endl;
  }

  wout << Output::Verbosity::kDefault
       << std::string(lower, ' ')
       << "IPv6 " << AP.getIPv6() << std::endl;

  if (AP.hasLinkLocalIPv6())
  {
    wout << Output::Verbosity::kVerbose
         << std::string(lower, ' ')
         << "Link Local IPv6 " << AP.getLinkLocalIPv6() << std::endl;
  }
}

/**
 * Function that prints a single AP, meant to be used to list them
 *
 * @method  PrintAP
 *
 * @param   AP       AccessPoint object to list data for
 * @param   index    Index to print. An index of 0 is omitted
 *                   (remember: index, not offset)
 */
void PrintAP(AccessPoint& AP, int depth) {
  PrintAP(AP, 0, depth);
}

/**
 * Adds AP to the config file
 *
 * @method  AddAPConfig
 *
 * @param   AP           AP information to add
 */
void AddAPConfig(AccessPoint &AP) {
  std::string already_exists = "\"" + AP.getName() + "\" already exists!",
             failure_message = "AddAPConfig(libconfig::Config &, "
                               "AccessPoint &AP) failed.";

  wout << "wrt: Adding \"" << AP.getName()
       << "\" to config file:" << std::endl;
  
  try
  {
    libconfig::Config& config  = ReadConfigFile();
    libconfig::Setting &APList = config.lookup(kAPList);

    for (int i = 0, size = APList.getLength(); i < size; i++)
    {
      std::string name = APList[i][kAPName],
                  mac  = APList[i][kAPMAC];

      if (name == AP.getName() || mac == AP.getMAC())
      {
        throw(std::runtime_error(already_exists));
      }
    }

    libconfig::Setting &NewEntry = APList.add(libconfig::Setting::TypeGroup);
    NewEntry.add(kAPName, libconfig::Setting::TypeString) = AP.getName();
    NewEntry.add(kAPType, libconfig::Setting::TypeString) = AP.getType();
    NewEntry.add(kAPMAC,  libconfig::Setting::TypeString) = AP.getMAC();
    NewEntry.add(kAPIPv4, libconfig::Setting::TypeString) = AP.autoIPv4();
    NewEntry.add(kAPIPv6, libconfig::Setting::TypeString) = AP.autoIPv6();

    WriteConfigFile(config);
  }

  catch(std::runtime_error const& e)
  {
    print_exception(e);
  } 

  catch(...)
  {
    std::throw_with_nested(std::runtime_error(failure_message));
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
void AddAPKey(AccessPoint &AP) {
  std::string failure_message  = "AddAPKey(AccessPoint &) failed.";
  std::string known_hosts_path = ReadConfigFile().lookup(kConfigDirectory);
  known_hosts_path += std::string("known_hosts");
  int child, status, known_hosts;

  wout << Output::Verbosity::kDebug1
       << "AddAPKey(AccessPoint &) called." << std::endl;

  wout << Output::Verbosity::kDebug2
       << "Opening file \"" << known_hosts_path << "\" for child process"
       << std::endl;

  known_hosts = open(known_hosts_path.c_str(),
                     O_WRONLY | O_APPEND | O_CREAT,
                     S_IRUSR | S_IWUSR);

  if (known_hosts != -1)
  {
    wout << Output::Verbosity::kDebug2
         << "File opened successfully!" << std::endl;
    
    wout << Output::Verbosity::kDebug3
         << "File \"" << known_hosts_path
         << "\" opened as fd " << known_hosts << std::endl;

    if((child = fork()) != -1)
    {
      if (child)
      {
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
      } 

      else
      {
        dup2(known_hosts, STDOUT_FILENO);
        close(STDIN_FILENO);
        close(STDERR_FILENO);

        if (AP.hasIPv4())
        {
          execlp("ssh-keyscan", AP.getIPv4().c_str(), (char *)NULL);
        }

        else if (AP.hasIPv6())
        {
          execlp("ssh-keyscan", AP.getIPv6().c_str(), (char *)NULL); 
        }
        
        else if (AP.hasLinkLocalIPv6())
        {
          execlp("ssh-keyscan", "ssh-keyscan", AP.getLinkLocalIPv6().append("\%eth0").c_str(), (char *)NULL);
        }

        std::exit(kExitFailure);
      }
    }
  }

  if (known_hosts == -1 || child == -1)
  {
    try
    {
      std::string error;
      if (known_hosts == -1)
      {
        error = "open(): cannot open file \"" + known_hosts_path + "\"";
      }

      else
      {
        error = "fork(): returned -1";
      }
      
      throw(std::runtime_error(error));
    }

    catch (...)
    {
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
void RemoveAPConfig(AccessPoint &AP) {
  std::string error_message = "RemoveAPConfig() failed";

  wout << "wrt: Removing \"" << AP.getName()
       << "\" from config file." << std::endl;

  try
  {
    libconfig::Config& config = ReadConfigFile();
    libconfig::Setting &APList = config.lookup(kAPList);

    for (int i = 0, size = APList.getLength(); i < size; i++)
    {
      std::string name = APList[i][kAPName],
                  mac  = APList[i][kAPMAC];

      if (name == AP.getName() || mac == AP.getMAC())
      {
        wout << Output::Verbosity::kDebug1
             << "Removing kAPList index " << i << std::endl;

        APList.remove(i);
      }
    }

    WriteConfigFile(config);
  }

  catch (const std::exception &exception)
  {
    print_exception(exception);
  }

  catch (...)
  {
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
void RemoveAPKey(AccessPoint &AP) {
  std::string failure_message  = "RemoveAPKey(AccessPoint &) failed.";
  std::string known_hosts_path = ReadConfigFile().lookup(kConfigDirectory);
  known_hosts_path += std::string("known_hosts");
  int child, status, known_hosts;

  wout << Output::Verbosity::kDebug1
       << "RemoveAPKey(AccessPoint &) called."
       << std::endl;

  wout << Output::Verbosity::kDefault
       << "Checking for keys to remove in \""
       << known_hosts_path
       << "\"" << std::endl;

  if (AP.hasIPv4()) /* Remove IPv4 from known_hosts */
  {
    wout << Output::Verbosity::kDefault
       << "Removing IPv4 hostname for \""
       << AP.getName()
       << "\"" << std::endl;

    if ((child = fork()) != -1)
    {
      if (child) /* Parent */
      {
        wout << Output::Verbosity::kDebug2
             << "Child process spawned... waiting for child."
             << std::endl;

        waitpid(child, &status, 0);

        wout << Output::Verbosity::kDebug2
             << "Child process termination."
             << std::endl << std::flush;

        wout << Output::Verbosity::kDebug3
             << "Child process exit status: "
             << WEXITSTATUS(status)
             << std::endl << std::flush;
      }

      else /* Child */
      {
        close(STDOUT_FILENO);
        close(STDIN_FILENO);
        close(STDERR_FILENO);

        execlp("ssh-keygen", 
               "ssh-keygen", "-q", 
               "-R", AP.getIPv4().c_str(), 
               "-f", known_hosts_path.c_str(),
               NULL);

        std::exit(kExitFailure);
      }
    }
  }

  if (AP.hasIPv6())  /* Remove IPv6 from known_hosts */
  {
    wout << Output::Verbosity::kDefault
         << "Removing IPv6 hostname for \""
         << AP.getName()
         << "\"" << std::endl;

    if ((child = fork()) != -1)
    {
      if (child) /* Parent */
      {
        wout << Output::Verbosity::kDebug2
             << "Child process spawned... waiting for child."
             << std::endl;

        waitpid(child, &status, 0);

        wout << Output::Verbosity::kDebug2
             << "Child process termination."
             << std::endl << std::flush;

        wout << Output::Verbosity::kDebug3
             << "Child process exit status: "
             << WEXITSTATUS(status)
             << std::endl << std::flush;
      }

      else /* Child */
      {
        close(STDOUT_FILENO);
        close(STDIN_FILENO);
        close(STDERR_FILENO);

        execlp("ssh-keygen", 
               "ssh-keygen", "-q", 
               "-R", AP.getIPv6().c_str(), 
               "-f", known_hosts_path.c_str(),
               NULL);
        
        std::exit(kExitFailure);
      }
    }
  }

  if (AP.hasLinkLocalIPv6())
  {
  wout << Output::Verbosity::kDefault
       << "Removing Link Local IPv6 hostname for \""
       << AP.getName()
       << "\"" << std::endl;

    if ((child = fork()) != -1)
    {
      if (child) /* Parent */
      {
        wout << Output::Verbosity::kDebug2
             << "Child process spawned... waiting for child."
             << std::endl;

        waitpid(child, &status, 0);

        wout << Output::Verbosity::kDebug2
             << "Child process termination."
             << std::endl << std::flush;

        wout << Output::Verbosity::kDebug3
             << "Child process exit status: "
             << WEXITSTATUS(status)
             << std::endl << std::flush;
      }

      else /* Child */
      {
        close(STDOUT_FILENO);
        close(STDIN_FILENO);
        close(STDERR_FILENO);

        execlp("ssh-keygen", 
               "ssh-keygen", "-q", 
               "-R", AP.getLinkLocalIPv6().append("\%eth0").c_str(), 
               "-f", known_hosts_path.c_str(),
               NULL);
        
        std::exit(kExitFailure);
      }
    }
  }

  return;
}

/**
 * Push configuration using child process and ssh
 *
 * @method  PushConfig
 *
 * @param   AP          [description]
 */
void PushConfig(AccessPoint& AP) {

  return;
}

/******************************************************************************
 * CONSOLE OUTPUT                                                   [main-CO] *
 ******************************************************************************/

/**
 * Prints the command line help prompt
 *
 * @method  Help
 */
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
  
  std::exit(kExitSuccess);
}

/**
 * Prints the command line usage prompt
 *
 * @method  Usage
 */
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
  
 
  std::exit(kExitSuccess);
}

/**
 * Prints the program version
 *
 * @method  Version
 */
void Version() {
  std::cout << "WRT 0.2" << std::endl;
  std::cout << "Copyright 2013"
            << " William Patrick Millard <wmillard1@gmail.com>" 
            << std::endl;
  
  std::exit(kExitSuccess);
}

/******************************************************************************
 * PROGRAM MAIN                                                     [main-MA] *
 ******************************************************************************/

/**
 * Where the magic the computer magic occurs
 */
int main(int argc, char* argv[]) {
  try 
  {
    libconfig::Setting &pendingNodes = ParseCommandLineOptions(argc, argv);
    libconfig::Config  &config       = ReadConfigFile(ConfigFile);

    if (List)
    {
      wout << Output::Verbosity::kBrief
           << "WRT APs Known:" << std::endl;

      for (auto& AP : GetAPList(config))
      {
        static int index = 1;
        PrintAP(AP.second, index, 1);
        WriteConfigFile(config);
        index++;
      }
    } 
    
    else if (Add) 
    {
      for (auto& AP : GetAddList(pendingNodes)) 
      {
        AddAPConfig(AP.second);
        AddAPKey(AP.second);
      }
    }
    
    else if (Remove)
    {
      for (auto& AP : GetRemoveList(pendingNodes))
      {
        RemoveAPConfig(AP.second);
        RemoveAPKey(AP.second);
      }
    }

    else if (Push)
    {
      for (auto AP : GetAPList(config))
      {
        PushConfig(AP.second);
      }
    }

    else
    {
      Usage();
    }

    wout << "wrt: Operation completed successfully" << std::endl;
  }

  catch (const std::exception &exception) 
  {
    std::cerr << "wrt: Operation unsuccessful!" << std::endl;
    PrintException(exception, 1);


    std::exit(kExitFailure);
  }

  std::exit(kExitSuccess);
}
