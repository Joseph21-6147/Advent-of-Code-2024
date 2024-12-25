// AoC 2024 - Day 23 - LAN Party
// =============================

// date:  2024-12-23
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day23"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <map>
#include <set>
#include <deque>

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

// the data consists of tuples (pairs) of computers indicated with two character names
typedef struct {
    std::string c1, c2;
} DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = { "kh", "tc" }; dData.push_back( cDatum );
    cDatum = { "qp", "kh" }; dData.push_back( cDatum );
    cDatum = { "de", "cg" }; dData.push_back( cDatum );
    cDatum = { "ka", "co" }; dData.push_back( cDatum );
    cDatum = { "yn", "aq" }; dData.push_back( cDatum );
    cDatum = { "qp", "ub" }; dData.push_back( cDatum );
    cDatum = { "cg", "tb" }; dData.push_back( cDatum );
    cDatum = { "vc", "aq" }; dData.push_back( cDatum );
    cDatum = { "tb", "ka" }; dData.push_back( cDatum );
    cDatum = { "wh", "tc" }; dData.push_back( cDatum );
    cDatum = { "yn", "cg" }; dData.push_back( cDatum );
    cDatum = { "kh", "ub" }; dData.push_back( cDatum );
    cDatum = { "ta", "co" }; dData.push_back( cDatum );
    cDatum = { "de", "co" }; dData.push_back( cDatum );
    cDatum = { "tc", "td" }; dData.push_back( cDatum );
    cDatum = { "tb", "wq" }; dData.push_back( cDatum );
    cDatum = { "wh", "td" }; dData.push_back( cDatum );
    cDatum = { "ta", "ka" }; dData.push_back( cDatum );
    cDatum = { "td", "qp" }; dData.push_back( cDatum );
    cDatum = { "aq", "cg" }; dData.push_back( cDatum );
    cDatum = { "wq", "ub" }; dData.push_back( cDatum );
    cDatum = { "ub", "vc" }; dData.push_back( cDatum );
    cDatum = { "de", "ta" }; dData.push_back( cDatum );
    cDatum = { "wq", "aq" }; dData.push_back( cDatum );
    cDatum = { "wq", "vc" }; dData.push_back( cDatum );
    cDatum = { "wh", "yn" }; dData.push_back( cDatum );
    cDatum = { "ka", "de" }; dData.push_back( cDatum );
    cDatum = { "kh", "ta" }; dData.push_back( cDatum );
    cDatum = { "co", "tc" }; dData.push_back( cDatum );
    cDatum = { "wh", "qp" }; dData.push_back( cDatum );
    cDatum = { "tb", "vc" }; dData.push_back( cDatum );
    cDatum = { "td", "yn" }; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        std::string sC1, sC2;
        split_token_dlmtd( "-", sLine, sC1, sC2 );
        DatumType cDatum = { sC1, sC2 };
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
    std::cout << iData.c1 << " -- ";
    std::cout << iData.c2;
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

struct sGraphNode;  // forward declare

// a GraphType object is a std::map of graph nodes, searchable by their id string
typedef std::map<std::string, sGraphNode *> GraphType;

typedef struct sGraphNode {
    std::string sComputer;
    GraphType vNeighbours;    // set of neighbours is not a graph, but needs the same functionality
} GraphNodeType;

void PrintGraphNode( GraphNodeType *pNode ) {
    if (pNode == nullptr) {
        std::cout << "NULL" << std::endl;
    } else {
        std::cout << "ID = " << pNode->sComputer;
        std::cout << ", neighbours = " << pNode->vNeighbours.size() << ": " << std::endl;
        for (auto m : pNode->vNeighbours) {
            std::cout << m.first << ",";
        }
        std::cout << std::endl;
    }
}

GraphNodeType *NewGraphNode( std::string &sCompID ) {
    GraphNodeType *pRes = new GraphNodeType;
    pRes->sComputer = sCompID;
    return pRes;
}

void AddGraphNode( GraphType &g, GraphNodeType *pNode ) {
    if (pNode == nullptr) {
        std::cout << "ERROR: AddGraphNode() --> won't add nullptr arguments :(" << std::endl;
    } else {
        g.insert( std::make_pair( pNode->sComputer, pNode ));
    }
}
bool AreConnected( GraphNodeType *pNode1, GraphNodeType *pNode2 ) {
    if (pNode1 == nullptr || pNode2 == nullptr) {
        std::cout << "ERROR: AreConnected() --> can't judge nullptr arguments :(" << std::endl;
        return false;
    }
    return (pNode1->vNeighbours.find( pNode2->sComputer ) != pNode1->vNeighbours.end());
}

