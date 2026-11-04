#include "server_connection.hpp"
#include "utils/utils.hpp"

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using namespace std;

int create_server_socket(int port_number) {
    const int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return -1;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, 1) == -1) {
        perror("Listen failed");
        close(server_socket);
        return -1;
    }
    return server_socket;
}

int accept_client(int server_socket) {
    const int client_socket = accept(server_socket, nullptr, nullptr);
    if (client_socket == -1) {
        perror("Accept failed");
    }
    return client_socket;
}

void close_socket(int socket) {
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, char random_letter,
                  vector<string> categories) {

  vector<string> answers;
  string accumulated_text = "";
  char buffer[1024];
  int turns = 0;
  bool client_closed = false;

  string letter_announce = "Wylosowana litera: " + string(1, random_letter) + "\n";
  write(clientId, letter_announce.c_str(), letter_announce.length());

  while (turns < categories.size() && !client_closed) {

    string prompt =
        "Podaj " + categories[turns] + " (kazde zatwierdz enterem):\n";
    write(clientId, prompt.c_str(), prompt.length());

    ssize_t bytes_read = read(clientId, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
      if (bytes_read == -1)
        perror("Read failed");
      break;
    }

    accumulated_text.append(buffer, bytes_read);

    size_t pos;
    while ((pos = accumulated_text.find('\n')) != string::npos) {
      string line = accumulated_text.substr(0, pos);

      if (!line.empty() && line.back() == '\r') {
        line.pop_back();
      }

      if (line == "close") {
        client_closed = true;
        break;
      }

      accumulated_text.erase(0, pos + 1);

      if (line.empty()) {
        break;
      }

      if (line[0] >= 'a' && line[0] <= 'z') {
        line[0] -= 32;
      }

      if (line[0] != random_letter) {
        break;
      }

      answers.push_back(line);
      turns++;

      if (turns >= categories.size())
        break;
    }
  }

// część diagnostyczna

  if (!answers.empty()) {
    lock_guard<mutex> lock(message_mutex);
    messages.push_back(answers);

    cout << "Ilosc zatwierdzonych pakietow odpowiedzi: " << messages.size()
         << endl;
    cout << "Zestawienie odpowiedzi wszystkich graczy: " << endl;

    display_answers(messages);
  }

  close(clientId);
}
