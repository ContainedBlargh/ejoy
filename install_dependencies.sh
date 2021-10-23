#!/bin/bash

sudo apt install -y build-essential wget libcurl4-openssl-dev libssl-dev

git clone https://github.com/cee-studio/orca.git
cd orca
make
sudo make install

