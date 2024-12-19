// AoC 2024 - Day 15 - Warehouse Woes
// ==================================

// date:  2024-12-15
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day15"

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

// the data for this puzzle consists of two parts:
// 1. a map (modeled as a character field)
// 2. a number of move attempts, spread out over multiple lines, containing characters '^', '>', 'v', '<' to model the direction

typedef std::string MapRowType;
typedef std::vector<MapRowType> MapType;

int glbMapX, glbMapY;

#define UP 0    // direction constants
#define RT 1
#define DN 2
#define LT 3

typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( MapType &mData, DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    MapRowType mapRow;
    DatumType cDatum;

//    mapRow = "########"; mData.push_back( mapRow );   // example for part 1
//    mapRow = "#..O.O.#"; mData.push_back( mapRow );
//    mapRow = "##@.O..#"; mData.push_back( mapRow );
//    mapRow = "#...O..#"; mData.push_back( mapRow );
//    mapRow = "#.#.O..#"; mData.push_back( mapRow );
//    mapRow = "#...O..#"; mData.push_back( mapRow );
//    mapRow = "#......#"; mData.push_back( mapRow );
//    mapRow = "########"; mData.push_back( mapRow );

    mapRow = "#######"; mData.push_back( mapRow );    // example for part 2
    mapRow = "#...#.#"; mData.push_back( mapRow );
    mapRow = "#.....#"; mData.push_back( mapRow );
    mapRow = "#..OO@#"; mData.push_back( mapRow );
    mapRow = "#..O..#"; mData.push_back( mapRow );
    mapRow = "#.....#"; mData.push_back( mapRow );
    mapRow = "#######"; mData.push_back( mapRow );

//    cDatum = "<^^>>>vv<v>>v<<"; dData.push_back( cDatum );   // example for part 1

    cDatum = "<vv<<^^<<^^"; dData.push_back( cDatum );       // example for part 2
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, MapType &mData, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    mData.clear();
    vData.clear();
    bool bMapPhase = true;
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        if (sLine.length() == 0) {
            bMapPhase = false;
        } else {
            if (bMapPhase) {
                mData.push_back( sLine );
            } else {
                vData.push_back( sLine );
            }
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   MapType &mData, DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, mData, dData ); }
void GetData_PUZZLE( MapType &mData, DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, mData, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintMapDataStream( MapType &mData ) {
    for (auto &e : mData) {
        std::cout << e << std::endl;
    }
    std::cout << std::endl;
}

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
void GetInput( MapType &mData, DataStream &dData, bool bDisplay = false ) {

    switch( glbProgPhase ) {
        case EXAMPLE: GetData_EXAMPLE( mData, dData ); glbMapX = mData[0].length(); glbMapY = mData.size(); break;
        case TEST:    GetData_TEST(    mData, dData ); glbMapX = mData[0].length(); glbMapY = mData.size(); break;
        case PUZZLE:  GetData_PUZZLE(  mData, dData ); glbMapX = mData[0].length(); glbMapY = mData.size(); break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }
    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintMapDataStream( mData );
        PrintDataStream( dData );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

// convert direction denoting character into value [0, 3] == [UP, LT]
int Char2Dir( char d ) {
    int result = -1;
    switch (d) {
        case '^': result = UP; break;
        case '>': result = RT; break;
        case 'v': result = DN; break;
        case '<': result = LT; break;
        default: std::cout << "ERROR: Char2Dir() --> Unexpected move char encountered: " << d << std::endl;
    }
    return result;
}

LocType glbRobPos;  // keep track of robot position in the map

void ProcessMapInput( MapType &mData ) {
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            if (mData[y][x] == '@') {
                glbRobPos = { x, y };
            }
        }
    }
    // set map dimensions
    glbMapY = mData.size();
    glbMapX = mData[0].length();
}

// instead of character ^>v< use int values in range [0, 3]
typedef std::vector<int> MovesType;

void ProcessMoveInput( DataStream &iData, MovesType &movesData ) {
    for (int i = 0; i < (int)iData.size(); i++) {
        for (int j = 0; j < (int)iData[i].length(); j++) {
            int nDir = Char2Dir( iData[i][j] );
            if (nDir < UP || nDir > LT) {
                std::cout << "ERROR: ProcessMoveInput() --> Unknown dir value: " << nDir << std::endl;
            } else {
                movesData.push_back( nDir );
            }
        }
    }
}

// offset array for easy addressing neighbours of a location
LocType glbOffsets[4] = {
    {  0, -1 },
    { +1,  0 },
    {  0, +1 },
    { -1,  0 }
};

// return coordinate of neighbour w.r.t. dir
LocType Neighbour( LocType &p, int dir ) {
    LocType lResult = { p.x + glbOffsets[dir].x, p.y + glbOffsets[dir].y };
    return lResult;
}

