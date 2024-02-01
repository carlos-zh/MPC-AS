#!/bin/bash

if [ $# -lt 3 ]; then
    echo "Usage: $0 <statistic_name> <key_param> <client_number>"
    exit 1
fi

statistic_name=$1
key_param=$2
client_number=$3

echo "Starting online benchmarking compilation..."

make MPC-AS-client.x

case "$1" in
    "S")
        echo "Starting SUM compilation..."
        ./compile.py -P 140737488355333 MPC-AS-SUM-$key_param 1 $client_number
        ;;
    "V")
        echo "Starting VAR compilation..."
        ./compile.py -P 140737488355333 MPC-AS-VAR-$key_param 1 $client_number
        ;;
    "F")
        echo "Starting FRQ compilation..."
        ./compile.py -P 140737488355333 MPC-AS-FRQ-$key_param 1 $client_number
        ;;
    "L")
        echo "Starting LR compilation..."
        ./compile.py -P 140737488355333 MPC-AS-LR-$key_param 1 $client_number
        ;;
    "M")
        echo "Starting MAX compilation..."
        ./compile.py -P 140737488355333 MPC-AS-MAX-$key_param 1 $client_number
        ;;
    *)
        echo "Invalid argument: $1"
        echo "Usage: $0 [S|V|F|L|M]"
        exit 1
        ;;
esac


echo "Online benchmarking protocol compilation completed."