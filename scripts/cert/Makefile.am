wrtconfdir = $(sysconfdir)/$(PACKAGE)

wrtconf_DATA = id_dsa id_dsa.pub

#If we clean - kill the keys
CLEANFILES = id_dsa id_dsa.pub

#Build the dsa keys
id_dsa.pub: id_dsa
id_dsa: ssh-keygen -q -t dsa -N '' -f id_dsa
