#include<iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <direct.h>

using namespace std;

#include <windows.h>

void read_directory(string name, vector<string>& v)
{
	string pattern = name+"\\*.txt";
	WIN32_FIND_DATA data;
	HANDLE hFind;
	wstring stemp = wstring(pattern.begin(), pattern.end());
	LPCWSTR sw = stemp.c_str();
	if ((hFind = FindFirstFile(sw, &data)) != INVALID_HANDLE_VALUE) {
		do {
			wstring ws(data.cFileName);
			string filename = string(ws.begin(), ws.end()-4);
			v.push_back(filename);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}

string get_current_path()
{
	char cCurrentPath[FILENAME_MAX];
	if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
	{
		return ""; //error
	}
	return cCurrentPath;
}

bool is_letter(char c) // Return true if char c is letter
{
	string temp = "";
	temp = c;
	regex reg_ex("[a-zA-Z]");
	return regex_match(temp, reg_ex);
}

bool is_letter_number(char c) // Return true if char c is letter or number
{
	string temp = "";
	temp = c;
	regex reg_ex("[a-zA-Z0-9]");
	return regex_match(temp, reg_ex);
}

bool is_reserved_word(string token) // Return true if token is reserved word 
{
	return token == "read" || token == "then" || token == "if" || token == "else" || token == "end" || token == "repeat" || token == "until" || token == "write";
}

bool is_symbol(string token) // Return true if token is a valid symbol 
{
	return token == "+" || token == "-" || token == "<" || token == "/" || token == "*" || token == "(" || token == ")" || token == ":=" || token == ";" || token == "=";
}


void scanner(string path, vector<string> &tokenValue, vector<string> &tokenType, bool &success)
{
	success = true;
	ifstream inFile;
	inFile.open(path);
	if (!inFile.good())
	{
		success = false;
		return;
	}
	string text;
	bool comment = false;
	while (inFile >> text) // Parsing the file line by line 
	{
		string token = "";	//empty string at the start of each line
		for (int i = 0; i < text.length(); i++)	//loop on each letter in the line
		{
			if (text[i] == '{') //the start of comment
			{
				comment = true;
			}
			if (text[i] == '}')  //the end of comment
			{
				comment = false;
				i++; //skip this character
			}

			if (comment == false)	//if not inside a comment
			{
				//skip white space at the start of each token 
				if (token == "")
				{
					while (i < text.length() && text[i] == ' ')
						i++;
					if (i >= text.length())	//the line is finished
						break;	//break from for loop to the next line
				}

				token += text[i];	//add char to token

									//Reserved word ("read", "if" , ...) 
				if (is_reserved_word(token))
				{
					//the next char must not be letter or number or underscore
					//otherwise it's a variable name. like: reading , repeat2 , if_2
					if (!is_letter_number(text[i + 1]))
					{
						// Pushing the names of the reserved words into TokenType
						tokenValue.push_back(token);
						if (token == "if") { tokenType.push_back("IF"); }
						else if (token == "else") { tokenType.push_back("ELSE"); }
						else if (token == "repeat") { tokenType.push_back("REPEAT"); }
						else if (token == "end") { tokenType.push_back("END"); }
						else if (token == "read") { tokenType.push_back("READ"); }
						else if (token == "write") { tokenType.push_back("WRITE"); }
						else if (token == "then") { tokenType.push_back("THEN"); }
						else if (token == "until") { tokenType.push_back("UNTIL"); }
						//tokenType.push_back("RESERVED");
						token = "";	//empty string to start a new one
					}
				}

				//Symbole ("+", ":=" , ...)
				else if (is_symbol(token))
				{
					if (token == "+")
					{
						tokenValue.push_back(token);
						tokenType.push_back("PLUS");
					}
					else if (token == "-")
					{
						tokenValue.push_back(token);
						tokenType.push_back("MINUS");
					}
					else if (token == "<")
					{
						tokenValue.push_back(token);
						tokenType.push_back("LESSTHAN");
					}
					else if (token == "/")
					{
						tokenValue.push_back(token);
						tokenType.push_back("DIV");
					}
					else if (token == "*")
					{
						tokenValue.push_back(token);
						tokenType.push_back("MULT");
					}
					else if (token == ";")
					{
						tokenValue.push_back(token);
						tokenType.push_back("SEMICOLON");
					}
					else if (token == ":=")
					{
						tokenValue.push_back(":=");
						tokenType.push_back("ASSIGN");
					}
					else if (token == "=")
					{
						tokenValue.push_back(token);
						tokenType.push_back("EQUAL");
					}
					else if (token == "(")
					{
						tokenValue.push_back(token);
						tokenType.push_back("OPENBRACKET");
					}
					else if (token == ")")
					{
						tokenValue.push_back(token);
						tokenType.push_back("CLOSEDBRACKET");
					}
					token = ""; //empty string to start a new one
				}

				//Identifiers & numbers
				// Token is neither a symbol nor a reserved word 
				// and the next letter isn't letter or number or underscore ( variable name is finished )
				else if (!is_letter_number(text[i + 1]) && token != ":")
				{
					if (token[0] >= '0' && token[0] <= '9')	//starts with number means it's a number 
					{
						bool all_digits = true;
						for (int j = 0; j < token.length(); j++)
						{
							if (token[j] < '0' || token[j] > '9')
								all_digits = false;
						}
						if (all_digits)
						{
							tokenValue.push_back(token);
							tokenType.push_back("NUMBER");
							token = ""; //empty string to start a new one
						}
						else
						{
							tokenValue.push_back(token);
							tokenType.push_back("ERROR");
							token = ""; //empty string to start a new one
						}
					}
					else //otherwise it's a variable
					{
						bool all_letters = true;
						for (int j = 0; j < token.length(); j++)
						{
							if (is_letter(token[j]) == false)
								all_letters = false;
						}
						if (all_letters)
						{
							tokenValue.push_back(token);
							tokenType.push_back("IDENTIFIER");
							token = ""; //empty string to start a new one
						}
						else
						{
							tokenValue.push_back(token);
							tokenType.push_back("ERROR");
							token = ""; //empty string to start a new one
						}
					}
				}
			}
		}
	}
	success = true;
	inFile.close();
}

int main()
{
	string currentDir = get_current_path();
	/*for (size_t i = 0; i < filenames.size(); i++)
	{
		cout << filenames[i] << endl;
	}*/
	while (1)
	{
		bool success ;
		string input = "";		// in same file directory
		string c;
		cout << "\n\n\tPlease enter file path or name (if in this folder) or press enter (for all files in current directory): ";
		getline(cin, input);
		if (input == "") //all files
		{
			vector<string> filenames;
			read_directory(currentDir, filenames);
			string outputdir = currentDir + "\\outputs";
			mkdir(outputdir.c_str());
			for (size_t i = 0; i < filenames.size(); i++)
			{
				vector<string> tokenValue, tokenType;
				scanner(currentDir + "\\" + filenames[i] + ".txt", tokenValue, tokenType, success);
				ofstream outFile;
				outFile.open(outputdir+ "\\" + filenames[i] + " - output.txt");	// in same file directory 
				
				for (int i = 0; i < tokenValue.size(); i++)
				{
					outFile << tokenValue[i] << " , " << tokenType[i] << endl; // Printing the Tokens value & type
				}
				outFile.close();
			}
			cout << "\t " << filenames.size() << " text files are dound" << endl;
		}
		else
		{
			vector<string> tokenValue, tokenType;
			scanner(input, tokenValue, tokenType, success);
			ofstream outFile;
			outFile.open(input.substr(0, input.length()-4) +" - output.txt");	// in same file directory 

			if (success)
			{
				for (int i = 0; i < tokenValue.size(); i++)
				{
					outFile << tokenValue[i] << " , " << tokenType[i] << endl; // Printing the Tokens value & type
				}
				outFile.close();
				cout << "\n\n\t\t\tDone. Output text file is included in folder with name: scanned Tiny code.txt" << endl;
			}
			else
				cout << "\n\n\t\t\t\t\t\tFile not found. \n";
		}

		cout << "EXIT CONSOLE? (Y/N) ";
		getline(cin, c);
		if (c == "Y" || c == "y")
			break;
		//else clear screen
		else
		{
			system("CLS");
			cout << flush;
		}
	}
}