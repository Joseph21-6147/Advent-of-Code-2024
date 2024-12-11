// AoC 2024 - Day 02 - Red-Nosed Reports
// =====================================

// date:  2024-12-02
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day02"

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

// the data consists of reports, which are (variable length) rows of levels (integer numbers)
typedef std::vector<int> DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = { 7, 6, 4, 2, 1 }; dData.push_back( cDatum );
    cDatum = { 1, 2, 7, 8, 9 }; dData.push_back( cDatum );
    cDatum = { 9, 7, 6, 2, 1 }; dData.push_back( cDatum );
    cDatum = { 1, 3, 2, 4, 5 }; dData.push_back( cDatum );
    cDatum = { 8, 6, 4, 4, 1 }; dData.push_back( cDatum );
    cDatum = { 1, 3, 6, 7, 9 }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        DatumType cDatum;
        while (sLine.length() > 0) {
            std::string sLevel = get_token_dlmtd( " ", sLine );
            if (sLevel.length() > 0) {
                cDatum.push_back( stoi( sLevel ) );
            }
        }
        if (!cDatum.empty()) {
            vData.push_back( cDatum );
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDatum( DatumType &iData ) {
    // define your datum printing code here

    for (auto e : iData) {
        std::cout << e << " ";
    }
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

// difference must be at least 1 and at most 3
bool IsGraduallyIncreasing( int a, int b ) { return (b > a) && (b <= a + 3); }
bool IsGraduallyDecreasing( int a, int b ) { return (b < a) && (b >= a - 3); }

// a report is safe if all elements are either decreasing, and pairwise gradually decreasing,
// or increasing and pairwise gradually increasing
bool IsSafe1( DatumType &dDatum ) {
    bool bAllIncreasing = true;
    bool bAllDecreasing = true;
    bool bValid = bAllIncreasing || bAllDecreasing;

    for (int i = 0; i < (int)dDatum.size() - 1 && bValid; i++) {
        if (i == 0) {
            bAllIncreasing = IsGraduallyIncreasing( dDatum[i], dDatum[i + 1] );
            bAllDecreasing = IsGraduallyDecreasing( dDatum[i], dDatum[i + 1] );
            bValid = bAllIncreasing || bAllDecreasing;
        } else {
            bool bTrendUp = bAllIncreasing;
            bAllIncreasing = IsGraduallyIncreasing( dDatum[i], dDatum[i + 1] );
            bAllDecreasing = IsGraduallyDecreasing( dDatum[i], dDatum[i + 1] );
            bool bTrendValid = (bAllIncreasing && bTrendUp) || (bAllDecreasing && !bTrendUp);
            bValid = bTrendValid && (bAllIncreasing || bAllDecreasing);
        }
    }
    return bValid;
}

// ----- PART 2

// if the report (line with levels) is not safe as is, try if it becomes safe by removing any level
bool IsSafe2( DatumType &dDatum ) {
    bool bSafe = IsSafe1( dDatum );

    // if not safe as is, attempt to make the report safe by removing any of its levels
    for (int j = 0; j < (int)dDatum.size() && !bSafe; j++) {

        DatumType vCorrected;
        for (int i = 0; i < (int)dDatum.size(); i++) {
            // copy dDatum, but remove level j
            if (i != j) {
                vCorrected.push_back( dDatum[i] );
            }
        }
        // if the corrected report is safe, the loop will terminate
        bSafe = IsSafe1( vCorrected );
    }
    return bSafe;
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

    int nTotalSafe1 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        if (IsSafe1( inputData[i] )) {
            nTotalSafe1 += 1;
//            std::cout << "Index: " << i << " is safe" << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 1: total safe = " << nTotalSafe1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    inputData = part2Data; // get fresh untouched copy of input data
    int nTotalSafe2 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        if (IsSafe2( inputData[i] )) {
            nTotalSafe2 += 1;
//            std::cout << "Index: " << i << " is safe" << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 2: total safe = " << nTotalSafe2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
