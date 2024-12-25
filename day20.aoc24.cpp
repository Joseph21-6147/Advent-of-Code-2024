// AoC 2024 - Day 20 - Race Condition
// ==================================

// date:  2024-12-20
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day20"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <deque>
#include <list>
#include <cfloat>
#include <cmath>
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


// the data consists of a map modeled as a 2d character field
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = "###############"; dData.push_back( cDatum );
    cDatum = "#...#...#.....#"; dData.push_back( cDatum );
    cDatum = "#.#.#.#.#.###.#"; dData.push_back( cDatum );
    cDatum = "#S#...#.#.#...#"; dData.push_back( cDatum );
    cDatum = "#######.#.#.###"; dData.push_back( cDatum );
    cDatum = "#######.#.#...#"; dData.push_back( cDatum );
    cDatum = "#######.#.###.#"; dData.push_back( cDatum );
    cDatum = "###..E#...#...#"; dData.push_back( cDatum );
    cDatum = "###.#######.###"; dData.push_back( cDatum );
    cDatum = "#...###...#...#"; dData.push_back( cDatum );
    cDatum = "#.#####.#.###.#"; dData.push_back( cDatum );
    cDatum = "#.#...#.#.#...#"; dData.push_back( cDatum );
    cDatum = "#.#.#.#.#.#.###"; dData.push_back( cDatum );
    cDatum = "#...#...#...###"; dData.push_back( cDatum );
    cDatum = "###############"; dData.push_back( cDatum );
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

int glbMapX, glbMapY;      // to hold map dimensions
LocType glbStart, glbEnd;  // to hold start and end point locations in the map

// ------------ PATH FINDING STUFF HERE ---------------

typedef struct sNode {
    bool bObstacle = false;			// Is the node an obstruction?
    bool bVisited  = false;			// Have we searched this node before?
    float fGlobalGoal;				// Distance to goal so far
    float fLocalGoal;				// Distance to goal if we took the alternative route
    LocType pos;					// Nodes position in 2D space
    std::vector<sNode*> vNeighbours;	// Connections to neighbours
    sNode* parent;					// Node connecting to this node that offers shortest parent
} NodeType;

NodeType *nodes     = nullptr;      // a pointer to the space

NodeType *nodeStart = nullptr;      // a pointer to start and end node in space
NodeType *nodeEnd   = nullptr;

// initialises the map dimensions and start and end point variables, and then sets up the
// noded model needed for A* algorithm
void InitMap( DataStream &m ) {

    // init map dimensions
    glbMapY = (int)m.size();
    glbMapX = (int)m[0].length();
    // find start and end locations
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            if (m[y][x] == 'S') {
                glbStart = { x, y };
            }
            if (m[y][x] == 'E') {
                glbEnd = { x, y };
            }
        }
    }

    // Create a 2D array of nodes - this is for convenience of rendering and construction
    // and is not required for the algorithm to work - the nodes could be placed anywhere
    // in any space, in multiple dimensions...
    nodes = new NodeType[glbMapX * glbMapY];
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            nodes[y * glbMapX + x].pos       = { x, y }; // ...because we give each node its own coordinates
            nodes[y * glbMapX + x].bObstacle = (m[y][x] == '#');
            nodes[y * glbMapX + x].bVisited  = false;
            nodes[y * glbMapX + x].parent    = nullptr;
        }
    }
    // Create connections - in this case nodes are on a regular grid
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            if (y >           0) nodes[y * glbMapX + x].vNeighbours.push_back(&nodes[(y - 1) * glbMapX + (x + 0)]);
            if (y < glbMapY - 1) nodes[y * glbMapX + x].vNeighbours.push_back(&nodes[(y + 1) * glbMapX + (x + 0)]);
            if (x >           0) nodes[y * glbMapX + x].vNeighbours.push_back(&nodes[(y + 0) * glbMapX + (x - 1)]);
            if (x < glbMapX - 1) nodes[y * glbMapX + x].vNeighbours.push_back(&nodes[(y + 0) * glbMapX + (x + 1)]);
        }
    }
    // Manually position the start and end markers so they are not nullptr
    nodeStart = &nodes[glbStart.y * glbMapX + glbStart.x ];
    nodeEnd   = &nodes[glbEnd.y   * glbMapX + glbEnd.x   ];
}

