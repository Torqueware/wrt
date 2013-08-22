/***********************************************************************
 * wrt.cxx                                                             *
 *                                                                     *
 * Copyright 2013 William Patrick Millard <wmillard@rack1>             *
 *                                                                     *
 * This file is the main driver program for the WRT config system.     *
 * As of now its singular purpose is to push configuration settings    *
 * over SSH using libssh                                               *
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

#include <cstdlib>
#include <iostream>
#include <exception>
#include <libssh/libssh.h>
#include <libssh/libsshpp.hpp>

#define WRT_USER_NAME      "root"
#define WRT_CERT_DIRECTORY "./cert/"

using namespace std;

int main(int argc, char **argv)
{
  ssh::Session session;

  try {
    if(argc > 1) {
    /* These lines control the ssh session options (username, ect) */
      session.setOption(SSH_OPTIONS_HOST, argv[1]);
      session.setOption(SSH_OPTIONS_USER, WRT_USER_NAME);
      session.setOption(SSH_OPTIONS_SSH_DIR, WRT_CERT_DIRECTORY);
      /* Other options go here... */

    } else {
    cout << *argv << ": No host specified" << endl;
    exit(EXIT_SUCCESS);
  }

  session.connect();
  session.userauthPublickeyAuto();

  switch (session.isServerKnown()) {
    case SSH_SERVER_KNOWN_OK:
      break; /* Host is verified */

    /* possible man in the middle attack*/
      case SSH_SERVER_KNOWN_CHANGED:
        cout << *argv << ": Remote host public key changed!" << endl;
        cout << *argv << ": Terminating for security reasons." << endl;
        exit(-1);

      /* possible man in the middle attack*/
      case SSH_SERVER_FOUND_OTHER:
        cout << *argv << ": Remote host public key not found, ";
        cout << "but another key exists!" << endl;
        cout << *argv << ": Terminating for security reasons." << endl;
        exit(-1);

      case SSH_SERVER_FILE_NOT_FOUND:
        /* Doesn't matter - We will make our own below. (falldown) */
      case SSH_SERVER_NOT_KNOWN:
        session.writeKnownhost();
        break;

      case SSH_SERVER_ERROR:
        cout << *argv << ": Remote server error! Terminating." << endl;
        cout << *argv << ": Terminating." << endl;
        exit(-1);
    }

    cout << *argv << "SUCCESS!" << endl;

    session.disconnect();

  } catch (ssh::SshException exception) {
    cout << *argv << ": Error during connection: ";
    cout << exception.getError() << endl;
  }

  return EXIT_SUCCESS;
}
