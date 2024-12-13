// AoC 2024 - Day 07 - Bridge Repair
// =================================

// date:  2024-12-07
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day07"

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

// the data consists of tuples (pairs) of numbers - had to use long long instead of int, since the
// input data contains very large numbers, and stoi() gave problems
typedef struct {
    long long testValue;
    std::vector<long long> vNrs;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = {    190, { 10, 19 }         }; dData.push_back( cDatum );
    cDatum = {   3267, { 81, 40, 27 }     }; dData.push_back( cDatum );
    cDatum = {     83, { 17,  5 }         }; dData.push_back( cDatum );
    cDatum = {    156, { 15,  6 }         }; dData.push_back( cDatum );
    cDatum = {   7290, {  6,  8,  6, 15 } }; dData.push_back( cDatum );
    cDatum = { 161011, { 16, 10, 13 }     }; dData.push_back( cDatum );
    cDatum = {    192, { 17,  8, 14 }     }; dData.push_back( cDatum );
    cDatum = {  21037, {  9,  7, 18, 13 } }; dData.push_back( cDatum );
    cDatum = {    292, { 11,  6, 16, 20 } }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        std::string sTestVal = get_token_dlmtd( ": ", sLine );
        DatumType cDatum;
        cDatum.testValue = stoll( sTestVal );
        while (sLine.length() > 0) {
            std::string sOperand = get_token_dlmtd( " ", sLine );
            cDatum.vNrs.push_back( stoll( sOperand ));
        }
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

    std::cout << iData.testValue << ": ";
    for (int i = 0; i < (int)iData.vNrs.size(); i++) {
        std::cout << iData.vNrs[i] << ", ";
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

// creates a tree (on stack) of all possible combinations of operators and operands by recursive calls until all operands are processed
// the tree is pruned if the intermediate result already becomes larger than the test value
bool FitOperators( DatumType cDatum, int curIndex, long long curValue, std::vector<char> &vOperators ) {
    bool bResult = false;

    // if there are no more operands, the result is only true if the intermediate value equals the test value
    if (curIndex >= (int)cDatum.vNrs.size()) {
        bResult = (curValue == cDatum.testValue);
//        if (bResult) {
//            std::cout << "match found - operators are: ";
//            for (int i = 0; i < (int)vOperators.size(); i++) {
//                std::cout << vOperators[i] << " ";
//            }
//            std::cout << std::endl;
//        }

    // there can be no match if the intermediate value is larger than the test value while there are still operands
    } else if (curValue > cDatum.testValue) {
        bResult = false;
    } else {

        if (curIndex == 0) {
            bResult = FitOperators( cDatum, curIndex + 1, cDatum.vNrs[curIndex], vOperators );
        } else {
            std::vector<char> vNewOps1 = vOperators; vNewOps1.push_back( '+' );
            bool bResultPlus  = FitOperators( cDatum, curIndex + 1, curValue + cDatum.vNrs[curIndex], vNewOps1 );
            std::vector<char> vNewOps2 = vOperators; vNewOps2.push_back( '*' );
            bool bResultTimes = FitOperators( cDatum, curIndex + 1, curValue * cDatum.vNrs[curIndex], vNewOps2 );
            bResult = bResultPlus || bResultTimes;
        }
    }
    return bResult;
}

// ----- PART 2

// implements concationation operator: 12345 || 6789 becomes 123456789
// not sure how well it performs, but it wasn't blocking for today's puzzle
long long NrConcatenate( long long a, long long b ) {
    std::string sResult = to_string( a );
    sResult.append( to_string( b ));
    return stoll( sResult );
}

// variation on FitOperators() including the concat operator on top of plus and times
bool FitOperators2( DatumType cDatum, int curIndex, long long curValue, std::vector<char> &vOperators ) {
    bool bResult = false;

    // if there are no more operands, the result is only true if the intermediate value equals the test value
    if (curIndex >= (int)cDatum.vNrs.size()) {
        bResult = (curValue == cDatum.testValue);
//        if (bResult) {
//            std::cout << "match found - operators are: ";
//            for (int i = 0; i < (int)vOperators.size(); i++) {
//                std::cout << vOperators[i] << " ";
//            }
//            std::cout << std::endl;
//        }

    // there can be no match if the intermediate value is larger than the test value while there are still operands
    } else if (curValue > cDatum.testValue) {
        bResult = false;
    } else {

        if (curIndex == 0) {
            bResult = FitOperators2( cDatum, curIndex + 1, cDatum.vNrs[curIndex], vOperators );
        } else {
            // compiler is forced to work out all results first, and then combine them. I thought this would be handy for
            // part 2, but it turned out not to be of use...
            std::vector<char> vNewOps1 = vOperators; vNewOps1.push_back( '+' );
            bool bResultPlus  = FitOperators2( cDatum, curIndex + 1, curValue + cDatum.vNrs[curIndex], vNewOps1 );
            std::vector<char> vNewOps2 = vOperators; vNewOps2.push_back( '*' );
            bool bResultTimes = FitOperators2( cDatum, curIndex + 1, curValue * cDatum.vNrs[curIndex], vNewOps2 );
            std::vector<char> vNewOps3 = vOperators; vNewOps3.push_back( '|' );
            bool bResultConcat = FitOperators2( cDatum, curIndex + 1, NrConcatenate( curValue, cDatum.vNrs[curIndex] ), vNewOps3 );
            bResult = bResultPlus || bResultTimes || bResultConcat;
        }
    }
    return bResult;
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

    long long  nTotalCalibrationResult = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType curDatum = inputData[i];
        std::vector<char> vOps;
        if (FitOperators( curDatum, 0, 0, vOps )) {
//            std::cout << "datum index: " << i << " results in operator match!" << std::endl;
            nTotalCalibrationResult += curDatum.testValue;
        } else {
//            std::cout << "datum index: " << i << " does NOT result in operator match!" << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 1: total calibration result = " << nTotalCalibrationResult << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here
    inputData = part2Data;

    long long  nTotalCalibrationResult2 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
        DatumType curDatum = inputData[i];
        std::vector<char> vOps;
        if (FitOperators2( curDatum, 0, 0, vOps )) {
//            std::cout << "datum index: " << i << " results in operator match!" << std::endl;
            nTotalCalibrationResult2 += curDatum.testValue;
        } else {
//            std::cout << "datum index: " << i << " does NOT result in operator match!" << std::endl;
        }
    }

    std::cout << std::endl << "Answer to part 2: total calibration result = " << nTotalCalibrationResult2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
