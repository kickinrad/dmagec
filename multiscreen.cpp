#include "multiscreen.h"
#include <sstream>
#include <fstream>
#include <string.h>

//**************************CONSTRUCTOR**************************
multiscreen::multiscreen(sqlite3* given_db)
{
    db = given_db;
}

//**************************GENERAL MULTISCREEN FUNCTIONS**************************
std::string multiscreen::act(std::string in, std::string* argv, std::string* alert_in)
{
    int c;
    alert = alert_in;

    if (in=="home" || in == "help") c = 0;
    else if (in=="charinfo") c = 1;
    else if (in=="charlist") c = 2;
    else if (in=="addpc") c = 3;
    else if (in=="addnpc") c = 4;
    else if (in=="scenelist") c = 5;
    else if (in=="setscene") c = 6;
    else if (in=="sceneinfo") c = 7;
    else if (in=="editchar") c = 8;
    else if (in=="editscene") c = 9;
    else if (in=="give") c = 10;
    else if (in=="remove") c = 11;
    else if (in=="heal") c = 12;
    else if (in=="damage") c = 13;

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
        case 5:
            return scenelist();
            break;
        case 6:
            return setscene(argv[0]);
            break;
        case 7:
            sceneinfo(argv[0]);
            return fromFile("home");
            break;
        case 8:
            editchar(argv);
            return fromFile("home");
            break;
        case 9:
            editscene(argv);
            return fromFile("home");
            break;
        case 10:
            give(argv);
            return fromFile("home");
            break;
        case 11:
            remove(argv[0]);
            return fromFile("home");
            break;
        case 12:
            argv[0].insert(0,"-");
            return damage(argv);
            break;
        case 13:
            return damage(argv);
            break;
    }
}
std::string multiscreen::fromFile(std::string in)
{
    std::string out = "";
    std::ifstream file;
    
    in = "multiscreens/" + in;
    file.open(in);
    std::getline(file,out,'\0');
    file.close();
    return out;
}

