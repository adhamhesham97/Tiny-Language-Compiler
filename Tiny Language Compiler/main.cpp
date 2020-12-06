#include<iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;


bool is_letter(char c) // Return true if char c is letter or number or underscore 
{
	string temp = "";
	temp = c;
	regex reg_ex("[a-zA-Z0-9_]");
	return regex_match(temp, reg_ex);
}

bool is_reserved_word(string token) // Return true if token is reserved word 
{
	return token == "read" || token == "then" || token == "if" || token == "else" || token == "end" || token == "repeat" || token == "until" || token == "write";
}

bool is_symbol(string token) // Return true if token is a valid symbol 
{
	return token == "+" || token == "-" || token == "<" || token == ">" || token == "/" || token == "*" || token == "(" || token == ")" || token == ":=" || token == ";" || token == "=";
}

void scanner(string path, vector<string> &tokenValue, vector<string> &tokenType)
{
	ifstream inFile;
	inFile.open(path);
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
					if (!is_letter(text[i + 1]))
					{
						tokenValue.push_back(token);
						tokenType.push_back("Reserved");
						token = "";	//empty string to start a new one
					}
				}

				//Symbole ("+", ":=" , ...)
				else if (is_symbol(token))
				{
					if (token == "+")
					{
						tokenValue.push_back(token);
						tokenType.push_back("plus");
					}
					else if (token == "-")
					{
						tokenValue.push_back(token);
						tokenType.push_back("minus");
					}
					else if (token == "<")
					{
						tokenValue.push_back(token);
						tokenType.push_back("smaller");
					}
					else if (token == ">")
					{
						tokenValue.push_back(token);
						tokenType.push_back("bigger");
					}
					else if (token == "/")
					{
						tokenValue.push_back(token);
						tokenType.push_back("divide");
					}
					else if (token == "*")
					{
						tokenValue.push_back(token);
						tokenType.push_back("times");
					}
					else if (token == ";")
					{
						tokenValue.push_back(token);
						tokenType.push_back("semicolon");
					}
					else if (token == ":=")
					{
						tokenValue.push_back(":=");
						tokenType.push_back("assign");
					}
					else if (token == "=")
					{
						tokenValue.push_back(token);
						tokenType.push_back("equal");
					}
					token = ""; //empty string to start a new one
				}

				//Identifiers & numbers
				// Token is neither a symbol nor a reserved word 
				// and the next letter isn't letter or number or underscore ( variable name is finished )
				else if (!is_letter(text[i + 1]) && token != ":")
				{
					if (token[0] >= '0' && token[0] <= '9')	//starts with number means it's a number 
					{
						tokenValue.push_back(token);
						tokenType.push_back("Number");
						token = ""; //empty string to start a new one
					}
					else //otherwise it's a variable
					{
						tokenValue.push_back(token);
						tokenType.push_back("Identifier");
						token = ""; //empty string to start a new one
					}
				}
			}
		}
	}
	inFile.close();
}

int main()
{
	string input = "Tiny code example.txt";		// in same file directory
	vector<string> tokenValue, tokenType;
	scanner(input, tokenValue, tokenType);
	ofstream outFile;
	outFile.open("scanned Tiny code.txt");	// in same file directory 

	for (int i = 0; i < tokenValue.size(); i++)
	{
		outFile << tokenValue[i] << " , " << tokenType[i] << endl; // Printing the Tokens value & type
	}
	outFile.close();
}