#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <stack>
using namespace std;

struct Edge {
    int from;
    int to;
    string label;
};

class LambdaNFA {
public:
    int initial_state;
    vector<int> final_states;
    vector<string> alphabet;
    unordered_map<int, vector<Edge>> adj;

    LambdaNFA() : initial_state(-1) {}

    bool is_final(int state) const {
        return find(final_states.begin(), final_states.end(), state) != final_states.end();
    }

    bool is_in_alphabet(const string& s) const {
        return find(alphabet.begin(), alphabet.end(), s) != alphabet.end();
    }

    unordered_set<int> epsilon_closure(const unordered_set<int>& states) const {
        unordered_set<int> closure = states;
        stack<int> s;
        for (int state : states) s.push(state);

        while (!s.empty()) {
            int current = s.top();
            s.pop();

            auto it = adj.find(current);
            if (it != adj.end()) {
                for (const auto& edge : it->second) {
                    if (edge.label.empty() || edge.label == "lambda" || edge.label == "epsilon") {
                        if (closure.find(edge.to) == closure.end()) {
                            closure.insert(edge.to);
                            s.push(edge.to);
                        }
                    }
                }
            }
        }
        return closure;
    }

    bool run(const string& input) const {
        unordered_set<int> current_states;
        current_states.insert(initial_state);
        current_states = epsilon_closure(current_states);

        size_t pos = 0;
        while (pos < input.length()) {
            unordered_set<int> next_states;
            bool found_symbol = false;

            for (size_t len = input.length() - pos; len > 0; --len) {
                string sub = input.substr(pos, len);
                if (is_in_alphabet(sub)) {
                    unordered_set<int> step_states;
                    for (int s : current_states) {
                        auto it = adj.find(s);
                        if (it != adj.end()) {
                            for (const auto& edge : it->second) {
                                if (edge.label == sub) {
                                    step_states.insert(edge.to);
                                }
                            }
                        }
                    }
                    if (!step_states.empty()) {
                        next_states = epsilon_closure(step_states);
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

bool read_lnfa(const string& filename, LambdaNFA& lnfa) {
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    if (!getline(file, line)) return false;
    lnfa.alphabet = split(line, ',');

    if (!getline(file, line)) return false;
    try {
        lnfa.initial_state = stoi(trim(line));
    } catch (...) { return false; }

    if (!getline(file, line)) return false;
    for (const string& s : split(line, ',')) {
        try {
            lnfa.final_states.push_back(stoi(s));
        } catch (...) { return false; }
    }

    while (getline(file, line)) {
        vector<string> parts = split(line, ',');
        if (parts.size() < 3) continue;
        try {
            int from = stoi(parts[0]);
            int to = stoi(parts[1]);
            string label = parts[2];
            if (label == "lambda" || label == "epsilon" || label == "λ") label = "";
            lnfa.adj[from].push_back({from, to, label});
        } catch (...) { return false; }
    }
    return true;
}

int main() {
    LambdaNFA lnfa;
    read_lnfa("lambda_nfa.txt", lnfa);

    cout << "LambdaNFA (lambda_nfa.txt):" << endl;
    string tests[] = {"abc", "cb", "aaa", "abb"};
    for (const string& s : tests)  cout << (s.empty() ? "empty" : s) << ": " << (lnfa.run(s) ? "true" : "false") << endl;

    return 0;
}