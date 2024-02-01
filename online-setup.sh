#!/bin/bash

make clean

make -j 8 tldr && make -j 8 online

make Fake-Offline.x highgear-party.x

Scripts/setup-online.sh && ./Fake-Offline.x 2 -lgp 48

Scripts/setup-ssl.sh 2 && Scripts/setup-clients.sh 3
