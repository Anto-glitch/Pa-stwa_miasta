#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include "systems/server_connection.hpp"
#include "utils/utills.hpp"

using namespace std;



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

  int serverId = create_server_socket(3333);
  if (serverId == -1) {
    return 1;
  }

  vector<vector<string>> messages;
  mutex message_mutex;

  while (true) {
    int clientId = accept_client(serverId);
    if (clientId == -1) {
      continue;
    }

    thread t1(handleClient, clientId, ref(messages), ref(message_mutex),
              random_letter, ref(categories));
    t1.detach();
  }
  close_socket(serverId);

  return 0;
}