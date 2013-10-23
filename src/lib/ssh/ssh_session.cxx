/***********************************************************************
 * ssh_session.cxx                                                     *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * The ssh::Session class contains the state of the SSH connection     *
 *                                                                     *
 * This file is a C++ library that acts as a wrapper for the libssh    *
 * library. This file's purpose is to wrap C code in the library for   *
 * clean use as objects in C++                                         *
 *                                                                     *
 * This file was created out of dissatisfaction (upon looking into the *
 * C++ wrapper from libssh). I feel that optimization that compromises *
 * readability is silly in the presense of no profiling.               *
 *                                                                     *
 * Therefore: I rewrote the header wrapper, and omitted functionality  *
 * I felt was unnecessary - thus I don't believe this code is covered  *
 * under the GPL.                                                      *
 *                                                                     *
 * HOWEVER: I AM NOT AN EXPERT ON THE GPL!!!                           *
 *                                                                     *
 * I am not adverse to licensing my code under the GPL, but would like *
 * to retain the freedom to choose my licensing later. If this is not  *
 * possible, so be it (I will happily correct the issue). Otherwise,   *
 * I am naming this file my own!                                       *
 *                                                                     *
 **********************************************************************/

#include <ssh_session.hxx>

namespace ssh {

Session::Session() {
  ssh_init();
  c_session_ = ssh_new();
  ssh_set_blocking(c_session_, 1);
  ssh_blocking_flush(c_session_, 1);
}

Session::~Session() {
  ssh_free(c_session_);
  c_session_=NULL;
}

  /**
   * Sets an SSH session options
   * param: type Type of option
   * param: option std::string containing the value of option
   * throws: SshException on error
   **/
void Session::setOption(enum ssh_options_e type, std::string option) {
  if(ssh_options_set(c_session_, type, option.c_str()) == SSH_ERROR) {
    throw SshException(c_session_);
  }
}

  /**
   * Sets an SSH session options
   * param: type - Type of option
   * param: option - cstring containing the value of option
   * throws: SshException on error
   **/
void Session::setOption(enum ssh_options_e type, const char *option) {
  if(ssh_options_set(c_session_, type, option) == SSH_ERROR) {
    throw SshException(c_session_);
  }
}

  /**
   * Sets an SSH session options
   * param: type Type of option
   * param: option long integer containing the value of option
   * throws: SshException on error
   **/
  void Session::setOption(enum ssh_options_e type, long int option) {
    if(ssh_options_set(c_session_, type, &option) == SSH_ERROR) {
      throw SshException(c_session_);
    }
  }

  /**
   * Sets an SSH session options
   * param: type Type of option
   * param: option void pointer containing the value of option
   * throws: SshException on error
   **/
  void Session::setOption(enum ssh_options_e type, void *option) {
    if(ssh_options_set(c_session_, type, option) == SSH_ERROR) {
      throw SshException(c_session_);
    }
  }

  /* Connects to the remote host
   * throws: SshException on error
   * see ssh_connect
   */
  void Session::connect() {
    if(ssh_connect(c_session_) == SSH_ERROR) {
      throw SshException(c_session_);
    }
  }

  /* Authenticates automatically using public key
   * throws: SshException on error
   * returns: SSH_AUTH_SUCCESS, SSH_AUTH_PARTIAL, SSH_AUTH_DENIED
   * see ssh_userauth_autopubkey
   */
  int Session::userauthPublickeyAuto() {
    int rtn = ssh_userauth_publickey_auto(c_session_, NULL, NULL);
    
    if(rtn == SSH_ERROR) {
      throw SshException(c_session_);
    }
    
    return rtn;
  }

  /* Authenticates using the password method.
   * param:   password to use for authentication
   * throws:  SshException on error
   * returns: SSH_AUTH_SUCCESS, SSH_AUTH_PARTIAL, SSH_AUTH_DENIED
   * see ssh_userauth_password
   */
  int Session::userauthPassword(const char *password) {
    int rtn = ssh_userauth_password(c_session_, NULL, password);
    
    if(rtn == SSH_ERROR) {
      throw SshException(c_session_);
    }

    return rtn;
  }

