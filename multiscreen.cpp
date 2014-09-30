#include "multiscreen.h"
#include <sstream>
#include <fstream>
#include <string.h>
#include <iostream> //for debugging, remove later

multiscreen::multiscreen(sqlite3* given_db)
{
    db = given_db;
}
std::string multiscreen::act(std::string in, int argc, std::string* argv)
{
    int c;
    if (in=="home" || in == "help") c = 0;
    else if (in=="charinfo") c = 1;
    else if (in=="charlist") c = 2;
    else if (in=="addpc") c = 3;
    else if (in=="addnpc") c = 4;

    switch(c)
    {
        case 0: //home, help, etc
            return fromFile(in);
            break;
        case 1: //charinfo
            return charinfo(argv[0]);
            break;
        case 2: //charlist
            return charlist();
            break;
        case 3: //addpc
            addchar(true,argv);
            return fromFile("home");
            break;
        case 4: //addnpc
            addchar(false, argv);
            return fromFile("home");
            break;
    }
}
std::string multiscreen::pclist()
{
    std::string** chars;
    std::string out = "";
    int numChars = getChars(chars);
    for (int i=0; i<numChars; i++)
    {
        if (chars[i][2] != "") //PC
        {
            out += chars[i][1] + "   " += chars[i][2] + "   " + chars[i][0] + "  " + chars[i][5] + "  " + chars[i][3] + '/' + chars[i][4] + " \n";
        }
    }
    return out;
}
std::string multiscreen::fromFile(std::string in)
{
    std::string out = "";
    std::string line;

    std::ifstream file;
    
    in = "multiscreens/" + in;
    file.open(in);
    std::getline(file,out,'\0');
    return out;
}
std::string multiscreen::charinfo(std::string id)
{
    std::string out = "*Multiscreen Interface~ `<Character Info>~ \n\n";
    std::string** chars;
    int numChars = getChars(chars);
    for (int i=0; i<numChars; i++)
    {
        if (id == chars[i][0])
        {
            out += "Character Name: " + chars[i][1] + " \n";
            if(chars[i][2] != "") //PC
            {
                out += "Portrayed by: " + chars[i][2] + " \n";
                out += "Experience Points: " + chars[i][5] + " \n";
            }
            out += "Hit Points: " + chars[i][3] + '/' + chars[i][4] + ' ';
            return out;
        }
    }
    out += "Not found or something. ";
    return out;
}

int multiscreen::getChars(std::string**& chars)
{
    const char *query = "SELECT Count(*) FROM CHARACTERS;";
    sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
    int count = sqlite3_step(stmt);
    if (!count) return 0;
    count = sqlite3_column_int(stmt,0);

    chars = new std::string*[count];

    const char *query2 = "SELECT * FROM CHARACTERS ORDER BY ID ASC; ";
    sqlite3_prepare(db, query2, strlen(query2), &stmt, &pz);

    for (int i=0; i<count; i++)
    {
        chars[i] = new std::string[6];
        sqlite3_step(stmt);
        chars[i][0] = std::to_string(sqlite3_column_int(stmt,0));//id
        chars[i][1] = std::string( reinterpret_cast< const char* >(sqlite3_column_text(stmt,1)));//name
        chars[i][3] = std::to_string(sqlite3_column_int(stmt,3));//cur hp
        chars[i][4] = std::to_string(sqlite3_column_int(stmt,4));//total hp

        if (sqlite3_column_text(stmt,2))
        {
            chars[i][5] = std::to_string(sqlite3_column_int(stmt,5));//xp
            chars[i][2] = std::string( reinterpret_cast< const char* >(sqlite3_column_text(stmt,2)));//real name
        }
    }
}
std::string multiscreen::charlist()
{
    std::string out = "*Multiscreen Interface~ `<Character List>~\n      \n*Character~                   *Real Name~           *ID~     *XP~      *HP~ \n\n\n"; //spaces a workaround. fix later

    std::string** chars;
    int numChars = getChars(chars);
    
    for (int i=0; i<numChars; i++)
    {
        out += chars[i][0] + "     " + chars[i][1] + "     " + chars[i][2] + "     " + chars[i][3] + "     " + chars[i][4] + "     " + chars[i][5] + '\n';
    }
    return out;
}
void multiscreen::addchar(bool pc, std::string* argv)
{
    std::string** chars;
    std::string q;
    int numChars = getChars(chars);
    int id = std::stoi(chars[numChars-1][0])+1;
    if (pc)
    {
        q = "INSERT INTO CHARACTERS (ID,NAME,REALNAME,CURHP,TOTALHP,XP) VALUES(" + std::to_string(id) + ",'" + argv[0] + "','" + argv[1] + "'," + argv[2] + "," + argv[2] + ",0);";
    }
    else
    {
        q = "INSERT INTO CHARACTERS (ID,NAME,CURHP,TOTALHP) VALUES(" + std::to_string(id) + ",'" + argv[0] + "'," + argv[1] + "," + argv[1] + ");";
    }
    const char *query = q.c_str();
    sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
    sqlite3_step(stmt);
}

void convertString(std::string& input)
{
    for (int i=0; i<input.length(); i++)
    {
        if (input[i] == '*') input.replace(i,1,"\033[4m\033[33m"); //orange text w/ underline
        if (input[i] == '~') input.replace(i,1,"\033[39m\033[24m"); //default text
        if (input[i] == '`') input.replace(i,1,"\033[37m"); //white text
        if (input[i] == '+') input.replace(i,1,"\033[41m"); //red highlight
        if (input[i] == '^') input.replace(i,1,"\033[31m\033[52m"); //red text
    }
}

int nf_len(std::string input)
{
    for (int i=0; i<input.length(); i++) if (input[i] == '*' || input[i] == '~' || input[i] == '`' || input[i] == '+' || input[i] == '^') input.erase(i--,1); //orange text w/ underline
    return input.length()-1;
}