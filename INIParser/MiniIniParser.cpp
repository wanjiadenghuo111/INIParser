#include "MiniIniParser.h"

//remove before and after blank space
static string& TrimString( string& str)
{
    string::size_type pos = 0;
    while(str[pos] == ' ' && str.npos != (pos = str.find(" ")))
    {
        str = str.replace(pos, pos + 1, "");
    }

    pos = str.size() - 1;
    while(str[pos] == ' ')
    {
        str = str.replace(pos, pos + 1, "");
        pos--;
    }
    return str;
}

//read in INI file and parse it
int MiniIniParser::ReadINI(const string&  path)
{
    std::lock_guard<std::mutex> lg(_vecMutex);

    ifstream in_conf_file(path.c_str());
    if(!in_conf_file)
    {
        return 0;
    }
    string str_line = "";
    string str_root = "";

    while(getline(in_conf_file, str_line))
    {
        string str_key = "";
        string str_value = "";

        TrimString(str_line);
        string::size_type left_pos = str_line.find("[");
        string::size_type right_pos = str_line.find("]");
        if(0 == left_pos  && str_line.npos != right_pos )
        {
            if(!str_root.empty())
            {
                //add new key
                bool findSection = false;
                for(auto iter = _setIniVec.begin(); iter != _setIniVec.end(); )
                {
                    if(str_root == iter->root)
                    {
                        findSection = true;
                        ININode ini_node(iter->root, iter->key, iter->value);
                        _readIniVec.push_back(ini_node);
                        iter = _setIniVec.erase(iter);
                    }
                    else if(findSection)
                    {
                        break;
                    }
                    ++iter;
                }
            }

            str_root = str_line.substr(left_pos + 1, right_pos - 1);
        }

        string::size_type equal_div_pos = str_line.find("=");
        if(str_line.npos != equal_div_pos)
        {
            str_key = str_line.substr(0, equal_div_pos);
            str_value = str_line.substr(equal_div_pos + 1, str_line.size() - 1);
            str_key = TrimString(str_key);
            str_value = TrimString(str_value);
        }

        if((!str_root.empty()) && (!str_key.empty()))
        {
            ININode ini_node(str_root, str_key, str_value);

            //update old key
            bool findSection = false;
            for(auto iter = _setIniVec.begin(); iter != _setIniVec.end(); )
            {
                if(str_root == iter->root)
                {
                    findSection = true;
                    if(str_key == iter->key)
                    {
                        ini_node.value = iter->value;
                        iter = _setIniVec.erase(iter);
                    }
                }
                else if(findSection)
                {
                    break;
                }
                ++iter;
            }

            _readIniVec.push_back(ini_node);
        }
    }
    if(!_readIniVec.size())
    {
        _readIniVec.swap(_setIniVec);
    }

    in_conf_file.close();
    in_conf_file.clear();
    _setIniVec.clear();

    return 1;
}

//write ini file
int MiniIniParser::WriteINI(const string&  path)
{
    int ret = ReadINI(path);
    if(!ret)
    {
        return -1;
    }

    std::lock_guard<std::mutex> lg(_vecMutex);

    ofstream out_conf_file(path.c_str(), ios::out);
    if(!out_conf_file)
    {
        return -1;
    }

    std::string section;
    for(auto iter = _readIniVec.begin(); iter != _readIniVec.end(); ++iter)
    {
        if(section != iter->root)
        {
            section = iter->root;
            out_conf_file << endl;
            out_conf_file << "[" << section << "]" << endl;
        }
        out_conf_file << iter->key << "=" << iter->value << endl;
    }

    out_conf_file.flush();
    out_conf_file.close();
    return 1;
}

//set value
size_t MiniIniParser::SetValue(const string&  root, const string&  key, const string&  value)
{
    std::lock_guard<std::mutex> lg(_vecMutex);
    _setIniVec.push_back(ININode(root, key, value));
    return _setIniVec.size();
}

size_t MiniIniParser::GetSize()
{
    std::lock_guard<std::mutex> lg(_vecMutex);
    return _setIniVec.size();
}
