// AoC 2024 - Day 08 - Resonant Collinearity
// =========================================

// date:  2024-12-08
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day08"

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

// the data consists of a map, modeled as a field of characters
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

//    cDatum = "............"; dData.push_back( cDatum );  // example part 1 and 2 - also in TEST file
//    cDatum = "........0..."; dData.push_back( cDatum );
//    cDatum = ".....0......"; dData.push_back( cDatum );
//    cDatum = ".......0...."; dData.push_back( cDatum );
//    cDatum = "....0......."; dData.push_back( cDatum );
//    cDatum = "......A....."; dData.push_back( cDatum );
//    cDatum = "............"; dData.push_back( cDatum );
//    cDatum = "............"; dData.push_back( cDatum );
//    cDatum = "........A..."; dData.push_back( cDatum );
//    cDatum = ".........A.."; dData.push_back( cDatum );
//    cDatum = "............"; dData.push_back( cDatum );
//    cDatum = "............"; dData.push_back( cDatum );


    cDatum = "T........."; dData.push_back( cDatum );  // example part 2
    cDatum = "...T......"; dData.push_back( cDatum );
    cDatum = ".T........"; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
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

int glbMapX, glbMapY;    // contains map sizes - call InitMap() to fill these

void InitMap( DataStream &dMap ) {
    glbMapY = (int)dMap.size();
    glbMapX = (int)dMap[0].length();
}

// location type - the operator < is there to apply them in a std::set
typedef struct sLocationStruct {
    int x, y;

    bool operator == (const sLocationStruct &rhs ) { return (this->x == rhs.x && this->y == rhs.y); }
    bool operator < (const sLocationStruct &rhs ) const {
        return ( this->x <  rhs.x) ||
               ((this->x == rhs.x) && (this->y < rhs.y));
    }
    std::string to_string() {
        return "(" + std::to_string( x ) + "," + std::to_string( y ) + ")";
    }
} LocType;

bool InBounds( int x, int y ) {
    return (x >= 0 && x < glbMapX && y >= 0 && y < glbMapY);
}
bool InBounds( LocType loc ) { return InBounds( loc.x, loc.y ); }   // overloaded version for LocType argument

// antenna type - contains a location and a type (modeled as a char)
typedef struct sAntennaStruct {
    LocType loc;
    char type;

    // the operator < is needed to apply std::sort on AntennaType
    bool operator < (const sAntennaStruct &rhs ) const {
        return ( this->type <  rhs.type) ||
               ((this->type == rhs.type) && (this->loc < rhs.loc));
    }

} AntennaType;

void PrintAntenna( AntennaType &curAntenna ) {
    std::cout << "type = " << curAntenna.type;
    std::cout << " location = " << curAntenna.loc.to_string() << std::endl;
}

void GetAntennaList( DataStream &dMap, std::vector<AntennaType> &vAntennas ) {
    vAntennas.clear();
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            if (dMap[y][x] != '.') {
                AntennaType newAntenna = { { x, y }, dMap[y][x] };
                vAntennas.push_back( newAntenna );
            }
        }
    }
    // sort the list - this is needed in the GetNodeSet() function
    std::sort( vAntennas.begin(), vAntennas.end() );
}

// ASSUMPTION: vAntenna's is sorted
void GetNodeSet( DataStream &dMap, std::vector<AntennaType> &vAntennas, std::set<LocType> &vNodes ) {
    vNodes.clear();
    int curIndex = 0;
    while (curIndex < (int)vAntennas.size()) {

        char curAntennaType = vAntennas[curIndex].type;
        int otherIndex = curIndex + 1;

        while (otherIndex < (int)vAntennas.size() && vAntennas[otherIndex].type == curAntennaType) {

            int diffX = vAntennas[curIndex].loc.x - vAntennas[otherIndex].loc.x;
            int diffY = vAntennas[curIndex].loc.y - vAntennas[otherIndex].loc.y;

            LocType node1 = { vAntennas[  curIndex].loc.x + diffX, vAntennas[  curIndex].loc.y + diffY };
            LocType node2 = { vAntennas[otherIndex].loc.x - diffX, vAntennas[otherIndex].loc.y - diffY };

            if (InBounds( node1 )) { vNodes.insert( node1 ); }
            if (InBounds( node2 )) { vNodes.insert( node2 ); }

            otherIndex += 1;
        }
        curIndex += 1;
    }
}


// ----- PART 2

// aux functions on locations - subtract, add and multiply with integer factor
LocType LocSub( LocType &a, LocType &b ) { LocType result = { a.x - b.x, a.y - b.y }; return result; }
LocType LocAdd( LocType &a, LocType &b ) { LocType result = { a.x + b.x, a.y + b.y }; return result; }
LocType LocMul( LocType &a, int n      ) { LocType result = { a.x * n  , a.y * n   }; return result; }

// ASSUMPTION: vAntenna's is sorted
void GetNodeSet2( DataStream &dMap, std::vector<AntennaType> &vAntennas, std::set<LocType> &vNodes ) {
    vNodes.clear();
    int curIndex = 0;
    while (curIndex < (int)vAntennas.size()) {

        char curAntennaType = vAntennas[curIndex].type;
        int otherIndex = curIndex + 1;

        while (otherIndex < (int)vAntennas.size() && vAntennas[otherIndex].type == curAntennaType) {

            LocType locDiff = {
                vAntennas[curIndex].loc.x - vAntennas[otherIndex].loc.x,
                vAntennas[curIndex].loc.y - vAntennas[otherIndex].loc.y
            };

            bool bOOB = false;
            for (int i = 0; !bOOB; i++) {
                LocType auxLoc = LocMul( locDiff, i );
                LocType node1 = LocAdd( vAntennas[  curIndex].loc, auxLoc );
                if (InBounds( node1 )) {
                    vNodes.insert( node1 );
                } else {
                    bOOB = true;
                }
            }
            bOOB = false;
            for (int i = 0; !bOOB; i++) {
                LocType auxLoc = LocMul( locDiff, i );
                LocType node2 = LocSub( vAntennas[otherIndex].loc, auxLoc );
                if (InBounds( node2 )) {
                    vNodes.insert( node2 );
                } else {
                    bOOB = true;
                }
            }
            otherIndex += 1;
        }
        curIndex += 1;
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

    std::vector<AntennaType> vAntennaList;
    std::set<LocType> vNodeSet;

    InitMap( inputData );
    GetAntennaList( inputData, vAntennaList );
    GetNodeSet( inputData, vAntennaList, vNodeSet );

    int nTotalNodes = vNodeSet.size();

    std::cout << std::endl << "Answer to part 1: nr of antinodes within map bounds = " << nTotalNodes << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    inputData = part2Data; // get fresh untouched copy of input data

    InitMap( inputData );
    GetAntennaList( inputData, vAntennaList );
    GetNodeSet2( inputData, vAntennaList, vNodeSet );

    int nTotalNodes2 = vNodeSet.size();

    std::cout << std::endl << "Answer to part 2: nr of antinodes within map bounds = " << nTotalNodes2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
