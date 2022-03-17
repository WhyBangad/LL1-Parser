#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <array>
#include <utility>

using namespace std;

int same_chars(string s1, string s2)
{
    int same = 0;

    for (int i = 0; i < (min(s1.length(), s2.length())); i++)
    {
        if (s1.at(i) == s2.at(i))
        {
            same++;
        }
        else
        {
            break;
        }
    }

    return same;
}

vector<pair<char, string>> helper(vector<pair<char, string>> grammar, char charflag, char new_char)
{
    cout << "Helper" << endl;

    vector<pair<char, string>> vec;
    int flag = 1;

    for (std::vector<pair<char, string>>::const_iterator iter = grammar.begin(); iter != grammar.end(); ++iter)
    {
        if (iter->first == charflag)
        {
            for (std::vector<pair<char, string>>::const_iterator iter2 = grammar.begin(); iter2 != grammar.end(); ++iter2)
            {
                if (iter2->first == charflag && iter2 != iter && same_chars(iter->second, iter2->second) > 0)
                {
                    int sa = same_chars(iter->second, iter2->second);
                    vec.push_back(make_pair(iter->first, iter->second.substr(0, sa) + new_char));
                    vec.push_back(make_pair(new_char, iter->second.substr(sa)));
                    vec.push_back(make_pair(new_char, iter2->second.substr(sa)));
                    for (std::vector<pair<char, string>>::const_iterator iter3 = grammar.begin(); iter3 != grammar.end(); ++iter3)
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

    cout << "########" << endl;
    for (std::vector<pair<char, string>>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter)
    {
        std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
    }

    return vec;
}

vector<pair<char, string>> remove_left_factoring(vector<pair<char, string>> grammar)
{
    int flag = 1;
    // int flag2 = 1;
    int ind = 0;
    char charflag;
    char new_chars[3] = {'P', 'Q', 'R'};
    vector<pair<char, string>> new_gra;

    while (flag == 1)
    {
        // cout << "Loop start" << endl;
        cout << "@@@@@@@@" << endl;
        for (std::vector<pair<char, string>>::const_iterator iiter = new_gra.begin(); iiter != new_gra.end(); ++iiter)
        {
            std::cout << "First: " << iiter->first << ", Second: " << iiter->second << std::endl;
        }
        cout << "********" << endl;
        charflag = 0;
        // flag2 = 1;
        for (std::vector<pair<char, string>>::const_iterator iter = grammar.begin(); iter != grammar.end(); ++iter)
        {
            cout << "First Loop" << endl;
            for (std::vector<pair<char, string>>::const_iterator iter2 = grammar.begin(); iter2 != grammar.end(); ++iter2)
            {
                cout << "Second Loop" << endl;
                cout << iter->first << "|" << iter2->first << endl;
                if (iter != iter2 && iter->first == iter2->first && iter->second[0] == iter2->second[0])
                {
                    charflag = iter->first;
                    flag = 1;
                    cout << "Calling Helper" << endl;
                    new_gra = helper(grammar, charflag, new_chars[ind]);
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

int main()
{
    vector<pair<char, string>> vec = {{'E', "a"}, {'E', "b"}, {'E', "c"}, {'E', "d"}};

    for (std::vector<pair<char, string>>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter)
    {
        std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
    }

    vec = remove_left_factoring(vec);

    cout << "~~~~~~~~~~~~~~~~~" << endl;
    for (std::vector<pair<char, string>>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter)
    {
        std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
    }

    return 0;
}