#!/bin/bash

# ブートローダのビルド
cd ~/edk2
source edksetup.sh
build

if [ $? -ne 0 ];
then
  echo "error 1" > 2
  exit 1
fi

# カーネルのビルド
cd ~/workspace/mikanos/kernel
source ~/osbook/devenv/buildenv.sh
make clean
make

if [ $? -ne 0 ];
then
  echo "error 2" > 2
  exit 1
fi
