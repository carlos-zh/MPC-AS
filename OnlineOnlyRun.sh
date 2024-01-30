#!/bin/bash

# 检查是否有足够的参数
if [ $# -lt 3 ]; then
    echo "Usage: $0 <statistic_name> <key_param> <is_valid>"
    exit 1
fi

# 获取命令行参数
statistic_name=$1
# key_param   the number of client values for LR, encoding length for other statistics
key_param=$2
is_valid=$3


# 定义要检查的路径
output_dir="Output"
# 检查Output路径是否存在
if [ ! -d "$output_dir" ]; then
    # 如果不存在，则创建该路径
    mkdir "$output_dir"
    echo "Path $output_dir created"
else
    echo "Path $output_dir already exists"
fi

# 设置玩家数量环境变量
export PLAYERS=2
# 定义输出文件路径
output_file_server="./Output/server_output.txt"
output_file_client="./Output/client_output.txt"
# 运行在线基准测试脚本，并将输出重定向到文件
echo "Starting online benchmarking server..."

# 使用case语句进行判断
case "$1" in
    "S")
        echo "Starting SUM computation..."
        Scripts/run-online.sh MPC-AS-SUM-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    "V")
        echo "Starting VAR computation..."
        Scripts/run-online.sh MPC-AS-VAR-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    "F")
        echo "Starting FRQ computation..."
        Scripts/run-online.sh MPC-AS-FRQ-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    "L")
        echo "Starting LR computation..."
        Scripts/run-online.sh MPC-AS-LR-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    "M")
        echo "Starting MAX computation..."
        Scripts/run-online.sh MPC-AS-MAX-$key_param-1 > "$output_file_server" 2>&1 &
        server_pid=$!
        ;;
    *)
        echo "Invalid argument: $1"
        echo "Usage: $0 [S|V|F|L|M]"
        exit 1
        ;;
esac


# Scripts/run-online.sh MPC-AS-$statistic_name-$key_param-1 > "$output_file_server" 2>&1 &
# server_pid=$!


# 等待服务器启动
sleep 1
# 运行客户端可执行文件，并将输出重定向到文件
echo "Starting online benchmarking client..."
./MPC-AS-client.x 0 2 1 $statistic_name $key_param $is_valid > "$output_file_client" 2>&1 &
client_pid=$!
# 等待所有后台作业完成
wait $server_pid
wait $client_pid
echo "Online benchmarking completed."