#!/bin/bash

make clean

make -j 8 tldr && make -j8 highgear-party.x

Scripts/setup-ssl.sh 2 && Scripts/setup-clients.sh 3