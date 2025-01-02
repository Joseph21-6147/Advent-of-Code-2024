// AoC 2024 - Day 16 - Reindeer Maze
// =================================

// date:  2025-01-02
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day16"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <deque>
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

#define NO -1
#define UP  0   // direction flags
#define RT  1
#define DN  2
#define LT  3

// converts direction to mnemonic description in a string
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

// generic type for storing 2D locations
typedef struct sLocStruct {
    int x, y;

    // convenience function
    std::string to_string() {
        return "(" + std::to_string( x ) + ", " + std::to_string( y ) + ")";
    }

    // needed for std::count()
    bool operator == (const sLocStruct &rhs ) {
        return (this->x == rhs.x && this->y == rhs.y);
    }

    // needed to apply LocType in std::set<>
    bool operator < (const sLocStruct &rhs ) const {
        return (
             this->x <  rhs.x ||
            (this->x == rhs.x && this->y < rhs.y)
        );
    }

    // is this position within low-hgh boundaries?
    bool WithinBounds( const sLocStruct &low, const sLocStruct &hgh ) {
        return (this->x >= low.x && this->x < hgh.x && this->y >= low.y && this->y < hgh.y);
    }

    // returns the immediate neighbour location stepping from this location in the direction of dir
    sLocStruct Neighbour( int dir ) {
        if (dir < 0 || dir >= 4) {
            std::cout << "ERROR: LocType::Neighbour() --> dir parameter out of range: " << dir << std::endl;
        }

        std::vector<sLocStruct> dirOffsets = {
            {  0, -1 }, { +1,  0 }, {  0, +1 }, { -1,  0 },  // up, right, down, left
        };

        return { this->x + dirOffsets[dir].x, this->y + dirOffsets[dir].y };
    }

} LocType;


// the data consists of a 2d map modeled as a vector of strings
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

int glbMapX, glbMapY;  // dimensions of input data map

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = "###############"; dData.push_back( cDatum );   // first example, the other one is in the test file
    cDatum = "#.......#....E#"; dData.push_back( cDatum );
    cDatum = "#.#.###.#.###.#"; dData.push_back( cDatum );
    cDatum = "#.....#.#...#.#"; dData.push_back( cDatum );
    cDatum = "#.###.#####.#.#"; dData.push_back( cDatum );
    cDatum = "#.#.#.......#.#"; dData.push_back( cDatum );
    cDatum = "#.#.#####.###.#"; dData.push_back( cDatum );
    cDatum = "#...........#.#"; dData.push_back( cDatum );
    cDatum = "###.#.#####.#.#"; dData.push_back( cDatum );
    cDatum = "#...#.....#.#.#"; dData.push_back( cDatum );
    cDatum = "#.#.#.###.#.#.#"; dData.push_back( cDatum );
    cDatum = "#.....#...#.#.#"; dData.push_back( cDatum );
    cDatum = "#.###.#.#.#.#.#"; dData.push_back( cDatum );
    cDatum = "#S..#.....#...#"; dData.push_back( cDatum );
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
    std::cout << iData << std::endl;
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

#define MY_INFINITY 100000000   // got overflow problems using INT_MAX...

struct sGraphNodeStruct;     // forward declare, since edges need a pointer to graph node

// pointer and a weight
typedef struct sGraphEdgeStruct {
    int nWeight = -1;
    sGraphNodeStruct *pLink = nullptr;
} EdgeType;

// nNodeID equals the index of the node in the graph std::vector.
typedef struct sGraphNodeStruct {
    int nNodeID = -1;
    LocType pos = { -1, -1 };
    int dir = NO;
    std::vector<EdgeType> vNghbrs;
    bool bVisited;
    int nDistance;
    // below std::vector was just a single pointer for part 1
    std::vector<EdgeType> vParents;
} GraphNodeType;
typedef std::vector<GraphNodeType> GraphType;

void PrintNodeSummary( GraphNodeType *pNode, std::string sInsert ) {
    if (pNode == nullptr) {
        std::cout << "NULL";
    } else {
        std::cout << sInsert
                  << "ID = " << pNode->nNodeID << ", @: " << pNode->pos.to_string() << "-" << Dir2String( pNode->dir )
                  << ", distance: " << (pNode->nDistance == MY_INFINITY ? "INF" : std::to_string( pNode->nDistance ));
    }
}

