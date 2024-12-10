#ifndef PARSE_FUNCTIONS_H
#define PARSE_FUNCTIONS_H

#include <iostream>

using namespace std;

struct sParseInfo {
    bool bFixed = false;    // signals fixed or variable size token
    int nTokenSize = -1;    // token size if fixed
    string sDelim = "";     // delimiter if variable
};
typedef vector<sParseInfo> ParseList;

// ==========/ token parsing /==========

// Cuts of and returns the front token from "input_to_be_adapted", using "delim" as delimiter.
// If delimiter is not found, the complete input string is passed as a token.
// The input string becomes shorter as a result, and may even become empty
string get_token_dlmtd( const string &delim, string &input_to_be_adapted );

// Like get_token_dlmtd(), but interprets the remainder as the second token.
// So it splits up the token "input" using the delimiter "delim", and returns the resulting parts via "out1" and "out2"
void split_token_dlmtd( const string &delim, const string &input, string &out1, string &out2 );

// Cuts of and returns a token of size nSize from the front of input_to_be_adapted.
// This input string becomes shorter as a result of this cut off.
string get_token_sized( const int nSize, string &input_to_be_adapted );

// Like get_token_sized(), but interprets the remainder as the second token
// so it splits up the input using the size nSize and returns two tokens via out1 and out2
void split_token_sized( const int nSize, const string &input, string &out1, string &out2 );

// takes the parse list parseInfo as instruction to cut of the tokens from input, and shorten the
// input string as a result. The resulting tokens are passed beck via output.
void split_token_generic( const ParseList &parseInfo, string &input, vector<string> &output );


// ==============================/   End PARSING LIB   /========================================


#endif // PARSE_FUNCTIONS_H
