// AoC 2024 - Day 14 - Restroom Redoubt
// ====================================

// date:  2024-12-14
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day14"

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

// generic type for storing 2D locations
typedef struct sLocStruct {
    int x, y;

    // convenience function
    std::string to_string() {
        std::string sResult = "(" + std::to_string( x ) + ", " + std::to_string( y ) + ")";
        return sResult;
    }
    // needed for std::count()
    bool operator == (const sLocStruct &rhs ) {
        return (this->x == rhs.x && this->y == rhs.y);
    }

    // needed to apply in std::set<>
    bool operator < (const sLocStruct &rhs ) const {
        return (
             this->x <  rhs.x ||
            (this->x == rhs.x && this->y < rhs.y)
        );
    }
    bool WithinBounds( const sLocStruct &low, const sLocStruct &hgh ) {
        return (this->x >= low.x && this->x < hgh.x && this->y >= low.y && this->y < hgh.y);
    }

} LocType;

//bool WithinBounds( LocType &cur, LocType &low, LocType &hgh ) {
//    return (cur.x >= low.x && cur.x < hgh.x && cur.y >= low.y && cur.y < hgh.y);
//}


// the data consists of robots having a position and a velocity
typedef struct {
    LocType pos;
    LocType vel;
} DatumType;
typedef std::vector<DatumType> DataStream;

int glbMapX, glbMapY;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = { 0, 4,  3, -3 }; dData.push_back( cDatum );
    cDatum = { 6, 3, -1, -3 }; dData.push_back( cDatum );
    cDatum = {10, 3, -1,  2 }; dData.push_back( cDatum );
    cDatum = { 2, 0,  2, -1 }; dData.push_back( cDatum );
    cDatum = { 0, 0,  1,  3 }; dData.push_back( cDatum );
    cDatum = { 3, 0, -2, -2 }; dData.push_back( cDatum );
    cDatum = { 7, 6, -1, -3 }; dData.push_back( cDatum );
    cDatum = { 3, 0, -1, -2 }; dData.push_back( cDatum );
    cDatum = { 9, 3,  2,  3 }; dData.push_back( cDatum );
    cDatum = { 7, 3, -1,  2 }; dData.push_back( cDatum );
    cDatum = { 2, 4,  2, -3 }; dData.push_back( cDatum );
    cDatum = { 9, 5, -3, -3 }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        std::string sIgnore, sPX, sPY, sVX, sVY;
        sIgnore = get_token_dlmtd( "p=" , sLine );
        sPX     = get_token_dlmtd( ","  , sLine );
        sPY     = get_token_dlmtd( " v=", sLine );
        sVX     = get_token_dlmtd( ","  , sLine );
        sVY     =                         sLine  ;

        DatumType cDatum = { stoi( sPX ), stoi( sPY ), stoi( sVX ), stoi( sVY ) };
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
    std::cout <<   "position: " << iData.pos.to_string();
    std::cout << ", velocity: " << iData.vel.to_string();
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
        case EXAMPLE: GetData_EXAMPLE( dData ); glbMapX =  11; glbMapY =   7; break;
        case TEST:    GetData_TEST(    dData ); glbMapX =  11; glbMapY =   7; break;
        case PUZZLE:  GetData_PUZZLE(  dData ); glbMapX = 101; glbMapY = 103; break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }
    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintDataStream( dData );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

LocType NextPosition( DatumType &robot ) {
    LocType result = {
        robot.pos.x + robot.vel.x,
        robot.pos.y + robot.vel.y
    };
    // wrap around map boundaries
    if (result.x <        0) { result.x += glbMapX; }
    if (result.y <        0) { result.y += glbMapY; }
    if (result.x >= glbMapX) { result.x -= glbMapX; }
    if (result.y >= glbMapY) { result.y -= glbMapY; }

    return result;
}

void SimulateOneSecond( DataStream &robData ) {
    DataStream newData;
    for (int i = 0; i < (int)robData.size(); i++) {
        DatumType newRobot = { NextPosition( robData[i] ), robData[i].vel };
        newData.push_back( newRobot );
    }
    robData.clear();
    robData = newData;
}

