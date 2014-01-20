#!/bin/bash

make clean
tar --exclude=src/jvm --exclude=src/tags --exclude=src/session.vim -czf wz292583.tgz src bin README Makefile latc-x86.sh
