
// network_client.hpp
#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include "socket_message.hpp"

class NetworkClient {
private:
    int socket_id = -1;
    std::thread reader_thread;
    std::atomic<bool> running{false};

    std::queue<std::string> incoming_messages;
    std::mutex queue_mutex;

    bool readLine(std::string& accumulated, std::string& out_line) {
        char buffer[1024];
        while (true) {
            size_t pos = accumulated.find('\n');
            if (pos != std::string::npos) {
                out_line = accumulated.substr(0, pos);
                if (!out_line.empty() && out_line.back() == '\r') out_line.pop_back();
                accumulated.erase(0, pos + 1);
                return true;
            }
            ssize_t bytes = read_from_socket(socket_id, buffer, sizeof(buffer) - 1);
            if (bytes <= 0) return false;
            accumulated.append(buffer, bytes);
        }
    }

    void listenLoop() {
        std::string accumulated;
        std::string line;
        while (running && readLine(accumulated, line)) {
            std::lock_guard<std::mutex> lock(queue_mutex);
            incoming_messages.push(line);
        }
        running = false;
    }

public:
    void startListening(int sock) {
        socket_id = sock;
        running = true;
        reader_thread = std::thread(&NetworkClient::listenLoop, this);
        reader_thread.detach();
    }

    void sendLine(const std::string& msg) {
        if (socket_id != -1) {
            write_to_socket(socket_id, msg + "\n");
        }
    }

    bool pollMessage(std::string& out_msg) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (incoming_messages.empty()) return false;
        out_msg = incoming_messages.front();
        incoming_messages.pop();
        return true;
    }
};