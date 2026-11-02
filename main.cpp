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
    map<pair<int, char>, int> transitions;
    int start_state;
    set<int> accept_states;
};

// a pair of existing DFA states
using StatePair = pair<int, int>;
// a map where the key is a destination pair, and the value is the set of original pairs that lead to it
using ReverseTransitions = map<StatePair, set<StatePair>>;

DFA preprocessDFA(const DFA& dfa);  // Preprocessing: remove unreachable states
DFA minimizeDFA(const DFA& dfa);  // Implement the minimization here
void printDFA(const DFA& dfa);
// build the reverse transition table to find which state pairs lead to each destination pair for every alphabet symbol
ReverseTransitions buildReverseTransitions(const DFA& dfa);

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
    // 5. Read transitions
    int transitionCount;

    cout << "Enter number of transitions: ";
    cin >> transitionCount;

    cout << "Enter transitions (from to symbol):\n";

    for (int i = 0; i < transitionCount; i++)
    {
        int from, to;
        char symbol;

        cin >> from >> to >> symbol;

        dfa.transitions[{from, symbol}] = to;
    }

    // 6. Minimize DFA

    // 7. Print result
    printDFA(dfa);

    return 0;
}

DFA preprocessDFA(const DFA& dfa) {
    set<int> reachable;
    queue<int> q;

    reachable.insert(dfa.start_state);
    q.push(dfa.start_state);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        for (char symbol : dfa.alphabet) {

            auto it = dfa.transitions.find({current, symbol});

            if (it == dfa.transitions.end())
                continue;

            int next = it->second;

            if (reachable.insert(next).second) {
                q.push(next);
            }
        }
    }

    DFA result;
    result.alphabet = dfa.alphabet;
    result.start_state = dfa.start_state;
    result.states = reachable;

    for (int s : reachable) {
        if (dfa.accept_states.count(s)) result.accept_states.insert(s);

        for (char symbol : dfa.alphabet) {

            auto it = dfa.transitions.find({s, symbol});

            if (it != dfa.transitions.end()) {
                result.transitions[{s, symbol}] = it->second;
            }
        }
    }

    return result;
}

void printDFA(const DFA& dfa) {
    cout << "\n===== DFA =====\n";

    cout << "States: ";
    for (int s : dfa.states) cout << s << " ";
    cout << "\n";

    cout << "Alphabet: ";
    for (char c : dfa.alphabet) cout << c << " ";
    cout << "\n";

    cout << "Start state: " << dfa.start_state << "\n";

    cout << "Final states: ";
    for (int s : dfa.accept_states) cout << s << " ";
    cout << "\n";

    cout << "Transitions:\n";
    for (int s : dfa.states)
     {
    for (char c : dfa.alphabet) {
        cout << "Delta(" << s << ", " << c << ") = ";

        auto it = dfa.transitions.find({s, c});

        if (it != dfa.transitions.end())
            cout << it->second;
        else
            cout << "-";

        cout << "\n";
    }
}
}

ReverseTransitions buildReverseTransitions(const DFA& dfa) {
    ReverseTransitions reverseTransitions;
    // loop over every possible pair of different states
    for(int p : dfa.states) {
        for(int q : dfa.states) {
            // avoid (0,0), (1,1), and duplicate pairs
            if(p >= q) {
                continue;
            }
            // check every alphabet symbol
            for (char symbol : dfa.alphabet) {
                // find where each state goes on this symbol
                int r = dfa.transitions.at({p, symbol});
            int s = dfa.transitions.at({q, symbol});
                // if both states go to the same state, there is no destination pair to store
                if (r == s) {
                    continue;
                } 
                // create the destination pair in a consistent order
                StatePair destinationPair = {
                    min(r, s), max(r, s)
                };
                StatePair originalPair = {p, q};
                // store: destination pair -> original pair
                reverseTransitions[destinationPair].insert(originalPair);
            }
        }
    }
    return reverseTransitions;
}