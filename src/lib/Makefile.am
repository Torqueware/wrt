SUBDIRS = wrt .

noinst_LTLIBRARIES = libwrt.la
libwrt_la_SOURCES = 
#libssh_la_SOURCES =
#Dummy lines to hint C++ linking
nodist_EXTRA_libwrt_la_SOURCES = cpp_wrt.cxx
#nodist_EXTRA_libssh_la_SOURCES = cpp_ssh.cxx
#Libraries in subdirs
libwrt_la_LIBADD = wrt/libwrt_ap.la wrt/libwrt_io.la wrt/libwrt_config.la
#libssh_la_LIBADD = ssh/libssh_exception.la \
#                   ssh/libssh_session.la   \
#                   ssh/libssh_keys.la
#                   ssh/libssh_channel.la

