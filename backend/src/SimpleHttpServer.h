#pragma once

#include <string>
#include <functional>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <map>
#include <vector>
#include <iostream>
#include <memory>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

class SimpleHttpServer {
public:
    using RequestHandler = std::function<std::string(const std::string&)>;
    
    SimpleHttpServer(int port = 8080) : port_(port), running_(false), server_socket_(INVALID_SOCKET) {}
    
    ~SimpleHttpServer() {
        if (running_) stop();
        if (server_socket_ != INVALID_SOCKET) {
            closesocket(server_socket_);
        }
        WSACleanup();
    }
    
    void registerRoute(const std::string& method, const std::string& path, RequestHandler handler) {
        routes_[method + " " + path] = handler;
    }
    
    bool start() {
        WSADATA wsa_data;
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return false;
        }
        
        server_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket_ == INVALID_SOCKET) {
            std::cerr << "socket failed" << std::endl;
            WSACleanup();
            return false;
        }
        
        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(port_);
        
        if (bind(server_socket_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            std::cerr << "bind failed" << std::endl;
            closesocket(server_socket_);
            WSACleanup();
            return false;
        }
        
        if (listen(server_socket_, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "listen failed" << std::endl;
            closesocket(server_socket_);
            WSACleanup();
            return false;
        }
        
        running_ = true;
        accept_thread_ = std::make_unique<std::thread>(&SimpleHttpServer::acceptConnections, this);
        
        std::cout << "Server listening on port " << port_ << std::endl;
        return true;
    }
    
    void stop() {
        running_ = false;
        if (accept_thread_ && accept_thread_->joinable()) {
            accept_thread_->join();
        }
    }
    
private:
    int port_;
    bool running_;
    SOCKET server_socket_;
    std::map<std::string, RequestHandler> routes_;
    std::unique_ptr<std::thread> accept_thread_;
    
    void acceptConnections() {
        while (running_) {
            sockaddr_in client_addr{};
            int client_addr_len = sizeof(client_addr);
            
            SOCKET client_socket = accept(server_socket_, (sockaddr*)&client_addr, &client_addr_len);
            if (client_socket == INVALID_SOCKET) {
                if (running_) {
                    std::cerr << "accept failed" << std::endl;
                }
                continue;
            }
            
            std::thread(&SimpleHttpServer::handleClient, this, client_socket).detach();
        }
    }
    
    void handleClient(SOCKET client_socket) {
        char buffer[8192] = {0};
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received > 0) {
            std::string request(buffer);
            std::string response = processRequest(request);
            
            send(client_socket, response.c_str(), response.length(), 0);
        }
        
        closesocket(client_socket);
    }
    
    std::string processRequest(const std::string& request) {
        // Parse request line
        std::istringstream stream(request);
        std::string method, path, version;
        stream >> method >> path >> version;
        
        // Extract just the path without query string
        size_t query_pos = path.find('?');
        if (query_pos != std::string::npos) {
            path = path.substr(0, query_pos);
        }
        
        std::string route_key = method + " " + path;
        
        if (routes_.find(route_key) != routes_.end()) {
            std::string body = routes_[route_key](request);
            return createHttpResponse(body);
        }
        
        return create404Response();
    }
    
    std::string createHttpResponse(const std::string& body) {
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: application/json\r\n";
        response << "Content-Length: " << body.length() << "\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << body;
        return response.str();
    }
    
    std::string create404Response() {
        std::string body = "{\"error\": \"Not Found\"}";
        std::ostringstream response;
        response << "HTTP/1.1 404 Not Found\r\n";
        response << "Content-Type: application/json\r\n";
        response << "Content-Length: " << body.length() << "\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << body;
        return response.str();
    }
};
