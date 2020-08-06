#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>

using namespace std;

class MiniIniParser
{
public:
    // inner class ININode
    class ININode
    {
    public:
        ININode(const string&  r, const string&  k, const string&  v)
            : root(r), key(k), value(v) {}
        string root;
        string key;
        string value;
    };

    size_t SetValue(const string& root, const string& key, const string& value);
    int WriteINI(const string& path);

    size_t GetSize();
private:
    //unrealized
    string GetValue(const string& root, const string& key);

    int ReadINI(const string& path);

    std::mutex _vecMutex;
    vector<ININode> _setIniVec;
    vector<ININode> _readIniVec;
};

#endif // INI_PARSER_H
