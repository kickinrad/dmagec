#ifndef MULTISCREEN_H
#define MULTISCREEN_H

#include <string>
#include <sqlite3.h>

void convertString(std::string&);
int nf_len(std::string input);

class multiscreen
{
public:
    multiscreen(sqlite3*);
    std::string act(std::string,int argc = 0,std::string* = NULL);
    std::string pclist();
private:
    sqlite3 *db;
    sqlite3_stmt *stmt;
    const char *pz;
    std::string fromFile(std::string);
    std::string charinfo(std::string);
    std::string charlist();
    void addchar(bool,std::string*);
    int getChars(std::string**&); //returns number of characters
};

#endif