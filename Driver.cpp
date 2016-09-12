/*
Written By: Eric Button
Date: September 12, 2016
Time: 4:12 a.m.

Purpose of Program: To lexically analyze source code for some unknown language.
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cctype>

using namespace std;

void DeHomogonize		(	char charBuffer[],	int charBufferIndex	);	// break up the tokens
int  DetermineTokenType	(	char token[]							);
bool IfIsSingleChar		(	char *tokens[],		char token[]		);
bool IfIsDoubleChar		(	char *tokens[],		char token[]		);
bool IfIsString			(	char *tokens[],		char token			);
bool IfIsNum			(	char charString[]						);
bool IfIsID				(	char charString[]						);
bool IfIsKeyWord		(	char charString[]						);

void GetNextDataBlock	(	register char	charBuffer[],	register	int	 &charBufferIndex,	char data[]											);	// get more data to process
void NukePound			(	char			inputBuffer[]																							);	// remove comments
void PrintData			(	ofstream		&outputFile,				char token[],			int tokenType										);
void CharShunter		(	char			CharBuffer[],				char direction,			int NumSpacesToMove,	int	startLoc,	int	endLoc	);	// move character data around

int main()
{
register	char charBuffer[100001] = {0};
register	int  charBufferIndex    =  0 ;

	cout << "File Name: ";
	cin.getline(charBuffer, 10001);

	ifstream inputFile(charBuffer);
	ofstream outputFile("output.txt");

	char *tokens[29] = {"+",		// single
						"-",		// singele/double
						"&",		// single
						"<-",		// double
						",",		// single
						"{",		// single
						"}",		// single
						"function",	// keyword
						"a1a",		// alpha
						"if",		// keyword
						"/",		// single
						"*",		// single
						"!",		// singele/double
						"num",		// number
						"|",		// single
						"(",		// single
						")",		// single
						"==",		// double
						"!=",		// double
						"<",		// singele/double
						">",		// singele/double
						"<=",		// double
						">=",		// double
						"return",	// keyword
						";"			// single
						"\"",		// string
						"var",		// keyword
						"while"		// keyword
	};

	// start getting the data.

	// scrub the old data out of charBuffer.
	int j = strlen(charBuffer) + 500;

	for(int i = 0; i < j; i++)
		charBuffer[i] = 0;

	char inputBuffer[10000] = {0};

	// get all the data from the file
	while(! inputFile.eof() )
	{
		inputFile.getline(inputBuffer, 10000);
		NukePound(inputBuffer);
		strcat(charBuffer, " ");
		strcat(charBuffer, inputBuffer);
	}

	// finish getting the data.

	DeHomogonize(charBuffer, charBufferIndex);	// who needs "unGet" now! No one, that's who!

	j = strlen(charBuffer);

	for(int charBufferIndex = 0; charBufferIndex < j;)	// go until we run out of possible tokens.
	{
		char singleChar = 0;
		char doubleChar[2] = {0};

		char stringChar[10000] = {0};

		char idChar[10000] = {0};
		char SpecialChar[100] = {0};

		char dataBlock[10000] = {0};

		GetNextDataBlock(charBuffer, charBufferIndex, dataBlock);
		
		if(charBuffer[charBufferIndex] == 0)
			return 0;
		
		char tempDataBlock[3] = { dataBlock[0], dataBlock[1] };
		if(IfIsSingleChar(tokens, tempDataBlock) )
		{
			PrintData(outputFile, tempDataBlock, DetermineTokenType(tempDataBlock) );
			continue;
		}
		else
		if(IfIsDoubleChar(tokens, tempDataBlock) )
		{
			PrintData(outputFile, tempDataBlock, DetermineTokenType(tempDataBlock) );
			continue;
		}
		else
		if(IfIsString(tokens, dataBlock[0]) )
		{
			//int dataBlockIndex = 1;	// move dataBlockIndex to the real string data
			//int stringCharIndex = 0;
			//	while(dataBlock[dataBlockIndex] != '\"')
			//{
			//	stringChar[stringCharIndex] = dataBlock[dataBlockIndex];
			//	stringCharIndex += 1;
			//	dataBlockIndex  += 1;
			//}

			
			cout << "TOKEN:" << "STRING" << " " << dataBlock << endl;

			outputFile << "TOKEN:" << "STRING" << " " << dataBlock << endl;

			continue;
		}
		else
		if(IfIsNum(dataBlock) )
		{
			cout << "TOKEN:" << "NUMBER" << " " << dataBlock << endl;

			outputFile << "TOKEN:" << "NUMBER" << " " << dataBlock << endl;

			continue;
		}
		else
		if(IfIsKeyWord(dataBlock) )
		{
			PrintData(outputFile, dataBlock, DetermineTokenType(dataBlock) );
			continue;
		}
		else
		if(IfIsID(dataBlock) )
		{
			cout << "TOKEN:" << "ID" << " " << dataBlock << endl;

			outputFile << "TOKEN:" << "ID" << " " << dataBlock << endl;

			continue;
		}

		// error
		
		cout << "TOKEN:" << "ERROR" << " " << dataBlock << endl;

		outputFile << "TOKEN:" << "ERROR" << " " << dataBlock << endl;


	}

	return 0;
}

void DeHomogonize(char charBuffer[], int charBufferIndex)
{
	// Function: Splits up different token throughout the data (charBuffer)

register	int j = strlen(charBuffer);
register	int i = 0;

	while(i < j)
	{
ALPHA:
		if( isalpha(charBuffer[i]) )
		{
			i++;
			while( isalpha(charBuffer[i]) )
				i += 1;

			while( isdigit(charBuffer[i]) )
				i += 1;

			if( ispunct(charBuffer[i]) )
			{
				i -= 1;
				CharShunter(charBuffer, 'r', 1, i, j - 1);
				j += 1;
				i += 1;

				continue;
			}

		}
DIGIT:
		if( isdigit(charBuffer[i]) )
		{
			i++;
			while( isdigit(charBuffer[i]) )
				i += 1;

			while( isalpha(charBuffer[i]) )
			{
				i += 1;
			}
			
			if( ispunct(charBuffer[i]) )
			{
				i -= 1;
				CharShunter(charBuffer, 'r', 1, i, j - 1);
				j += 1;     
				i += 1;

				continue;
			}

		}

		if( ispunct(charBuffer[i]) )
		{
			char token[3] = {charBuffer[i], charBuffer[i+1]};
			
			i++;
			if
			(	strcmp(token ,	"<-") == 0	// double
			||	strcmp(token ,	"==") == 0	// double
			||	strcmp(token ,	"!=") == 0	// double
			||	strcmp(token ,	"<=") == 0	// double
			||	strcmp(token ,	">=") == 0	// double
			)
			{
				CharShunter(charBuffer, 'r', 1, i, j - 1);
				j += 1;
				i += 1;

				continue;
			}

			i -= 1;

			if(charBuffer[i] == '\"')
			{
				i += 1;

				while(charBuffer[i] != '\"')
				{
					if(charBuffer[i] == 32)
						charBuffer[i] = '_';
					i += 1;
				}

				CharShunter(charBuffer, 'r', 1, i, j - 1);
				j += 1;
				i += 1;
				continue;
			}

			CharShunter(charBuffer, 'r', 1, i, j - 1);
			j += 1;
			i += 1;
			continue;
		}

		if(charBuffer[i] == ' ')
			i += 1;
	}
}

int  DetermineTokenType(char token[])
{
	if(	token[0] ==		'+'		
	||	token[0] ==		'-'		)
		return 0;	// addop

	if(	token[0] ==		'&'		)
		return 1;	// and

	if(	token[0] ==		','		)
		return 3;	// comma

	if(	token[0] ==		'{'		)
		return 4;	// curll

	if(	token[0] ==		'}'		)
		return 5;	// curlr

	if(	token[0] ==		'/'		
	||	token[0] ==		'*'		)
		return 6;	// mulop

	if(	token[0] ==		'!'		)
		if( strcmp(token ,	"!=") == 0 )
			return 11;
		else
			return 7;

	if(	token[0] ==		'|'		)
		return 8;

	if(	token[0] ==		'('		)
		return 9;	// parenl

	if(	token[0] ==		')'		)
		return 10;	// parenr

	if(	token[0] ==		'<'		
	||	token[0] ==		'>'		)
		return 11;	// relop

	if(	token[0] ==		';'		)
		return 12;	// semicolon
	
	
	
	if(	strcmp(token ,	"<-") == 0	)
		return 2;	// assign op

	if(	strcmp(token ,	"==") == 0	
	||	strcmp(token ,	"!=") == 0	
	||	strcmp(token ,	"<=") == 0	
	||	strcmp(token ,	">=") == 0
	)
		return 11;	// relop

	if( strcmp(token , "else") == 0 )
		return 13;	// else
	
	if( strcmp(token , "function") == 0 )
		return 14;	// function
		
	if( strcmp(token , "if") == 0 )
		return 15;	// if
	
	if( strcmp(token , "return") == 0 )
		return 16;	// return
		
	if( strcmp(token , "var") == 0 )
		return 17;	// var
		
	if( strcmp(token , "while") == 0 )
		return 18;	// while
}

bool IfIsSingleChar(char *tokens[], char token[])
{
	if
	(	token[0] ==		'+'		// single
	||	token[0] ==		'-'		// singele/double
	||	token[0] ==		'&'		// single
	||	token[0] ==		','		// single
	||	token[0] ==		'{'		// single
	||	token[0] ==		'}'		// single
	||	token[0] ==		'/'		// single
	||	token[0] ==		'*'		// single
	||	token[0] ==		'!'		// singele/double
	||	token[0] ==		'|'		// single
	||	token[0] ==		'('		// single
	||	token[0] ==		')'		// single
	||	token[0] ==		'<'		// singele/double
	||	token[0] ==		'>'		// singele/double
	||	token[0] ==		';'		// single
	)
	{
		if
		(	strcmp(token ,	"<-") == 0	// double
		||	strcmp(token ,	"==") == 0	// double
		||	strcmp(token ,	"!=") == 0	// double
		||	strcmp(token ,	"<=") == 0	// double
		||	strcmp(token ,	">=") == 0	// double
		)
		{
			return false;
		}

		return true;
	}

	return false;
}

bool IfIsDoubleChar(char *tokens[], char token[])
{
	if
	(	strcmp(token ,	"<-") == 0	// double
	||	strcmp(token ,	"==") == 0	// double
	||	strcmp(token ,	"!=") == 0	// double
	||	strcmp(token ,	"<=") == 0	// double
	||	strcmp(token ,	">=") == 0	// double
	)
	{
		return true;
	}

	return false;
}

bool IfIsString(char *tokens[], char token)
{
	if(token == '\"')
		return true;

	return false;
}

bool IfIsNum(char charString[])
{
	int j = strlen(charString);

	if(! isdigit(charString[0]) )
		return false;

	for (int i = 0; i < j; i++)
	{
		if( isdigit( charString[i] ) )
			continue;
		if( isspace( charString[i] ) )
			return true;
		else
			return false;
	}

	return true;
}

bool IfIsID(char charString[])
{
	int j = strlen(charString);

	if(! isalpha(charString[0]) )
		return false;

	for(int i = 0; i < j; i++)
	{
		if( isalnum(charString[i]  ) )
			continue;
		if( isdigit( charString[i] ) )
			continue;
		if( ispunct( charString[i] ) )
			return true;
		if( isspace( charString[i] ) )
			return true;
		else
			return false;
	}
	
	return true;
}

bool IfIsKeyWord(char charString[])
{
	if
	(	strcmp(charString ,	"function"	) == 0	// double
	||	strcmp(charString ,	"if"		) == 0	// double
	||	strcmp(charString ,	"return"	) == 0	// double
	||	strcmp(charString ,	"var"		) == 0	// double
	||	strcmp(charString ,	"while"		) == 0	// double
	)
	{
		return true;
	}

	return false;
}

void NukePound(char inputBuffer[])
{
	// Function: Removes comments from the data (inputBuffer) being taken in from the file.
	int j = strlen(inputBuffer);
	int i = 0;

	for(i = 0; i < j; i)
	{
		if(inputBuffer[i] == '#')
			break;

		i++;
	}

	for(i; i < j; i++)
		inputBuffer[i] = 0;
}

void GetNextDataBlock(register char charBuffer[], register int &charBufferIndex, char data[])
{
	// Function: In the data (charBuffer) all the tokens are split up. This function gets the next token (or block of data, or whatever you want to call a string of characters).

	while(charBuffer[charBufferIndex] == ' ')	// Get past the whitespace.
		charBufferIndex += 1;
	
	int i = 0;
	while(charBuffer[charBufferIndex] != ' ' && charBuffer[charBufferIndex] != 0)	// read in data until you hit whitespace again.
	{
		data[i] = charBuffer[charBufferIndex];
		i += 1;
		charBufferIndex += 1;

	}

}

void PrintData(ofstream &outputFile, char token[], int tokenType)
{
	char tokenName[100] = {0};
	
	if(tokenType == 0)
		strcpy(tokenName, "ADDOP");		// addop

	if(tokenType == 1)
		strcpy(tokenName, "AND");		// and
	
	if(tokenType == 2)
		strcpy(tokenName, "ASSIGNOP");	// assign op

	if(tokenType == 3)
		strcpy(tokenName, "COMMA");		// comma

	if(tokenType == 4)
		strcpy(tokenName, "CURLL");		// curll

	if(tokenType == 5)
		strcpy(tokenName, "CURLR");		// curlr

	if(tokenType == 6)
		strcpy(tokenName, "MULOP");		// mulop

	if(tokenType == 7)
		strcpy(tokenName, "NOT");		// not

	if(tokenType == 8)
		strcpy(tokenName, "OR");		// or

	if(tokenType == 9)
		strcpy(tokenName, "PARENL");	// parenl

	if(tokenType == 10)
		strcpy(tokenName, "PARENR");	// parenr

	if(tokenType == 11)
		strcpy(tokenName, "RELOP");		// relop

	if(tokenType == 12)
		strcpy(tokenName, "SEMICOLON");	// semicolon

	if(tokenType == 13)
		strcpy(tokenName, "ELSE");		// else
	
	if(tokenType == 14)
		strcpy(tokenName, "FUNCTION");	// function
		
	if(tokenType == 15)
		strcpy(tokenName, "IF");		// if
	
	if(tokenType == 16)
		strcpy(tokenName, "RETURN");	// return
		
	if(tokenType == 17)
		strcpy(tokenName, "VAR");		// var
		
	if(tokenType == 18)
		strcpy(tokenName, "WHILE");		// while

	

	cout << "TOKEN:" << tokenName << " " << token << endl;

	outputFile << "TOKEN:" << tokenName << " " << token << endl;
}

void CharShunter(char CharBuffer[], char direction, int NumSpacesToMove, int startLoc, int endLoc)
{
	// CharBuffer-contains the char array that needs to be shunted.
	// direction--should be an 'l' (L) or an 'r' (R) to specify which direction to shunt the text.
	// NumSpacesToMove---is the number of spaces to move the text right or left.
	// startLoc---is the beginning location, in the Char array, of the block of text to move.
	// endLoc-----is the end location, in the Char array, of the block of text to move.

	// This function moves a block of text (that is inside a Char array) left or right. It will
	// overwrite the areas in the ORIGINAL Char array with the block of text that is moved there.
	// The array spaces left over, from where the block of text used to be, will be filled in
	// with blank spaces.

	if(toupper(direction) == 'L')
		for(int index = startLoc; index != endLoc; index++)
		{
			CharBuffer[index - NumSpacesToMove] = CharBuffer[index];	// shunts the character to the left
			CharBuffer[index] = ' ';									// Where the character originally was, it is replaced by a space (' ')
		}

	if(toupper(direction) == 'R')
		for(int index = endLoc; index != startLoc; index--)
		{
			CharBuffer[index + NumSpacesToMove] = CharBuffer[index];	// shunts the character to the right
			CharBuffer[index] =' ';									// Where the character originally was, it is replaced by a space (' ')
		}
}