// data structure to hold paths
typedef struct sPathStepStruct {
    int nr;
    NodeType *pNode;
} PathStep;
typedef std::deque<PathStep> PathType;

// see javidx9's vid on path finding
bool Solve_AStar() {

    // Reset Navigation Graph - default all node states
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            nodes[y * glbMapX + x].bVisited    = false;
            nodes[y * glbMapX + x].fGlobalGoal = FLT_MAX;  // INFINITY;
            nodes[y * glbMapX + x].fLocalGoal  = FLT_MAX;  // INFINITY;
            nodes[y * glbMapX + x].parent      = nullptr;  // No parents
        }
    }

    auto distance = [](NodeType *a, NodeType *b) { // For convenience
        return sqrtf((a->pos.x - b->pos.x) * (a->pos.x - b->pos.x) + (a->pos.y - b->pos.y) * (a->pos.y - b->pos.y));
    };

    auto heuristic = [distance](NodeType *a, NodeType *b) { // So we can experiment with heuristic
        return distance(a, b);
    };

    // Setup starting conditions
    NodeType *nodeCurrent = nodeStart;
    nodeStart->fLocalGoal = 0.0f;
    nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);

    // Add start node to not tested list - this will ensure it gets tested.
    // As the algorithm progresses, newly discovered nodes get added to this
    // list, and will themselves be tested later
    std::list<NodeType*> listNotTestedNodes;
    listNotTestedNodes.push_back(nodeStart);

    // if the not tested list contains nodes, there may be better paths
    // which have not yet been explored. However, we will also stop
    // searching when we reach the target - there may well be better
    // paths but this one will do - it wont be the longest.

//    while (!listNotTestedNodes.empty() && nodeCurrent != nodeEnd) { // Find absolutely shortest path // && nodeCurrent != nodeEnd)
    while (!listNotTestedNodes.empty()) { // Find absolutely shortest path // && nodeCurrent != nodeEnd)
        // if you stop stop searching when you reach the end, you'll just have a path, but not
        // necessarily the shortest one. So out comment "&& nodeCurrent != nodeEnd" if you
        // want to have the absolute shortest path

        // Sort Untested nodes by global goal, so lowest is first
        listNotTestedNodes.sort(
            [](const NodeType * lhs, const NodeType * rhs) {
                return lhs->fGlobalGoal < rhs->fGlobalGoal;
            }
        );

        // Front of listNotTestedNodes is potentially the lowest distance node. Our
        // list may also contain nodes that have been visited, so ditch these...
        while(!listNotTestedNodes.empty() && listNotTestedNodes.front()->bVisited)
            listNotTestedNodes.pop_front();

        // ...or abort because there are no valid nodes left to test
        if (!listNotTestedNodes.empty()) {

            nodeCurrent = listNotTestedNodes.front();
            nodeCurrent->bVisited = true; // We only explore a node once

            // Check each of this node's neighbours...
            for (auto nodeNeighbour : nodeCurrent->vNeighbours) {
                // ... and only if the neighbour is not visited and is
                // not an obstacle, add it to NotTested List
                if (!nodeNeighbour->bVisited && !nodeNeighbour->bObstacle)
                    listNotTestedNodes.push_back(nodeNeighbour);

                // Calculate the neighbours potential lowest parent distance
                // the graph is not weighted so each step to a neighbour is just 1
                float fPossiblyLowerGoal = nodeCurrent->fLocalGoal + 1;

                // If choosing to path through this node is a lower distance than what
                // the neighbour currently has set, update the neighbour to use this node
                // as the path source, and set its distance scores as necessary
                if (fPossiblyLowerGoal < nodeNeighbour->fLocalGoal) {
                    nodeNeighbour->parent = nodeCurrent;
                    nodeNeighbour->fLocalGoal = fPossiblyLowerGoal;

                    // The best path length to the neighbour being tested has changed, so
                    // update the neighbour's score. The heuristic is used to globally bias
                    // the path algorithm, so it knows if its getting better or worse. At some
                    // point the algo will realise this path is worse and abandon it, and then go
                    // and search along the next best path.
                    nodeNeighbour->fGlobalGoal = nodeNeighbour->fLocalGoal + heuristic(nodeNeighbour, nodeEnd);
                }
            }
        }
    }

    return true;
}