// quadrant numbering - this is the same as the puzzle description
//    1 = up lt
//    2 = up rt
//    3 = dn lt
//    4 = dn rt
int CountQuadrant( DataStream &robData, int nQuadrant ) {
    // determine quadrant boundaries
    int nLowX, nLowY, nHghX, nHghY;
    switch (nQuadrant) {
        case  0: nLowX =               0; nLowY =               0; nHghX = glbMapX / 2; nHghY = glbMapY / 2; break;
        case  1: nLowX = glbMapX / 2 + 1; nLowY =               0; nHghX = glbMapX;     nHghY = glbMapY / 2; break;
        case  2: nLowX =               0; nLowY = glbMapY / 2 + 1; nHghX = glbMapX / 2; nHghY = glbMapY;     break;
        case  3: nLowX = glbMapX / 2 + 1; nLowY = glbMapY / 2 + 1; nHghX = glbMapX;     nHghX = glbMapY;     break;
        default: std::cout << "ERROR: CountQuadrant() --> unknown quadrant nr: " << nQuadrant << std::endl;
    }
    LocType lLow = { nLowX, nLowY };
    LocType lHgh = { nHghX, nHghY };
    int nResult = 0;
    for (int i = 0; i < (int)robData.size(); i++) {
        if (robData[i].pos.WithinBounds( lLow, lHgh )) {
            nResult += 1;
        }
    }
    return nResult;
}

// ----- PART 2

