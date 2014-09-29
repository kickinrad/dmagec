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
		if (input[i] == '*') input.replace(i,1,"\033[4m\033[33m"); //orange text w/ underline
		if (input[i] == '~') input.replace(i,1,"\033[39m\033[24m");	//default text
		if (input[i] == '`') input.replace(i,1,"\033[37m"); //white text
		if (input[i] == '+') input.replace(i,1,"\033[41m"); //red highlight
		if (input[i] == '^') input.replace(i,1,"\033[31m\033[52m"); //red text
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
			for (int i=0; i<3; i++) update += ' ';

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

void alert(std::string str)
{
	convertString(str);
	str.resize(71,' ');
	std::cout << "\033[39;2H" << str << "\033[41;12H                                                                                       \033[41;12H";
}

int main()
{
	char* clear = "\033[2J\033[1;1H\033[0m";
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
	rc = sqlite3_open("dmagec.db", &db);
	updatePCs(db);
	//end db stuff
	while (input!="q")
	{
		std::cout << "\033[41;12H                                                                                       \033[41;12H"; //clear command window
		std::cin >> input; //wait for input
		alert("");
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
			alert("^Please enter a character ID!~");
			std::cin >> id;
			if (id)
			{
				alert("");
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
				else
				{
					alert("^ID was not recognized! Please try again.~");
					changeScreen(2,20,18,71,"multiscreens/home");
				}
			}
			else alert("^There was an error with the ID you entered.~");
		}
		else if (input =="charlist")
		{
			const char *query = "SELECT Count(*) FROM CHARACTERS;";
			sqlite3_prepare(db, query, strlen(query), &stmt, &pzTest);
			int id = sqlite3_step(stmt);

			if (id)
			{
				changeString(2,20,18,71,"*Multiscreen Interface~ `<Character List>~");
				alert("`Press enter to advance page. · = Player Character~");
				std::cout << "\033[22;2H\033[4m\033[33mID\033[24m    \033[4mName\033[39m\033[24m";
				int numChars = sqlite3_column_int(stmt,0);

				const char *query2 = "SELECT * FROM CHARACTERS ORDER BY ID ASC;";
				sqlite3_prepare(db, query2, strlen(query2), &stmt, &pzTest);
				for (int i=0; i<numChars; i++)
				{
					sqlite3_step(stmt);
					if (sqlite3_column_text(stmt,2)) std::cout << "\033[" << 23+i << ";7H·";
					std::cout << "\033[" << 23+i << ";2H" << sqlite3_column_int(stmt,0) << "\033[" << 23+i << ";8H" << sqlite3_column_text(stmt,1);
				}
			}
			else
			{
				alert("^Something went wrong. No characters in DB?~");
				changeScreen(2,20,18,71,"multiscreens/home");
			}
		}
		else if (input == "addchar")
		{
			const char *query = "SELECT * FROM CHARACTERS ORDER BY ID DESC;";
			sqlite3_prepare(db, query, strlen(query), &stmt, &pzTest);
			int id = sqlite3_step(stmt);
			if (id) id = 1 + sqlite3_column_int(stmt,0);
			else id = 1;
			alert("Is this a player character? (y/n)");
			char c_input;
			int i_input;
			std::string s_input;
			std::cin >> c_input;
			if (c_input == 'y') //PC
			{
				std::string q = "INSERT INTO CHARACTERS (ID,NAME,REALNAME,CURHP,TOTALHP,XP) VALUES(";
				q += std::to_string(id);
				alert("Enter the character's name.");
				std::cin >> s_input;
				q += ",'" + s_input;
				alert("Enter the player's name.");
				std::cin >> s_input;
				q += "','" + s_input;
				alert("Enter the character's hit points.");
				std::cin >> i_input;
				q += "'," + std::to_string(i_input) + ',' + std::to_string(i_input) + ",0);";
				//std::cout << "\033[43;2H" << q;
				const char *query2 = q.c_str();
				sqlite3_prepare(db, query2, strlen(query2), &stmt, &pzTest);
				sqlite3_step(stmt);

				alert("Character added!");
				updatePCs(db);
				changeScreen(2,20,18,71,"multiscreens/home");
			}
			else if (c_input == 'n') //NPC
			{
				std::string q = "INSERT INTO CHARACTERS (ID,NAME,CURHP,TOTALHP) VALUES(";
				q += std::to_string(id);
				alert("Enter the character's name.");
				std::cin >> s_input;
				q += ",'" + s_input;
				alert("Enter the character's hit points.");
				std::cin >> i_input;
				q += "'," + std::to_string(i_input) + ',' + std::to_string(i_input) + ");";
				//std::cout << "\033[43;2H" << q;
				const char *query2 = q.c_str();
				sqlite3_prepare(db, query2, strlen(query2), &stmt, &pzTest);
				int err2 = sqlite3_step(stmt);

				alert("Character added!");
				changeScreen(2,20,18,71,"multiscreens/home");
			}
			else
			{
				alert("^Error with input.?~");
				changeScreen(2,20,18,71,"multiscreens/home");
			}
		}
		else
		{
			alert("^Command was not recognized! Please try again.~");
		}
	}
	
}
