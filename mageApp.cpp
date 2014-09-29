#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <sstream>
#include <sqlite3.h>

void convertString(std::string& input)
{
	for (int i=0; i<input.length(); i++)
	{
		if (input[i] == '*') input.replace(i,1,"\033[4m\033[33m");
		if (input[i] == '~') input.replace(i,1,"\033[39m\033[24m");		
		if (input[i] == '`') input.replace(i,1,"\033[37m");
		if (input[i] == '+') input.replace(i,1,"\033[41m");
		if (input[i] == '^') input.replace(i,1,"\033[31m\033[52m");
	}
}

void clearSpace(int x, int y, int h, int w)
{
	for (int i=0; i<h; i++)
	{
		std::cout << "\033[" << y+i <<";" << x << "H";
		for (int n=0; n<w; n++) std::cout << " ";
	}
}

void changeScreen(int x, int y, int h, int w, std::string fileName)
{
	clearSpace(x,y,h,w);
	std::ifstream multiScreenFile;
	std::string fileOutput[h];
	multiScreenFile.open(fileName);
	for (int i=0; i<h; i++)
	{
		std::getline(multiScreenFile, fileOutput[i], '\n');
		convertString(fileOutput[i]);
	}
	for (int i=0; i<h; i++)
	{
		std::cout << "\033[" << y+i << ";" << x <<"H" << fileOutput[i];
	}
}

void changeString(int x, int y, int h, int w, std::string str)
{
	clearSpace(x,y,h,w);
	std::stringstream stringStream;
	std::string fileOutput[h];
	stringStream.str(str);
	for (int i=0; i<h; i++)
	{
		std::getline(stringStream, fileOutput[i], '\n');
		convertString(fileOutput[i]);
	}
	for (int i=0; i<h; i++)
	{
		std::cout << "\033[" << y+i << ";" << x <<"H" << fileOutput[i];
	}
}

void updatePCs(sqlite3 *db)
{
	std::string update = "";
	sqlite3_stmt *stmt;
	const char *pzTest;
	const char *query = "SELECT * FROM CHARACTERS WHERE REALNAME IS NOT NULL ORDER BY NAME ASC;";
	sqlite3_prepare(db, query, strlen(query), &stmt, &pzTest);
	int s;

	while (s!=SQLITE_DONE)
	{
		s = sqlite3_step(stmt);
		if (s==SQLITE_ROW)
		{
			std::string text;

			//name
			text = std::string( reinterpret_cast< const char* >(sqlite3_column_text(stmt,1)));
			update.append(text);
			for (int i=0; i<28-text.length(); i++) update += ' ';

			//realname
			text = std::string( reinterpret_cast< const char* >(sqlite3_column_text(stmt,2)));
			update.append(text);
			for (int i=0; i<20-text.length(); i++) update += ' ';

			//id
			text = std::to_string(sqlite3_column_int(stmt,0));
			for (int i=0; i<4-text.length(); i++) update += '0'; //leading zeroes
			update.append(text);
			for (int i=0; i<4-text.length(); i++) update += ' ';

			//xp
			text = std::to_string(sqlite3_column_int(stmt,5));
			update.append(text);
			for (int i=0; i<8-text.length(); i++) update += ' ';

			//cur hp
			text = std::to_string(sqlite3_column_int(stmt,3));
			update.append(text);
			update += '/';
			//total hp
			text = std::to_string(sqlite3_column_int(stmt,4));
			update.append(text);
			update += '\n';
		}
	}
	changeString(2,4,6,71,update);
}

int main()
{
	char* clear = "\033[2J\033[1;1H";
	char *err=0;
	char *sql;
	//const char* data = "Callback function called";
	std::ifstream consoleLayoutFile("consoleLayout.formatted");
	std::string input, line[42], multiScreen[18];
	std::cout << clear;
	for (int i=0; i<42; i++)
	{
		std::getline(consoleLayoutFile, line[i], '\n');
		convertString(line[i]);
	}
	for (int i=0; i<42; i++) std::cout << line[i] << std::endl;
	changeScreen(2,20,18,71,"multiscreens/home");

	//db stuff
	sqlite3_stmt *stmt;
	const char *pzTest;

	sqlite3 *db;
	int rc;
	rc = sqlite3_open("mageApp.db", &db);
	updatePCs(db);
	//end db stuff
	while (input!="q")
	{
		std::cout << "\033[41;12H                                                                                       \033[41;12H"; //clear command window
		std::cin >> input; //wait for input
		std::cout << "\033[39;3H                                                                      "; //clear error window
		if (input=="q" || input=="quit")
		{
			std::cout << clear << "Sucessfully saved and exited." << std::endl;
			sqlite3_close(db);
			return 0;
		}
		else if (input=="home" || input=="help")
		{
			const unsigned char *text;
			changeScreen(2,20,18,71,"multiscreens/"+input);
		}
		else if (input=="charinfo")
		{
			changeString(2,20,18,71,"*Multiscreen Interface~ `<Character Info>~");
			int id;
			std::cin >> id;
			std::string q = "SELECT * FROM CHARACTERS WHERE ID = ";
			q.append(std::to_string(id));
			q += ';';
			const char *query = q.c_str();

			sqlite3_prepare(db, query, strlen(query), &stmt, &pzTest);
			id = sqlite3_step(stmt);
			if (sqlite3_column_int(stmt,0))
			{
				std::cout << "\033[22;2H\033[37mCharacter Name\033[0m: " << sqlite3_column_text(stmt,1);
				if (sqlite3_column_text(stmt,2))
				{
					std::cout << "\033[23;2H\033[37mPortrayed by\033[0m: " << sqlite3_column_text(stmt,2);
					std::cout << "\033[24;2H\033[37mExperience Points\033[0m: " << sqlite3_column_int(stmt,5);
				}
				else
				{
					std::cout << "\033[23;2HNon-Player Character";
				}
				std::cout << "\033[26;2H\033[37mHit Points\033[0m: " << sqlite3_column_int(stmt,3) << '/' << sqlite3_column_int(stmt,4);
			}
			else std::cout << "\033[39;3H\033[31mID was not recognized! Please try again.                              \033[0m\033[41;12H";
		}
		else
		{
			std::cout << "\033[39;3H\033[31mCommand was not recognized! Please try again.                         \033[0m\033[41;12H";
		}
	}
	
}
