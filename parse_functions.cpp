#include <vector>

#include "parse_functions.h"

// ==============================/   PARSING LIB   /========================================

// this is the beginning of my parsing library :)

// Cuts of and returns the front token from "input_to_be_adapted", using "delim" as delimiter.
// If delimiter is not found, the complete input string is passed as a token.
// The input string becomes shorter as a result, and may even become empty
string get_token_dlmtd( const string &delim, string &input_to_be_adapted ) {
    size_t splitIndex = input_to_be_adapted.find( delim );
    string token = input_to_be_adapted.substr( 0, splitIndex );
    input_to_be_adapted = (splitIndex == string::npos) ? "" : input_to_be_adapted.substr( splitIndex + delim.length());

    return token;
}

// Cuts of and returns a token of size nSize from the front of input_to_be_adapted.
// This input string becomes shorter as a result of this cut off, and may even become empty.
string get_token_sized( const int nSize, string &input_to_be_adapted ) {
    string token;
    token = input_to_be_adapted.substr( 0, nSize );
    input_to_be_adapted = (nSize == (int)string::npos) ? "" : input_to_be_adapted.substr( nSize );
    return token;
}

// Like get_token_dlmtd(), but interprets the remainder as the second token.
// So it splits up the token "input" using the delimiter "delim", and returns the resulting parts via "out1" and "out2"
void split_token_dlmtd( const string &delim, const string &input, string &out1, string &out2 ) {
    size_t splitIndex = input.find( delim );
    out1 = input.substr( 0, splitIndex );
    out2 = (splitIndex == string::npos) ? "" : input.substr( splitIndex + 1 );
}

// Like get_token_sized(), but interprets the remainder as the second token
// so it splits up the input using the size nSize and returns two tokens via out1 and out2
void split_token_sized( const int nSize, const string &input, string &out1, string &out2 ) {
    out1 = input.substr( 0, nSize );
    out2 = (nSize == (int)string::npos) ? "" : input.substr( nSize );
}

// takes the parse list parseInfo as instruction to cut of the tokens from input, and shorten the
// input string as a result. The resulting tokens are passed back via output as separate strings
void split_token_generic( const ParseList &parseInfo, string &input, vector<string> &output ) {
    output.clear();
    for (int i = 0; i < (int)parseInfo.size(); i++) {
        sParseInfo curPI = parseInfo[i];
        if (curPI.bFixed) {
            if (curPI.nTokenSize <= 0) {
                    cout << "ERROR: split_token_generic() --> invalid token size = " << curPI.nTokenSize << " at index " << i << endl;
            } else {
                string tmp = get_token_sized( curPI.nTokenSize, input );
                output.push_back( tmp );
            }
        } else {
            if (curPI.sDelim == "") {
                    cout << "ERROR: split_token_generic() --> invalid delimiter = " << curPI.sDelim << " at index " << i << endl;
            } else {
                string tmp = get_token_dlmtd( curPI.sDelim, input );
                output.push_back( tmp );
            }
        }
    }
}

// this is an example how split_token_generic() could be used to build arbitrary splitting up parsing functions.
// In this case it's only 2, but this could easily be 3, 4, 5, ...

// Like get_sized_token(), but interprets the remainder as the second token
// so it splits up the input using the size nSize and returns two tokens via out1 and out2
void split_token_2( const int nSize, const string &input, string &out1, string &out2 ) {
    // prepare the call to split_token_generic()
    string inputCopy( input );
    ParseList parseInstructions = { { true, nSize, "" }, { false, -1, " " } };
    vector<string> results;
    // do the parsing
    split_token_generic( parseInstructions, inputCopy, results );
    // pass the resulting tokens
    out1 = results[0];
    out2 = results[1];
}

// Like get_sized_token(), but interprets the remainder as the second token
// so it splits up the input using the size nSize and returns two tokens via out1 and out2
void split_token_3( const int nSize1, const int nSize2, const string &input, string &out1, string &out2, string &out3 ) {
    // prepare the call to split_token_generic()
    string inputCopy( input );
    ParseList parseInstructions = { { true, nSize1, "" }, { true, nSize2, "" }, { false, -1, " " } };
    vector<string> results;
    // do the parsing
    split_token_generic( parseInstructions, inputCopy, results );
    // pass the resulting tokens
    out1 = results[0];
    out2 = results[1];
    out3 = results[2];
}



// ==============================/   End PARSING LIB   /========================================
