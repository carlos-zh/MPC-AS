#!/bin/bash

if [ $# -lt 3 ]; then
    echo "Usage: $0 <statistic_name> <key_param> <is_valid>"
    exit 1
fi

statistic_name=$1
# key_param   the number of client values for LR, encoding length for other statistics
key_param=$2
is_valid=$3

output_dir="Output"

if [ ! -d "$output_dir" ]; then

    mkdir "$output_dir"
    echo "Path $output_dir created"
else
    echo "Path $output_dir already exists"
fi

export PLAYERS=2

output_file_server="./Output/server_output.txt"
output_file_client="./Output/client_output.txt"

echo "Starting online and offline benchmarking server..."

case "$1" in
    "S")
        echo "Starting SUM computation..."
        Scripts/highgear.sh MPC-AS-SUM-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    "V")
        echo "Starting VAR computation..."
        Scripts/highgear.sh MPC-AS-VAR-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    "F")
        echo "Starting FRQ computation..."
        Scripts/highgear.sh MPC-AS-FRQ-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    "L")
        echo "Starting LR computation..."
        Scripts/highgear.sh MPC-AS-LR-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    "M")
        echo "Starting MAX computation..."
        Scripts/highgear.sh MPC-AS-MAX-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    *)
        echo "Invalid argument: $1"
        echo "Usage: $0 [S|V|F|L|M]"
        exit 1
        ;;
esac

sleep 1

echo "Starting online and offline benchmarking client..."
./MPC-AS-client.x 0 2 1 $statistic_name $key_param $is_valid > "$output_file_client" 2>&1 &
client_pid=$!

wait $server_pid
wait $client_pid
echo "Online and offline benchmarking completed."