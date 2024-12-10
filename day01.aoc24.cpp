// AoC 2024 - Day 01 - Historian Hysteria
// ======================================

// date:  2024-12-01
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day01"

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

// the data consists of tuples (pairs) of numbers
typedef struct {
    int left, rght;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { 3, 4 }; dData.push_back( cDatum );
    cDatum = { 4, 3 }; dData.push_back( cDatum );
    cDatum = { 2, 5 }; dData.push_back( cDatum );
    cDatum = { 1, 3 }; dData.push_back( cDatum );
    cDatum = { 3, 9 }; dData.push_back( cDatum );
    cDatum = { 3, 3 }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        std::string sLeft, sRght;
        split_token_dlmtd( "   ", sLine, sLeft, sRght );
        DatumType cDatum = { stoi( sLeft ), stoi( sRght ) };
        vData.push_back( cDatum );
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDatum( DatumType &iData ) {
    // define your datum printing code here

    std::cout << "left: " << iData.left << " ";
    std::cout << "rght: " << iData.rght;
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

void SortInputData( DataStream &dData ) {
    std::vector<int> vLeft, vRght;
    for (auto &e : dData) {
        vLeft.push_back( e.left );
        vRght.push_back( e.rght );
    }
    std::sort( vLeft.begin(), vLeft.end() );
    std::sort( vRght.begin(), vRght.end() );

    for (int i = 0; i < (int)vLeft.size(); i++) {
        dData[i].left = vLeft[i];
        dData[i].rght = vRght[i];
    }
}

int CumulatedDistance( DataStream &dData ) {
    int nResult = 0;
    for (auto e : dData) {
        nResult += abs( e.left - e.rght );
    }
    return nResult;
}

// ----- PART 2

int GetSimilarityScore( DataStream &dData ) {
    std::vector<int> vLeft, vRght;
    for (auto &e : dData) {
        vLeft.push_back( e.left );
        vRght.push_back( e.rght );
    }

    int nResult = 0;
    for (int i = 0; i < (int)vLeft.size(); i++) {
        int nCurVal = vLeft[i];
        int nFactor = std::count( vRght.begin(), vRght.end(), nCurVal );
        nResult += nCurVal * nFactor;
    }
    return nResult;
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

    SortInputData( inputData );
    int nCumulatedDistances = CumulatedDistance( inputData );

    std::cout << std::endl << "Answer to part 1: cumulated distances = " << nCumulatedDistances << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    inputData = part2Data; // get fresh untouched copy of input data
    int nSimScore = GetSimilarityScore( inputData );

    std::cout << std::endl << "Answer to part 2: total similarity score = " << nSimScore << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
