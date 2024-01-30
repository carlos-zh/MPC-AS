#!/bin/bash

# 检查是否有足够的参数
if [ $# -lt 2 ]; then
    echo "Usage: $0 <statistic_name> <key_param>"
    exit 1
fi

# 获取命令行参数
statistic_name=$1
# key_param   the number of client values for LR, encoding length for other statistics
key_param=$2

echo "Starting online and offline benchmarking protocol compilation..."

make MPC-AS-client.x

# 使用case语句进行判断
case "$1" in
    "S")
        echo "Starting SUM compilation..."
        ./compile.py -P 140737488486401 MPC-AS-SUM-$key_param 1
        ;;
    "V")
        echo "Starting VAR compilation..."
        ./compile.py -P 140737488486401 MPC-AS-VAR-$key_param 1
        ;;
    "F")
        echo "Starting FRQ compilation..."
        ./compile.py -P 140737488486401 MPC-AS-FRQ-$key_param 1
        ;;
    "L")
        echo "Starting LR compilation..."
        ./compile.py -P 140737488486401 MPC-AS-LR-$key_param 1
        ;;
    "M")
        echo "Starting MAX compilation..."
        ./compile.py -P 140737488486401 MPC-AS-MAX-$key_param 1
        ;;
    *)
        echo "Invalid argument: $1"
        echo "Usage: $0 [S|V|F|L|M]"
        exit 1
        ;;
esac


echo "Online and offline benchmarking protocol compilation completed."