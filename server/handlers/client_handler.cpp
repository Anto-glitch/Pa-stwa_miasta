
#include "client_handler.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <mutex>

using namespace std;

void handleClient(int clientId, vector<vector<string>> &messages,
                  mutex &message_mutex, char random_letter,
                  vector<string> categories) 
{

  vector<string> answers;
  string accumulated_text = "";
  char buffer[1024];
  int turns = 0;
  bool client_closed = false;

  while (turns < categories.size() && !client_closed) {

    string prompt =
        "Podaj " + categories[turns] + " (kazde zatwierdz enterem):\n";
    write_to_socket(clientId, prompt);

    ssize_t bytes_read = read_from_socket(clientId, buffer, sizeof(buffer) - 1);

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