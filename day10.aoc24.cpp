// AoC 2024 - Day 10 - Hoof It
// ===========================

// date:  2024-12-10
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day10"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>

#include "../flcTimer.h"
#include "../parse_functions.h"
#include "my_utility.h"

// ==========   PROGRAM PHASING

enum eProgPhase {     // what programming phase are you in - set at start of main()
    EXAMPLE = 0,      //    1. start with hard coded examples to test your implemented solution on small test sets
    TEST,             //    2. read the same small test set from file, to test on your file reading and parsing
    PUZZLE            //    3. put your algorithm to work on the full scale puzzle data
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

// the data consists of a map, modeled as a 2d field of numerical characters
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = "89010123";  dData.push_back( cDatum );
    cDatum = "78121874";  dData.push_back( cDatum );
    cDatum = "87430965";  dData.push_back( cDatum );
    cDatum = "96549874";  dData.push_back( cDatum );
    cDatum = "45678903";  dData.push_back( cDatum );
    cDatum = "32019012";  dData.push_back( cDatum );
    cDatum = "01329801";  dData.push_back( cDatum );
    cDatum = "10456732";  dData.push_back( cDatum );
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

// convenience type - locations on the map. The operators are for applying this type in
// combination with std::set etc
typedef struct sLocationStruct {
    int x, y;

    bool operator == (const sLocationStruct &rhs ) { return (this->x == rhs.x && this->y == rhs.y); }
    bool operator < (const sLocationStruct &rhs ) const {
        return ( this->x <  rhs.x) ||
               ((this->x == rhs.x) && (this->y < rhs.y));
    }
} LocType;
typedef std::set<LocType> LocSet;

// is denoted location in bounds of the map?
bool InBounds( DataStream &mapData, int x, int y ) {
    return (x >= 0 && x < (int)mapData[0].length() && y >= 0 && y < (int)mapData.size());
}
bool InBounds( DataStream &mapData, LocType &loc ) {
    return InBounds( mapData, loc.x, loc.y );
}

// does denoted map location match char c?
bool MatchPosition( DataStream &mapData, int x, int y, char c ) {
    return InBounds( mapData, x, y ) && mapData[y][x] == c;
}

// works out all hiking trail end points starting from location (x, y) in the map
// the end points are returned in the std::set lSet
void AnalyseTrail( DataStream &mapData, int x, int y, char startChar, LocSet &lSet ) {
    if (MatchPosition( mapData, x, y, startChar )) {
        if (startChar == '9') {
            // recursion end condition
            lSet.insert( { x, y } );
        } else {
            // attempt all next directions using recursion
            AnalyseTrail( mapData, x    , y - 1, startChar + 1, lSet );
            AnalyseTrail( mapData, x    , y + 1, startChar + 1, lSet );
            AnalyseTrail( mapData, x - 1, y    , startChar + 1, lSet );
            AnalyseTrail( mapData, x + 1, y    , startChar + 1, lSet );
        }
    }
}

// Trail head score: gather all possible trail endpoints in lSet, and returns its size
int AnalysePosition( DataStream &mapData, int x, int y ) {
    LocSet lSet;
    AnalyseTrail( mapData, x, y, '0', lSet );
    return lSet.size();
}

// ----- PART 2

// for part 2 we need to gather unique trails instead of unique end points, so I need
// a std::set of trails
typedef std::vector<LocType> TrailType;
typedef std::set<TrailType> TrailSet;

// returns the number of hiking trails starting from location (x, y) in the map
void AnalyseTrail2( DataStream &mapData, int x, int y, char startChar, TrailType &curTrail, TrailSet &tSet ) {
    // a hiking trail starts with 0, ends with 9 and has an even, gradual uphill slope
    // so it must have values 0 1 2 ... 9
    if (MatchPosition( mapData, x, y, startChar )) {
        curTrail.push_back( { x, y } );
        if (startChar == '9') {
            tSet.insert( curTrail );
        } else {
            // make local copies of current trail
            TrailType upTrail = curTrail;
            TrailType dnTrail = curTrail;
            TrailType ltTrail = curTrail;
            TrailType rtTrail = curTrail;
            // recursive calls in all cardinal directions
            AnalyseTrail2( mapData, x    , y - 1, startChar + 1, upTrail, tSet );
            AnalyseTrail2( mapData, x    , y + 1, startChar + 1, dnTrail, tSet );
            AnalyseTrail2( mapData, x - 1, y    , startChar + 1, ltTrail, tSet );
            AnalyseTrail2( mapData, x + 1, y    , startChar + 1, rtTrail, tSet );
        }
    }
}

// Trail head rating: gather the set of unique trails in tSet, and return the number
int AnalysePosition2( DataStream &mapData, int x, int y ) {
    TrailType trl;
    TrailSet tSet;
    AnalyseTrail2( mapData, x, y, '0', trl, tSet );
    return tSet.size();
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

    int nTrailHeadScoreSum = 0;
    for (int y = 0; y < (int)inputData.size(); y++) {
        for (int x = 0; x < (int)inputData[0].length(); x++) {
            int nRes = AnalysePosition( inputData, x, y );
//            if (nRes > 0) {
//                std::cout << "Map position (" << x << ", " << y << ") has a score of " << nRes << std::endl;
//            }
            nTrailHeadScoreSum += nRes;
        }
    }

    std::cout << std::endl << "Answer to part 1: Sum of all trail head scores = " << nTrailHeadScoreSum << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here

    int nTrailHeadRatingSum = 0;
    for (int y = 0; y < (int)inputData.size(); y++) {
        for (int x = 0; x < (int)inputData[0].length(); x++) {
            int nRes = AnalysePosition2( inputData, x, y );
//            if (nRes > 0) {
//                std::cout << "Map position (" << x << ", " << y << ") has a rating of " << nRes << std::endl;
//            }
            nTrailHeadRatingSum += nRes;
        }
    }

    std::cout << std::endl << "Answer to part 2: Sum of all trail head ratings = " << nTrailHeadRatingSum << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
