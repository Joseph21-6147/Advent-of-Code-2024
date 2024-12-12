// AoC 2024 - Day 05 - Print Queue
// ===============================

// date:  2024-12-05
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day05"

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

// the data consists of two types:
// 1. update ordering rules (uor's), which are pairs of numbers
// 2. updates, which are rows of numbers
typedef struct {
    int left, rght;
} UORType;
typedef std::vector<UORType> UORStream;

typedef std::vector<int> DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData, UORStream &uData ) {

    // hard code your examples here - use the same data for your test input file
    UORType uDatum;

    uDatum = { 47, 53 }; uData.push_back( uDatum );
    uDatum = { 97, 13 }; uData.push_back( uDatum );
    uDatum = { 97, 61 }; uData.push_back( uDatum );
    uDatum = { 97, 47 }; uData.push_back( uDatum );
    uDatum = { 75, 29 }; uData.push_back( uDatum );
    uDatum = { 61, 13 }; uData.push_back( uDatum );
    uDatum = { 75, 53 }; uData.push_back( uDatum );
    uDatum = { 29, 13 }; uData.push_back( uDatum );
    uDatum = { 97, 29 }; uData.push_back( uDatum );
    uDatum = { 53, 29 }; uData.push_back( uDatum );
    uDatum = { 61, 53 }; uData.push_back( uDatum );
    uDatum = { 97, 53 }; uData.push_back( uDatum );
    uDatum = { 61, 29 }; uData.push_back( uDatum );
    uDatum = { 47, 13 }; uData.push_back( uDatum );
    uDatum = { 75, 47 }; uData.push_back( uDatum );
    uDatum = { 97, 75 }; uData.push_back( uDatum );
    uDatum = { 47, 61 }; uData.push_back( uDatum );
    uDatum = { 75, 61 }; uData.push_back( uDatum );
    uDatum = { 47, 29 }; uData.push_back( uDatum );
    uDatum = { 75, 13 }; uData.push_back( uDatum );
    uDatum = { 53, 13 }; uData.push_back( uDatum );

    DatumType cDatum;

    cDatum = { 75,47,61,53,29 }; dData.push_back( cDatum );
    cDatum = { 97,61,53,29,13 }; dData.push_back( cDatum );
    cDatum = { 75,29,13       }; dData.push_back( cDatum );
    cDatum = { 75,97,47,61,53 }; dData.push_back( cDatum );
    cDatum = { 61,13,29       }; dData.push_back( cDatum );
    cDatum = { 97,13,75,29,47 }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData, UORStream &uData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    uData.clear();
    std::string sLine;
    bool bUORphase = true;

    while (getline( dataFileStream, sLine )) {
        if (sLine.length() == 0) {
            bUORphase = false;
        } else {
            if (bUORphase) {
                std::string sLeft, sRght;
                split_token_dlmtd( "|", sLine, sLeft, sRght );
                UORType uDatum = { stoi( sLeft ), stoi( sRght ) };
                uData.push_back( uDatum );
            } else {
                DatumType cDatum;
                while (sLine.length() > 0) {
                    std::string sNumber = get_token_dlmtd( ",", sLine );
                    cDatum.push_back( stoi( sNumber ));
                }
                vData.push_back( cDatum );
            }
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData, UORStream &uData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData, uData ); }
void GetData_PUZZLE( DataStream &dData, UORStream &uData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData, uData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintUOR( UORType &uor ) {
    std::cout << uor.left << " | " << uor.rght << std::endl;
}

void PrintUORStream( UORStream &uData ) {
    for (auto &e : uData) {
        PrintUOR( e );
    }
    std::cout << std::endl;
}

void PrintDatum( DatumType &iData ) {
    for (int i = 0; i < (int)iData.size(); i++) {
        std::cout << iData[i] << ", ";
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
void GetInput( DataStream &dData, UORStream &uData, bool bDisplay = false ) {

    switch( glbProgPhase ) {
        case EXAMPLE: GetData_EXAMPLE( dData, uData ); break;
        case TEST:    GetData_TEST(    dData, uData ); break;
        case PUZZLE:  GetData_PUZZLE(  dData, uData ); break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }
    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintUORStream( uData );
        PrintDataStream( dData );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

int GetMiddlePageNumber( DatumType &cData ) {
    int nNrPages = (int)cData.size();
    if (nNrPages % 2 == 0) {
        std::cout << "ERROR: even nr of pages!!" << std::endl;
    }
    return cData[ nNrPages / 2 ];
}

bool CheckOrder( UORStream &uData, int nLeftVal, int nRghtVal ) {
    for (int i = 0; i < (int)uData.size(); i++) {
        UORType curUOR = uData[i];
        if (curUOR.left == nRghtVal && curUOR.rght == nLeftVal) {
            return false;
        }
    }
    return true;
}

bool IsCorrectlyOrdered( UORStream &uData, DatumType &cData ) {
    bool bStillCorrect = true;

    for (int i = 0; i < (int)cData.size() - 1 && bStillCorrect; i++) {
        int nCurValue = cData[i];
        for (int j = i + 1; j < (int)cData.size() && bStillCorrect; j++) {
            int nOtherValue = cData[j];
            bStillCorrect = CheckOrder( uData, nCurValue, nOtherValue );
        }
    }
    return bStillCorrect;
}

// ----- PART 2

// for part 2: if ordering is not correct, the nLeftIx and nRghtIx contain the indices where the violating ordering was found
bool IsCorrectlyOrdered2( UORStream &uData, DatumType &cData, int &nLeftIx, int &nRghtIx ) {
    bool bStillCorrect = true;

    for (int i = 0; i < (int)cData.size() - 1 && bStillCorrect; i++) {
        int nCurValue = cData[i];
        for (int j = i + 1; j < (int)cData.size() && bStillCorrect; j++) {
            int nOtherValue = cData[j];
            bStillCorrect = CheckOrder( uData, nCurValue, nOtherValue );
            // report back violating indices if incorrect ordering is found
            if (!bStillCorrect) {
                nLeftIx = i;
                nRghtIx = j;
            }
        }
    }
    return bStillCorrect;
}

void SwapElements( DatumType &cData, int nIndex1, int nIndex2 ) {
    int nTmp = cData[nIndex1];
    cData[nIndex1] = cData[nIndex2];
    cData[nIndex2] = nTmp;
}

void CorrectOrdering( UORStream &uData, DatumType &cData ) {
    int n1, n2;
    while (!IsCorrectlyOrdered2( uData, cData, n1, n2 )) {
        SwapElements( cData, n1, n2 );
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
    UORStream uorData;
    GetInput( inputData, uorData, glbProgPhase != PUZZLE );
    DataStream part2Data = inputData;
    UORStream uor2Data = uorData;

    std::cout << "Data stats - size of uor stream  " << uorData.size() << std::endl << std::endl;
    std::cout << "Data stats - size of data stream " << inputData.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here

    int nSumMiddlePageNumbers1 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
//        std::cout << "Update " << i << ":" << std::endl;
//        PrintDatum( inputData[i] );
        if (IsCorrectlyOrdered( uorData, inputData[i] )) {
            int nMidPnr = GetMiddlePageNumber( inputData[i] );
//            std::cout << "is CORRECTLY ordered, middle page number is: " << nMidPnr << std::endl;
            nSumMiddlePageNumbers1 += nMidPnr;
        } else {
//            std::cout << "is NOT correctly ordered " << std::endl;
        }
//        std::cout << std::endl;
    }

    std::cout << std::endl << "Answer to part 1: sum of all middle page numbers of correct updates = " << nSumMiddlePageNumbers1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    uorData = uor2Data;
    inputData = part2Data;

    int nSumMiddlePageNumbers2 = 0;
    for (int i = 0; i < (int)inputData.size(); i++) {
//        std::cout << "Update " << i << ":" << std::endl;
//        PrintDatum( inputData[i] );
        if (IsCorrectlyOrdered( uorData, inputData[i] )) {
//            std::cout << "is CORRECTLY ordered" << std::endl;
        } else {
//            std::cout << "is NOT correctly ordered " << std::endl;
            CorrectOrdering( uorData, inputData[i] );
            int nMidPnr = GetMiddlePageNumber( inputData[i] );
//            std::cout << "mid page number after correction is: " << nMidPnr << std::endl;
            nSumMiddlePageNumbers2 += nMidPnr;
        }
//        std::cout << std::endl;
    }

    std::cout << std::endl << "Answer to part 2: sum of all middle page numbers of corrected updates = " << nSumMiddlePageNumbers2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
