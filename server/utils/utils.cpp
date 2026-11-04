#include "utils.hpp"
#include <string_view>
#include <random>
#include <vector>
#include <string>
#include <iostream>
#include <cstddef>
#include <array>

using namespace std;

char generateRandomChar(){
    static constexpr string_view possibleCharacters = "ABCDEFGHIJKLMNOPRSTUWZ";

    static random_device rd;
    static mt19937 gen(rd());

    uniform_int_distribution<size_t> dist(0, possibleCharacters.size() - 1);

    return possibleCharacters[dist(gen)];
}

void display_answers(vector<vector<string>> &messages) {
    for (size_t i = 0; i < messages.size(); i++) {
        cout << "Gracz " << i + 1 << ": ";
        for (size_t j = 0; j < messages[i].size(); j++) {
            cout << "[" << messages[i][j] << "] ";
        }
        cout << endl;
    }
}
