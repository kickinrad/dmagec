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
    std::string act(std::string,std::string* =NULL,std::string* =NULL);
    std::string pclist();
private:
    sqlite3 *db;
    sqlite3_stmt *stmt;
    const char *pz;
    std::string *alert;
    std::string fromFile(std::string);
    std::string charinfo(std::string);
    std::string charlist();
    std::string addchar(bool,std::string*);
    int getChars(std::string**&); //returns number of characters
    void editchar(std::string*);

    void give(std::string*);
    void remove(std::string);

    std::string damage(std::string*);
    std::string rollInit();
    std::string advance();

    int currentScene;
    int getScenes(std::string**&); //returns number of scenes
    std::string setscene(std::string);
    std::string scenelist();
    std::string sceneinfo(std::string);
    void editscene(std::string*);
    std::string sceneNPCs(std::string);
};

#endif