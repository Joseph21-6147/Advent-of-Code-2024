// AoC 2024 - Day 06 - Guard Gallivant
// ===================================

// date:  2024-12-06
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day06"

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

// the data consists of a map, containing empty locations, obstructions and a guard
// it is modelled as a 2D field of characters
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = "....#....."; dData.push_back( cDatum );
    cDatum = ".........#"; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = "..#......."; dData.push_back( cDatum );
    cDatum = ".......#.."; dData.push_back( cDatum );
    cDatum = ".........."; dData.push_back( cDatum );
    cDatum = ".#..^....."; dData.push_back( cDatum );
    cDatum = "........#."; dData.push_back( cDatum );
    cDatum = "#........."; dData.push_back( cDatum );
    cDatum = "......#..."; dData.push_back( cDatum );
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

//#define EMPTY '.'
//#define OBSTR '#'

typedef struct sLocationStruct {
    int x, y;

    bool operator < (const sLocationStruct &rhs ) const {
        return ( this->x <  rhs.x) ||
               ((this->x == rhs.x) && (this->y < rhs.y));
    }
    bool operator == (const sLocationStruct &rhs ) const {
        return (this->x == rhs.x && this->y == rhs.y);
    }

} LocationType;

#define UP 0
#define RT 1
#define DN 2
#define LT 3

// guard
LocationType glbGuardLoc;
int glbGuardDir;

std::vector<LocationType> vDirOffsets = {
    {  0, -1 },   // up
    { +1,  0 },   // right
    {  0, +1 },   // down
    { -1,  0 },   // left
};

int glbMapSizeX;
int glbMapSizeY;

void InitGuard( DataStream &dData ) {
    glbMapSizeY = (int)dData.size();
    glbMapSizeX = (int)dData[0].length();
    for (int y = 0; y < glbMapSizeY; y++) {
        for (int x = 0; x < glbMapSizeX; x++) {
            switch (dData[y][x]) {
                case '.': break;
                case '#': break;
                case '^': glbGuardLoc = { x, y }; glbGuardDir = UP; break;
                case '>': glbGuardLoc = { x, y }; glbGuardDir = RT; break;
                case 'v': glbGuardLoc = { x, y }; glbGuardDir = DN; break;
                case '<': glbGuardLoc = { x, y }; glbGuardDir = LT; break;
                default: std::cout << "ERROR: InitGuard() --> unidentified cell content: " << dData[y][x] << std::endl;
            }
        }
    }
}

bool InBounds( int x, int y ) {
    return (x >= 0 && x < glbMapSizeX && y >= 0 && y < glbMapSizeY);
}
// overload for LocationType
bool InBounds( LocationType loc ) { return InBounds( loc.x, loc.y ); }

bool GuardObstructed( DataStream &dData ) {

    LocationType locFacingGuard = {
        glbGuardLoc.x + vDirOffsets[glbGuardDir].x,
        glbGuardLoc.y + vDirOffsets[glbGuardDir].y
    };
    if (InBounds( locFacingGuard )) {
        return dData[locFacingGuard.y][locFacingGuard.x] == '#';
    }
    return false;
}

void OneStep( DataStream &dData ) {

    auto guard_step = [=]() {
        glbGuardLoc.x += vDirOffsets[glbGuardDir].x;
        glbGuardLoc.y += vDirOffsets[glbGuardDir].y;
    };

    auto guard_rotate = [=]() {
        glbGuardDir = (glbGuardDir + 1) % 4;
    };

    if (GuardObstructed( dData )) {
        guard_rotate();
    } else {
        guard_step();
    }
}

int Walk1( DataStream &dData ) {
    std::set<LocationType> vVisited;
    while (InBounds( glbGuardLoc )) {
        vVisited.insert( glbGuardLoc );
        OneStep( dData );
    }
    return vVisited.size();
}

// ----- PART 2

typedef struct sPositionStruct {
    LocationType loc;
    int dir;

    bool operator < (const sPositionStruct &rhs ) const {
        return ( this->loc <  rhs.loc) ||
               ((this->loc == rhs.loc) && (this->dir < rhs.dir));
    }

} PosType;

// WARNING: although it works, this method is very time consuming!
// Better check on the sizes of the container before or after insertion, that makes the difference between a
// couple of hours or 12 seconds solution time...
bool InLoop( PosType curPos, std::set<PosType> prevVisited ) {
    auto search = prevVisited.find( curPos );
    return search != prevVisited.end();
}

bool Walk2( DataStream &dData, LocationType addedObstr ) {

    dData[addedObstr.y][addedObstr.x] = '#';
    // cache initial guard location and direction
    PosType cacheGuard = { glbGuardLoc, glbGuardDir };

    std::set<PosType> vVisited;
    bool bInBounds = InBounds( glbGuardLoc );
    bool bLooped = false;

    while (bInBounds && !bLooped) {
        int nSizeBefore = vVisited.size();
        vVisited.insert( { glbGuardLoc, glbGuardDir } );
        // if insertion had no effect, this position is already in the set, so we are looped
        bLooped = (nSizeBefore == (int)vVisited.size());

        OneStep( dData );
        bInBounds = InBounds( glbGuardLoc );
    }
    // reset map to remove added obstruction
    dData[addedObstr.y][addedObstr.x] = '.';
    // reset guard to initial location and direction
    glbGuardDir = cacheGuard.dir;
    glbGuardLoc = cacheGuard.loc;

    return bLooped;
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

    InitGuard( inputData );
    int nVisited = Walk1( inputData );

    std::cout << std::endl << "Answer to part 1: nr of distinct cells visited = " << nVisited << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here
    inputData = part2Data;
    InitGuard( inputData );

    int nNrObstructions = 0;
    for (int y = 0; y < glbMapSizeY; y++) {
        for (int x = 0; x < glbMapSizeX; x++) {
            // attempt next additional obstruction, but only on empty locations
            if (inputData[y][x] == '.') {
                if (Walk2( inputData, { x, y } )) {
                    nNrObstructions += 1;
                }
            }
        }
    }

    std::cout << std::endl << "Answer to part 2: number of added obstructions that leads to looped outcome = " << nNrObstructions << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