void AddConnection( GraphNodeType *pNode1, GraphNodeType *pNode2 ) {
    if (pNode1 == nullptr || pNode2 == nullptr) {
        std::cout << "ERROR: AddConnection() --> won't process nullptr arguments :(" << std::endl;
    } else {
        if (AreConnected( pNode1, pNode2 )) {
            std::cout << "WARNING: AddConnection() --> nodes were already connected" << std::endl;
        } else {
            // make bidirectional connection: if a is connected to b, then b is also connected to a
            AddGraphNode( pNode1->vNeighbours, pNode2 );
            AddGraphNode( pNode2->vNeighbours, pNode1 );
        }
    }
}

// use std::map functionality to find graph node with a specific ID
GraphNodeType *FindGraphNode( GraphType &g, std::string &sCompID ) {
    GraphNodeType *pRes = nullptr;
    GraphType::iterator it = g.find( sCompID );
    if (it != g.end()) {
        pRes = (*it).second;
    }
    return pRes;
}

void PrintGraph( GraphType &g, std::string sMsg = "" ) {
    std::cout << sMsg << std::endl;
    for (auto &e : g) {
        PrintGraphNode( e.second );
    }
}

void AssembleGraph( GraphType &g, DataStream &gData ) {
    g.clear();
    for (int i = 0; i < (int)gData.size(); i++) {

        std::string sComp1 = gData[i].c1;
        std::string sComp2 = gData[i].c2;
        // get a pointer to the graph node with id c1 - create it if needed
        GraphNodeType *p1 = FindGraphNode( g, sComp1 );
        if (p1 == nullptr) {
            // if the graph node didn't exist yet, create and insert it (while retaining the pointer)
            p1 = NewGraphNode( sComp1 );
            AddGraphNode( g, p1 );
        }

        // get a pointer to the graph node with id c2 - create it if needed
        GraphNodeType *p2 = FindGraphNode( g, sComp2 );
        if (p2 == nullptr) {
            // if the graph node didn't exist yet, create and insert it (while retaining the pointer)
            p2 = NewGraphNode( sComp2 );
            AddGraphNode( g, p2 );
        }
        // now add the connection between the two nodes
        AddConnection( p1, p2 );
    }
}

// aux datastructure for part 1

typedef struct sTripletStruct {
    std::string s1, s2, s3;

    std::string ToString() {
        return s1 + ", " + s2 + ", " + s3;
    }

    // needed for std::count()
    bool operator == (const sTripletStruct &rhs ) {
        return (this->s1 == rhs.s1 && this->s2 == rhs.s2 && this->s3 == rhs.s3);
    }

    // needed to apply in std::set<>
    bool operator < (const sTripletStruct &rhs ) const {
        return (
             this->s1 <  rhs.s1 ||
            (this->s1 == rhs.s1 && (this->s2  < rhs.s2 ||
                                   (this->s2 == rhs.s2 && this->s3 < rhs.s3)
                                   )
            )
        );
    }

    void Sort() {
        std::vector<std::string> vAux = { s1, s2, s3 };
        std::sort( vAux.begin(), vAux.end() );
        s1 = vAux[0], s2 = vAux[1], s3 = vAux[2];
    }

    bool AnyStartWithT() const {
        return (s1[0] == 't' || s2[0] == 't' || s3[0] == 't');
    }
} TripletType;

// solution for part 1 - gets all unique sets of 3 nodes that are interconnected
void FindSetsOfThree( GraphType &g, std::set<TripletType> &sResult ) {
    // iterate whole graph
    for (auto &e : g) {
        // select a node as the current node
        GraphNodeType *pCur = e.second;
        // iterate over it's neighbours, see if any of them has a neighbour that is
        // connected to the current node
        for (auto &curNghb : pCur->vNeighbours) {
            GraphNodeType *pNghb = curNghb.second;
            for (auto &nghbNghb : pNghb->vNeighbours) {
                GraphNodeType *pNghbNghb = nghbNghb.second;

                if (AreConnected( pCur, pNghbNghb )) {
                    // if found, create a new triplet, sort it and add it to the set
                    TripletType newTriplet = {
                        pCur->sComputer,
                        pNghb->sComputer,
                        pNghbNghb->sComputer
                    };
                    newTriplet.Sort();
                    sResult.insert( newTriplet );
                }
            }
        }
    }
}

