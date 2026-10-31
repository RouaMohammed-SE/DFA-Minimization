#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
using namespace std;

struct DFA {
    set<int> states;
    set<char> alphabet;
    map<int, map<char, int>> transitions;
    int start_state;
    set<int> accept_states;
};

DFA minimizeDFA(const DFA& dfa);  // Implement the minimization here
void printDFA(const DFA& dfa);

int main() {
    DFA dfa;
    int n, k;

    // 1. Read states
    cout << "Enter number of states: ";
    cin >> n;

    for (int i = 0; i < n; i++) {
        dfa.states.insert(i);
    }

    // 2. Read alphabet
    cout << "Enter number of alphabet symbols: ";
    cin >> k;

    cout << "Enter alphabet symbols: ";
    for (int i = 0; i < k; i++) {
        char symbol;
        cin >> symbol;
        dfa.alphabet.insert(symbol);
    }

    // 3. Read start state
    cout << "Enter start state: ";
    cin >> dfa.start_state;

    // 4. Read final states
    int finalCount;
    cout << "Enter number of final states: ";
    cin >> finalCount;

    cout << "Enter final states: ";
    for (int i = 0; i < finalCount; i++) {
        int state;
        cin >> state;
        dfa.accept_states.insert(state);
    }

    // 5. Read transitions
    cout << "\nEnter transitions:\n";
    for (int state : dfa.states) {
        for (char symbol : dfa.alphabet) {
            int destination;

            cout << "Delta(" << state << ", " << symbol << ") = ";
            cin >> destination;

            dfa.transitions[state][symbol] = destination;
        }
    }

    // 6. Minimize DFA

    // 7. Print result

    return 0;
}