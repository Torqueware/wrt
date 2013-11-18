SUBDIRS = include lib

bin_PROGRAMS = wrt WRTd
wrt_SOURCES  = main.cxx main.hxx
wrt_LDADD    = lib/libwrt.la -lconfig++ -L/usr/lib

WRTd:
	@echo 'WRT: Generating WRT Daemon script'	
	@echo 					>> ./WRTd
	@echo '#!/bin/sh' 			>> ./WRTd
	@echo 'while true; do' 			>> ./WRTd
	@echo 'wrt -p > /dev/null 2>&1 &'	>> ./WRTd
	@echo 'sleep 60'			>> ./WRTd
	@echo 'kill -TERM $!'			>> ./WRTd
	@echo 'done'				>> ./WRTd
	@echo					>> ./WRTd
	@echo 'WRT: Ensuring proper permissions'
	chmod 700 ./WRTd