void PrintGraphNode( GraphNodeType &node, std::string sInsert, bool bEndLine = true ) {
    std::cout << sInsert << "------------" << std::endl;

    PrintNodeSummary( &node, sInsert );

    std::cout << std::endl << sInsert << "# nghbrs: " << node.vNghbrs.size() << " [ ";
    for (int i = 0; i < (int)node.vNghbrs.size(); i++) {
        PrintNodeSummary( node.vNghbrs[i].pLink, "" );
        std::cout << "-weight=" << node.vNghbrs[i].nWeight;
        std::cout << ", ";
    }
    std::cout << "]" << std::endl << sInsert << "# parnts: " << node.vParents.size() << " [ ";
    for (int i = 0; i < (int)node.vParents.size(); i++) {
        PrintNodeSummary( node.vParents[i].pLink, "" );
        std::cout << "-weight=" << node.vParents[i].nWeight;
        std::cout << ", ";
    }
    std::cout << std::endl << sInsert << "------------" << std::endl;

    if (bEndLine) {
        std::cout << std::endl;
    }
}

void PrintGraphNode( GraphNodeType *pNode, std::string sInsert, bool bEndLine = true ) {
    if (pNode == nullptr) {
        std::cout << sInsert << "NULL";
        if (bEndLine) {
            std::cout << std::endl;
        }
    } else {
        PrintGraphNode( *pNode, sInsert, bEndLine );
    }
}

// returns the node index of the node identified by the (unique) combination of nodePos and nodeDir
int FindNodeIndex( GraphType &g, LocType &nodePos, int nodeDir ) {
    int nFoundIx = -1;
    for (int i = 0; i < (int)g.size() && nFoundIx == -1; i++) {
        if (g[i].pos == nodePos && g[i].dir == nodeDir) {
            nFoundIx = i;
        }
    }
    return nFoundIx;
}

// ppStrtNode is a pointer to a pointer, to enable changing the pointer from within the function
// for the stop node a location is returned, not a pointer
void CreateGraphFromInputData( DataStream &iData, GraphType &g, GraphNodeType **ppStrtNode, LocType &stopNodePos ) {

    // clear result parameters
    *ppStrtNode = nullptr;
    g.clear();
    // get dimensions of the input data map
    glbMapY = (int)iData.size();
    glbMapX = (int)iData[0].length();

    LocType strtPos, stopPos;

    // first create all graph nodes, resulting in a stable graph std::vector
    int nNodeCnt = 0;
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            // process only usable cells as graph nodes
            if (iData[y][x] != '#') {
                LocType curPos = { x, y };
                // NOTE: for each map cell, create 4 graph nodes!
                for (int dir = UP; dir <= LT; dir++) {
                    GraphNodeType newNode;
                    newNode.nNodeID   = nNodeCnt;
                    newNode.pos       = curPos;
                    newNode.dir       = dir;
                    newNode.bVisited  = false;
                    newNode.nDistance = 0;
                    newNode.vNghbrs.reserve( 4 );
                    newNode.vParents.reserve( 4 );

                    g.push_back( newNode );
                    nNodeCnt += 1;
                }
                // collect start or stop locations when encountered
                if (iData[y][x] == 'S') { strtPos = curPos; }
                if (iData[y][x] == 'E') { stopPos = curPos; }
            }
        }
    }
    // all nodes that represent the graph are now in the std::vector, so it can be considered "stable", and
    // neighbour pointers can be created to its elements.
    for (int y = 0; y < glbMapY; y++) {
        for (int x = 0; x < glbMapX; x++) {
            // only process map cells that exist as an empty cell
            if (iData[y][x] != '#') {
                // get indices and pointers to each of the four graph nodes that represent this cell
                LocType curPos = { x, y };
                int nIndices[4];
                GraphNodeType *pNode[4];
                for (int d = UP; d <= LT; d++) {
                    nIndices[d] = FindNodeIndex( g, curPos, d );
                    pNode[d] = &(g[nIndices[d]]);
                }
                for (int d = UP; d <= LT; d++) {
                    // having pointers to each of the four graph nodes representing this cell, connect them
                    int nPrev = (d - 1 + 4) % 4;
                    int nNext = (d + 1    ) % 4;
                    pNode[d]->vNghbrs.push_back( { 1000, pNode[nPrev] } );
                    pNode[d]->vNghbrs.push_back( { 1000, pNode[nNext] } );

                    // also create edges with appropriate weight with neighbour graph nodes outside this cell
                    LocType nghbPos = curPos.Neighbour( d );
                    if (nghbPos.WithinBounds( { 0, 0 }, { glbMapX, glbMapY } ) && (iData[nghbPos.y][nghbPos.x] != '#')) {
                        int nNghbIx = FindNodeIndex( g, nghbPos, d );
                        GraphNodeType *pNghbNode = &(g[nNghbIx]);
                        pNode[d]->vNghbrs.push_back( { 1, pNghbNode } );
                    }
                }
            }
        }
    }

    // set pointer to start node in graph
    int nStrtIx = FindNodeIndex( g, strtPos, RT );
    if (nStrtIx < 0) {
        std::cout << "ERROR: CreateGraphFromInputData() --> could not find start node" << std::endl;
    } else {
        *ppStrtNode = &(g[nStrtIx]);
    }
    // set end node position
    stopNodePos = stopPos;
}