// return if the robot (or box) at position pos in the map mapData is blocked going in direction dir
bool IsBlocked( MapType &mapData, LocType &pos, int dir ) {
    bool bResult = false;
    // get position next to pos (w.r.t. direction dir)
    LocType nextPos = Neighbour( pos, dir );

    switch (mapData[nextPos.y][nextPos.x]) {
        case '.': bResult = false; break;
        case '#': bResult = true ; break;
        case 'O': bResult = IsBlocked( mapData, nextPos, dir ); break;
        default: std::cout << "ERROR: IsBlocked() --> unexpected map value encountered: " << mapData[nextPos.y][nextPos.x] << std::endl;
    }
    return bResult;
}

// exchange to values in the map
void MapSwap( MapType &mData, LocType &pos1, LocType &pos2 ) {
    char cCache = mData[pos1.y][pos1.x];
    mData[pos1.y][pos1.x] = mData[pos2.y][pos2.x];
    mData[pos2.y][pos2.x] = cCache;
}

// the "thing" at pos can only be a robot or a box
// check up front that moving is possible, using IsBlocked()
void MoveThing( MapType &mapData, LocType &pos, int moveDir ) {
    LocType nextPos = Neighbour( pos, moveDir );

    switch (mapData[nextPos.y][nextPos.x]) {
        case '.': MapSwap( mapData, pos, nextPos ); break;
        case 'O': MoveThing( mapData, nextPos, moveDir );
                  MapSwap( mapData, pos, nextPos ); break;
        case '#': std::cout << "ERROR: MoveThing() --> cannot move a wall... " << std::endl; break;
        default : std::cout << "ERROR: MoveThing() --> unexpected neighbour value: " << mapData[nextPos.y][nextPos.x] << std::endl;
    }
}

// Attempt to move robot one step in direction moveDir.
// Also move boxes if they are in the way and are not blocked.
void OneMoveAttempt( MapType &mapData, int moveDir ) {
    if (!IsBlocked( mapData, glbRobPos, moveDir )) {
        MoveThing( mapData, glbRobPos, moveDir );
        glbRobPos = Neighbour( glbRobPos, moveDir );
    }
}

// converts direction to shorthand description
std::string Dir2String( int dir ) {
    std::string sResult = "<empty>";
    switch (dir) {
        case UP: sResult = "UP"; break;
        case RT: sResult = "RT"; break;
        case DN: sResult = "DN"; break;
        case LT: sResult = "LT"; break;
        default: std::cout << "ERROR: Dir2String() --> unknown dir value: " << dir << std::endl;
    }
    return sResult;
}

// according to puzzle description
int GetGPScoord( LocType pos ) {
    return (100 * pos.y + pos.x);
}

// accumulate all GPS coordinate scores for all boxes in the map
int GetWarehouseScore( MapType &m ) {
    int nResult = 0;
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            if (m[y][x] == 'O') {
                nResult += GetGPScoord( { x, y } );
            }
        }
    }
    return nResult;
}

// ----- PART 2

// enlarge the width of the map as per puzzle description
void EnlargeMap( MapType &m ) {
    MapType cm = m;  // make a copy
    m.clear();

    for (int y = 0; y < glbMapY; y++) {
        MapRowType newRow;
        for (int x = 0; x < glbMapX; x++) {
            switch (cm[y][x]) {
                case '#': newRow.append( "##" ); break;
                case 'O': newRow.append( "[]" ); break;
                case '.': newRow.append( ".." ); break;
                case '@': newRow.append( "@." ); break;
                default: std::cout << "ERROR: EnlargeMap() --> unknown map value: " << m[y][x] << std::endl;
            }
        }
        m.push_back( newRow );
    }
    // reset map dimension values
    glbMapX = m[0].length();
    glbMapY = m.size();
    // search and set robot location
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            if (m[y][x] == '@') {
                glbRobPos = { x, y };
            }
        }
    }
}

// return if the robot (or box) at position pos in the map mapData is blocked going in direction dir
bool IsBlocked2( MapType &mapData, LocType &pos, int dir ) {
    bool bResult = false;
    // get position next to pos (w.r.t. direction dir)
    LocType nextPos = Neighbour( pos, dir );

    if (mapData[nextPos.y][nextPos.x] == '.') {
        bResult = false;
    } else if (mapData[nextPos.y][nextPos.x] == '#') {
        bResult = true;
    } else if (mapData[nextPos.y][nextPos.x] == '[' || mapData[nextPos.y][nextPos.x] == ']') {
        if (dir == LT || dir == RT) {
            LocType nextNextPos = Neighbour( nextPos, dir );
            bResult = IsBlocked2( mapData, nextNextPos, dir );
        } else {   // dir is UP or DOWN
            LocType otherPos = (mapData[nextPos.y][nextPos.x] == '[') ? Neighbour( nextPos, RT ) : Neighbour( nextPos, LT );
            bResult = IsBlocked2( mapData,  nextPos, dir ) ||
                      IsBlocked2( mapData, otherPos, dir );
        }
    } else {
        std::cout << "ERROR: IsBlocked() --> unexpected map value encountered: " << mapData[nextPos.y][nextPos.x] << std::endl;
    }
    return bResult;
}


