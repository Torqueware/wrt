#!/bin/sh
# This file (script) automagically calls autotools for a rebuild

autoreconf --install --force --verbose -I m4 $@ 

./configure --prefix=''
