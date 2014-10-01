#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <sqlite3.h>
#include "multiscreen.h"

void alert(std::string str) //place given string into alert window and clear command window
{
    convertString(str);
    str.resize(71,' ');
    std::cout << "\033[39;2H" << str << "\033[41;12H                                                                                       \033[41;12H";
}

void placeString(std::string in, int x, int y, int w, int h) //place given string at given coordinates, limiting to given width and height
{
    std::stringstream stream;
    stream.str(in);
    for (int row=0; row<h; row++)
    {
        if (std::getline(stream, in,'\n') && in[0]!=13)
        {
            int len = nf_len(in);
            convertString(in);
            std::cout << "\033[" << y+row << ';' << x << 'H' << in;
            std::cout << "\033[" << y+row << ";" << x+len << 'H';
            for(int ws=len; ws<w; ws++) std::cout << ' ';
        }
        else
        {
            std::cout << "\033[" << y+row << ";" << x << 'H';
            for (int ws=0; ws<w; ws++) std::cout << ' ';
        }
    }
}

int main()
{
    //**************************INITIALIZE DATABASE**************************
    sqlite3 *db;
    sqlite3_open("dmagec.db", &db);
    multiscreen ms(db);

    //**************************INITIALIZE CONSOLE VIEWPORT**************************
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
    std::string defaultScene = "1";
    placeString(ms.act("setscene",&defaultScene), 3, 11, 70, 1);

    //**************************COMMAND LINE LOOP**************************
    while (true)
    {
        std::string input[6]; //input[0] is the full console input. any additional arguments are put into the following indices.
        std::cout << "\033[41;12H"; //place cursor in command line

        std::string alert_string; //for alerts returned from multiscreen::act()
        std::getline(std::cin,input[0]); //get input from user, place raw input (terminated by newline) into input[0]

        std::stringstream stream; //stringstream for parsing user input
        stream.str(input[0]); //intialize stream with raw user input
        int counter = 0;
        while(std::getline(stream,input[++counter],' ')); //go through each clause of input, seperated by ' 's, and put each into input array.
        alert(""); //clear the alert window
        
        if (input[1]=="q" || input[1]=="quit")
        {
            std::cout << "\033[2J\033[1;1H\033[0mSucessfully saved and exited." << std::endl;
            sqlite3_close(db);
            return 0;
        }
        else if (input[1]=="home" || input[1]=="help" || input[1]== "charlist" || input[1]=="scenelist") placeString(ms.act(input[1]), 2, 20, 71, 18);
        else if (input[1]=="charinfo") placeString(ms.act(input[1],&input[2],&alert_string), 2, 20, 71, 18);
        else if (input[1]=="addpc") ms.act(input[1],input+2);
        else if (input[1]=="addnpc") ms.act(input[1],input+2);
        else if (input[1]=="setscene") placeString(ms.act(input[1],&input[2],&alert_string), 3, 11, 70, 1);
        else
        {
            alert("^Command was not recognized! Please try again.~");
            continue;
        }

        alert(alert_string);
    }
    
}
