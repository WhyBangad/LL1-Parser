#include<bits/stdc++.h>

using namespace std;

int main(int argc, char* argv[]){
    vector<pair<char, string>> grammar;
    map<char, set<char>> first, follow;
    set<char> terminals, non_terminals;

    if(argc != 3){
        cout << "Please run the file as ./parser <grammar file> <input string>"
    }

    // reading grammar
    fstream grammar_file;
    grammar_file.open(argv[1], ios::in);
    if(grammar_file.fail()){
        cout << "Could not open grammar file!\n";
        return 1;
    }

    cout << "Finished reading grammar : \n";
    while(!grammar_file.eof()){
        char buffer[25];
        grammar_file.getline(buffer, 24);
        char lhs = buffer[0];
        string rhs = buffer+3;
        pair<char, string> prod = make_pair(lhs, rhs);
        grammar.push_back(prod);
        cout << grammar.size() << ". " << prod.first << " -> " << prod.second << endl;
    }

    // segregating terminals and non terminals
    for(auto &&prod : grammar){
        non_terminals.add(prod.first);
        for(auto &c : prod.second){
            if(isupper(c)){
                non_terminals.add(c);
            }
            else{
                terminals.add(c);
            }
        }
    }
    terminals.erase('e');
    terminals.insert('$');
    
    return 0;
}