#include <array>
#include <utility>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>

using namespace std;

void input_parser(string input, vector<pair<char, string>> grammar, vector<vector<int>> table, vector<char> non_term, map<char, int> term_map)
{
    stack<char> stack;
    stack.push('$');
    stack.push('S');

    int i = 0;
    int flag = 0;
    int len = input.length();
    for (i = 0; i < len; i++)
    {
        if (stack.top() == input[i] && input[i] == '$')
        {
            cout << "Accepted !!" << endl;
            flag = 1;
        }
        else if (stack.top() == input[i])
        {
            stack.pop();
        }
        else if (find(non_term.begin(), non_term.end(), stack.top()) != non_term.cend())
        {
            int u = distance(non_term.begin(), find(non_term.begin(), non_term.end(), stack.top()));
            int v = term_map[input.at(i)];
            if (table[u][v] != 0)
            {
                // length of the production rule
                // int y = 0;
                // int t = non_map[stack.top()];

                stack.pop();

                int ind = table[u][v];
                int y = grammar[ind].second.length();
                for (int k = 0; k < y; k++)
                {
                    stack.push(grammar[ind].second[k]);
                }
            }
        }
        else
        {
            cout << "Error !!" << endl;
            break;
        }
    }

    if (flag == 0)
    {
        cout << "Not accepted !!" << endl;
    }
}

int main()
{
    string in = "i=i+5";
    vector<pair<char, string>> vec = {{'E', "E+T"}, {'E', "E-T"}, {'E', "T"}, {'T', "T*F"}, {'T', "T/F"}, {'T', "F"}, {'S', "E"}, {'F', "n"}, {'F', "i"}, {'F', "(E)"}};

    return 0;
}