// get cardinal direction stepping from location fm to location to
int GetDirection( LocType &fm, LocType &to ) {
    int nResult = NO;
    if (fm.x == to.x) {       // no horizontal movement
        if (fm.y == to.y) {
            nResult = NO;     // stand still (should not occur)
        } else if (fm.y < to.y) {
            nResult = DN;     // step down
        } else {
            nResult = UP;     // step up
        }
    } else if (fm.x < to.x) { // step to right
        if (fm.y == to.y) {
            nResult = RT;
        } else {
            nResult = NO;     // diagonal movement (should not occur)
        }
    } else {                  // step to left
        if (fm.y == to.y) {
            nResult = LT;
        } else {
            nResult = NO;     // diagonal movement (should not occur)
        }
    }
    return nResult;
}

// used by Dijkstra's algorithm:
// if the newly calculated distance value to pNghbNode trhough pCurNode is less then or equal to the old distance value of pNghbNode,
// replace it (or add it as an alternative short path)
void UpdateNeighbourNode( GraphNodeType *pCurNode, GraphNodeType *pNghbNode, int nNewDistVal, int nLinkWeight ) {
    if (nNewDistVal == pNghbNode->nDistance) {
        // distance values are equal: add another parent to the parents vector
        pNghbNode->vParents.push_back( { nLinkWeight, pCurNode } );
    } else if (nNewDistVal < pNghbNode->nDistance) {
        // new distance value is less than old one: clear parents vector and insert new parent, and
        // also update distance value of neighbour
        pNghbNode->vParents.clear();
        pNghbNode->vParents.push_back( { nLinkWeight, pCurNode } );
        pNghbNode->nDistance = nNewDistVal;
    } else {
        // error
        std::cout << "ERROR: UpdateNeighbourNode() --> unexpected situation" << std::endl;
    }
}

// Implementation of Dijkstra's shortest path algorithm
// See for instance: https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
void AnalyseShortestPath1( GraphType &g, GraphNodeType *pStrt, LocType &stopPos ) {

    std::deque<GraphNodeType *> localQ;    // internal queue
    // reset graph stats
    for (int i = 0; i < (int)g.size(); i++) {
        GraphNodeType &curNode = g[i];
        curNode.bVisited  = false;
        curNode.nDistance = MY_INFINITY;
        curNode.vParents.clear();
        // enter pointers to all nodes in the queue as input for the Dijkstra algo
        localQ.push_back( &(g[i]) );
    }

    // set distance of start node to 0, for sorting the queue
    pStrt->nDistance = 0;
    // process the nodes in the queue until it is empty
    while (!localQ.empty()) {

        // keep queue sorted on distance value
        std::sort( localQ.begin(), localQ.end(), [=]( GraphNodeType *a, GraphNodeType *b ) {
                return a->nDistance < b->nDistance;
            }
        );

        // get next node to process
        GraphNodeType *pCurNode = localQ.front();
        localQ.pop_front();
        pCurNode->bVisited = true;
        // analyze the neighbours of the current node
        for (int i = 0; i < (int)pCurNode->vNghbrs.size(); i++) {

            // for convenient addressing
            EdgeType &edgeToNghb     = pCurNode->vNghbrs[i];
            GraphNodeType *pNghbNode = edgeToNghb.pLink;
            int nWeightToNghb        = edgeToNghb.nWeight;

            if (!pNghbNode->bVisited) {

                // current distance of neighbour
                int nNghbDistance = pNghbNode->nDistance;
                // possible shorter distance of neighbour via current node
                int nAltDistance = pCurNode->nDistance + nWeightToNghb;
                // update neighbour's distance and parent values if new distance is shorter or the same
                if (nAltDistance <= nNghbDistance) {
                    UpdateNeighbourNode( pCurNode, pNghbNode, nAltDistance, nWeightToNghb );
                }
            }
        }
    }
}

