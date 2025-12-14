#!/bin/bash

# 检查 g++ 是否安装
if ! command -v g++ &> /dev/null; then
    echo "Error: g++ compiler not found. Please install it first."
    exit 1
fi

echo "正在编译服务端..."
g++ server.cpp -o server -lpthread -lssl -lcrypto

echo "正在编译客户端..."
g++ client.cpp -o client -lpthread -lssl -lcrypto

if [ -f "server" ] && [ -f "client" ]; then
    echo "编译成功！生成了 server 和 client 可执行文件。"
else
    echo "编译失败，请检查代码是否完整。"
fi