// ------------ END OF PATH FINDING STUFF ---------------

// converts the global map addressed via the "nodes" pointer to a printable, character based version
// Puts the steps from path p into that map as character 'O'
DataStream GetVisualMap( PathType &p ) {
    // create and populate character field representing the map
    DataStream vResultMap;
    for (int y = 0; y < glbMapY; y++) {
        std::string sRow;
        for (int x = 0; x < glbMapX; x++) {
            if (nodes[y * glbMapX + x].bObstacle) {
                sRow.append( "#" );
            } else {
                sRow.append( "." );
            }
        }
        vResultMap.push_back( sRow );
    }
    // add path nodes to it
    for (int i = 0; i < (int)p.size(); i++) {
        vResultMap[p[i].pNode->pos.y][p[i].pNode->pos.x] = 'O';
    }
    return vResultMap;
}

// Prints a visual image of the map to screen
void PrintVisualMap( const std::string &sMsg, PathType &p ) {
    // print message
    std::cout << sMsg << std::endl << std::endl;
    // stats
    std::cout << "Map size = " << glbMapX << " wide by " << glbMapY << " tall" << std::endl;
    std::cout << "Start location = " << glbStart.to_string() << std::endl;
    std::cout << "End   location = " << glbEnd.to_string() << std::endl;
    std::cout << "Path length = " << p.size() << " (so race time = " << p.size() - 1 << " picoseconds) " << std::endl;
    std::cout << std::endl;

    // prepare and populate visual map
    DataStream vMap = GetVisualMap( p );
    // output it
    for (int y = 0; y < (int)vMap.size(); y++) {
        std::cout << vMap[y] << std::endl;
    }
    std::cout << std::endl;
}

// After A* has run, get the shortest path from the nodes graph. Note: if no path was found,
// an empty PathType container will be returned
PathType ExtractPath() {
    PathType result;
    NodeType *cachPtr = nullptr; // to check if the path is complete
    int nCntr = 0;
    // walk the nodes graph following the chain of parent pointers
    // store pointer to the node and step number per path step
    for (NodeType *ptr = nodeEnd; ptr != nullptr; ptr = ptr->parent) {
        result.push_front( { nCntr, ptr } );
        cachPtr = ptr;
        nCntr += 1;
    }
    // previous loop terminated with nullptr ptr, so check if it arrived at the start node
    if (cachPtr != nodeStart) {
        // if the start node wasn't reached, the path is not complete, so clear it
        result.clear();
    }
    return result;
}

// a CheatType struct has two locations, and two indices into the path that are
// connected (short cut) by the cheat
typedef struct sCheatStruct {
    LocType s1, s2;
    int     n1, n2;
} CheatType;
typedef std::vector<CheatType> CheatStream;

// direction indicators
#define UP 0
#define RT 1
#define DN 2
#define LT 3

// offsets in order of direction
std::vector<LocType> vOffsets = {
    {  0, -1 },
    { +1,  0 },
    {  0, +1 },
    { -1,  0 }
};

// returns true if cur is a location within map boundaries
bool InBounds( LocType &cur ) {
    return cur.x >= 0 && cur.x < glbMapX && cur.y >= 0 && cur.y < glbMapY;
}

// returns the location of the neighbour of cur in direction nDir
// Note: no boundary checking is done here
LocType Neighbour( LocType &cur, int nDir ) {
    LocType result = { cur.x + vOffsets[nDir].x, cur.y + vOffsets[nDir].y };
    return result;
}

// if pos is a location that corresponds with a step in path p, then the index of that step is returned
// this is the same as the step number.
// If not found, -1 is returned.
int GetPathStep( PathType &p, LocType &pos ) {
    int nFound = -1;
    for (int i = 0; i < (int)p.size() && nFound == -1; i++) {
        if (p[i].pNode->pos == pos) {
            nFound = i;
        }
    }
    return nFound;
}