// a path is just a sequence of pointers to graph nodes
typedef std::deque<GraphNodeType *> PathType;


// extracts a shortest path from the graph after it was processed with Dijkstra's
PathType ExtractPath1( GraphType &g, GraphNodeType *pStrt, LocType &stopNodePos ) {
    PathType dResult;

    // We know the end node location, but not which of the four associated graph nodes actually is
    // the end node for the path. Find that first by looking at the smallest distance value
    int nMinValue = MY_INFINITY;
    int nMinIndex = -1;
    for (int d = UP; d <= LT; d++) {
        int nLocalIndex = FindNodeIndex( g, stopNodePos, d );
        int nLocalDist = g[nLocalIndex].nDistance;
        if (nLocalDist < nMinValue) {
            nMinValue = nLocalDist;
            nMinIndex = nLocalIndex;
        }
    }
    // now we can get a graph node pointer from the min index value
    GraphNodeType *pStop = &(g[nMinIndex]);
    // start at the last node and work your way backwards, following the parent trail of pointers
    GraphNodeType *pCur = pStop;

    while (pCur != nullptr) {

        dResult.push_front( pCur );
        if (pCur->vParents.empty()) {
            pCur = nullptr;
        } else {
            pCur = pCur->vParents[0].pLink;
        }
    }

    return dResult;
}

// outputs the graph info as text data
void PrintDataGraph1( GraphType &g ) {
    for (int i = 0; i < (int)g.size(); i++) {
        std::cout << "Index: " << i << ": ";
        PrintGraphNode( &(g[i]), "" );
    }
}

// outputs the graph info in a visual way. The path p is output along the direction / orientation of the nodes
// that are in p
void PrintVisualGraph1( GraphType &g, GraphNodeType *pStrt, LocType &posStop, PathType &p ) {
    // create a printable map and init it with all blocks
    DataStream printMap;
    for (int y = 0; y < glbMapY; y++) {
        std::string sRow;
        for (int x = 0; x < glbMapX; x++) {
            sRow.append( "#" );
        }
        printMap.push_back( sRow );
    }
    // put empty cells in it
    for (int i = 0; i < (int)g.size(); i++) {
        GraphNodeType curNode = g[i];
        LocType curLoc = curNode.pos;
        printMap[curLoc.y][curLoc.x] = '.';
    }
    // put path steps in it
    for (int i = 0; i < (int)p.size(); i++) {
        LocType &curLoc = p[i]->pos;
        char cFill = 'O';
        switch (p[i]->dir) {
            case UP: cFill = '^'; break;
            case RT: cFill = '>'; break;
            case DN: cFill = 'v'; break;
            case LT: cFill = '<'; break;
        }
        printMap[curLoc.y][curLoc.x] = cFill;
    }
    // put start and end positions in it
    LocType strtLoc = pStrt->pos;
    LocType stopLoc = posStop;
    printMap[strtLoc.y][strtLoc.x] = 'S';
    printMap[stopLoc.y][stopLoc.x] = 'E';
    // output it to screen
    PrintDataStream( printMap );
}

// for part 1, get the path score by accumulating the weights along the way of the path
int GetPathScore( PathType &p ) {
    int nResult = 0;
    for (int i = 0; i < (int)p.size(); i++) {
        // must be solved more sophisticated than this
        if (!p[i]->vParents.empty() &&  p[i]->vParents[0].pLink != nullptr) {
            nResult += p[i]->vParents[0].nWeight;
        }
    }
    return nResult;
}

// ----- PART 2

