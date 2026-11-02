#include <cstddef>
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

void display_answers(vector<vector<string>> &messages) {
  for (size_t i = 0; i < messages.size(); i++) {
    cout << "Gracz " << i + 1 << ": ";
    for (size_t j = 0; j < messages[i].size(); j++) {
      cout << "[" << messages[i][j] << "] ";
    }
    cout << endl;
  }
}

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, char random_letter,
                  vector<string> categories) {

  vector<string> answers;
  string accumulated_text = "";
  char buffer[1024];
  int turns = 0;
  bool client_closed = false;

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

      if (line.empty() || line[0] != random_letter) {
        break;
      }

      answers.push_back(line);
      turns++;

      if (turns >= categories.size())
        break;
    }
  }

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

int main(void) {

  char random_letter = 'k';
  // wpisz wszystkie kategorie
  vector<string> categories = {"IMIE", "NAZWISKO", "MIASTO", "ROSLINA"};

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

  vector<vector<string>> messages;
  mutex message_mutex;

  while (true) {
    int clientId = accept(serverId, nullptr, nullptr);
    if (clientId == -1) {
      perror("Accept failed");
      continue;
    }

    thread t1(handleClient, clientId, ref(messages), ref(message_mutex),
              random_letter, ref(categories));
    t1.detach();
  }
  close(serverId);

  return 0;
}