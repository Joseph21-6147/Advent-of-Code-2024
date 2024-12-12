// AoC 2024 - Day 04 - Ceres Search
// ================================

// date:  2024-12-04
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day04"

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

// the data consists of rows of letters where to find XMAS or MAS in an X layout from
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = "MMMSXXMASM"; dData.push_back( cDatum );
    cDatum = "MSAMXMSMSA"; dData.push_back( cDatum );
    cDatum = "AMXSXMAAMM"; dData.push_back( cDatum );
    cDatum = "MSAMASMSMX"; dData.push_back( cDatum );
    cDatum = "XMASAMXAMM"; dData.push_back( cDatum );
    cDatum = "XXAMMXXAMA"; dData.push_back( cDatum );
    cDatum = "SMSMSASXSS"; dData.push_back( cDatum );
    cDatum = "SAXAMASAAA"; dData.push_back( cDatum );
    cDatum = "MAMMMXMMMM"; dData.push_back( cDatum );
    cDatum = "MXMXAXMASX"; dData.push_back( cDatum );
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

bool MatchChar( char a, char b ) { return a == b; }

// provides 8 coordinate offset patterns
std::vector<int> vOffsets1 = {
    0,  0, +1,  0, +2,  0, +3,  0,  // horizontal to right
    0,  0, -1,  0, -2,  0, -3,  0,  //               left
    0,  0,  0, +1,  0, +2,  0, +3,  // vertical down
    0,  0,  0, -1,  0, -2,  0, -3,  //          up
    0,  0, +1, -1, +2, -2, +3, -3,  // diagonal right up
    0,  0, +1, +1, +2, +2, +3, +3,  //                down
    0,  0, -1, -1, -2, -2, -3, -3,  // diagonal left up
    0,  0, -1, +1, -2, +2, -3, +3   //               down
};

// count the number of matches for "XMAS", where the "X" is at position (x, y) in the input data
int MatchPosition1( DataStream &inputData, int x, int y ) {
    int inputHeight = inputData.size();
    int inputWidth  = inputData[0].length();
    int nCountMatches = 0;

    auto in_bounds = [=]( int x, int y ) {
        return (x >= 0 && x < inputWidth && y >= 0 && y < inputHeight);
    };

    for (int dir = 0; dir < 8; dir ++) {
        bool bMatch = true;
        for (int i = 0; i < 4 && bMatch; i++) {
            int tmpx = vOffsets1[ 8 * dir + 2 * i + 0 ];
            int tmpy = vOffsets1[ 8 * dir + 2 * i + 1 ];

            char cCharToMatch = "XMAS"[i];

            bMatch = in_bounds( x + tmpx, y + tmpy ) &&
                     MatchChar( cCharToMatch, inputData[ (y + tmpy) ][ (x + tmpx) ]);
        }
        // if bMatch comes out of the loop being true, then all 4 characters must have matched
        if (bMatch) {
            nCountMatches += 1;
        }
    }
    return nCountMatches;
}

// iterate over all positions of the input data and cumulate matches
int FindAllMatches1( DataStream &inputData ) {
    int inputHeight = inputData.size();
    int inputWidth  = inputData[0].length();
    int nCountMatches = 0;

    for (int y = 0; y < inputHeight; y++) {
        for (int x = 0; x < inputWidth; x++) {
            nCountMatches += MatchPosition1( inputData, x, y );
        }
    }
    return nCountMatches;
}

// ----- PART 2

// provides 4 coordinate offset patterns
std::vector<int> vOffsets2 = {
    -1, +1,  0,  0, +1, -1,  // diagonal right up
    -1, -1,  0,  0, +1, +1,  //                down
    +1, +1,  0,  0, -1, -1,  // diagonal left up
    +1, -1,  0,  0, -1, +1,  //               down
};

// count the number of matches for "MAS", where the "A" is at position (x, y) in the input data
// if there are two matches, then it counts as one X-"MAS" match, so one is returned
int MatchPosition2( DataStream &inputData, int x, int y ) {
    int inputHeight = inputData.size();
    int inputWidth  = inputData[0].length();
    int nCountMatches = 0;

    auto in_bounds = [=]( int x, int y ) {
        return (x >= 0 && x < inputWidth && y >= 0 && y < inputHeight);
    };

    for (int dir = 0; dir < 4; dir ++) {
        bool bMatch = true;
        for (int i = 0; i < 3 && bMatch; i++) {
            int tmpx = vOffsets2[ 6 * dir + 2 * i + 0 ];
            int tmpy = vOffsets2[ 6 * dir + 2 * i + 1 ];

            char cCharToMatch = "MAS"[i];

            bMatch = in_bounds( x + tmpx, y + tmpy ) &&
                     MatchChar( cCharToMatch, inputData[ (y + tmpy) ][ (x + tmpx) ]);
        }
        if (bMatch) {
            nCountMatches += 1;
        }
    }
    // if there were 2 matches, they must have been in an X shape
    return (nCountMatches == 2) ? 1 : 0;
}

// iterate over all positions of the input data and cumulate matches
int FindAllMatches2( DataStream &inputData ) {
    int inputHeight = inputData.size();
    int inputWidth  = inputData[0].length();
    int nCountMatches = 0;

    for (int y = 0; y < inputHeight; y++) {
        for (int x = 0; x < inputWidth; x++) {
            nCountMatches += MatchPosition2( inputData, x, y );
        }
    }
    return nCountMatches;
}


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

    int nTotalMatches1 = FindAllMatches1( inputData );

    std::cout << std::endl << "Answer to part 1: total matches = " << nTotalMatches1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    int nTotalMatches2 = FindAllMatches2( inputData );

    std::cout << std::endl << "Answer to part 2: total matches = " << nTotalMatches2  << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
