// AoC 2024 - Day 13 - Claw Contraption
// ====================================

// date:  2024-12-13
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day13"

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

// the data consists of sets of 6 numbers
typedef struct {
    long long ax, ay;  // offsets for button A
    long long bx, by;  //                    B
    long long px, py;  // location of prize
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = { 94, 34, 22, 67,  8400,  5400 }; dData.push_back( cDatum );
    cDatum = { 26, 66, 67, 21, 12748, 12176 }; dData.push_back( cDatum );
    cDatum = { 17, 86, 84, 37,  7870,  6450 }; dData.push_back( cDatum );
    cDatum = { 69, 23, 27, 71, 18641, 10279 }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        std::string sIgnore, sAX, sAY, sBX, sBY, sPX, sPY;

        sIgnore = get_token_dlmtd( "Button A: X", sLine );   // button A line
        sAX     = get_token_dlmtd( ", Y", sLine );
        sAY     = sLine;

        getline( dataFileStream, sLine );
        sIgnore = get_token_dlmtd( "Button B: X", sLine );   // button B line
        sBX     = get_token_dlmtd( ", Y+", sLine );
        sBY     = sLine;

        getline( dataFileStream, sLine );
        sIgnore = get_token_dlmtd( "Prize: X=", sLine );     // prize line
        sPX     = get_token_dlmtd( ", Y=", sLine );
        sPY     = sLine;

        getline( dataFileStream, sLine );                    // empty line

        DatumType cDatum = { stoll( sAX ), stoll( sAY ), stoll( sBX ), stoll( sBY ), stoll( sPX ), stoll( sPY ) };
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
    std::cout << "Button A: X = "    << iData.ax << ", Y = " << iData.ay;
    std::cout << " - button B: X = " << iData.bx << ", Y = " << iData.by;
    std::cout << " - prize at: X = " << iData.px << ", Y = " << iData.py;
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

// applied Cramers rule, see: https://en.wikipedia.org/wiki/Cramer%27s_rule

long long FindA( DatumType &d ) { return (d.px * d.by - d.py * d.bx) / (d.ax * d.by - d.ay * d.bx); }
long long FindB( DatumType &d ) { return (d.ax * d.py - d.ay * d.px) / (d.ax * d.by - d.ay * d.bx); }

// ----- PART 2

// add 10^13 to each prize x and y value
void PreProcessInputData( DataStream &ds ) {
    for (int i = 0; i < (int)ds.size(); i++) {
        DatumType &d = ds[i];
        d.px += 10000000000000;
        d.py += 10000000000000;
    }
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

    int nFewestTokens = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {

        int A = FindA( inputData[i] );
        int B = FindB( inputData[i] );
        bool bSolutionFound = (
            A * inputData[i].ax + B * inputData[i].bx == inputData[i].px &&
            A * inputData[i].ay + B * inputData[i].by == inputData[i].py
        );
        if (bSolutionFound) {
            int cost = A * 3 + B * 1;
            nFewestTokens += cost;
//            std::cout << "Solution for data set " << i + 1 << " where (A, B) = (" << A << ", " << B << "), cost = " << cost << ", fewest tokens = " << nFewestTokens << std::endl;
        }
    }
    std::cout << std::endl << "Answer to part 1: fewest tokens to spend to win all prizes = " << nFewestTokens << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here
    PreProcessInputData( inputData );

    long long nFewestTokens2 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {

        long long A = FindA( inputData[i] );
        long long B = FindB( inputData[i] );
        bool bSolutionFound = (
            A * inputData[i].ax + B * inputData[i].bx == inputData[i].px &&
            A * inputData[i].ay + B * inputData[i].by == inputData[i].py
        );
        if (bSolutionFound) {
            long long cost = A * 3 + B * 1;
            nFewestTokens2 += cost;
//            std::cout << "Solution for data set " << i + 1 << " where (A, B) = (" << A << ", " << B << "), cost = " << cost << ", fewest tokens = " << nFewestTokens2 << std::endl;
        }
    }
    std::cout << std::endl << "Answer to part 2: fewest tokens to spend to win all prizes = " << nFewestTokens2<< std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
