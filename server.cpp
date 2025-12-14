#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "thread_pool.h"
#include "md5_util.h"

#define PORT 8888
#define BUFFER_SIZE 1024

void handle_client(int client_socket, const std::string& client_ip) {
    std::cout << "New connection from: " << client_ip << std::endl;

    char buffer[BUFFER_SIZE] = {0};
    ssize_t valread = read(client_socket, buffer, BUFFER_SIZE);
    
    if (valread > 0) {
        std::cout << "Received: " << buffer << std::endl;
        
        // 这里是简单的逻辑，你需要替换为你的文件接收逻辑
        // 例如：接收文件名 -> 接收文件大小 -> 接收数据块 -> 校验MD5
        
        // 模拟回复
        std::string response = "Server received: " + std::string(buffer);
        send(client_socket, response.c_str(), response.length(), 0);
    }

    close(client_socket);
    std::cout << "Connection closed: " << client_ip << std::endl;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // 创建线程池，假设使用 4 个线程
    ThreadPool pool(4);

    // 1. 创建 socket 文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. 设置 socket 选项，允许端口复用
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 监听所有网卡
    address.sin_port = htons(PORT);

    // 3. 绑定端口
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 4. 开始监听
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    // 5. 循环 accept
    while (true) {
        int new_socket;
        // 等待客户端连接
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue; // 继续等待下一个连接，不退出
        }

        std::string client_ip = inet_ntoa(address.sin_addr);

        // 将任务提交给线程池
        pool.enqueue([new_socket, client_ip]() {
            handle_client(new_socket, client_ip);
        });
    }

    close(server_fd);
    return 0;
}
