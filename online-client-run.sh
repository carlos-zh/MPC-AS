#!/bin/bash

if [ $# -lt 4 ]; then
    echo "Usage: $0 <statistic_name> <key_param> <is_valid> <client_number>"
    exit 1
fi

statistic_name=$1
key_param=$2
is_valid=$3
client_number=$4

echo "Starting online benchmarking client..."
./MPC-AS-client.x 0 2 1 $statistic_name $key_param $is_valid $client_number