// the "thing" at pos can only be a robot or a box
// check up front that moving is possible, using IsBlocked2()
void MoveThing2( MapType &mapData, LocType &pos, int moveDir ) {
    LocType nextPos = Neighbour( pos, moveDir );

    if (mapData[nextPos.y][nextPos.x] == '.') {
        MapSwap( mapData, pos, nextPos );
    } else if (mapData[nextPos.y][nextPos.x] == '[' || mapData[nextPos.y][nextPos.x] == ']') {

        if (moveDir == LT || moveDir == RT) {
            LocType nextNextPos = Neighbour( nextPos, moveDir );
            MoveThing2( mapData, nextNextPos, moveDir );
            MapSwap( mapData, nextPos, nextNextPos );
            MapSwap( mapData, pos, nextPos );
        } else {
            LocType otherPos, adjctPos;
            if (mapData[nextPos.y][nextPos.x] == '[') {
                otherPos = Neighbour(   nextPos, RT );
                adjctPos = Neighbour( glbRobPos, RT );

            } else {
                otherPos = Neighbour(   nextPos, LT );
                adjctPos = Neighbour( glbRobPos, LT );
            }
            MoveThing2( mapData,  nextPos, moveDir );
            MoveThing2( mapData, otherPos, moveDir );

            MapSwap( mapData, nextPos, pos );
            // do NOT swap the map value next to the robot
        }
    } else if (mapData[nextPos.y][nextPos.x] == '#') {
        std::cout << "ERROR: MoveThing2() --> cannot move a wall... " << std::endl;
    } else {
        std::cout << "ERROR: MoveThing2() --> unexpected neighbour value: " << mapData[nextPos.y][nextPos.x] << std::endl;
    }
}

// similar to OneMoveAttempt(), but obeying the part 2 rules
void OneMoveAttempt2( MapType &mapData, int moveDir ) {
    if (!IsBlocked2( mapData, glbRobPos, moveDir )) {
        MoveThing2( mapData, glbRobPos, moveDir );
        glbRobPos = { glbRobPos.x + glbOffsets[moveDir].x, glbRobPos.y + glbOffsets[moveDir].y };
    }
}

// similar to GetWarehousScore(), but obeying the part 2 rules
int GetWarehouseScore2( MapType &m ) {
    int nResult = 0;
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            if (m[y][x] == '[') {
                nResult += GetGPScoord( { x, y } );
            }
        }
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
    MapType    mapData;
    DataStream inputData;
    GetInput( mapData, inputData, glbProgPhase != PUZZLE );
    // make copies to be used for part 2
    MapType    part2Map  = mapData;
    DataStream part2Data = inputData;

    int nMoveAttempts = 0;
    for (auto &e : inputData) { nMoveAttempts += e.length(); }
    std::cout << "Data stats - size of map: (" << glbMapX << " x " << glbMapY << ") - size of data stream " << nMoveAttempts << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here
    MovesType movesData;

    ProcessMapInput( mapData );
    ProcessMoveInput( inputData, movesData );

//    std::cout << "initial state" << std::endl;
//    PrintMapDataStream( mapData );

    for (int i = 0; i < (int)movesData.size(); i++) {
        OneMoveAttempt( mapData, movesData[i] );

//        std::cout << "Move = " << Dir2String( movesData[i] ) << std::endl;
//        PrintMapDataStream( mapData );
    }

    int nScore = GetWarehouseScore( mapData );

    std::cout << std::endl << "Answer to part 1: sum of all boxes' GPS coordinates = " << nScore << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data
    mapData   = part2Map;

    // part 2 code here
    EnlargeMap( mapData );
    ProcessMapInput( mapData );

//    std::cout << "initial state" << std::endl;
//    PrintMapDataStream( mapData );

    for (int i = 0; i < (int)movesData.size(); i++) {
        OneMoveAttempt2( mapData, movesData[i] );

//        std::cout << "Move = " << Dir2String( movesData[i] ) << std::endl;
//        PrintMapDataStream( mapData );
    }

    int nScore2 = GetWarehouseScore2( mapData );

    std::cout << std::endl << "Answer to part 2: sum of all boxes' GPS coordinates = " << nScore2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
