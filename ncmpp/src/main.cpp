
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include "ncmlib/ncmdump.h"

using namespace std;

int main(int argc, char argv[])
{
    system("chcp>nul 2>nul 65001");
    vector<string> music_list;
    filesystem::directory_iterator iter(".");

    for (auto& i : iter)
    {
        if (i.is_directory())
        {
            continue;
        }
        if (i.path().extension() != ".ncm")
        {
            continue;
        }
        string s = i.path().u8string();
        music_list.push_back(s);
    }

    if (music_list.size() == 0)
    {
        return 0;
    }

    if (!filesystem::exists("unlock"))
    {
        filesystem::create_directory("unlock");
    }

    for (auto& i : music_list)
    {
        cout << "unlocking...    " << i << endl;
        ncm::ncmDump(i, "unlock");
    }

    cout << endl << "Finish..." << endl;
    system("pause");

    return 0;
}
