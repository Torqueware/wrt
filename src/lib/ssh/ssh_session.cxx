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
  _c_session = ssh_new();
}

Session::~Session() {
  ssh_free(_c_session);
  _c_session=NULL;
}

  /**
   * Sets an SSH session options
   * param: type Type of option
   * param: option std::string containing the value of option
   * throws: SshException on error
   **/
void Session::setOption(enum ssh_options_e type, std::string option) {
  if(ssh_options_set(_c_session, type, option.c_str()) == SSH_ERROR) {
    throw SshException(_c_session);
  }
}

  /**
   * Sets an SSH session options
   * param: type - Type of option
   * param: option - cstring containing the value of option
   * throws: SshException on error
   **/
void Session::setOption(enum ssh_options_e type, const char *option) {
  if(ssh_options_set(_c_session, type, option) == SSH_ERROR) {
    throw SshException(_c_session);
  }
}

  /**
   * Sets an SSH session options
   * param: type Type of option
   * param: option long integer containing the value of option
   * throws: SshException on error
   **/
  void Session::setOption(enum ssh_options_e type, long int option) {
    if(ssh_options_set(_c_session, type, &option) == SSH_ERROR) {
      throw SshException(_c_session);
    }
  }

  /**
   * Sets an SSH session options
   * param: type Type of option
   * param: option void pointer containing the value of option
   * throws: SshException on error
   **/
  void Session::setOption(enum ssh_options_e type, void *option) {
    if(ssh_options_set(_c_session, type, option) == SSH_ERROR) {
      throw SshException(_c_session);
    }
  }

  /* Connects to the remote host
   * throws: SshException on error
   * see ssh_connect
   */
  void Session::connect() {
    if(ssh_connect(_c_session) == SSH_ERROR) {
      throw SshException(_c_session);
    }
  }

  /* Authenticates automatically using public key
   * throws: SshException on error
   * returns: SSH_AUTH_SUCCESS, SSH_AUTH_PARTIAL, SSH_AUTH_DENIED
   * see ssh_userauth_autopubkey
   */
  int Session::userauthPublickeyAuto() {
    int rtn = ssh_userauth_publickey_auto(_c_session, NULL, NULL);
    
    if(rtn == SSH_ERROR) {
      throw SshException(_c_session);
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
    int rtn = ssh_userauth_password(_c_session, NULL, password);
    
    if(rtn == SSH_ERROR) {
      throw SshException(_c_session);
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
    int rtn = ssh_userauth_try_publickey(_c_session, NULL, pubkey);
    
    if(rtn == SSH_ERROR) {
      throw SshException(_c_session);
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
    int rtn = ssh_userauth_publickey(_c_session, NULL, privkey);
    
    if(rtn == SSH_ERROR) {
     throw SshException(_c_session);
    }
    
    return rtn;
  }

  /* Returns the available authentication methods from the server
   * throws:  SshException on error
   * returns: Bitfield of available methods.
   * see ssh_userauth_list
   */
  int Session::getAuthList() {
    int rtn = ssh_userauth_list(_c_session, NULL);
    
    if(rtn == SSH_ERROR) {
      throw SshException(_c_session);
    }
    
    return rtn;
  }

  /**
   * Disconnects from the SSH server and closes connection
   **/
  void Session::disconnect() {
    ssh_disconnect(_c_session);
  }

  /**
   * Returns the disconnect message from the server, if any
   * returns: pointer to the message, or NULL. Do not free the pointer.
   **/
  const char* Session::getDisconnectMessage() {
    const char *msg = ssh_get_disconnect_message(_c_session);
    return msg;
  }

  const char* Session::getError() {
    return ssh_get_error(_c_session);
  }

  int Session::getErrorCode() {
    return ssh_get_error_code(_c_session);
  }

  /* getSocket description
   * Returns the file descriptor used for the communication
   * returns: the file descriptor
   * warning: if a proxycommand is used, this function will only return
   *          one of the two file descriptors being used
   **/
  socket_t Session::getSocket() {
    return ssh_get_fd(_c_session);
  }

  /**
   * Gets the Issue banner from the ssh server
   * returns: the issue banner. This is generally a MOTD from server
   **/
  std::string Session::getIssueBanner() {
    char *banner = ssh_get_issue_banner(_c_session);
    
    std::string str = std::string(banner);
    std::free(banner);
    
    return str;
  }

  /**
   * Returns the OpenSSH version (server) if possible
   * returns: openssh version code
   **/
  int Session::getOpensshVersion() {
    return ssh_get_openssh_version(_c_session);
  }

  /**
   * Returns the version of the SSH protocol being used
   * returns: the SSH protocol version
   **/
  int Session::getVersion() {
    return ssh_get_version(_c_session);
  }

  /**
   * Verifies that the server is known
   * throws:  SshException on error
   * returns: Integer value depending on the knowledge of the server key
   **/
  int Session::isServerKnown() {
    int rtn = ssh_is_server_known(_c_session);
    
    if(rtn == SSH_ERROR) {
      throw SshException(_c_session);
    }
    
    return rtn;
  }

  /**
   * Copies options from a session to another
   * throws: SshException on error
   **/
  void Session::optionsCopy(const Session &source) {
    if(ssh_options_copy(source._c_session, &_c_session) == SSH_ERROR) {
      throw SshException(_c_session);
    }
  }

  /**
   * Parses a configuration file for options
   * throws: SshException on error
   * param:  file configuration file name
   **/
  void Session::optionsParseConfig(const char *file) {
    if(ssh_options_parse_config(_c_session, file) == SSH_ERROR) {
      throw SshException(_c_session);
    }
  }

  /**
   * Silently disconnect from remote host
   **/
  void Session::silentDisconnect() {
    ssh_silent_disconnect(_c_session);
  }

  /**
   * Writes the known host file with current host key
   * throws: SshException on error
   **/
  int Session::writeKnownhost() {
    int rtn = ssh_write_knownhost(_c_session);
    
    if(rtn == SSH_ERROR) {
      throw SshException(_c_session);
    }

    return rtn;
  }

SFTPSession::SFTPSession(Session &session) {
  _c_sftp = sftp_new(session._c_session);

  if(!_c_sftp) {
  std::string error = "Error opening sftp channel: ";
  error += ssh_get_error(session._c_session);
    throw SshException(error);
  }
}

SFTPSession::~SFTPSession() {
  sftp_free(_c_sftp);
  _c_sftp = NULL;
}



} //namespace ssh

//EOF
