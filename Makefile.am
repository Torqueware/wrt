AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = autogen.sh examples
.PHONY: wrt-images wrt-certs build-wrt-images

MOSTLYCLEANFILES = ./images/*

# HUMAN-BUILT PROJECT DIRECTORIES:
#	examples doc man src scripts test
#
# NOTE: tests should always be built last
SUBDIRS = doc man src scripts . test

wrt-images: build-wrt-images
	@echo 'Moving built images to a local directory "images".'
	-@mkdir -p images
	-@mv scripts/wrt-image/*.bin ./images 
	-@mv scripts/wrt-image/*.img ./images

build-wrt-images: wrt-certs
	@echo 'Building WRT system images.'
	@make -C scripts -C wrt-image images
	

wrt-certs:
	@echo 'Generating WRT System auth certificates.'
	@make -C scripts -C cert