// create an on screen displayable map (modeled as a character field) and put it on screen
void DisplayMap( DataStream &robData ) {

    // create and dummy populate character field
    std::vector<std::vector<char>> dispMap;
    for (int y = 0; y < glbMapY; y++) {
        std::vector<char> curRow;
        for (int x = 0; x < glbMapX; x++) {
            curRow.push_back( '.' );
        }
        dispMap.push_back( curRow );
    }
    // overwrite locations in the map where robots are
    for (int i = 0; i < (int)robData.size(); i++) {
        DatumType &curRob = robData[i];
        LocType &curLoc = curRob.pos;
        switch (dispMap[curLoc.y][curLoc.x]) {
            case '.': dispMap[curLoc.y][curLoc.x] = '1'; break;

            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
                dispMap[curLoc.y][curLoc.x] += 1; break;
        }
    }
    // put it on screen
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            std::cout << dispMap[y][x];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// from here I tried a number of approaches to analyse if the resulting configuration contains the picture of a christmas tree
// as you can see I needed quite some attempts :)
// I commented why the approach didn't succeed for each subsequent function


// At this point I just assumed that all robots would be part of the tree. Later on I found in the description that *most* robots were (but not all)
bool CouldBeTree1( DataStream &robData ) {
    // I assume that a tree can be identified by having a single robot in the center column of the map, with no robots above it
    // Let's see if it works
    auto cmp_function = [=]( DatumType &a, DatumType &b ) {
        return (
            a.pos.y < b.pos.y ||
            (a.pos.y == b.pos.y && a.pos.x < b.pos.x)
        );
    };
    std::sort( robData.begin(), robData.end(), cmp_function );
    return (
        robData[0].pos.x == glbMapX / 2 + 1 &&
        robData[0].pos.y <  robData[1].pos.y
    );
}

// Although the condition I looked for is valid for the picture with the tree, many more configurations were having this condition...
bool CouldBeTree2( DataStream &robData ) {
    // I assume that a tree can be identified by having each robot occupying a single position (so no overlapping robots)
    // Let's see if it works
    std::set<LocType> locSet;
    for (int i = 0; i < (int)robData.size(); i++) {
        locSet.insert( robData[i].pos );
    }
    return locSet.size() == robData.size();
}

// It turned out that the tree was not symmetrical along the central column - it wasn't even centered on the map
// neither vertically nor horizontally. Furthermore, this function doesn't take the "most" condition into account.
bool CouldBeTree3( DataStream &robData ) {
    // I assume that a tree can be identified by all rows being symmetrical along the central column of the map
    // Let's see if it works

    // check on each robot left of the central column
    int nCentralCol = glbMapX / 2;
    bool bSymmetrical = true;  // iterate until asymmetricality is found
    for (int i = 0; i < (int)robData.size() && bSymmetrical; i++) {
        DatumType &curRob = robData[i];
        LocType   &curLoc = curRob.pos;
        // if robot is left of central column, check it has a mirrored counterpart
        if (curLoc.x < nCentralCol) {
            int nDist = nCentralCol - curLoc.x;
            LocType lMirror = { nCentralCol + nDist, curLoc.y };

            auto cmp_function = [=]( DatumType &a ) {
                return a.pos == lMirror;
            };
            bSymmetrical = std::count_if( robData.begin(), robData.end(), cmp_function ) == 1;
        }
    }
    return bSymmetrical;
}


float glbCmpPerc = 0.10f;
// This approach still assumed that the tree would be centered on the map (both hor and vertically), and it isn't
// Furthermore, the tree doesn't span all of the map, just a portion of it.
bool CouldBeTree4( DataStream &robData ) {
    // I assume that a tree can be identified by the following conditions:
    //   * nr of robots in quadrants 0 and 1 must be very much alike
    //   * the same holds for quadrants 2 and 3
    //   * the number of robots in quadrant 0 (and 1) must be less than in quadrant 2 (and 3)
    // Let's see if it works
    int nQuadCounts[4];
    int cmpMargin = int( robData.size() * glbCmpPerc );

    for (int i = 0; i < 4; i++) {
        nQuadCounts[i] = CountQuadrant( robData, i );
    }
    auto equal_with_margin = [=]( int a, int b, int margin ) {
        return abs( a - b ) <= margin;
    };
    return (
        equal_with_margin( nQuadCounts[0], nQuadCounts[1], cmpMargin ) &&
        equal_with_margin( nQuadCounts[2], nQuadCounts[3], cmpMargin ) &&
        nQuadCounts[0] < nQuadCounts[2]
    );
}

// The assumption that the tree is centered vertically doesn't hold
bool CouldBeTree5( DataStream &robData ) {
    // I assume that a tree can be identified by the following conditions:
    //   * the nr of robots in the central column is > 50% of the height of the map
    // Let's see if it works

    int nCentralCol = glbMapX / 2;

    auto cmp_function = [=]( DatumType &a ) {
        return a.pos.x == nCentralCol;
    };

    int nNrCentral = std::count_if( robData.begin(), robData.end(), cmp_function );

    float fPercentage = 0.70f;
    return nNrCentral >= int( glbMapY * fPercentage);
}

// see comments on CouldBeTree4()
bool CouldBeTree6( DataStream &robData ) {
    // I assume that a tree can be identified by the following conditions:
    //   * top and bottom row is empty, and
    //   * the conditions for CouldBeTree4() apply
    // Let's see if it works

    int nCmpRow;
    auto cmp_function = [=]( DatumType &a ) {
        return a.pos.y == nCmpRow;
    };

    int nTopRow, nBotRow;
    nCmpRow =           0; nTopRow = std::count_if( robData.begin(), robData.end(), cmp_function );
    nCmpRow = glbMapY - 1; nBotRow = std::count_if( robData.begin(), robData.end(), cmp_function );

    if (nTopRow == 0 && nBotRow == 0) {
        return CouldBeTree4( robData );
    }
    return false;
}

// this assumes that the tree spans most of the map, and it didn't irl
bool CouldBeTree7( DataStream &robData ) {
    // I assume that a tree can be identified by the following conditions:
    //   * the number of occupied locations in a row must be weakly increasing for at least 3 rows
    // Let's see if it works

    int nCheckRows = 7;

    auto occupied_in_row = [=]( DataStream &dData, int nRow ) {
        std::set<LocType> locSet;
        for (auto &e : dData) {
            if (e.pos.y == nRow) {
                locSet.insert( e.pos );
            }
        }
        return locSet.size();
    };

    auto is_weakly_increasing = [=]( int a, int b ) {
        return ( a <= b && b <= a + 4 );
    };

    int y = 0;
    int nFirstFound = 0;
    int nPrevFound = 0;
    bool bFirstFound = false;
    bool bWeaklyIncreasing = true;
    do {
        int nrOccupied = occupied_in_row( robData, y );
        if (bFirstFound) {
            bWeaklyIncreasing = is_weakly_increasing( nPrevFound, nrOccupied );
            nPrevFound = nrOccupied;
        } else {
            bFirstFound = nrOccupied > 0;
            if (bFirstFound) {
                nFirstFound = y;
                nPrevFound = nrOccupied;
            }
        }
        y += 1;

    } while (bWeaklyIncreasing && (!bFirstFound || (bFirstFound && (y - nFirstFound) < nCheckRows)));

    return bWeaklyIncreasing;
}

// the tree was not centered along the central column
bool CouldBeTree8( DataStream &robData ) {
    // I assume that a tree can be identified by rows being mostly symmetrical along the central column of the map
    // Mostly symmetrical means > 50% of the nodes must have a symmetrical counterpart
    // Let's see if it works

    // now check on each robot left of the central column
    int nCentralCol = glbMapX / 2;
    int nNrSymmetrical = 0;     // count how many are symmetrical

    for (int i = 0; i < (int)robData.size(); i++) {
        DatumType &curRob = robData[i];
        LocType   &curLoc = curRob.pos;
        // if robot is left of central column, check it has a mirrored counterpart
        if (curLoc.x < nCentralCol) {
            int nDist = nCentralCol - curLoc.x;
            LocType lMirror = { nCentralCol + nDist, curLoc.y };

            auto cmp_function1 = [=]( DatumType &a ) {
                return a.pos == lMirror;
            };
            if (std::count_if( robData.begin(), robData.end(), cmp_function1 ) > 0) {
                nNrSymmetrical += 2;
            }
        }
    }
    // consider enough symmetricality if nr of nodes that has symm. counter part is larger than this percentage
    float fUsePerc = 0.50f;
    return nNrSymmetrical >= int( robData.size() * fUsePerc );
}

// I tried this with 50% and got a too much hits.
// I tried this with 80% and got no hits
// Then I just tested in the middle, with 65%, and a tree appeared on screen :)
bool CouldBeTree9( DataStream &robData ) {
    // I assume that a tree can be identified by most of the robots being 8-connected with another robot
    // Most of the robots means > 65%
    // Let's see if it works

    // how many are 8 connected?
    int nNr8connected = 0;
    // had some difficulty getting std::count_if() to work, so wrote my own
    auto my_count = [=]( DataStream &robData, LocType pos ) {
        int nResult = 0;
        for (auto &e : robData) {
            if (e.pos == pos) {
                nResult += 1;
            }
        }
        return nResult;
    };
    // returns true if robot r has any connection in an 8 connected config
    auto is_8connected = [=]( DataStream &robData, DatumType &r ) {
        int nConnections = (
            my_count( robData, { r.pos.x - 1, r.pos.y - 1 } ) +    // line above left to right
            my_count( robData, { r.pos.x    , r.pos.y - 1 } ) +
            my_count( robData, { r.pos.x + 1, r.pos.y - 1 } ) +

            my_count( robData, { r.pos.x - 1, r.pos.y     } ) +    // same line left and right
            my_count( robData, { r.pos.x + 1, r.pos.y     } ) +

            my_count( robData, { r.pos.x - 1, r.pos.y + 1 } ) +    // line below left to right
            my_count( robData, { r.pos.x    , r.pos.y + 1 } ) +
            my_count( robData, { r.pos.x + 1, r.pos.y + 1 } )
        );
        return nConnections > 0;
    };
    // count nr of robots that is 8-connected with a neighbouring robot
    for (int i = 0; i < (int)robData.size(); i++) {
        if (is_8connected( robData, robData[i] )) {
            nNr8connected += 1;
        }
    }
    // consider enough coherence if nr of robots that are connected is larger than this percentage
    float fUsePerc = 0.65f;
    return nNr8connected >= int( robData.size() * fUsePerc );
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

    for (int i = 1; i <= 100; i++) {
        SimulateOneSecond( inputData );
    }
//    std::cout << "After simulating 100 seconds --- " << std::endl;
    int nSafetyFactor = 1;
    for (int i = 0; i < 4; i++) {
        int nRobCount = CountQuadrant( inputData, i );
        nSafetyFactor *= nRobCount;
//        std::cout << "nr of robots in quadrant: " << i << " = " << nRobCount << ", current safety factor = " << nSafetyFactor << std::endl;
    }

    std::cout << std::endl << "Answer to part 1: Safety factor after 100 seconds = " << nSafetyFactor << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here
//    DisplayMap( inputData );

    bool bHaltFlag = false;
    int nSecondsNeeded = 0;
    for (int i = 1; !bHaltFlag; i++) {

        SimulateOneSecond( inputData );

//        DisplayMap( inputData );

        bHaltFlag = CouldBeTree9( inputData );
        if (bHaltFlag) {
            nSecondsNeeded = i;
//            std::cout << "After: " << i << " simulated seconds - I think this a tree... " << std::endl << std::endl;
//            DisplayMap( inputData );
        }
    }

    std::cout << std::endl << "Answer to part 2: simulated seconds to find easter egg = " << nSecondsNeeded << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