// analyses, collects and returns all cheat options based on the input shortest path pShortest
CheatStream GetCheatOptions1( PathType &pShortest ) {
    DataStream vMap = GetVisualMap( pShortest );
    CheatStream vResultCheats;

    // lambda to help addressing of map
    auto GetMapValue = [=]( DataStream &m, LocType &p ) -> char {
        return m[p.y][p.x];
    };

    // look for shortcuts by walking along the shortest path for the uncheated map
    for (int i = 0; i < (int)pShortest.size(); i++) {

        // if current location is a step in the shortest path
        // and another step in the shortest path is available within 1 or 2 separating wall locations
        // then these 1 or 2 separating wall locations are a cheat option
        LocType curLoc = pShortest[i].pNode->pos;
        int curStep = i;

        // iterate all four directions
        for (int d = UP; d <= LT; d++) {
            LocType nghbr1 = Neighbour( curLoc, d );
            LocType nghbr2 = Neighbour( nghbr1, d );
            LocType nghbr3 = Neighbour( nghbr2, d );
            // check on single cell barriers
            if (
                InBounds( nghbr1 ) && GetMapValue( vMap, nghbr1 ) == '#' &&
                InBounds( nghbr2 ) && GetMapValue( vMap, nghbr2 ) == 'O'
            ) {
                int nextStep = GetPathStep( pShortest, nghbr2 );
                if (nextStep > curStep) {
                    // this situation has 2 possible cheats, so add them both --> no! the examples show otherwise!!
//                    vResultCheats.push_back( { curLoc, nghbr1, curStep, nextStep } );
                    vResultCheats.push_back( { nghbr1, nghbr2, curStep, nextStep } );
                }
            }
            // check on double cell barriers
            if (
                InBounds( nghbr1 ) && GetMapValue( vMap, nghbr1 ) == '#' &&
                InBounds( nghbr2 ) && GetMapValue( vMap, nghbr2 ) == '#' &&  // could this also be a '.'?
                InBounds( nghbr3 ) && GetMapValue( vMap, nghbr3 ) == 'O'
            ) {
                int nextStep = GetPathStep( pShortest, nghbr3 );
                if (nextStep > curStep) {
                    // this situation has only one possible cheat, so add it
                    vResultCheats.push_back( { nghbr1, nghbr2, curStep, nextStep } );
                }
            }
        }
    }
    return vResultCheats;
}

// returns how many steps are saved using cheat c
int GetStepsSaved1( CheatType &c ) {
    return c.n2 - c.n1 - 2;
}

// ----- PART 2

// part 2 is a generalization of part 1. Instead of cheat options having 1 or two steps, you now have a set of cheat options
// having at most 20 steps. A set of locations is created to help examine what the cheat options are.

typedef std::set<LocType> FieldType;

// for testing/debugging
void PrintField( FieldType &f ) {
    for (auto e : f ) {
        std::cout << e.to_string() << std::endl;
    }
    std::cout << std::endl;

}

// Create a set of offsets (from coord (0, 0)) with a radius of nFieldSize. The radius is the maximum
// manhattan distance of the offset from (0, 0).
FieldType GetCheatField( int nFieldSize, bool bOutput = false ) {

    FieldType vResult;
    for (int v = UP; v <= DN; v += 2) {   // iterate over 4 quadrants (= combination of a vert + hor direction)
        int yFactor = vOffsets[v].y;
        for (int h = RT; h <= LT; h += 2) {
            int xFactor = vOffsets[h].x;
            // populate the offsets
            for (int yOffset = 0; yOffset <= nFieldSize; yOffset++) {
                int xStart = (yOffset == 0) ? xFactor : 0;
                for (int xOffset = xStart; xOffset <= nFieldSize - yOffset; xOffset++) {
                    vResult.insert( { xOffset * xFactor, yOffset * yFactor } );
                }
            }
        }
    }

    if (bOutput) {
        PrintField( vResult );
    }

    return vResult;
}

