wrtconfdir = $(sysconfdir)/$(PACKAGE)
wrtinitdir = $(sysconfdir)/init.d

SUBDIRS = cert config wrt-image

nobase_dist_wrtconf_DATA = wrt.cfg         \
			   ssh_config      \
			   config/6relayd  \
			   config/dhcp     \
			   config/dhcp6c   \
			   config/dropbear \
			   config/firewall \
			   config/radvd    

dist_wrtinit_DATA	 = WRTd.sh

wrt.cfg:
	@echo "WRT: WARNING: NO DEFAULT WRT CONFIG FILE, MAKING EMPTY FILE"
	touch ./wrt.cfg

ssh_config:
	@echo "WRT: WARNING: NO DEFAULT SSH CONFIG FILE, MAKING EMPTY FILE"
	touch ./wrt.cfg


