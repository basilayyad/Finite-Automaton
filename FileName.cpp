#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

class FiniteAutomaton 
{
private:
    unordered_set<string> states;
    unordered_set<string> alphabet;
    unordered_map<string, vector<string>> transitionFunction;
    string startState;
    unordered_set<string> acceptStates;

public:
    FiniteAutomaton(
        const vector<string>& states,
        const vector<string>& alphabet,
        const unordered_map<string, vector<string>>& transitionFunction,
        const string& startState,
        const vector<string>& acceptStates
    ) {
        this->states.insert(states.begin(), states.end());
        this->alphabet.insert(alphabet.begin(), alphabet.end());
        this->transitionFunction = transitionFunction;
        this->startState = startState;
        this->acceptStates.insert(acceptStates.begin(), acceptStates.end());
    }

    bool isDeterministic() const {
        for (const auto& transition : transitionFunction)
        {// Check if there is more than one next state for this transition
            if (transition.second.size() > 1) {
                return false;
            }
        }// If all transitions have at most one next state
        return true;
    }

    bool acceptsString(const string& inputString) const {
        unordered_set<string> currentStates = { startState }; 

        for (char symbol : inputString) {
            unordered_set<string> nextStates;
            string symStr(1, symbol);

            for (const string& state : currentStates) {
                string key = state + "," + symStr;
                auto it = transitionFunction.find(key);
                if (it != transitionFunction.end()) {
                    for (const string& nextState : it->second) 
                    {
                        nextStates.insert(nextState);
                    }
                }
            }

            if (nextStates.empty()) {
                return false; 
            }

            currentStates = nextStates;
        }

        for (const string& state : currentStates) {
            if (acceptStates.count(state)) {
                return true;
            }
        }
        return false;
    }

    void displayDetails() const {
        cout << "States: ";
        for (const string& state : states) {
            cout << state << " ";
        }
        cout << "\nAlphabet: ";
        for (const string& sym : alphabet) {
            cout << sym << " ";
        }

        cout << "\nTransition Function:"<<endl;
        for (const auto& transition : transitionFunction) {
            cout << transition.first << " -> ";
            for (const string& state : transition.second) {
                cout << state << " ";
            }
            cout << endl;
        }

        cout << "Start State: " << startState << endl;
        cout << "Accept States: ";
        for (const string& state : acceptStates) {
            cout << state << " ";
        }
        cout << endl;
        cout << "Deterministic: " << (isDeterministic() ? "Yes" : "No") << endl;
    }
    FiniteAutomaton* convertToNFA() const {
        // Since every DFA is already an NFA, we just need to copy the components
        // But we'll represent it in a way that makes it explicitly an NFA

        // Copy all the existing components
        vector<string> nfaStates(states.begin(), states.end());
        vector<string> nfaAlphabet(alphabet.begin(), alphabet.end());
        unordered_map<string, vector<string>> nfaTransitions = transitionFunction;
        string nfaStartState = startState;
        vector<string> nfaAcceptStates(acceptStates.begin(), acceptStates.end());

        // The only difference is that in an NFA, transitions can have multiple possible states
        // But since we're already using vector<string> for transitions, our DFA representation
        // is already compatible with NFA representation (just with single-element vectors)

        return new FiniteAutomaton(nfaStates, nfaAlphabet, nfaTransitions,
            nfaStartState, nfaAcceptStates);
    }
};

vector<string> split(const string& s, char delimiter) {//s ,
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}
//q0,0->q1,q2 //cut
pair<string, vector<string>> parseTransition(const string& transition) {//pair to return the tow option
    size_t arrowPos = transition.find("->");
    if (arrowPos == string::npos) {
        return make_pair("", vector<string>());
    }

    string input = transition.substr(0, arrowPos);
    string output = transition.substr(arrowPos + 2);

    vector<string> outputs = split(output, ',');
    return make_pair(input, outputs);
}

int main() {
    FiniteAutomaton* currentFA = nullptr;
    string input;

    while (true) {
        cout << "\n1. Create Finite Automaton\n2. Test String\n3. Convert To NFA\n4. Exit\nChoose option: ";
        int choice;
        cin >> choice;
        cin.ignore(); // ignore newline

        if (choice == 1) {
            cout << "Enter states (comma separated): ";
            getline(cin, input);
            vector<string> states = split(input, ',');

            cout << "Enter alphabet (comma separated): ";
            getline(cin, input);
            vector<string> alphabet = split(input, ',');

            cout << "Enter transitions format: (state,symbol->nextstate;) separate by semicolons:"<<endl;
            getline(cin, input);
            vector<string> transitions = split(input, ';');

            unordered_map<string, vector<string>> transitionFunction;
            for (const string& transition : transitions) {
                pair<string, vector<string>> parsed = parseTransition(transition);
                if (!parsed.first.empty()) {
                    transitionFunction[parsed.first] = parsed.second;
                }
            }

            cout << "Enter start state: ";
            getline(cin, input);
            string startState = input;

            cout << "Enter accept states (comma separated): ";
            getline(cin, input);
            vector<string> acceptStates = split(input, ',');

            if (currentFA) {
                delete currentFA;
            }
            currentFA = new FiniteAutomaton(states, alphabet, transitionFunction, startState, acceptStates);

            cout << "\nFinite Automaton created:"<<endl;
            currentFA->displayDetails();

        }
        else if (choice == 2) {
            if (!currentFA) {
                cout << "Please create a finite automaton first."<<endl;
                continue;
            }

            cout << "Enter string to test: "<<endl;
            getline(cin, input);

            bool result = currentFA->acceptsString(input);
            cout << "Result: " << (result ? "Accepted" : "Rejected") << endl;

        } 
        else if (choice == 3) {
            if (!currentFA) {
                cout << "Please create a finite automaton first." << endl;
                continue;
            }
            FiniteAutomaton* nfa = currentFA->convertToNFA();
            cout << "\nNFA version created:" << endl;
            nfa->displayDetails();
            delete nfa;
        }
        else if (choice == 4) {
            if (currentFA) {
                delete currentFA;
            }
            break;
        }
  
    }

    return 0;
}