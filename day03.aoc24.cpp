// AoC 2024 - Day 03 - Mull It Over
// ================================

// date:  2024-12-03
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day03"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>

#include "../flcTimer.h"
#include "../parse_functions.h"
#include "my_utility.h"

// ==========   PROGRAM PHASING

enum eProgPhase {     // what programming phase are you in - set at start of main()
    EXAMPLE = 0,      //    1. start with hard coded examples to test your implemented solution on small test sets
    TEST,             //    2. read the same small test set from file, to test on your file reading and parsing
    PUZZLE            //    3. put your algo to work on the full scale puzzle data
} glbProgPhase;

std::string ProgPhase2string() {
    switch (glbProgPhase) {
        case EXAMPLE: return "EXAMPLE";
        case TEST   : return "TEST"   ;
        case PUZZLE : return "PUZZLE" ;
    }
    return "unknown";
}

// ==========   INPUT DATA STRUCTURES          <<<<< ========== adapt to match columns of input file

// the data consists of sections of memory containing (possibly invalid) mul instructions
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
//    cDatum = "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))";  // example part 1
    cDatum = "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))";  // example part 2
    dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        vData.push_back( sLine );
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDatum( DatumType &iData ) {
    // define your datum printing code here

    std::cout << iData;
    std::cout << std::endl;
}

// output to console for testing
void PrintDataStream( DataStream &dData ) {
    for (auto &e : dData) {
        PrintDatum( e );
    }
    std::cout << std::endl;
}

// ==========   PROGRAM PHASING

// populates input data, by calling the appropriate input function that is associated
// with the global program phase var
void GetInput( DataStream &dData, bool bDisplay = false ) {

    switch( glbProgPhase ) {
        case EXAMPLE: GetData_EXAMPLE( dData ); break;
        case TEST:    GetData_TEST(    dData ); break;
        case PUZZLE:  GetData_PUZZLE(  dData ); break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }
    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintDataStream( dData );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

// generic keyword parsing function:
//     if the head of iDatum matches sKeyWord, then that part of iDatum is cut off and true is returned
//     else false is returned and iDatum is not changed
bool ParseKeyword( DatumType &iDatum, std::string sKeyWord ) {
    bool bSuccess = false;
    if (iDatum.length() >= sKeyWord.length()) {
        bool bMatch = true;
        for (int i = 0; i < (int)sKeyWord.length() && bMatch; i++) {
            bMatch = (iDatum[i] == sKeyWord[i]);
        }
        bSuccess = bMatch;
    }
    if (bSuccess) {
        std::string sRemove = get_token_sized( sKeyWord.length(), iDatum );
    }
    return bSuccess;
}

bool ParseKeywordMUL( DatumType &iDatum ) {
    return ParseKeyword( iDatum, "mul" );
}

bool ParseLeftPar( DatumType &iDatum ) {
    if (
        iDatum.length() >= 1 &&
        iDatum[0] == '('
    ) {
        std::string sRemove = get_token_sized( 1, iDatum );
        return true;
    }
    return false;
}

bool ParseRghtPar( DatumType &iDatum ) {
    if (
        iDatum.length() >= 1 &&
        iDatum[0] == ')'
    ) {
        std::string sRemove = get_token_sized( 1, iDatum );
        return true;
    }
    return false;
}

bool ParseComma( DatumType &iDatum ) {
    if (
        iDatum.length() >= 1 &&
        iDatum[0] == ','
    ) {
        std::string sRemove = get_token_sized( 1, iDatum );
        return true;
    }
    return false;
}

bool IsNumeric( char c ) {
    return (c >= '0' && c <= '9');
}

bool ParseArg( DatumType &iDatum, int &nArg ) {
    if (iDatum.length() >= 1) {
        bool bNumeric = IsNumeric( iDatum[0] );
        std::string sArg;
        int i = 0;
        while (bNumeric && i < (int)iDatum.length() && i < 3) {
            sArg.push_back( iDatum[i] );
            i += 1;
            bNumeric = IsNumeric( iDatum[i] );
        }
        if (sArg.length() > 0) {
            nArg = stoi( sArg );
            std::string sRemove = get_token_sized( sArg.length(), iDatum );
            return true;
        }
    }
    return false;
}

bool ParseMul( DatumType &iDatum, int &a, int &b ) {
    bool bSuccess = false;
    if (ParseKeywordMUL( iDatum )) {
        if (ParseLeftPar( iDatum )) {
            if (ParseArg( iDatum, a )) {
                if (ParseComma( iDatum )) {
                    if (ParseArg( iDatum, b )) {
                        if (ParseRghtPar( iDatum )) {
                            bSuccess = true;
//                            std::cout << "ParseMull() --> succes with args: " << a << " and " << b << std::endl;
                        }
                    }
                }
            }
        }
    }
//    std::cout << "ParseMull() --> remaining string: " << iDatum << std::endl;
    return bSuccess;
}

// ----- PART 2

// For part 2 I first created a generic keyword parser, and partly rebuild part 1 with it.

bool ParseDo(   DatumType &iDatum ) { return ParseKeyword( iDatum, "do()"    ); }
bool ParseDont( DatumType &iDatum ) { return ParseKeyword( iDatum, "don't()" ); }

// ==========   MAIN()

int main()
{
    glbProgPhase = PUZZLE;     // program phase to EXAMPLE, TEST or PUZZLE
    std::cout << "Phase: " << ProgPhase2string() << std::endl << std::endl;
    flcTimer tmr;

/* ========== */   tmr.StartTiming();   // ============================================vvvvv

    // get input data, depending on the glbProgPhase (example, test, puzzle)
    DataStream inputData;
    GetInput( inputData, glbProgPhase != PUZZLE );
    DataStream part2Data = inputData;
    std::cout << "Data stats - size of data stream " << inputData.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here

    int nCumulatedResult1 = 0;

    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType curDatum = inputData[i];
        while (curDatum.length() > 0) {
            int n1, n2;
            if (ParseMul( curDatum, n1, n2 )) {
                nCumulatedResult1 += n1 * n2;
            } else {
                std::string sRemove = get_token_sized( 1, curDatum );
            }
        }
    }

    std::cout << std::endl << "Answer to part 1: cumulated result = " << nCumulatedResult1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    int nCumulatedResult2 = 0;
    bool bEnabled = true;

    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType curDatum = inputData[i];
        while (curDatum.length() > 0) {
            int n1, n2;
            if (ParseMul( curDatum, n1, n2 )) {
                if (bEnabled) {
                    nCumulatedResult2 += n1 * n2;
                }
            } else if (ParseDo( curDatum )) {
                bEnabled = true;
            } else if (ParseDont( curDatum )) {
                bEnabled = false;
            } else {
                std::string sRemove = get_token_sized( 1, curDatum );
            }
        }
    }

    std::cout << std::endl << "Answer to part 2: total similarity score = " << nCumulatedResult2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