// this function analyses all cheat options using the set of location in parameter cField.
// it returns a std::vector of CheatType structs.
CheatStream GetCheatOptions2( PathType &pShortest, FieldType &cField ) {
    CheatStream vResultCheats;

    // debug info
//    int nPathLen = pShortest.size();
//    int nFieldSize = cField.size();
//    std::cout << "INFO: GetCheatOptions2() --> path length = " << nPathLen
//              << ", field size = " << nFieldSize
//              << ", so analyzing: " << nPathLen * nFieldSize << " possibilities" << std::endl;

    // look for shortcuts by walking along the shortest path for the uncheated map
    for (int i = 0; i < (int)pShortest.size(); i++) {

        LocType curLoc = pShortest[i].pNode->pos;
        int curStep = i;

        // iterate all offsets in the cheat field, apply them on the current location, and
        // see which ones land on a path location that is indeed a shortcut (and not a cycle backwards)
        for (auto &e : cField) {
            LocType curOffset = e;

            // If the other (trial) location is in bounds and is indeed a short cut, we found a cheat
            LocType otherLoc = { curLoc.x + curOffset.x, curLoc.y + curOffset.y };
            if (InBounds( otherLoc )) {

                int otherStep = GetPathStep( pShortest, otherLoc );
                if (otherStep > curStep) {     // Note: this if clause also covers the case where otherStep == -1
                    vResultCheats.push_back( { curLoc, otherLoc, curStep, otherStep } );
                }
            }
        }
    }
    return vResultCheats;
}

int ManhattanDistance( LocType &a, LocType &b ) {
    return abs( a.x - b.x ) + abs( a.y - b.y );
}

// returns the steps saved minus the length of the cheat path
int GetStepsSaved2( CheatType &c ) {
    return c.n2 - c.n1 - ManhattanDistance( c.s1, c.s2 );
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
    InitMap( inputData );
    PathType vPath;

//    PrintVisualMap( "after init, before A*: ", vPath );

    Solve_AStar();
    vPath = ExtractPath();

//    PrintVisualMap( "after A*: ", vPath );

    CheatStream vCheatOptions = GetCheatOptions1( vPath );

    // sort cheat options so that you can count them per yield
//    std::sort(
//        vCheatOptions.begin(),
//        vCheatOptions.end(),
//        []( CheatType &a, CheatType &b ) {
//            return GetStepsSaved1( a ) < GetStepsSaved1( b );
//        }
//    );

//    for (int i = 0; i < (int)vCheatOptions.size(); i++) {
//        CheatType &curCheat = vCheatOptions[i];
//        std::cout << "Cheat option: " << i << " = "
//                  << curCheat.s1.to_string() << ", " << curCheat.s2.to_string()
//                  << " from path step: " << curCheat.n1 << " to step: " << curCheat.n2
//                  << " saves: " << GetStepsSaved1( curCheat ) << " steps..."
//                  << std::endl;
//    }

    int nCheckNr = (glbProgPhase == PUZZLE ? 100 : 0);
    int nrOptions = std::count_if(
        vCheatOptions.begin(),
        vCheatOptions.end(),
        [=]( CheatType &c ) {
            return GetStepsSaved1( c ) >= nCheckNr;
        }
    );


    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1:  = " << nrOptions << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here

    // create coordinate field with radius 20 and exclusion zone at the center
    FieldType fullField = GetCheatField( 20, false );
    FieldType exclField = GetCheatField(  1, false );
    FieldType cheatField;
    std::set_difference(
        fullField.begin(), fullField.end(),
        exclField.begin(), exclField.end(),
        std::inserter( cheatField, cheatField.begin() )
    );

//    PrintField( cheatField );

    CheatStream vCheatOptions2 = GetCheatOptions2( vPath, cheatField );

//    std::sort(
//        vCheatOptions2.begin(),
//        vCheatOptions2.end(),
//        []( CheatType &a, CheatType &b ) {
//            return GetStepsSaved2( a ) < GetStepsSaved2( b );
//        }
//    );

//    for (int i = 0; i < (int)vCheatOptions2.size(); i++) {
//        CheatType &curCheat = vCheatOptions2[i];
//        std::cout << "Cheat option " << i << " = "
//                  << curCheat.s1.to_string() << ", " << curCheat.s2.to_string()
//                  << " from path step: " << curCheat.n1 << " to step: " << curCheat.n2
//                  << " saves: " << GetStepsSaved2( curCheat ) << " steps..."
//                  << std::endl;
//    }

    int nCheckNr2 = (glbProgPhase == PUZZLE ? 100 : 50);
    int nrOptions2 = std::count_if(
        vCheatOptions2.begin(),
        vCheatOptions2.end(),
        [=]( CheatType &c ) {
            return GetStepsSaved2( c ) >= nCheckNr2;
        }
    );

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: = " << nrOptions2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
