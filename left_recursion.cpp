#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <array>
#include <utility>

using namespace std;

vector<pair<char, string>> remove_left_recursion(vector<pair<char, string>> grammar)
{
    vector<pair<char, string>> vec_new;
    set<char> done;
    char new_chars[5] = {'V', 'W', 'X', 'Y', 'Z'};

    int t = 0;
    int index = 0;
    while (true)
    {
        cout << "Loop start" << endl;

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
            done.insert(iter->first);
        }
    }

    cout << "@@@@@@@@" << endl;
    for (std::vector<pair<char, string>>::const_iterator iter = vec_new.begin(); iter != vec_new.end(); ++iter)
    {
        std::cout << "First: " << iter->first << ", Second: " << iter->second << std::endl;
    }

    // for (auto it = done.begin(); it != done.end(); ++it)
    //     cout << ' ' << *it;
    return vec_new;
}

int main()
{
    vector<pair<char, string>> vec = {
        {'E', "E+T"}, {'E', "E-T"}, {'E', "T"}, {'T', "T*F"}, {'T', "T/F"}, {'T', "F"}, {'S', "E"}};

    // for (std::vector<pair<int, int>>::const_iterator iter = vec.begin();
    //      iter != vec.end();
    //      ++iter)
    // {
    //     std::cout << "First: " << iter->first
    //               << ", Second: " << iter->second << std::endl;
    // }

    auto res = remove_left_recursion(vec);
    for(auto &&p : res){
        cout << p.first << " -> " << p.second << endl;
    }

    return 0;
}