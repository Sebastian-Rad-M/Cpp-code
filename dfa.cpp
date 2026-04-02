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

class DFA {
public:
    int initial_state;
    vector<int> final_states;
    vector<string> alphabet;
    unordered_map<int, vector<Edge>> adj;

    DFA() : initial_state(-1) {}

    bool is_final(int state) const {
        return find(final_states.begin(), final_states.end(), state) != final_states.end();
    }

    bool is_in_alphabet(const string& s) const {
        return find(alphabet.begin(), alphabet.end(), s) != alphabet.end();
    }

    bool run(const string& input) const {
        int current_state = initial_state;
        size_t pos = 0;
        while (pos < input.length()) {
            bool found = false;
            for (size_t len = input.length() - pos; len > 0; len--) {
                string sub = input.substr(pos, len);
                if (is_in_alphabet(sub)) {
                    auto it = adj.find(current_state);
                    if (it != adj.end()) 
                        for (const auto& edge : it->second) 
                            if (edge.label == sub) {
                                current_state = edge.to;
                                pos += len;
                                found = true;
                                break;
                            }
                        
                    
                    if (found) break;
                }
            }
            if (!found) return false;
        }
        return is_final(current_state);
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

bool read_dfa(const string& filename, DFA& dfa) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    if (!getline(file, line)) return false;
    dfa.alphabet = split(line, ',');

    if (!getline(file, line)) return false;
    try {
        dfa.initial_state = stoi(trim(line));
    } catch (...) { return false; }

    if (!getline(file, line)) return false;
    for (const string& s : split(line, ',')) {
        try {
            dfa.final_states.push_back(stoi(s));
        } catch (...) { return false; }
    }

    while (getline(file, line)) {
        vector<string> parts = split(line, ',');
        if (parts.size() < 3) continue;
        try {
            int from = stoi(parts[0]);
            int to = stoi(parts[1]);
            string label = parts[2];
            dfa.adj[from].push_back({from, to, label});
        } catch (...) { return false; }
    }
    return true;
}

int main() {
    DFA dfa;
   read_dfa("dfa.txt", dfa);

    cout << "DFA (dfa.txt):" << endl;
    string tests[] = {"abaa", "abbb", "aaaa",};
    for (const string& s : tests) 
        cout << s << ": " << (dfa.run(s) ? "true" : "false") << endl;

    return 0;
}
