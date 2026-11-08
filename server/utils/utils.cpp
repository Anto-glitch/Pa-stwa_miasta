#include "utils.hpp"
#include <string_view>
#include <random>
#include <vector>
#include <string>
#include <iostream>
#include <cstddef>
#include <cctype>
#include <algorithm>

using namespace std;

std::string capitalize(std::string str) {
    if (str.empty()) {
        return str;
    }

    str[0] = std::toupper(static_cast<unsigned char>(str[0]));

    std::transform(str.begin() + 1, str.end(), str.begin() + 1,
                   [](unsigned char c) { return std::tolower(c); });

    return str;
}

char generateRandomChar(){
    static constexpr string_view possibleCharacters = "ABCDEFGHIJKLMNOPRSTUWZ";

    static random_device rd;
    static mt19937 gen(rd());

    uniform_int_distribution<size_t> dist(0, possibleCharacters.size() - 1);

    return possibleCharacters[dist(gen)];
}

void display_answers(vector<vector<string>> &messages) {
    for (size_t i = 0; i < messages.size(); i++) {
        if (messages[i].empty()) continue;
        cout << "Gracz " << messages[i][0] << " (nr " << i + 1 << "): ";
        for (size_t j = 1; j < messages[i].size(); j++) {
            cout << "[" << messages[i][j] << "] ";
        }
        cout << endl;
    }
}