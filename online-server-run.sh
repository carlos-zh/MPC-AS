#!/bin/bash

if [ $# -lt 3 ]; then
    echo "Usage: $0 <statistic_name> <key_param> <client_number>"
    exit 1
fi

statistic_name=$1
key_param=$2
client_number=$3

echo "Starting online benchmarking server..."

case "$1" in
    "S")
        echo "Starting SUM computation..."
        Scripts/run-online.sh MPC-AS-SUM-$key_param-1-$client_number
        ;;
    "V")
        echo "Starting VAR computation..."
        Scripts/run-online.sh MPC-AS-VAR-$key_param-1-$client_number
        ;;
    "F")
        echo "Starting FRQ computation..."
        Scripts/run-online.sh MPC-AS-FRQ-$key_param-1-$client_number
        ;;
    "L")
        echo "Starting LR computation..."
        Scripts/run-online.sh MPC-AS-LR-$key_param-1-$client_number
        ;;
    "M")
        echo "Starting MAX computation..."
        Scripts/run-online.sh MPC-AS-MAX-$key_param-1-$client_number
        ;;
    *)
        echo "Invalid argument: $1"
        echo "Usage: $0 [S|V|F|L|M]"
        exit 1
        ;;
esac