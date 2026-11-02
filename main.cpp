
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>

using namespace std;

struct DFA
{
    set<string> states;
    set<char> alphabet;
    map<pair<string, char>, string> transitions;
    string start_state;
    set<string> accept_states;
};

// A map where the key is a destination pair,
// and the value is the set of original pairs that lead to it.
using ReverseTransitions = map<pair<string, string>, set<pair<string, string>>>;

DFA preprocessDFA(const DFA& dfa);  // Preprocessing: remove unreachable states
DFA minimizeDFA(DFA& dfa, ReverseTransitions& reverse_transitions);
void printDFA(const DFA& dfa);

// Build the reverse transition table to find which state pairs
// lead to each destination pair for every alphabet symbol.
ReverseTransitions buildReverseTransitions(const DFA& dfa);

int main()
{
    DFA dfa;
    int n, k;

    // 1. Read states
    cout << "Enter number of states: ";
    cin >> n;

    for (int i = 0; i < n; i++)
    {
        dfa.states.insert(to_string(i));
    }

    // 2. Read alphabet
    cout << "Enter number of alphabet symbols: ";
    cin >> k;

    cout << "Enter alphabet symbols: ";
    for (int i = 0; i < k; i++)
    {
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
    for (int i = 0; i < finalCount; i++)
    {
        string state;
        cin >> state;
        dfa.accept_states.insert(state);
    }

    // 5. Read transitions
    int transitionCount;

    cout << "Enter number of transitions: ";
    cin >> transitionCount;

    cout << "Enter transitions (from to symbol):\n";

    for (int i = 0; i < transitionCount; i++)
    {
        string from, to;
        char symbol;

        cin >> from >> to >> symbol;

        dfa.transitions[{from, symbol}] = to;
    }

    // 6. Minimize DFA
    dfa = preprocessDFA(dfa);

    ReverseTransitions reverseTransitions = buildReverseTransitions(dfa);

    DFA minimizedDFA = minimizeDFA(dfa, reverseTransitions);

    // 7. Print result
    printDFA(minimizedDFA);

    return 0;
}

// Function to remove unreachable states
DFA preprocessDFA(const DFA& dfa)
{
    set<string> reachable;
    queue<string> q;

    reachable.insert(dfa.start_state);
    q.push(dfa.start_state);

    while (!q.empty())
    {
        string current = q.front();
        q.pop();

        for (char symbol : dfa.alphabet)
        {
            auto it = dfa.transitions.find({current, symbol});

            if (it == dfa.transitions.end())
            {
                continue;
            }

            string next = it->second;

            if (reachable.insert(next).second)
            {
                q.push(next);
            }
        }
    }

    DFA result;
    result.alphabet = dfa.alphabet;
    result.start_state = dfa.start_state;
    result.states = reachable;

    for (string s : reachable)
    {
        if (dfa.accept_states.count(s))
        {
            result.accept_states.insert(s);
        }

        for (char symbol : dfa.alphabet)
        {
            auto it = dfa.transitions.find({s, symbol});

            if (it != dfa.transitions.end())
            {
                result.transitions[{s, symbol}] = it->second;
            }
        }
    }

    return result;
}

void printDFA(const DFA& dfa)
{
    cout << "\n===== Minimized DFA =====\n";

    cout << "States: ";
    for (string s : dfa.states)
    {
        cout << s << ' ';
    }
    cout << "\n";

    cout << "Alphabet: ";
    for (char c : dfa.alphabet)
    {
        cout << c << " ";
    }
    cout << "\n";

    cout << "Start state: " << dfa.start_state << "\n";

    cout << "Final states: ";
    for (string s : dfa.accept_states)
    {
        cout << s << ' ';
    }
    cout << "\n";

    cout << "Transitions:\n";

    for (string s : dfa.states)
    {
        for (char c : dfa.alphabet)
        {
            cout << "(" << s << ", " << c << ") = ";

            auto it = dfa.transitions.find({s, c});

            if (it != dfa.transitions.end())
            {
                cout << it->second;
            }
            else
            {
                cout << "-";
            }

            cout << "\n";
        }
    }
}

ReverseTransitions buildReverseTransitions(const DFA& dfa)
{
    ReverseTransitions reverseTransitions;

    // Loop over every possible pair of different states
    for (string p : dfa.states)
    {
        for (string q : dfa.states)
        {
            // Avoid (0,0), (1,1), and duplicate pairs
            if (p >= q)
            {
                continue;
            }

            // Check every alphabet symbol
            for (char symbol : dfa.alphabet)
            {
                // Find where each state goes on this symbol
                string r = dfa.transitions.at({p, symbol});
                string s = dfa.transitions.at({q, symbol});

                // If both states go to the same state,
                // there is no destination pair to store
                if (r == s)
                {
                    continue;
                }

                // Create the destination pair in a consistent order
                pair<string, string> destinationPair = {min(r, s), max(r, s)};
                pair<string, string> originalPair = {p, q};

                // Store: destination pair -> original pair
                reverseTransitions[destinationPair].insert(originalPair);
            }
        }
    }

    return reverseTransitions;
}

DFA minimizeDFA(DFA& dfa, ReverseTransitions& reverseTransitions)
{
    map<pair<string, string>, bool> marks;
    queue<pair<string, string>> q;

    // Initialize every pair as equivalent
    for (string p : dfa.states)
    {
        for (string r : dfa.states)
        {
            if (p < r)
            {
                marks[{p, r}] = true;
            }
        }
    }

    // Mark pairs with different final states as false
    for (auto& [statePair, equivalent] : marks)
    {
        if (dfa.accept_states.count(statePair.first) +
            dfa.accept_states.count(statePair.second) == 1)
        {
            marks[statePair] = false;
            q.push(statePair);
        }
    }

    // Loop on non-equivalent states using reverse transitions
    while (!q.empty())
    {
        auto p = q.front();
        q.pop();

        auto it = reverseTransitions.find(p);

        if (it == reverseTransitions.end())
        {
            continue;
        }

        for (auto predecessor : it->second)
        {
            if (marks[predecessor] == true)
            {
                marks[predecessor] = false;
                q.push(predecessor);
            }
        }
    }

    vector<vector<string>> classes;
    map<string, string> newState;

    // Function to check if 2 states are equivalent
    auto isEquivalent = [&](string p, string r)
    {
        if (p == r)
        {
            return true;
        }

        return marks[{min(p, r), max(p, r)}];
    };

    for (string s : dfa.states)
    {
        bool found = false;

        for (int i = 0; i < classes.size(); i++)
        {
            string representative = classes[i][0];

            if (isEquivalent(s, representative))
            {
                classes[i].push_back(s);
                found = true;
                break;
            }
        }

        if (!found)
        {
            int id = classes.size();
            classes.push_back({s});
        }
    }

    // Construct the minimized DFA
    DFA minimizedDFA;
    minimizedDFA.alphabet = dfa.alphabet;

    vector<string> classNames(classes.size());

    for (int i = 0; i < classes.size(); i++)
    {
        string curState = "{";

        for (int j = 0; j < classes[i].size(); j++)
        {
            curState += classes[i][j];

            if (j != classes[i].size() - 1)
            {
                curState += ",";
            }
        }

        curState += "}";

        classNames[i] = curState;
        minimizedDFA.states.insert(curState);

        for (string state : classes[i])
        {
            newState[state] = curState;
        }

        if (dfa.accept_states.count(classes[i][0]))
        {
            minimizedDFA.accept_states.insert(curState);
        }
    }

    minimizedDFA.start_state = newState.at(dfa.start_state);

    // Construct transitions between equivalence classes
    for (int i = 0; i < classes.size(); i++)
    {
        string representative = classes[i][0];
        string currentClass = classNames[i];

        for (char symbol : dfa.alphabet)
        {
            auto it = dfa.transitions.find({representative, symbol});

            // Handle missing transitions
            if (it == dfa.transitions.end())
            {
                continue;
            }

            string destination = it->second;
            string newDestination = newState.at(destination);

            minimizedDFA.transitions[{currentClass, symbol}] = newDestination;
        }
    }

    return minimizedDFA;
}
