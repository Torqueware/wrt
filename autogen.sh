#!/bin/sh
# This file (script) automagically calls autotools for a rebuild

autoreconf --install --force --verbose $@ 

./configure --prefix=''