//**************************CHARACTER FUNCTIONS**************************
int multiscreen::getChars(std::string**& chars)
{
    const char *query = "SELECT Count(*) FROM CHARACTERS;";
    sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
    int count = sqlite3_step(stmt);
    if (!count) return 0;
    count = sqlite3_column_int(stmt,0);

    chars = new std::string*[count];

    const char *query2 = "SELECT * FROM CHARACTERS ORDER BY ID ASC;";
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
    if (!count) *alert = "^No characters were found!~";
}
std::string multiscreen::charlist()
{
    std::string **chars, out = "*Multiscreen Interface~ `<Character List>~\n      \n*Character~                   *Real Name~           *ID~     *XP~      *HP~\n\n"; //spaces a workaround. fix later
    int numChars = getChars(chars);
    
    for (int i=0; i<numChars; i++)
    {
        if (chars[i][2] != "") //PC
        {
            out += chars[i][1];
            for (int w=0; w<28-chars[i][1].length(); w++) out += ' ';
            out += chars[i][2];
            for (int w=0; w<20-chars[i][2].length(); w++) out += ' ';
            out += chars[i][0];
            for (int w=0; w<7-chars[i][0].length(); w++) out += ' ';
            out += chars[i][5];
            for (int w=0; w<8-chars[i][5].length(); w++) out += ' ';
        }
        else //NPC
        {
            out += chars[i][1];
            for (int w=0; w<48-chars[i][1].length(); w++) out += ' ';
            out += chars[i][0];
            for (int w=0; w<15-chars[i][0].length(); w++) out += ' ';
        }
        out += chars[i][3] + '/' + chars[i][4] + '\n';
    }
    return out;
}
void multiscreen::addchar(bool pc, std::string* argv)
{
    std::string **chars, q;
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
std::string multiscreen::charinfo(std::string id)
{
    std::string **chars, out = "*Multiscreen Interface~ `<Character Info>~\n  \n";
    int numChars = getChars(chars);
    for (int i=0; i<numChars; i++)
    {
        if (id == chars[i][0])
        {
            out += "Character Name:    `" + chars[i][1] + "~\n";
            if(chars[i][2] != "") //PC
            {
                out += "Portrayed by:      `" + chars[i][2] + "~\n";
                out += "Experience Points: `" + chars[i][5] + "~\n";
            }
            out += "Hit Points:        `" + chars[i][3] + "~/`" + chars[i][4] + "~\n\nInventory:\n\n";

            //get number of items in inventory
            std::string q = "SELECT COUNT(*) FROM CHARACTERINVENTORY WHERE CHAR_ID=" + id + ";";
            const char *query = q.c_str();
            sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
            sqlite3_step(stmt);
            int itemcount = sqlite3_column_int(stmt,0);

            //get list of item ids in inventory
            q = "SELECT * FROM CHARACTERINVENTORY WHERE CHAR_ID=" + id + ";";
            const char *query2 = q.c_str();
            sqlite3_prepare(db, query2, strlen(query2), &stmt, &pz);
            std::string items[itemcount];
            for(i=0; i<itemcount; i++)
            {
                sqlite3_step(stmt);
                items[i] = std::string(reinterpret_cast< const char* >(sqlite3_column_text(stmt,2)));
            }

            //lookup each item
            
            for (int i=0; i<itemcount; i++)
            {
                q = "SELECT * FROM ITEMS WHERE ID=" + items[i] + ";";
                const char *query3 = q.c_str();
                sqlite3_prepare(db, query3, strlen(query3), &stmt, &pz);
                sqlite3_step(stmt);
                out += std::string(reinterpret_cast< const char* >(sqlite3_column_text(stmt,1))) + "    " + std::string(reinterpret_cast< const char* >(sqlite3_column_text(stmt,2))) + "\n";
            }

            return out;
        }
    }
    *alert = "^No characters matching that ID were found!~";
    return out;
}
std::string multiscreen::pclist()
{
    std::string **chars, out = "";
    int numChars = getChars(chars);
    for (int i=0; i<numChars; i++)
    {
        if (chars[i][2] != "") //PC
        {
            out += chars[i][1];
            for (int w=0; w<28-chars[i][1].length(); w++) out += ' ';
            out += chars[i][2];
            for (int w=0; w<20-chars[i][2].length(); w++) out += ' ';
            out += chars[i][0];
            for (int w=0; w<7-chars[i][0].length(); w++) out += ' ';
            out += chars[i][5];
            for (int w=0; w<8-chars[i][5].length(); w++) out += ' ';
            if (chars[i][3]=="0") out += '^' + chars[i][3] + '~';
            else out += chars[i][3];
            out += '/' + chars[i][4] + '\n';
        }
    }
    return out;
}
void multiscreen::editchar(std::string* argv)
{
    std::string q = "UPDATE CHARACTERS SET " + argv[1] + " = " + '\'' + argv[2] + "\' WHERE ID = " + argv[0] + ';';
    const char *query = q.c_str();
    sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
    sqlite3_step(stmt);
}

//**************************ITEM FUNCTIONS**************************
void multiscreen::give(std::string* argv)
{ //argv[0] = character id; argv[1] = item name, argv[2] = item description

    //figure out what our item's ID should be..
    const char *query = "SELECT Count(*) FROM ITEMS;";
    sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
    sqlite3_step(stmt);
    int id = sqlite3_column_int(stmt,0) + 1;

    //create new item with that id
    std::string q = "INSERT INTO ITEMS (ID, NAME, DESC) VALUES(" + std::to_string(id) + ",'" + argv[1] + "','" + argv[2] + "');";
    const char *query2 = q.c_str();
    sqlite3_prepare(db, query2, strlen(query2), &stmt, &pz);
    sqlite3_step(stmt);

    //figure out what our key's ID in CHARACTERINVENTORY should be..
    const char *query3 = "SELECT Count(*) FROM CHARACTERINVENTORY;";
    sqlite3_prepare(db, query3, strlen(query3), &stmt, &pz);
    sqlite3_step(stmt);
    int id_2 = sqlite3_column_int(stmt,0) + 1;

    //add new item to player inventory
    q = "INSERT INTO CHARACTERINVENTORY (KEY, CHAR_ID, ITEM_ID) VALUES(" + std::to_string(id_2) + "," + argv[0] + "," + std::to_string(id) + ");";
    const char *query4 = q.c_str();
    sqlite3_prepare(db, query4, strlen(query4), &stmt, &pz);
    sqlite3_step(stmt);
    id_2 = sqlite3_column_int(stmt,0) + 1;
}
void multiscreen::remove(std::string id)
{
    //delete item from ITEMS
    std::string q = "DELETE FROM ITEMS WHERE ID=" + id;
    const char *query = q.c_str();
    sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
    sqlite3_step(stmt);

    //delete item from CHARACTERINVENTORY
    q = "DELETE FROM CHARACTERINVENTORY WHERE ITEM_ID=" + id;
    const char *query2 = q.c_str();
    sqlite3_prepare(db, query2, strlen(query2), &stmt, &pz);
    sqlite3_step(stmt);
}

//**************************COMBAT FUNCTIONS**************************
std::string multiscreen::damage(std::string* argv)
{ //argv[0] = amount; argv[1] = tar<id>; argv[2] = src<id>; argv[3] = scene id

    //figure out key
    const char* query = "SELECT COUNT(*) FROM DAMAGEINSCENE";
    sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
    sqlite3_step(stmt);
    int key = sqlite3_column_int(stmt,0) + 1;

    //get current target HP
    //CHARACTERS(ID,NAME,REALNAME,CURHP,TOTALHP,XP)
    std::string q = "SELECT * FROM CHARACTERS WHERE ID =" + argv[1];
    const char* query3 = q.c_str();
    sqlite3_prepare(db, query3, strlen(query3), &stmt, &pz);
    sqlite3_step(stmt);
    int curhp = sqlite3_column_int(stmt,3);
    int totalhp = sqlite3_column_int(stmt,4);

    if (curhp-stoi(argv[0]) > totalhp)
    {
        //*alert = "Healed for too much, instead set HP to max!";
        *alert = "^Trying to heal by " + argv[0].substr(1,argv[0].length()-1) + " but that puts you over " + std::to_string(totalhp) + "!~";
        argv[0] = std::to_string((totalhp-curhp)*-1);
    }
    else if (curhp-stoi(argv[0]) < 0)
    {
        //*alert = "Damaged for too much, set HP to zero.";
        *alert = "^Trying to damage for " + argv[0] + " but that you under zero.~";
        argv[0] = std::to_string(curhp);
    }

    //insert into DAMAGEINSCENE(KEY,SCENE_ID,SRC_ID,TAR_ID,VAL)
    q = "INSERT INTO DAMAGEINSCENE (KEY, SCENE_ID, SRC_ID, TAR_ID, VAL) VALUES(" + std::to_string(key) + ",'" + argv[3] + "','" + argv[2] + "','" + argv[1] + "','" + argv[0] + "');";
    const char* query2 = q.c_str();
    sqlite3_prepare(db, query2, strlen(query2), &stmt, &pz);
    sqlite3_step(stmt);

    //update character HP
    //CHARACTERS(ID,NAME,REALNAME,CURHP,TOTALHP,XP)
    q = "UPDATE CHARACTERS SET CURHP = '" + std::to_string(curhp-stoi(argv[0])) + "' WHERE ID = '" + argv[1] + "';";
    const char* query4 = q.c_str();
    sqlite3_prepare(db, query4, strlen(query4), &stmt, &pz);
    sqlite3_step(stmt);

    return pclist();
}

//**************************SCENE FUNCTIONS**************************
int multiscreen::getScenes(std::string**& chars)
{
    const char *query = "SELECT Count(*) FROM SCENES;";
    sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
    int count = sqlite3_step(stmt);
    if (!count) return 0;
    count = sqlite3_column_int(stmt,0);

    chars = new std::string*[count];

    const char *query2 = "SELECT * FROM SCENES ORDER BY ID ASC;";
    sqlite3_prepare(db, query2, strlen(query2), &stmt, &pz);

    for (int i=0; i<count; i++)
    {
        chars[i] = new std::string[2];
        sqlite3_step(stmt);
        chars[i][0] = std::to_string(sqlite3_column_int(stmt,0));//id
        chars[i][1] = std::string( reinterpret_cast< const char* >(sqlite3_column_text(stmt,1)));//name
    }
    if (!count) *alert = "^No scenes were found!~";
}
std::string multiscreen::scenelist()
{
    std::string **scenes, out = "*Multiscreen Interface~ `<Scene List>~\n      \n*ID~      *Title~     \n\n"; //spaces a workaround. fix later
    int numScenes = getScenes(scenes);
    for (int i=0; i<numScenes; i++)
    {
        out += scenes[i][0];
        for (int w=0; w<8-scenes[i][0].length(); w++) out += ' ';
        out += scenes[i][1] + " \n";
    }
    return out;
}
std::string multiscreen::setscene(std::string id)
{
    std::string **scenes, out = "";
    int numScenes = getScenes(scenes);
    for (int i=0; i<numScenes; i++)
    {
        if (id == scenes[i][0])
        {
            currentScene = std::stoi(id);
            out += "`Scene: " + scenes[i][1] + "~";
            for (int w=0; w<57-scenes[i][1].length(); w++) out += ' ';
            out += "`ID: " + scenes[i][0] + "~ \n";
            return out;
        }
    }
    *alert = "^No scene matching that ID was found!~";
    if (numScenes) setscene("1");
    else return "";
}
std::string multiscreen::sceneinfo(std::string id)
{
    std::string **scenes, out = "*Multiscreen Interface~ `<Scene Info>~\n  \n";
    int numScenes = getScenes(scenes);
    for (int i=0; i<numScenes; i++)
    {
        if (id == scenes[i][0])
        {
            out += "ID:              `" + scenes[i][0] + "~\n";
            out += "Scene Name:      `" + scenes[i][1] + "~\n";
            return out;
        }
    }
    *alert = "^No scenes matching that ID were found!~";
    return out;
}
void multiscreen::editscene(std::string* argv)
{
    std::string q = "UPDATE SCENES SET " + argv[1] + " = " + '\'' + argv[2] + "\' WHERE ID = " + argv[0] + ';';
    const char *query = q.c_str();
    sqlite3_prepare(db, query, strlen(query), &stmt, &pz);
    sqlite3_step(stmt);
}

//**************************STRING HELPER FUNCTIONS**************************
void convertString(std::string& input)
{
    for (int i=0; i<input.length(); i++)
    {
        if (input[i] == '*') input.replace(i,1,"\033[4m\033[33m"); //orange text w/ underline
        if (input[i] == '~') input.replace(i,1,"\033[39m\033[24m"); //default text
        if (input[i] == '`') input.replace(i,1,"\033[37m"); //white text
        if (input[i] == '^') input.replace(i,1,"\033[31m\033[52m"); //red text
    }
}

int nf_len(std::string input)
{
    //13 is the return carriage character, it shows up before newline characters in files. can safely remove that.
    for (int i=0; i<input.length(); i++) if (input[i] == '*' || input[i] == '~' || input[i] == '`' || input[i] == '+' || input[i] == '^' || input[i] == 13) input.erase(i--,1);
    return input.length();
}