  /* Try to authenticate using the publickey method.
   * param:   pubkey public key to use for authentication
   * throws:  SshException on error
   * returns: SSH_AUTH_SUCCESS if the pubkey is accepted,
   * returns: SSH_AUTH_DENIED if the pubkey is denied
   * see ssh_userauth_try_pubkey
   */
  int Session::userauthTryPublickey(ssh_key pubkey) {
    int rtn = ssh_userauth_try_publickey(c_session_, NULL, pubkey);
    
    if(rtn == SSH_ERROR) {
      throw SshException(c_session_);
    }
    
    return rtn;
  }

  /* Authenticates using the publickey method.
   * param: privkey private key to use for authentication
   * throws: SshException on error
   * returns: SSH_AUTH_SUCCESS, SSH_AUTH_PARTIAL, SSH_AUTH_DENIED
   * see ssh_userauth_pubkey
   */
  int Session::userauthPublickey(ssh_key privkey) {
    int rtn = ssh_userauth_publickey(c_session_, NULL, privkey);
    
    if(rtn == SSH_ERROR) {
     throw SshException(c_session_);
    }
    
    return rtn;
  }

  /* Returns the available authentication methods from the server
   * throws:  SshException on error
   * returns: Bitfield of available methods.
   * see ssh_userauth_list
   */
  int Session::getAuthList() {
    int rtn = ssh_userauth_list(c_session_, NULL);
    
    if(rtn == SSH_ERROR) {
      throw SshException(c_session_);
    }
    
    return rtn;
  }



  /**
   * Disconnects from the SSH server and closes connection
   **/
  void Session::disconnect() {
    ssh_disconnect(c_session_);
  }

  /**
   * Returns the disconnect message from the server, if any
   * returns: pointer to the message, or NULL. Do not free the pointer.
   **/
  const char* Session::getDisconnectMessage() {
    const char *msg = ssh_get_disconnect_message(c_session_);
    return msg;
  }

  const char* Session::getError() {
    return ssh_get_error(c_session_);
  }

  int Session::getErrorCode() {
    return ssh_get_error_code(c_session_);
  }

  /* getSocket description
   * Returns the file descriptor used for the communication
   * returns: the file descriptor
   * warning: if a proxycommand is used, this function will only return
   *          one of the two file descriptors being used
   **/
  socket_t Session::getSocket() {
    return ssh_get_fd(c_session_);
  }

  /**
   * Gets the Issue banner from the ssh server
   * returns: the issue banner. This is generally a MOTD from server
   **/
  std::string Session::getIssueBanner() {
    char *banner = ssh_get_issue_banner(c_session_);
    
    std::string str = std::string(banner);
    std::free(banner);
    
    return str;
  }

  /**
   * Returns the OpenSSH version (server) if possible
   * returns: openssh version code
   **/
  int Session::getOpensshVersion() {
    return ssh_get_openssh_version(c_session_);
  }

  /**
   * Returns the version of the SSH protocol being used
   * returns: the SSH protocol version
   **/
  int Session::getVersion() {
    return ssh_get_version(c_session_);
  }

  /**
   * Verifies that the server is known
   * throws:  SshException on error
   * returns: Integer value depending on the knowledge of the server key
   **/
  int Session::isServerKnown() {
    int rtn = ssh_is_server_known(c_session_);
    
    if(rtn == SSH_ERROR) {
      throw SshException(c_session_);
    }
    
    return rtn;
  }

  /**
   * Copies options from a session to another
   * throws: SshException on error
   **/
  void Session::optionsCopy(const Session &source) {
    if(ssh_options_copy(source.c_session_, &c_session_) == SSH_ERROR) {
      throw SshException(c_session_);
    }
  }

  /**
   * Parses a configuration file for options
   * throws: SshException on error
   * param:  file configuration file name
   **/
  void Session::optionsParseConfig(const char *file) {
    if(ssh_options_parse_config(c_session_, file) == SSH_ERROR) {
      throw SshException(c_session_);
    }
  }

  /**
   * Silently disconnect from remote host
   **/
  void Session::silentDisconnect() {
    ssh_silent_disconnect(c_session_);
  }

  /**
   * Writes the known host file with current host key
   * throws: SshException on error
   **/
  int Session::writeKnownhost() {
    int rtn = ssh_write_knownhost(c_session_);
    
    if(rtn == SSH_ERROR) {
      throw SshException(c_session_);
    }

    return rtn;
  }

} //namespace ssh

//EOF
