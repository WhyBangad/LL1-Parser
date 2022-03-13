#include<bits/stdc++.h>

using namespace std;

void get_first(const char &non_term, map<char, set<char>> &first, vector<pair<char, string>> &grammar);
void get_follow(const char &non_term, map<char, set<char>> &first, map<char, set<char>> &follow, vector<pair<char, string>> &grammar);

int main(int argc, char* argv[]){
    vector<pair<char, string>> grammar;
    map<char, set<char>> first, follow;
    set<char> terminals, non_terminals;

    if(argc != 3){
        cout << "Please run the file as ./parser <grammar file> <input string>";
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
        non_terminals.insert(prod.first);
        for(auto &c : prod.second){
            if(isupper(c)){
                non_terminals.insert(c);
            }
            else{
                terminals.insert(c);
            }
        }
    }
    terminals.erase('e');
    terminals.insert('$');
    
    // eliminating left recursion and left factoring


    // finding firsts
    for(char term : terminals){
        if(first.find(term) == first.end()){
            get_first(term, first, grammar);
        }
    }

    cout << "Firsts are : \n";
    for(auto &&p : first){
        cout << p.first << " : ";
        for(auto &c : p.second){
            cout << c << " ";
        }
        cout << endl;
    }

    // finding follows
    follow[grammar.begin()->first].insert('$');
    for(char non_term : non_terminals){
        if(follow.find(non_term) == follow.end()){
            get_follow(non_term, first, follow, grammar);
        }
    }

    cout << "Follows are : \n";
    for(char c : non_terminals){
        cout << c << " : ";
        for(char ele : follow[c]){
            cout << ele << " ";
        }
        cout << endl;
    }
    return 0;
}

void get_first(const char &non_term, map<char, set<char>> &first, vector<pair<char, string>> &grammar){
    for(auto &&prod : grammar){
        if(prod.first == non_term){
            for(auto &c : prod.second){
                bool eps = 0;
                if(islower(c)){
                    first[prod.first].insert(c);
                }
                else{
                    if(first.find(c) == first.end()){
                        get_first(c, first, grammar);
                    }
                    for(char term : first[c]){
                        if(term == 'e') eps = 1;
                        first[prod.first].insert(term);
                    }
                }
                if(islower(c) || !eps) break;
            }
        }
    }
}

void get_follow(const char &non_term, map<char, set<char>> &first, map<char, set<char>> &follow, vector<pair<char, string>> &grammar){
    for(auto &&prod : grammar){
        int start = 0;
        while(start < prod.second.length() && (start = prod.second.find(non_term, start)) != string::npos){
            for(int i = start+1; i < prod.second.length(); ++i, ++start){
                char curr = prod.second[i];
                bool eps = 0;
                if(islower(curr)){
                    follow[non_term].insert(curr);
                }
                else{
                    for(char c : first[curr]){
                        if(c == 'e') eps = 1;
                        else follow[non_term].insert(c);
                    }
                }
                if(islower(curr) || !eps) break;
            }
            if(start == prod.second.length()){
                if(follow.find(prod.first) == follow.end()){
                    get_follow(prod.first, first, follow, grammar);
                }
                for(char c : follow[prod.first]){
                    follow[non_term].insert(c);
                }
            }
            ++start;
        }
    }
}