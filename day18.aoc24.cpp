// AoC 2024 - Day 18 - RAM Run
// ===========================

// date:  2024-12-18
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day18"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <deque>
#include <list>
#include <cfloat>
#include <cmath>

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


// the data consists of tuples (pairs) of numbers, modeling corruptions in a 2D memory space
typedef LocType DatumType;
typedef std::vector<DatumType> DataStream;

int glbSpaceW, glbSpaceH;    // to hold the dimensions of the 2D memory space

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { 5, 4 }; dData.push_back( cDatum );
    cDatum = { 4, 2 }; dData.push_back( cDatum );
    cDatum = { 4, 5 }; dData.push_back( cDatum );
    cDatum = { 3, 0 }; dData.push_back( cDatum );
    cDatum = { 2, 1 }; dData.push_back( cDatum );
    cDatum = { 6, 3 }; dData.push_back( cDatum );
    cDatum = { 2, 4 }; dData.push_back( cDatum );
    cDatum = { 1, 5 }; dData.push_back( cDatum );
    cDatum = { 0, 6 }; dData.push_back( cDatum );
    cDatum = { 3, 3 }; dData.push_back( cDatum );
    cDatum = { 2, 6 }; dData.push_back( cDatum );
    cDatum = { 5, 1 }; dData.push_back( cDatum );
    cDatum = { 1, 2 }; dData.push_back( cDatum );
    cDatum = { 5, 5 }; dData.push_back( cDatum );
    cDatum = { 2, 5 }; dData.push_back( cDatum );
    cDatum = { 6, 5 }; dData.push_back( cDatum );
    cDatum = { 1, 4 }; dData.push_back( cDatum );
    cDatum = { 0, 4 }; dData.push_back( cDatum );
    cDatum = { 6, 4 }; dData.push_back( cDatum );
    cDatum = { 1, 1 }; dData.push_back( cDatum );
    cDatum = { 6, 1 }; dData.push_back( cDatum );
    cDatum = { 1, 0 }; dData.push_back( cDatum );
    cDatum = { 0, 5 }; dData.push_back( cDatum );
    cDatum = { 1, 6 }; dData.push_back( cDatum );
    cDatum = { 2, 0 }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        std::string sLeft, sRght;
        split_token_dlmtd( ",", sLine, sLeft, sRght );
        DatumType cDatum = { stoi( sLeft ), stoi( sRght ) };
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
    std::cout << iData.to_string();
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

// ------------ PATH FINDING STUFF HERE ---------------

typedef struct sNode {
    bool bObstacle = false;			// Is the node an obstruction?
    bool bVisited  = false;			// Have we searched this node before?
    float fGlobalGoal;				// Distance to goal so far
    float fLocalGoal;				// Distance to goal if we took the alternative route
    LocType pos;					// Nodes position in 2D space
    std::vector<sNode*> vecNeighbours;	// Connections to neighbours
    sNode* parent;					// Node connecting to this node that offers shortest parent
} NodeType;

NodeType *nodes     = nullptr;      // a pointer to the space

NodeType *nodeStart = nullptr;      // a pointer to start and end node in space
NodeType *nodeEnd   = nullptr;


void InitMap() {
    // set 2D space dimensions
    if (glbProgPhase != PUZZLE) {
        glbSpaceW =  7; glbSpaceH =  7;
    } else {
        glbSpaceW = 71; glbSpaceH = 71;
    }
    // Create a 2D array of nodes - this is for convenience of rendering and construction
    // and is not required for the algorithm to work - the nodes could be placed anywhere
    // in any space, in multiple dimensions...
    nodes = new NodeType[glbSpaceW * glbSpaceH];
    for (int y = 0; y < glbSpaceH; y++) {
        for (int x = 0; x < glbSpaceW; x++) {
            nodes[y * glbSpaceW + x].pos = { x, y }; // ...because we give each node its own coordinates
            nodes[y * glbSpaceW + x].bObstacle = false;
            nodes[y * glbSpaceW + x].bVisited  = false;
            nodes[y * glbSpaceW + x].parent = nullptr;
        }
    }

    // Create connections - in this case nodes are on a regular grid
    for (int y = 0; y < glbSpaceH; y++) {
        for (int x = 0; x < glbSpaceW; x++) {
            if(y > 0)
                nodes[y * glbSpaceW + x].vecNeighbours.push_back(&nodes[(y - 1) * glbSpaceW + (x + 0)]);
            if(y < glbSpaceH - 1)
                nodes[y * glbSpaceW + x].vecNeighbours.push_back(&nodes[(y + 1) * glbSpaceW + (x + 0)]);
            if (x > 0)
                nodes[y * glbSpaceW + x].vecNeighbours.push_back(&nodes[(y + 0) * glbSpaceW + (x - 1)]);
            if(x < glbSpaceW - 1)
                nodes[y * glbSpaceW + x].vecNeighbours.push_back(&nodes[(y + 0) * glbSpaceW + (x + 1)]);
        }
    }

    // Manually position the start and end markers so they are not nullptr
    nodeStart = &nodes[             0  * glbSpaceW +              0 ];
    nodeEnd   = &nodes[(glbSpaceH - 1) * glbSpaceW + (glbSpaceW - 1)];
}

