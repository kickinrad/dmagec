#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <sqlite3.h>
#include "multiscreen.h"

void alert(std::string str) //place given string into alert window and clear command window
{
    convertString(str); //convert string, changing fancy characters into ansi escape sequences
    str.resize(71,' '); //resize string, filling any extra space with whitespace to clear window
    std::cout << "\033[39;2H" << str << "\033[41;12H                                                                                       \033[41;12H";
}

void placeString(std::string in, int x, int y, int w, int h) //place given string at given coordinates, limiting to given width and height
{
    std::stringstream stream; //stringstream for parsing the string line by line
    stream.str(in); //intialize stream with string
    for (int row=0; row<h; row++)
    {
        if (std::getline(stream, in,'\n'))
        {
            int len = nf_len(in); //store the "no format" length in a variable before we convert it
            convertString(in); //convert it, changing fancy characters into ansi escape sequences
            std::cout << "\033[" << y+row << ';' << x << 'H' << in; //place the line itself
            std::cout << "\033[" << y+row << ";" << x+len << 'H'; //move cursor to the end of the line
            for(int ws=0; ws<w-len; ws++) std::cout << ' '; //fill the rest of the line with whitespace
        }
        else //no lines left.. fill the rest with whitespace
        {
            std::cout << "\033[" << y+row << ";" << x << 'H'; //move cursor to beginning of line
            for (int ws=0; ws<w; ws++) std::cout << ' '; //whitespace to end
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
    std::cout << "\033[2J"; //clear out console
    std::ifstream consoleLayoutFile("consoleLayout.formatted");
    std::string consoleLayoutFileLine;
    std::getline(consoleLayoutFile,consoleLayoutFileLine,'\0');
    placeString(consoleLayoutFileLine, 1, 1, 33, 42); //place initial console layout

    placeString(ms.pclist(), 2, 4, 71, 6); //place list of PCs
    placeString(ms.act("home"), 2, 20, 71, 18); //place default (home) screen
    std::string defaultScene = "1"; //set default scene
    placeString(ms.act("setscene",&defaultScene), 3, 11, 70, 1); //place default scene

    //**************************COMMAND LINE LOOP**************************
    while (true)
    {
        std::string input[6]; //input[0] is the full console input. any additional arguments are put into the following indices.
        std::cout << "\033[41;12H"; //place cursor in command line

        std::string alert_string = ""; //for alerts returned from multiscreen::act()
        std::getline(std::cin,input[0]); //get input from user, place raw input (terminated by newline) into input[0]

        std::stringstream stream; //stringstream for parsing user input
        stream.str(input[0]); //intialize stream with raw user input
        int counter = 0;
        while(std::getline(stream,input[++counter],' ')); //go through each clause of input, seperated by ' 's, and put each into input array.
        alert(alert_string); //clear the alert window
        
        if (input[1]=="q" || input[1]=="quit")
        {
            alert("`Exited without errors. cya~");
            std::cout << "\033[43;0H" << std::endl; //place cursor below console before returning
            sqlite3_close(db); //close database
            return 0;
        }
        else if (input[1]=="home" || input[1]=="help" || input[1]== "charlist" || input[1]=="scenelist") placeString(ms.act(input[1]), 2, 20, 71, 18);
        else if (input[1]=="charinfo" || input[1]=="sceneinfo") placeString(ms.act(input[1],&input[2],&alert_string), 2, 20, 71, 18);
        else if (input[1]=="addpc" || input[1]=="addnpc") ms.act(input[1],input+2);
        else if (input[1]=="setscene") placeString(ms.act(input[1],&input[2],&alert_string), 3, 11, 70, 1);
        else if (input[1]=="editchar" || input[1]=="editscene") ms.act(input[1],&input[2]);
        else
        {
            alert("^Command was not recognized! Please try again.~");
            continue; //loop around, so we don't hit that alert below that's used by above database::act() statements
        }
        if (alert_string == "") alert("`Enter 'home' to return to the home screen.~");
        else alert(alert_string);
    }
    
}