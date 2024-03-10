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
source ~/osbook/devenv/buildenv.sh
cd ~/workspace/mikanos/kernel
make clean
cd ~/workspace/mikanos
./build.sh

if [ $? -ne 0 ];
then
  echo "error 2" > 2
  exit 1
fi
