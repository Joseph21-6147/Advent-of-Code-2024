// AoC 2024 - Day 09 - Disk Fragmenter
// ===================================

// date:  2024-12-09
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day09"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <list>

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

// the input data consists of one (huge) string, so the datum type should be char
typedef char DatumType;
typedef std::string DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    dData.clear();

    dData.append( "12345" );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        vData.append( sLine );
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDataStream( DataStream &dData ) {
    std::cout << dData;
    std::cout << std::endl << std::endl;
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

// disk blocks have an ID, an address and a length
// empty blocks are modeled as nId == -1
typedef struct sBlockStruct {
    int nId;
    int nAddress;
    int nLength;
} BlockType;
typedef std::list<BlockType> BlockStream;
typedef BlockStream::iterator BlockIter;   // since I'm working with std::list its handy to have the iterator type predefined

// Print functions type 1 - provide the raw data as stored in the block list
void PrintBlock1( BlockType &blk ) {
    std::cout << "ID = "        << blk.nId
              << ", address = " << blk.nAddress
              << ", size = "    << blk.nLength << std::endl;
}

void PrintBlockStream1( BlockStream &vBlocks ) {
    for (auto &e : vBlocks ) {
        PrintBlock1( e );
    }
    std::cout << std::endl;
}

// print functions type 2 provide visual output to be used with small examples
void PrintBlock2( BlockType &blk ) {
    for (int i = 0; i < blk.nLength; i++) {
        if (blk.nId == -1) {
            std::cout << ".";
        } else {
            std::cout << blk.nId;
        }
    }
}

int FreeSpace( BlockType &a, BlockType &b ) {
    return b.nAddress - a.nAddress - a.nLength;
}

void PrintBlockStream2( BlockStream &vBlocks, BlockIter curIter, BlockIter lstIter ) {
    std::string sCurIterString, sLstIterString;
    for (BlockIter iter = vBlocks.begin(); iter != vBlocks.end(); iter++) {

        PrintBlock2( *iter );
        if (iter == curIter) {
            sCurIterString.append( ">" );
        } else {
            for (int j = 0; j < (*iter).nLength; j++) {
                sCurIterString.append( " " );
            }
        }
        if (iter == lstIter) {
            sLstIterString.append( "<" );
        } else {
            for (int j = 0; j < (*iter).nLength; j++) {
                sLstIterString.append( " " );
            }
        }
    }
    std::cout << std::endl;
    std::cout << sCurIterString << std::endl;
    std::cout << sLstIterString << std::endl;
}

// no error checking
int ctoi( char c ) {
    return int( c - '0' );
}

// processes the input data into a stream (std::list) of BlockType elements
void ProcessInput( DataStream &dData, BlockStream &vBlocks ) {

    vBlocks.clear();

    int nGlbAddress = 0;
    int nGlbID = 0;
    int nCurIndex = 0;
    while (nCurIndex < (int)dData.size()) {
        int nLength = ctoi( dData[nCurIndex] );
        BlockType curBlock = { nGlbID, nGlbAddress, nLength };
        vBlocks.push_back( curBlock );
        nCurIndex += 1;
        nGlbAddress += nLength;

        if (nCurIndex< (int)dData.size()) {
            BlockType emptyBlock = { -1, nGlbAddress, ctoi( dData[nCurIndex] ) };
            vBlocks.push_back( emptyBlock );
            nGlbAddress += ctoi( dData[nCurIndex] );  // empty spaces
        }
        nGlbID += 1;
        nCurIndex += 1;
    }

    // append empty block of size 0 if the last block wasn't empty already
    // this is convenient for the Degragment() function later on
    if (vBlocks.back().nId != -1) {
        vBlocks.push_back( { -1, nGlbAddress, 0 } );
    }
}

void Defragment1( BlockStream &vOld ) {

    BlockIter iCur = vOld.begin();
    BlockIter iLst = vOld.end();
    BlockIter iEmp = vOld.end(); iEmp--;  // points to trailing empty block
    do {
        iLst--;
    } while ((*iLst).nId == -1);

    while ((*iCur).nAddress < (*iLst).nAddress) {

        if ((*iCur).nId == -1) {
            int nSpace  = (*iCur).nLength;  // available space
            int nNeeded = (*iLst).nLength;

            // three possible situations
            if (nSpace > nNeeded) {
                // adapt empty spaces, insert a new block with contents of last block and erase last block
                (*iCur).nLength -= nNeeded;
                (*iEmp).nLength += nNeeded;
                vOld.insert( iCur, (*iLst ));
                BlockIter toRemove = iLst;
                do {
                    iLst--;
                } while ((*iLst).nId == -1);
                vOld.erase( toRemove );

            } else if (nSpace < nNeeded) {
                // reduce space needed of last block with amount of free space available
                (*iEmp).nLength += nSpace;
                (*iLst).nLength -= nSpace;
                (*iCur).nId = (*iLst).nId;

            } else {   // nSpace == nNeeded
                // fill empty block with contents of last block, and erase last block
                (*iEmp).nLength += nNeeded;
                *iCur = *iLst;
                BlockIter toRemove = iLst;
                do {
                    iLst--;
                } while ((*iLst).nId == -1);
                vOld.erase( toRemove );
            }
        }
        // set iCur to next empty space block
        while (iCur != vOld.end() && iCur != iLst && (*iCur).nId != -1) {
            iCur++;
        }
    }
}

long long FileCheckSum1( BlockType &b, int nStartBlock ) {
    long long result = 0;
    if (b.nId != -1) {
        for (int i = 0; i < b.nLength; i++) {
            result += (nStartBlock + i) * b.nId;
        }
    }
    return result;
}

long long SystemCheckSum1( BlockStream &bs ) {
    long long result = 0;
    int nBlockCount = 0;
    for (auto &e : bs) {
        result += FileCheckSum1( e, nBlockCount );
        nBlockCount += e.nLength;
    }
    return result;

}

// ----- PART 2

void Defragment2( BlockStream &vOld ) {

    BlockIter iCur = vOld.begin();
    BlockIter iEmp = vOld.end(); iEmp--;  // points to trailing empty block
    BlockIter iLst = iEmp;

    // search next empty block from front of list
    // be careful not to select the trailing empty block
    auto find_next_empty = [=] ( BlockIter cur ) {
        if (cur != vOld.end() && cur != iEmp) {
            cur++;
        }
        while (cur != vOld.end() && cur != iEmp && (*cur).nId != -1) {
            cur++;
        }
        return cur;
    };
    // search first previous non empty block with another ID than the current one
    auto find_prev_file = [=] ( BlockIter lst ) {
        int nCacheID;
        if (lst != vOld.begin()) {
            nCacheID = (*lst).nId;
            lst--;
        }
        while (lst != vOld.begin() && ((*lst).nId == nCacheID || (*lst).nId == -1)) {
            lst--;
        }
        return lst;
    };

    // set iterators to initial positions
    iCur = find_next_empty( iCur );
    iLst = find_prev_file( iLst );

    bool bDone = false;
    while (!bDone) {

        // find a location that fits the needed number length
        int nNeeded = (*iLst).nLength;
        iCur = find_next_empty( vOld.begin());   // reset current iterator
        while (
            iCur != vOld.end() &&
            iCur != iEmp       &&
            (*iCur).nAddress < (*iLst).nAddress &&
            !((*iCur).nLength >= nNeeded && (*iCur).nId == -1)
        ) {
            iCur = find_next_empty( iCur );
        }

        bool bFound = (iCur != vOld.end() && iCur != iEmp && (*iCur).nAddress < (*iLst).nAddress);
        if (bFound) {

            // result of above loop is that available space >= needed space ...
            int nSpace = (*iCur).nLength;  // available space
            // ... so there's two possible situations
            if (nSpace > nNeeded) {

                // adapt empty spaces, insert a new block with contents of last block and erase last block
                (*iEmp).nLength += nNeeded;

                BlockType newBlock = { (*iLst).nId, (*iCur).nAddress, (*iLst).nLength };
                vOld.insert( iCur, newBlock );
                (*iCur).nLength  -= nNeeded;   // fix address and length of current block
                (*iCur).nAddress += nNeeded;

                BlockIter toRemove = iLst;
                iLst = find_prev_file( iLst );
                vOld.erase( toRemove );

            } else {   // nSpace == nNeeded

                // fill empty block with contents of last block, and erase last block
                (*iEmp).nLength += nNeeded;
                (*iCur).nId = (*iLst).nId;

                BlockIter toRemove = iLst;
                iLst = find_prev_file( iLst );
                vOld.erase( toRemove );
            }

            iCur = find_next_empty( iCur );
        } else {
            iCur = find_next_empty( vOld.begin() ); // reset iCur iterator
            iLst = find_prev_file( iLst );
        }
        bDone = iLst == vOld.begin();
    }
}


long long FileCheckSum2( BlockType &b ) {
    long long result = 0;
    if (b.nId != -1) {
        for (int i = 0; i < b.nLength; i++) {
            result += (b.nAddress + i) * b.nId;
        }
    }
    return result;
}

long long SystemCheckSum2( BlockStream &bs ) {
    long long result = 0;
    for (auto &e : bs) {
        result += FileCheckSum2( e );
    }
    return result;

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
    BlockStream vBlockData;
    ProcessInput( inputData, vBlockData );
    Defragment1( vBlockData );

    long long llCheckSum1 = SystemCheckSum1( vBlockData );

    std::cout << std::endl << "Answer to part 1: system check sum = " << llCheckSum1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here
    vBlockData.clear();
    ProcessInput( inputData, vBlockData );
    Defragment2( vBlockData );

    long long llCheckSum2 = SystemCheckSum2( vBlockData );


    std::cout << std::endl << "Answer to part 2: system check sum = " << llCheckSum2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