// for part 2 - back trace the graph, starting at the end node (pStop) and continuing until the start node
// (pStrt). Collect all path positions in a std::set along the way. Use a recursive approach that can also be
// used to deal with multiple shortest paths
std::set<LocType> BacktrackShortestPaths( GraphType &g, GraphNodeType *pStop, GraphNodeType *pStrt ) {
    std::set<LocType> resultSet;
    if (pStop != nullptr) {
        resultSet.insert( pStop->pos );
        if (pStop != pStrt) {

            // if there's only one parent available ...
            if (pStop->vParents.size() == 1) {
                // ... just follow the one parent that is available
                std::set<LocType> tmpSet = BacktrackShortestPaths( g, pStop->vParents[0].pLink, pStrt );
                // add the result to the resultSet
                for (auto e : tmpSet) {
                    resultSet.insert( e );
                }
            } else if (pStop->vParents.size() > 1) {
                // If multiple parents exist, first select which ones have the same (shortest) path length.
                // Each parent node is on one of the shortest paths if the distance of that parent node plus the weight
                // to that parent node equals the distance of the current node!
                std::vector<GraphNodeType *> vCandidates;
                int nCurDistance = pStop->nDistance;
                for (int i = 0; i < (int)pStop->vParents.size(); i++) {
                    int nParentDistance = pStop->vParents[i].pLink->nDistance;
                    int nParentWeight   = pStop->vParents[i].nWeight;
                    if (nParentDistance + nParentWeight == nCurDistance) {
                        vCandidates.push_back( pStop->vParents[i].pLink );
                    }
                }
                // having filtered the candidates, process them
                for (int i = 0; i < (int)vCandidates.size(); i++) {
                    std::set<LocType> tmpSet = BacktrackShortestPaths( g, vCandidates[i], pStrt );
                    // add the result to the resultSet
                    for (auto e : tmpSet) {
                        resultSet.insert( e );
                    }
                }
            }
        }
    }
    return resultSet;
}

// outputs the graph info in a visual way. The set of locations locs is plotted into the image
void PrintVisualGraph2( GraphType &g, std::set<LocType> &locs ) {
    // create a printable map and init it with all blocks
    DataStream printMap;
    for (int y = 0; y < glbMapY; y++) {
        std::string sRow;
        for (int x = 0; x < glbMapX; x++) {
            sRow.append( "#" );
        }
        printMap.push_back( sRow );
    }
    // put empty cells in it
    for (int i = 0; i < (int)g.size(); i++) {
        GraphNodeType curNode = g[i];
        LocType curLoc = curNode.pos;
        printMap[curLoc.y][curLoc.x] = '.';
    }
    // put path locations in it
    for (auto e : locs) {
        printMap[e.y][e.x] = 'o';
    }
    // output it to screen
    PrintDataStream( printMap );
}

// ==========   MAIN()

int main()
{
    glbProgPhase = TEST;     // program phase to EXAMPLE, TEST or PUZZLE
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

    // put input data into a workable data structure and get pointer to start node and location to end node
    GraphType graph;
    GraphNodeType *pSrc;
    LocType posDst;
    CreateGraphFromInputData( inputData, graph, &pSrc, posDst );

    // call the function to do Dijkstra's shortest path finding
    PathType dShortestPath;

//    std::cout << "Graph after graph creation ..." << std::endl;
//    PrintVisualGraph1( graph, pSrc, posDst, dShortestPath );

    AnalyseShortestPath1( graph, pSrc, posDst );
    // extract the shortest path from the graph
    dShortestPath = ExtractPath1( graph, pSrc, posDst );

//    std::cout << "Graph after shortest path analysis ..." << std::endl;
//    PrintVisualGraph1( graph, pSrc, posDst, dShortestPath );

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1:  = " << GetPathScore( dShortestPath ) << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    // first find which of the subnodes of the destination map cell is the right one to start backtracking
    int nMinValue = MY_INFINITY;
    int nMinIndex = -1;
    for (int d = UP; d <= LT; d++) {
        int nDestIndex = FindNodeIndex( graph, posDst, d );
        if (graph[nDestIndex].nDistance < nMinValue) {
            nMinValue = graph[nDestIndex].nDistance;
            nMinIndex = d;
        }
    }
    int nBackTrackStartIx = FindNodeIndex( graph, posDst, nMinIndex );
    GraphNodeType *pDst = &(graph[nBackTrackStartIx]);

    // start backtracking, collecting shortest path locations in pathPositions
    std::set<LocType> pathPositions = BacktrackShortestPaths( graph, pDst, pSrc );

//    // print the result of the backtracking
//    PrintVisualGraph2( graph, pathPositions );

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: = " << pathPositions.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
