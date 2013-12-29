#!/bin/bash

pkill -f pox.py
./run_pox.sh &

sudo python hw8.py
pkill -f pox.py