// ----- PART 2

bool Contains( GraphType &neighbourSet, std::string &sID ) {
    return (neighbourSet.find( sID ) != neighbourSet.end());
}

// The problem is about "looking for the largest possible set where every node has all other nodes in that set
// in it's neighbour set". I found out that this is known in graph theory as the problem of finding the maximum
// clique (where a clique apparently is a fully connected subgraph in a larger graph).

// See: https://en.wikipedia.org/wiki/Clique_problem for explanation and details.

typedef std::vector<std::string> CliqueType;

// a graph node pNode is adjacent to the set curClique if all id's in curClique are also neighbours of pNode
bool IsAdjacentToSet( GraphNodeType *pNode, CliqueType &curClique ) {
    bool bValid = true;
    for (int i = 0; i < (int)curClique.size() && bValid; i++) {
        bValid = Contains( pNode->vNeighbours, curClique[i] );
    }
    return bValid;
}

// get largest possible clique from a specific node (strtNode)
CliqueType GetCliqueFromNode( GraphType &g, GraphNodeType *strtNode ) {
    CliqueType vResult;
    if (strtNode == nullptr) {
        // can't process unknown startNodes
        std::cout << "ERROR: GetCliqueFromNode() --> can't handle nullptr nodes" << std::endl;
    } else {
        // start with arbitrary clique, containing only the strtNode
        vResult.push_back( strtNode->sComputer );
        // grow current clique by looping over all *other* vertices in the graph
        for (auto e : g) {
            if (e.first != strtNode->sComputer) {
                if (IsAdjacentToSet( e.second, vResult )) {
                    vResult.push_back( e.first );
                }
            }
        }
    }
    // sort resulting clique before returning it
    std::sort( vResult.begin(), vResult.end() );
    return vResult;
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
    GraphType glbGraph;
    AssembleGraph( glbGraph, inputData );

//    PrintGraph( glbGraph, "Graph after assembly" );

    std::set<TripletType> sSetsOfThree;
    FindSetsOfThree( glbGraph, sSetsOfThree );

//    std::cout << "Sets of three before filtering: " << std::endl;
//    for (auto s : sSetsOfThree) {
//        std::cout << s.ToString() << std::endl;
//    }

    // filter all triplets without any t-starting computers in it
    std::set<TripletType> sAuxSet;
    for (auto &s : sSetsOfThree) {
        if (s.AnyStartWithT()) {
            sAuxSet.insert( s );
        }
    }
    sSetsOfThree.clear();
    sSetsOfThree = sAuxSet;

//    std::cout << "Sets of three after filtering: " << std::endl;
//    for (auto s : sSetsOfThree) {
//        std::cout << s.ToString() << std::endl;
//    }

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 1: nr of sets of three containing at least one t started id = " << sSetsOfThree.size() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here

    CliqueType vLongestClique;
    int nMaxLen = -1;
    for (auto n : glbGraph) {

        CliqueType curClique = GetCliqueFromNode( glbGraph, n.second );

        if (nMaxLen < (int)curClique.size()) {
            vLongestClique = curClique;
            nMaxLen = curClique.size();
        }

//        std::cout << "Node: ";
//        PrintGraphNode( n.second );
//        std::cout << " clique - size: " << curClique.size() << ", elements: ";
//        for (int i = 0; i < (int)curClique.size(); i++) {
//            std::cout << curClique[i] << ",";
//        }
//        std::cout << std::endl;
    }

    // assemble longest clique as a string to output the answer
    std::string sLongestClique;
    for (int i = 0; i < (int)vLongestClique.size(); i++) {
        sLongestClique.append( vLongestClique[i] );
        sLongestClique.append( "," );
    }
    sLongestClique = sLongestClique.substr( 0, sLongestClique.length() - 1 ); // strip off trailing ','

    std::cout << std::endl << "Answer to " << DAY_STRING << " part 2: largest sub graph with full connectivity = " << sLongestClique << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
