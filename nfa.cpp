#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

using namespace std;

struct Edge {
    int from;
    int to;
    string label;
};

class NFA {
public:
    int initial_state;
    vector<int> final_states;
    vector<string> alphabet;
    unordered_map<int, vector<Edge>> adj;

    NFA() : initial_state(-1) {}

    bool is_final(int state) const {
        return find(final_states.begin(), final_states.end(), state) != final_states.end();
    }

    bool is_in_alphabet(const string& s) const {
        return find(alphabet.begin(), alphabet.end(), s) != alphabet.end();
    }

    bool run(const string& input) const {
        unordered_set<int> current_states;
        current_states.insert(initial_state);

        size_t pos = 0;
        while (pos < input.length()) {
            unordered_set<int> next_states;
            bool found_symbol = false;

            for (size_t len = input.length() - pos; len > 0; --len) {
                string sub = input.substr(pos, len);
                if (is_in_alphabet(sub)) {
                    for (int s : current_states) {
                        auto it = adj.find(s);
                        if (it != adj.end()) {
                            for (const auto& edge : it->second) {
                                if (edge.label == sub) {
                                    next_states.insert(edge.to);
                                }
                            }
                        }
                    }
                    if (!next_states.empty()) {
                        pos += len;
                        found_symbol = true;
                        break;
                    }
                }
            }
            if (!found_symbol) return false;
            current_states = next_states;
        }

        for (int s : current_states) {
            if (is_final(s)) return true;
        }
        return false;
    }
};

string trim(const string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

vector<string> split(const string& s, char delim) {
    vector<string> result;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        string t = trim(item);
        if (!t.empty()) result.push_back(t);
    }
    return result;
}

bool read_nfa(const string& filename, NFA& nfa) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    if (!getline(file, line)) return false;
    nfa.alphabet = split(line, ',');

    if (!getline(file, line)) return false;
    try {
        nfa.initial_state = stoi(trim(line));
    } catch (...) { return false; }

    if (!getline(file, line)) return false;
    for (const string& s : split(line, ',')) {
        try {
            nfa.final_states.push_back(stoi(s));
        } catch (...) { return false; }
    }

    while (getline(file, line)) {
        vector<string> parts = split(line, ',');
        if (parts.size() < 3) continue;
        try {
            int from = stoi(parts[0]);
            int to = stoi(parts[1]);
            string label = parts[2];
            nfa.adj[from].push_back({from, to, label});
        } catch (...) { return false; }
    }
    return true;
}

int main() {
    NFA nfa;
    read_nfa("nfa.txt", nfa);

    cout << "NFA (nfa.txt):" << endl;
    string tests[] = {"abc", "aaabbb", "aaaccc","aaa"};
    for (const string& s : tests)  cout << s << ": " << (nfa.run(s) ? "true" : "false") << endl;

    return 0;
}
