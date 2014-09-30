#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <sqlite3.h>
#include "multiscreen.h"

void alert(std::string str)
{
    convertString(str);
    str.resize(71,' ');
    std::cout << "\033[39;2H" << str << "\033[41;12H                                                                                       \033[41;12H";
}

void placeString(std::string in, int x, int y, int w, int h)
{
    std::stringstream stream;
    stream.str(in);
    for (int row=0; row<h; row++)
    {
        if (std::getline(stream, in))
        {
            int len = nf_len(in);
            convertString(in);
            std::cout << "\033[" << y+row << ';' << x << 'H' << in.substr(0,in.length()-1);
            for (int ws=0; ws<w-len; ws++) std::cout << ' ';
        }
        else
        {
            std::cout << "\033[" << y+row << ';' << x << 'H';
            for (int ws=0; ws<w; ws++) std::cout << ' ';
        }
    }
}

int main()
{
    //**************************INITIALIZE DATABASE**************************
    sqlite3_stmt *stmt;
    const char *pzTest;
    sqlite3 *db;
    int rc;
    rc = sqlite3_open("dmagec.db", &db);
    multiscreen ms(db);
    //updatePCs(db);
    placeString(ms.pclist(), 2, 4, 71, 6);

    //**************************INITIALIZE CONSOLE VIEWPORT**************************
    //std::string input[6]; //input[0] is the full console input. any additional arguments are put into the following indices.
    //std::stringstream stream;

    std::ifstream consoleLayoutFile("consoleLayout.formatted");
    std::string consoleLayoutFileLine[42];
    std::cout << "\033[2J\033[1;1H\033[0m";
    for (int i=0; i<42; i++)
    {
        std::getline(consoleLayoutFile, consoleLayoutFileLine[i], '\n');
        convertString(consoleLayoutFileLine[i]);
    }
    for (int i=0; i<42; i++) std::cout << consoleLayoutFileLine[i] << std::endl;

    placeString(ms.pclist(), 2, 4, 71, 6);
    placeString(ms.act("home"), 2, 20, 71, 18);

    //**************************COMMAND LINE LOOP**************************
    while (true) //temporary, there's gotta be a better way to do this.
    {
        std::string input[6]; //input[0] is the full console input. any additional arguments are put into the following indices.
        std::cout << "\033[41;12H"; //place cursor in command line

        std::string alert_string = "1";
        //placeString(ms.act("setscene", &alert_string), 3, 11, 70, 1); //initialization not working, fix
        alert_string = "";

        std::cin.clear();
        std::getline(std::cin,input[0]);

        std::stringstream stream;
        stream.str(input[0]);
        int counter = 0;
        while(std::getline(stream,input[++counter],' '));

        alert("");
        //quit
        if (input[1]=="q" || input[1]=="quit")
        {
            std::cout << "\033[2J\033[1;1H\033[0mSucessfully saved and exited." << std::endl;
            sqlite3_close(db);
            return 0;
        }
        //multiscreen
        else if (input[1]=="home" || input[1]=="help" || input[1]== "charlist" || input[1]=="scenelist") placeString(ms.act(input[1]), 2, 20, 71, 18);
        else if (input[1]=="charinfo") placeString(ms.act(input[1],&input[2],&alert_string), 2, 20, 71, 18);
        else if (input[1]=="addpc") ms.act(input[1],input+2);
        else if (input[1]=="addnpc") ms.act(input[1],input+2);
        else if (input[1]=="setscene") placeString(ms.act(input[1],&input[2],&alert_string), 3, 11, 70, 1);
        else
        {
            alert("^Command was not recognized! Please try again.~");
        }

        alert(alert_string);
    }
    
}
