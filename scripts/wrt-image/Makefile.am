noinst_DATA: images
IMAGE_LIST = ar71xx
AR71XX_IMAGES = tl-mr3020 tl-wr703n
AR71XX_DEPS = ar71xx-imagebuilder/files ar71xx-imagebuilder 
.PHONY: images $(IMAGE_LIST) $(AR71XX_IMAGES)

CLEANFILES = $(AR71XX_DEPS)
MOSTLYCLEANFILES = passwd shadow
EXTRA_DIST = ar71xx-imagebuilder.tar.bz2

images: $(IMAGE_LIST)

ar71xx: $(AR71XX_IMAGES)

tl-mr3020: $(AR71XX_DEPS)
	@make -C ar71xx-imagebuilder image PROFILE=TLMR3020 FILES=files/
	-@cp ar71xx-imagebuilder/bin/ar71xx/openwrt-ar71xx-generic-tl-mr3020-* .

tl-wr703n: $(AR71XX_DEPS)
	@make -C ar71xx-imagebuilder image PROFILE=TLWR703 FILES=files/
	-@cp ar71xx-imagebuilder/bin/ar71xx/openwrt-ar71xx-generic-tl-wr703n-* .

ar71xx-imagebuilder/files: passwd shadow
	@mkdir -p ar71xx-imagebuilder/files/etc/dropbear
	@cp passwd ar71xx-imagebuilder/files/etc
	@cp shadow ar71xx-imagebuilder/files/etc
	@cp ../cert/id_*.pub ar71xx-imagebuilder/files/etc/dropbear/authorized_keys

shadow: passwd
	@echo -n 'root:' >> ./shadow
	@mkpasswd -m md5 | tr -d [:space:] >> ./shadow
	@echo ':1:0:99999:7:::' >> ./shadow
	@echo 'daemon:*:0:0:99999:7:::' >> ./shadow
	@echo 'ftp:*:0:0:99999:7:::' >> ./shadow
	@echo 'network:*:0:0:99999:7:::' >> ./shadow
	@echo 'nobody:*:0:0:99999:7:::' >> ./shadow
	@chmod 600 shadow

passwd:
	@echo 'WRT: Generating default password file'
	@echo 'root:x:0:0:root:/root:/bin/ash' >> ./passwd
	@echo 'daemon:*:1:1:daemon:/var:/bin/false' >> ./passwd
	@echo 'ftp:*:55:55:ftp:/home/ftp:/bin/false' >> ./passwd
	@echo 'network:*:101:101:network:/var:/bin/false' >> ./passwd
	@echo 'nobody:*:65534:65534:nobody:/var:/bin/false' >> ./passwd

ar71xx-imagebuilder:
	@tar xfo ar71xx-imagebuilder.tar.bz2

