#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <mutex>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

void handleClient(int clientId, vector<string> &messages,
                  mutex &message_mutex) {
  string receive = "";
  char buffer[1024];
  while (true) {
    ssize_t bytes_read = read(clientId, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
      if (bytes_read == -1)
        perror("Read failed");
      break;
    }

    receive.assign(buffer, bytes_read);

    while (!receive.empty() &&
           (receive.back() == '\n' || receive.back() == '\r'))
      receive.pop_back();

    if (receive == "close") {
      break;
    }

    {
      lock_guard<mutex> lock(message_mutex);
      messages.push_back(receive);
      cout << "odebrano wiadomosc: " << receive << ", razem " << messages.size() << " wiadomosci" << endl;
    }
  }
  close(clientId);
}

int main(void) {
  int serverId = socket(PF_INET, SOCK_STREAM, 0);
  if (serverId == -1) {
    perror("Socket creation failed");
    return 1;
  }

  int opt = 1;
  setsockopt(serverId, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(3333);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(serverId, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("Bind failed");
    return 1;
  }
  if (listen(serverId, 1) == -1) {
    perror("Listen failed");
    close(serverId);
    return 1;
  }

  vector<string> messages;
  mutex message_mutex;

  while (true) {
    int clientId = accept(serverId, nullptr, nullptr);
    if (clientId == -1) {
      perror("Accept failed");
      continue;
    }

    thread t1(handleClient, clientId, ref(messages), ref(message_mutex));
    t1.detach();
  }
  close(serverId);

  return 0;
}