// data structure to hold paths
typedef std::deque<NodeType*> PathType;


bool Solve_AStar() {

    // Reset Navigation Graph - default all node states
    for (int y = 0; y < glbSpaceH; y++) {
        for (int x = 0; x < glbSpaceW; x++) {
            nodes[y * glbSpaceW + x].bVisited = false;
            nodes[y * glbSpaceW + x].fGlobalGoal = FLT_MAX;  // INFINITY;
            nodes[y * glbSpaceW + x].fLocalGoal  = FLT_MAX;  // INFINITY;
            nodes[y * glbSpaceW + x].parent = nullptr;	// No parents
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
            for (auto nodeNeighbour : nodeCurrent->vecNeighbours) {
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

// Create a visual image to put on screen.
void PrintVisualSpace( const std::string &sMsg, PathType &p ) {
    // print message
    std::cout << sMsg << std::endl;
    // prepare and populate visual map
    std::vector<std::string> sMap;
    for (int y = 0; y < glbSpaceH; y++) {
        std::string sRow;
        for (int x = 0; x < glbSpaceW; x++) {
            if (nodes[y * glbSpaceW + x].bObstacle) {
                sRow.append( "#" );
            } else {
                sRow.append( "." );
            }
        }
        sMap.push_back( sRow );
    }
    // add path nodes to it
    for (int i = 0; i < (int)p.size(); i++) {
        sMap[p[i]->pos.y][p[i]->pos.x] = 'O';
    }
    // now output it
    for (int y = 0; y < (int)sMap.size(); y++) {
        std::cout << sMap[y] << std::endl;
    }
    std::cout << std::endl;
}

// After A* has run, get the shortest path from it. Note: if no path was found,
// an empty PathType container will be returned
PathType ExtractPath() {
    PathType result;
    NodeType *cachPtr = nullptr;
    for (NodeType *ptr = nodeEnd; ptr != nullptr; ptr = ptr->parent) {
        result.push_front( ptr );
        cachPtr = ptr;
    }
    if (cachPtr != nodeStart) {
        result.clear();
    }
    return result;
}

// ----- PART 2

/* Didn't need additional code for part 2, just adapted ExtractPath() a little to cope with non complete paths
 * and applied the existing functions in a slightly different way.
 */

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

    InitMap();
    PathType foundPath;

//    PrintVisualSpace( "after InitMap()", foundPath );

    // corrupt first n bytes from input stream
    int nBytesToCorrupt = (glbProgPhase != PUZZLE ? 12 : 1024);
    for (int i = 0; i < nBytesToCorrupt; i++) {
        LocType curPos = inputData[i];
        nodes[curPos.y * glbSpaceW + curPos.x].bObstacle = true;
    }

//    PrintVisualSpace( "after first 12 bytes were corrupted", foundPath );

    // run A* algo and extra shortest path afterwards
    Solve_AStar();
    foundPath = ExtractPath();

//    PrintVisualSpace( "after running A* algo", foundPath );

    std::cout << std::endl << "Answer to part 1: shortest path # steps = " << foundPath.size() - 1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here

    // corrupt first n bytes from input stream
    int nStartFromByte = (glbProgPhase != PUZZLE ? 12 : 1024);
    bool bFound = false;
    int nCacheIndex = -1;
    for (int i = nStartFromByte; !bFound; i++) {
        // corrupt the next byte from the input data
        LocType curPos = inputData[i];
        nodes[curPos.y * glbSpaceW + curPos.x].bObstacle = true;

        // run A* algo and extra shortest path afterwards
        Solve_AStar();
        foundPath = ExtractPath();

        // if no shortest path was found, terminate the loop and store the index
        if (foundPath.size() == 0) {
            bFound = true;
            nCacheIndex = i;
        }
    }

    std::cout << std::endl << "Answer to part 2: blocking location found at index = " << nCacheIndex << " being = " << inputData[nCacheIndex].to_string() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
