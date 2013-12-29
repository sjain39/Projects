#!/bin/bash

sudo apt-get -y install python-pip git
wget https://pypi.python.org/packages/source/l/ltprotocol/ltprotocol-0.2.1.tar.gz
sudo pip install ./ltprotocol-0.2.1.tar.gz

cd pox_module
sudo python setup.py develop

pkill -9 sr_solution
pkill -9 sr

