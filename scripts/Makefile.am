wrtconfdir = $(sysconfdir)/$(PACKAGE)

SUBDIRS = cert config radio wrt-image
nobase_dist_wrtconf_DATA = wrt.cfg         \
			   config/dhcp     \
			   config/dhcp6c   \
			   config/dropbear \
			   config/firewall \
			   config/network  \
			   config/radvd    \
			   config/system   \
			   radio/wr703n    \
			   radio/mr3020    \
			   radio/wrt54g    \
			   radio/whrhpg300n

wrt.cfg:
	@echo "WRT: WARNING: NO DEFAULT WRT CONFIG FILE, MAKING EMPTY FILE"
	touch ./wrt.cfg

