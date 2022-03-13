#include <fstream>
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <array>
#include <utility>

using namespace std;

vector<pair<char, string>> remove_left_recursion(vector<pair<char, string>> grammar);
void get_first(const char &non_term, map<char, set<char>> &first, vector<pair<char, string>> &grammar);
void get_follow(const char &non_term, map<char, set<char>> &first, map<char, set<char>> &follow, vector<pair<char, string>> &grammar);

int main(int argc, char *argv[])
{
    vector<pair<char, string>> grammar;
    map<char, set<char>> first, follow;
    set<char> terminals, non_terminals;

    if (argc != 3)
    {
        cout << "Please run the file as ./parser <grammar file> <input string>";
    }

    // reading grammar
    fstream grammar_file;
    grammar_file.open(argv[1], ios::in);
    if (grammar_file.fail())
    {
        cout << "Could not open grammar file!\n";
        return 1;
    }

    cout << "Finished reading grammar : \n";
    while (!grammar_file.eof())
    {
        char buffer[25];
        grammar_file.getline(buffer, 24);
        char lhs = buffer[0];
        string rhs = buffer + 3;
        pair<char, string> prod = make_pair(lhs, rhs);
        grammar.push_back(prod);
        cout << grammar.size() << ". " << prod.first << " -> " << prod.second << endl;
    }

    // eliminating left recursion and left factoring
    grammar = remove_left_recursion(grammar);
    cout << "After left recursion : \n";
    for (auto &&prod : grammar)
    {
        cout << prod.first << " -> " << prod.second << endl;
    }

    // segregating terminals and non terminals
    for (auto &&prod : grammar)
    {
        non_terminals.insert(prod.first);
        for (auto &c : prod.second)
        {
            if (isupper(c))
            {
                non_terminals.insert(c);
            }
            else
            {
                terminals.insert(c);
            }
        }
    }
    terminals.erase('e');
    terminals.insert('$');

    // finding firsts
    for (char non_term : non_terminals)
    {
        // cout << non_term << " ";
        if (first.find(non_term) == first.end())
        {
            get_first(non_term, first, grammar);
        }
    }
    // cout << endl;

    cout << "Firsts are : \n";
    for (auto &&p : first)
    {
        cout << p.first << " : ";
        for (auto &c : p.second)
        {
            cout << c << " ";
        }
        cout << endl;
    }

    // finding follows
    follow['S'].insert('$');
    for (char non_term : non_terminals)
    {
        if (follow.find(non_term) == follow.end())
        {
            get_follow(non_term, first, follow, grammar);
        }
    }

    cout << "Follows are : \n";
    for (char c : non_terminals)
    {
        cout << c << " : ";
        for (char ele : follow[c])
        {
            cout << ele << " ";
        }
        cout << endl;
    }
    return 0;
}

vector<pair<char, string>> remove_left_recursion(vector<pair<char, string>> grammar)
{
    vector<pair<char, string>> vec_new;
    set<char> done;
    char new_chars[5] = {'V', 'W', 'X', 'Y', 'Z'};

    int t = 0;
    int index = 0;
    while (true)
    {
        // cout << "Loop start" << endl;

        char flag;
        for (std::vector<pair<char, string>>::const_iterator iter = grammar.begin(); iter != grammar.end(); ++iter)
        {
            // std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
            if (done.find(iter->first) == done.end() && iter->first == iter->second[0])
            {
                // cout << "same" << endl;
                flag = iter->first;
                done.insert(iter->first);
                t = 1;
                break;
            }
        }
        if (t == 0)
            break;

        int flag2 = 0;
        int j = 0;
        int flag3 = 0;
        for (std::vector<pair<char, string>>::const_iterator iter = grammar.begin(); iter != grammar.end(); ++iter)
        {
            // std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
            if (iter->first == flag)
            {
                if (iter->first == iter->second[0])
                {
                    vec_new.push_back(make_pair(new_chars[index], iter->second.substr(1, iter->second.size() - 1) + new_chars[index]));
                    // cout << 'X' << iter->second.substr(1, iter->second.size() - 1) + 'X' << endl;
                    // grammar.erase(grammar.begin() + index);
                    // erase the original rule

                    if (flag3 == 0)
                        vec_new.push_back(make_pair(new_chars[index], "e"));
                    flag3 = 1;
                }
                else
                {
                    vec_new.push_back(make_pair(flag, iter->second + new_chars[index]));
                    // cout << iter->second.substr(1, iter->second.size() - 1) << endl;
                    // grammar.erase(grammar.begin() + index);
                    // erase the original rule
                    flag2 = 1;
                }
            }
            j++;
        }
        index++;

        if (flag2 == 0)
        {
            vec_new.push_back(make_pair(flag, "" + new_chars[index]));
        }

        t = 0;
    }

    for (std::vector<pair<char, string>>::const_iterator iter = grammar.begin(); iter != grammar.end(); ++iter)
    {
        // std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
        if (done.find(iter->first) == done.end())
        {
            vec_new.push_back(make_pair(iter->first, iter->second));
            // done.insert(iter->first);
        }
    }

    // cout << "@@@@@@@@" << endl;
    // for (std::vector<pair<char, string>>::const_iterator iter = vec_new.begin(); iter != vec_new.end(); ++iter)
    // {
    //     std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
    // }

    // for (auto it = done.begin(); it != done.end(); ++it)
    //     cout << ' ' << *it;
    return vec_new;
}

void get_first(const char &non_term, map<char, set<char>> &first, vector<pair<char, string>> &grammar)
{
    for (auto &&prod : grammar)
    {
        if (prod.first == non_term)
        {
            for (auto &c : prod.second)
            {
                bool eps = 0;
                if (c < 'A' || c > 'Z')
                {
                    first[prod.first].insert(c);
                }
                else
                {
                    if (first.find(c) == first.end())
                    {
                        get_first(c, first, grammar);
                    }
                    for (char term : first[c])
                    {
                        if (term == 'e')
                            eps = 1;
                        first[prod.first].insert(term);
                    }
                }
                if (islower(c) || !eps)
                    break;
            }
        }
    }
}

void get_follow(const char &non_term, map<char, set<char>> &first, map<char, set<char>> &follow, vector<pair<char, string>> &grammar)
{
    for (auto &&prod : grammar)
    {
        int start = 0;
        int idx = 0;
        bool consider = 0;
        for (; idx < prod.second.length(); ++idx)
        {
            if (prod.second[idx] == non_term)
            {
                consider = 1;
            }
            else if (consider)
            {
                char curr = prod.second[idx];
                bool eps = 0;
                if (curr < 'A' || curr > 'Z')
                {
                    follow[non_term].insert(curr);
                }
                else
                {
                    for (char c : first[curr])
                    {
                        if (c == 'e')
                            eps = 1;
                        else
                            follow[non_term].insert(c);
                    }
                }
                if (curr < 'A' || curr > 'Z' || !eps)
                {
                    consider = 0;
                }
            }
        }
        if (idx == prod.second.length() && consider)
        {
            if (prod.first == non_term)
                continue;
            if (follow.find(prod.first) == follow.end())
            {
                get_follow(prod.first, first, follow, grammar);
            }
            for (char c : follow[prod.first])
            {
                follow[non_term].insert(c);
            }
        }
    }
}