#include <fstream>
#include <iostream>
#include <regex>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <array>
#include <utility>
#include <sstream>
#include <map>
#include <stack>

using namespace std;

string tokenize(string input);
vector<pair<char, string>> remove_left_recursion(vector<pair<char, string>> &grammar, const set<char> &non_terminals);
void get_first(const char &non_term, map<char, set<char>> &first, vector<pair<char, string>> &grammar);
void get_follow(const char &non_term, map<char, set<char>> &first, map<char, set<char>> &follow, vector<pair<char, string>> &grammar);
vector<vector<int>> get_parsing_table(vector<pair<char, string>> &grammar, map<char, set<char>> &first, map<char, set<char>> &follow, set<char> &terminals, set<char> &non_terminals);
void print_table(vector<vector<int>> &table, set<char> &terminals, set<char> &non_terminals);
bool parse(vector<vector<int>> &table, string &input, vector<pair<char, string>> &grammar, set<char> &terms, set<char> &non_terms);
vector<pair<char, string>> remove_left_factoring(vector<pair<char, string>> grammar, const set<char> &non_terms);
vector<pair<char, string>> remove_left_factoring_helper(vector<pair<char, string>> grammar, char charflag, char new_char);
int longest_common_prefix(const string &s1, const string &s2);

int main(int argc, char *argv[])
{
    vector<pair<char, string>> grammar;
    map<char, set<char>> first, follow;
    set<char> terminals, non_terminals;
    string input = "";

    if (argc < 2)
    {
        cout << "Please run the file as ./parser <grammar file>" << endl;
        return 1;
    }

    cout << "Enter the input string : ";
    cin >> input;

    // tokenizing string
    input = tokenize(input);
    if (input == "")
    {
        cout << "Illegal expression!" << endl;
        return 3;
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

    // eliminating left recursion and left factoring
    grammar = remove_left_recursion(grammar, non_terminals);
    if (grammar.size() == 0)
    {
        return -1;
    }
    grammar = remove_left_factoring(grammar, non_terminals);
    cout << "\nAfter removing left recursion and left factoring : \n";
    for (int i = 0; i < grammar.size(); ++i)
    {
        cout << i + 1 << ". " << grammar[i].first << " -> " << grammar[i].second << endl;
    }

    // re-finding non terminals
    non_terminals.clear();
    for (auto &&prod : grammar)
        non_terminals.insert(prod.first);

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

    cout << "\nFirsts are : \n";
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

    cout << "\nFollows are : \n";
    for (char c : non_terminals)
    {
        cout << c << " : ";
        for (char ele : follow[c])
        {
            cout << ele << " ";
        }
        cout << endl;
    }

    // predictive parsing table
    auto parsing_table = get_parsing_table(grammar, first, follow, terminals, non_terminals);
    if (parsing_table.size() == 0)
        return 1;
    print_table(parsing_table, terminals, non_terminals);

    if (!parse(parsing_table, input, grammar, terminals, non_terminals))
    {
        cout << "Invalid expression!\n";
    }
    else
    {
        cout << "\nValid expression!\n";
    }
    return 0;
}

string tokenize(string input)
{
    regex id("[a-zA-Z_][a-zA-Z_0-9]*"), num("[0-9]+"), newline("[\r\n]");
    regex operators("[+-/*()]");
    string res, curr;
    stringstream ss_input(input);
    while (getline(ss_input, curr, ' '))
    {
        if (curr == "")
            continue;
        smatch m;
        regex_match(curr, m, operators);
        if (m.length() != 0)
        {
            res += m.str();
            // cout << m.str() << endl;
            continue;
        }
        regex_match(curr, m, id);
        if (m.length() != 0)
        {
            res += 'i';
            // cout << m.str() << endl;
            continue;
        }
        regex_match(curr, m, num);
        if (m.length() != 0)
        {
            res += 'n';
            // cout << m.str() << endl;
            continue;
        }
        regex_match(curr, m, newline);
        if (m.length() != 0)
        {
            continue;
        }
        cout << curr << endl;
        if (m.length() == 0)
        {
            cout << "Unable to tokenize string..." << endl;
            return "";
        }
    }
    cout << "Tokenized string : " << res << endl;
    return res;
}

vector<pair<char, string>> remove_left_recursion(vector<pair<char, string>> &grammar, const set<char> &non_term)
{
    // characters that can be used
    set<char> usable;
    // mapping of characters to those that have been used in their left recursion elimination
    map<char, char> used;
    vector<pair<char, string>> new_grammar;

    for (char c = 'A'; c <= 'Z'; ++c)
        usable.insert(c);
    for (char c : non_term)
        usable.erase(c);
    for (auto &&prod : grammar)
    {
        char lhs = prod.first;
        string rhs = prod.second;
        if (used.find(lhs) != used.end())
            continue;
        if (lhs == rhs[0])
        {
            // left recursion
            if (usable.empty())
            {
                fprintf(stderr, "No characters remaining to substitute for left recursions!\n");
                return vector<pair<char, string>>{};
            }
            char to_use = *usable.begin();
            bool terminator = 0;
            usable.erase(usable.begin());
            used[lhs] = to_use;
            new_grammar.push_back({to_use, "e"});
            for (auto &&m_prod : grammar)
            {
                // E -> E+T | T
                // yields
                // E -> TV
                // V -> +TV | e
                if (m_prod.first == lhs)
                {
                    rhs = m_prod.second;
                    string new_rhs;
                    if (lhs == rhs[0])
                    {
                        new_rhs = rhs.substr(1) + used[lhs];
                        new_grammar.push_back({used[lhs], new_rhs});
                    }
                    else
                    {
                        terminator = 1;
                        new_rhs = rhs + to_use;
                        new_grammar.push_back({lhs, new_rhs});
                    }
                }
            }
            if (!terminator)
            {
                // Can't remove this left recursion
                // Grammar is not LL(1)
                fprintf(stderr, "Left recursion of %c does not have a terminator and can't be eliminated!\n", lhs);
                return vector<pair<char, string>>{};
            }
        }
        else
        {
            new_grammar.push_back(prod);
        }
    }
    return new_grammar;
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

vector<vector<int>> get_parsing_table(vector<pair<char, string>> &grammar, map<char, set<char>> &first, map<char, set<char>> &follow, set<char> &terminals, set<char> &non_terminals)
{
    vector<vector<int>> parsing_table(non_terminals.size(), vector<int>(terminals.size(), -1));
    for (int i = 0; i < grammar.size(); ++i)
    {
        string rhs = grammar[i].second;
        char lhs = grammar[i].first;
        set<int> yield;
        bool early_exit = 0;
        if (rhs != "e")
        {
            for (char c : rhs)
            {
                bool eps = 0;
                if (c == 'e')
                    continue;
                if (c < 'A' || c > 'Z')
                {
                    yield.insert(c);
                    early_exit = 1;
                    break;
                }
                else
                {
                    yield.insert(first[c].begin(), first[c].end());
                    eps = (first[c].find('e') != first[c].end());
                    if (eps)
                        yield.erase('e');
                    else
                    {
                        early_exit = 1;
                        break;
                    }
                }
            }
        }
        if (!early_exit)
        {
            yield.insert(follow[lhs].begin(), follow[lhs].end());
        }
        // cout << "CHAR : " << lhs << endl;
        // cout << "YIELD : ";
        for (char c : yield)
        {
            // cout << c << " ";
            int row = distance(non_terminals.begin(), non_terminals.find(lhs));
            int col = distance(terminals.begin(), terminals.find(c));
            if (parsing_table[row][col] != -1)
            {
                cout << "Ambiguous grammar!\n2 or more productions in entry table[" << lhs << "][" << c << "]\n";
                return vector<vector<int>>{};
            }
            parsing_table[row][col] = i;
        }
        // cout << endl;
    }
    return parsing_table;
}

void print_table(vector<vector<int>> &table, set<char> &terminals, set<char> &non_terminals)
{
    cout << "\nParsing table : \n";
    cout << "  ";
    for (char c : terminals)
    {
        cout << c << " ";
    }
    cout << "\n";
    auto it = non_terminals.begin();
    for (int i = 0; i < table.size(); ++i)
    {
        cout << *it;
        for (int j = 0; j < table[i].size(); ++j)
        {
            if (table[i][j] == -1)
                cout << " -";
            else
                cout << " " << table[i][j] + 1;
        }
        cout << "\n";
        ++it;
    }
}

bool parse(vector<vector<int>> &table, string &input, vector<pair<char, string>> &grammar, set<char> &terms, set<char> &non_terms)
{
    stack<char> states;
    states.push('$');
    states.push('S');
    input += '$';
    // cout << "PARSING\n\n";
    int index = 0;
    while (index < input.length())
    {
        char c = input[index];
        // cout << states.top() << " " << c << endl;
        if (c == '$' && states.top() == '$')
        {
            return 1;
        }
        else if (states.size() == 1)
        {
            return 0;
        }
        else if (states.top() == c)
        {
            states.pop();
            ++index;
        }
        else
        {
            int row = distance(non_terms.begin(), non_terms.find(states.top()));
            int col = distance(terms.begin(), terms.find(c));
            if (table[row][col] == -1)
            {
                cout << "\nParsing reached empty table entry!" << endl;
                return 0;
            }
            states.pop();
            string rhs = grammar[table[row][col]].second;
            reverse(rhs.begin(), rhs.end());
            if (rhs != "e")
            {
                for (char c : rhs)
                {
                    states.push(c);
                }
            }
        }
    }
    return 1;
}

int longest_common_prefix(const string &s1, const string &s2)
{
    int cnt = 0;
    for (int i = 0; i < min(s1.length(), s2.length()); ++i)
    {
        if (s1[i] != s2[i])
            break;
        ++cnt;
    }
    return cnt;
}

vector<pair<char, string>> remove_left_factoring_helper(vector<pair<char, string>> grammar, char charflag, char new_char)
{
    // cout << "remove_left_factoring_helper" << endl;

    vector<pair<char, string>> vec;
    int flag = 1;

    for (auto iter = grammar.begin(); iter != grammar.end(); ++iter)
    {
        if (iter->first == charflag)
        {
            for (auto iter2 = grammar.begin(); iter2 != grammar.end(); ++iter2)
            {
                if (iter2->first == charflag && iter2 != iter && longest_common_prefix(iter->second, iter2->second) > 0)
                {
                    int sa = longest_common_prefix(iter->second, iter2->second);
                    vec.push_back(make_pair(iter->first, iter->second.substr(0, sa) + new_char));
                    vec.push_back(make_pair(new_char, iter->second.substr(sa)));
                    vec.push_back(make_pair(new_char, iter2->second.substr(sa)));
                    for (auto iter3 = grammar.begin(); iter3 != grammar.end(); ++iter3)
                    {
                        if (iter3 != iter && iter3 != iter2)
                        {
                            vec.push_back(make_pair(iter3->first, iter3->second));
                        }
                    }
                    flag = 0;
                    break;
                }
            }
            if (flag == 0)
                break;
        }
    }

    // cout << "########" << endl;
    // for (std::vector<pair<char, string>>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter)
    // {
    //     std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
    // }

    return vec;
}

vector<pair<char, string>> remove_left_factoring(vector<pair<char, string>> grammar, const set<char> &non_terms)
{
    int flag = 1;
    // int flag2 = 1;
    int ind = 0;
    char charflag;
    // set<char> new_chars;
    // for(char c = 'A'; c <= 'Z'; ++c) new_chars.insert(c);
    // for(char c : non_terms) new_chars.erase(c);
    char new_chars[3] = {'P', 'Q', 'R'};
    vector<pair<char, string>> new_gra;

    while (flag == 1)
    {
        // cout << "Loop start" << endl;
        // cout << "@@@@@@@@" << endl;
        // for (std::vector<pair<char, string>>::const_iterator iiter = new_gra.begin(); iiter != new_gra.end(); ++iiter)
        // {
        //     std::cout << "First: " << iiter->first << ", Second: " << iiter->second << std::endl;
        // }
        // cout << "********" << endl;
        charflag = 0;
        // flag2 = 1;
        for (auto iter = grammar.begin(); iter != grammar.end(); ++iter)
        {
            // cout << "First Loop" << endl;
            for (auto iter2 = grammar.begin(); iter2 != grammar.end(); ++iter2)
            {
                // cout << "Second Loop" << endl;
                // cout << iter->first << "|" << iter2->first << endl;
                if (iter != iter2 && iter->first == iter2->first && iter->second[0] == iter2->second[0])
                {
                    charflag = iter->first;
                    flag = 1;
                    // cout << "Calling remove_left_factoring_helper" << endl;
                    new_gra = remove_left_factoring_helper(grammar, charflag, new_chars[ind]);
                    ind++;
                    break;
                }
                else
                {
                    flag = 0;
                }
            }
            if (flag == 1)
                break;
        }
        if (flag == 0)
        {
            return grammar;
        }
        grammar = new_gra;
    }

    // cout << "@@@@@@@@" << endl;
    // for (std::vector<pair<char, string>>::const_iterator iter = new_gra.begin(); iter != new_gra.end(); ++iter)
    // {
    //     std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
    // }

    return new_gra;
}