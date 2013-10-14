/***********************************************************************
 * ssh.hpp                                                             *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard1@gmail.com>        *
 *                                                                     *
 * This file is a header file that acts as a wrapper for the libssh    *
 * library. This file's purpose is to wrap C code in the library for   *
 * use as objects in C++                                               *
 *                                                                     *
 * THIS FILE IS _HEAVILY_ BASED OFF OF THE LIBSSHPP.HPP WRAPPER        *
 *                                                                     *
 * However, said wrapper is kinda broken, so I rolled my own based off *
 * of theirs. Regardless, this portion of the software is FOSS.        *
 *                                                                     *
 * This program is free software; you can redistribute it and/or       *
 * modify it under the terms of the GNU General Public License as      *
 * published by the Free Software Foundation; either version 2 of the  *
 * License, or (at your option) any later version.                     *
 *                                                                     *
 * This program is distributed in the hope that it will be useful,     *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 * GNU General Public License for more details.                        *
 *                                                                     *
 * You should have received a copy of the GNU General Public License   *
 * along with this program; if not, write to the Free Software         *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,          *
 * MA 02110-1301, USA.                                                 *
 *                                                                     *
 **********************************************************************/

#ifndef LIBSSHPP_HPP_
#define LIBSSHPP_HPP_

/* do not use deprecated functions */
#define LIBSSH_LEGACY_0_4

#include <libssh/libssh.h>
#include <libssh/server.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <ssh_session.hxx>

/* Internal macros for throwing exceptions if there are errors */
#define ssh_throw(x) if((x)==SSH_ERROR) throw SshException(getCSession())
#define ssh_throw_null(CSession,x) if((x)==NULL) throw SshException(CSession)
#define void_throwable void
#define return_throwable return

namespace ssh {

/** @brief the ssh::Channel class describes the state of an SSH
 * channel.
 * @see ssh_channel
 */
class Channel {
  friend class Session;
public:
  Channel(Session &session){
    channel=ssh_channel_new(session.getCSession());
    this->session=&session;
  }
  ~Channel(){
    ssh_channel_free(channel);
    channel=NULL;
  }

  /** @brief accept an incoming X11 connection
   * @param[in] timeout_ms timeout for waiting, in ms
   * @returns new Channel pointer on the X11 connection
   * @returns NULL in case of error
   * @warning you have to delete this pointer after use
   * @see ssh_channel_accept_x11
   * @see Channel::requestX11
   */
  Channel *acceptX11(int timeout_ms){
    ssh_channel x11chan = ssh_channel_accept_x11(channel,timeout_ms);
    ssh_throw_null(getCSession(),x11chan);
    Channel *newchan = new Channel(getSession(),x11chan);
    return newchan;
  }
  /** @brief change the size of a pseudoterminal
   * @param[in] cols number of columns
   * @param[in] rows number of rows
   * @throws SshException on error
   * @see ssh_channel_change_pty_size
   */
  void_throwable changePtySize(int cols, int rows){
    int err=ssh_channel_change_pty_size(channel,cols,rows);
    ssh_throw(err);
    return_throwable;
  }

  /** @brief closes a channel
   * @throws SshException on error
   * @see ssh_channel_close
   */
  void_throwable close(){
    ssh_throw(ssh_channel_close(channel));
    return_throwable;
  }

  int getExitStatus(){
    return ssh_channel_get_exit_status(channel);
  }
  Session &getSession(){
    return *session;
  }
  /** @brief returns true if channel is in closed state
   * @see ssh_channel_is_closed
   */
  bool isClosed(){
    return ssh_channel_is_closed(channel) != 0;
  }
  /** @brief returns true if channel is in EOF state
   * @see ssh_channel_is_eof
   */
  bool isEof(){
    return ssh_channel_is_eof(channel) != 0;
  }
  /** @brief returns true if channel is in open state
   * @see ssh_channel_is_open
   */
  bool isOpen(){
    return ssh_channel_is_open(channel) != 0;
  }
  int openForward(const char *remotehost, int remoteport,
      const char *sourcehost=NULL, int localport=0){
    int err=ssh_channel_open_forward(channel,remotehost,remoteport,
        sourcehost, localport);
    ssh_throw(err);
    return err;
  }
  /* TODO: completely remove this ? */
  void_throwable openSession(){
    int err=ssh_channel_open_session(channel);
    ssh_throw(err);
    return_throwable;
  }
  int poll(bool is_stderr=false){
    int err=ssh_channel_poll(channel,is_stderr);
    ssh_throw(err);
    return err;
  }
  int read(void *dest, size_t count, bool is_stderr=false){
    int err;
    /* handle int overflow */
    if(count > 0x7fffffff)
      count = 0x7fffffff;
    err=ssh_channel_read(channel,dest,count,is_stderr);
    ssh_throw(err);
    return err;
  }
  int readNonblocking(void *dest, size_t count, bool is_stderr=false){
    int err;
    /* handle int overflow */
    if(count > 0x7fffffff)
      count = 0x7fffffff;
    err=ssh_channel_read_nonblocking(channel,dest,count,is_stderr);
    ssh_throw(err);
    return err;
  }
  void_throwable requestEnv(const char *name, const char *value){
    int err=ssh_channel_request_env(channel,name,value);
    ssh_throw(err);
    return_throwable;
  }

  void_throwable requestExec(const char *cmd){
    int err=ssh_channel_request_exec(channel,cmd);
    ssh_throw(err);
    return_throwable;
  }
  void_throwable requestPty(const char *term=NULL, int cols=0, int rows=0){
    int err;
    if(term != NULL && cols != 0 && rows != 0)
      err=ssh_channel_request_pty_size(channel,term,cols,rows);
    else
      err=ssh_channel_request_pty(channel);
    ssh_throw(err);
    return_throwable;
  }

  void_throwable requestShell(){
    int err=ssh_channel_request_shell(channel);
    ssh_throw(err);
    return_throwable;
  }
  void_throwable requestSendSignal(const char *signum){
    int err=ssh_channel_request_send_signal(channel, signum);
    ssh_throw(err);
    return_throwable;
  }
  void_throwable requestSubsystem(const char *subsystem){
    int err=ssh_channel_request_subsystem(channel,subsystem);
    ssh_throw(err);
    return_throwable;
  }
  int requestX11(bool single_connection,
      const char *protocol, const char *cookie, int screen_number){
    int err=ssh_channel_request_x11(channel,single_connection,
        protocol, cookie, screen_number);
    ssh_throw(err);
    return err;
  }
  void_throwable sendEof(){
    int err=ssh_channel_send_eof(channel);
    ssh_throw(err);
    return_throwable;
  }
  /** @brief Writes on a channel
   * @param data data to write.
   * @param len number of bytes to write.
   * @param is_stderr write should be done on the stderr channel (server only)
   * @returns number of bytes written
   * @throws SshException in case of error
   * @see channel_write
   * @see channel_write_stderr
   */
  int write(const void *data, size_t len, bool is_stderr=false){
    int ret;
    if(is_stderr){
      ret=ssh_channel_write_stderr(channel,data,len);
    } else {
      ret=ssh_channel_write(channel,data,len);
    }
    ssh_throw(ret);
    return ret;
  }
private:
  ssh_session getCSession(){
    return session->getCSession();
  }
  Channel (Session &session, ssh_channel c_channel){
    this->channel=c_channel;
    this->session=&session;
  }
  Session *session;
  ssh_channel channel;
  /* No copy and no = operator */
  Channel(const Channel &);
  Channel &operator=(const Channel &);
};

} // namespace ssh

/** @} */
#endif /* LIBSSHPP_HPP_ */
