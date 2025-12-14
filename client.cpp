#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <vector>
#include "md5_util.h"

#define PORT 8888
#define SERVER_IP "127.0.0.1" // 本地回环地址，如在不同机器请修改
#define BUFFER_SIZE 1024

void send_data(int sock, const std::string& data) {
    send(sock, data.c_str(), data.length(), 0);
    std::cout << "Sent: " << data << std::endl;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // 1. 创建 socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 2. 转换 IP 地址
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    // 3. 连接服务器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
        return -1;
    }

    // 4. 发送数据 (这里是简单的字符串，你需要替换为文件分片发送逻辑)
    std::string message = "Hello from client! Let's transfer a file.";
    send_data(sock, message);

    // 5. 接收服务器回复
    read(sock, buffer, BUFFER_SIZE);
    std::cout << "Server response: " << buffer << std::endl;

    // 关闭连接
    close(sock);
    return 0;
}
