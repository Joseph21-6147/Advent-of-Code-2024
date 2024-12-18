// AoC 2024 - Day 12 - Garden Groups
// =================================

// date:  2024-12-12
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day12"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
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

// the data consists of a map modeled as a 2d field of characters
// which are grouped together in regions
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;

    cDatum = "AAAA"; dData.push_back( cDatum );
    cDatum = "BBCD"; dData.push_back( cDatum );
    cDatum = "BBCC"; dData.push_back( cDatum );
    cDatum = "EEEC"; dData.push_back( cDatum );
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
} LocType;

#define UP 0   // direction constants
#define RT 1
#define DN 2
#define LT 3

// a plot is a cell on the map having some characteristics
typedef struct sPlotStruct {
    int x, y;
    char c;
    bool bVisited = false;
    bool bFenced[4] = { false };  // use direction constants for indexing
} PlotType;
typedef std::vector<PlotType> PlotRow;
typedef std::vector<PlotRow>  PlotMap;

// print one cell of the map
void PrintPlot( PlotType &p ) {
    std::cout << "Location: (" << p.x << ", " << p.y << "), ID: " << p.c;
    std::cout << (p.bVisited ? " is visited" : " is NOT visited");
    std::cout << " and has bFenced values of: ";
    for (int i = 0; i < 4; i++) {
        std::cout << (p.bFenced[i] ? " TRUE  " : " FALSE " );
    }
    std::cout << std::endl;
}

// output the whole map
void PrintPlotStream( PlotMap &pm ) {
    for (int y = 0; y < (int)pm.size(); y++) {
        for (int x = 0; x < (int)pm[0].size(); x++) {
            PrintPlot( pm[y][x] );
        }
    }
    std::cout << std::endl;
}

// a region is a group of coherent plots that have the same identifying character and
// are 4-connected. It's area is just the size of the vLocs vector. For determining
// the perimeter a separate function is available
typedef struct sRegionStruct {
    char c;
    std::vector<LocType> vLocs;
    int perim = 0;
} RegionType;
typedef std::vector<RegionType> RegionStream;

int GetArea( RegionType &r ) { return r.vLocs.size(); }
int GetPerimeter( RegionType &r ) { return r.perim; }

// output one region
void PrintRegion( RegionType &r ) {
    std::cout << "Region ID = " << r.c << ", perimeter = " << r.perim;
    std::cout << " locations: ";
    for (int i = 0; i < (int)r.vLocs.size(); i++) {
        std::cout << r.vLocs[i].to_string() << ", ";
    }
    std::cout << " Area = " << GetArea( r );
    std::cout << " Perimeter = " << GetPerimeter( r );
    std::cout << " Price = " << GetArea( r ) * GetPerimeter( r );

    std::cout << std::endl;
}

// output all available regions
void PrintRegionStream( RegionStream &rs ) {
    for (auto &e : rs) {
        PrintRegion( e );
    }
    std::cout << std::endl;
}

// returns whether location (x, y) is within map boundaries
bool InBounds( PlotMap &pm, int x, int y ) {
    return x >= 0 && x < (int)pm[0].size() && y >= 0 && y < (int)pm.size();
}
// overloaded version for LocType
bool InBounds( PlotMap &pm, LocType &loc ) {
    return InBounds( pm, loc.x, loc.y );
}

// global offset variable for easy accessing 4-connected neighbours
std::vector<LocType> vGlbOffsets = {
    {  0, -1 },  // up, right, down, left
    { +1,  0 },
    {  0, +1 },
    { -1,  0 }
};

// Upon calling this function, (x, y) is the first unvisited plot of a newly encountered region
// The function figures out all cells of this new region, and puts them in an additional
// RegionStream entry using a flood fill approach. This is also why the PlotType has a bVisited flag.
void FindRegion( PlotMap &pm, int x, int y, RegionStream &rs ) {
    char curID = pm[y][x].c;
    RegionType newRegion;
    newRegion.c = curID;
    newRegion.perim = 0;  // perimeter is not known yet

    std::deque<LocType> locQueue;
    locQueue.push_back( { x, y } );

    while (!locQueue.empty()) {
        LocType curLoc = locQueue.front();
        locQueue.pop_front();

        // populate newRegion
        if (!pm[curLoc.y][curLoc.x].bVisited) {
            newRegion.vLocs.push_back( curLoc );
            // set plot to visited in plot map
            pm[curLoc.y][curLoc.x].bVisited = true;

            // attempt other directions
            for (int i = UP; i <= LT; i++) {
                LocType attemptLoc = { curLoc.x + vGlbOffsets[i].x, curLoc.y + vGlbOffsets[i].y };
                // it must be in bounds of map, not visited yet, same character and not already in the queue
                bool bPresent = (std::count( locQueue.begin(), locQueue.end(), attemptLoc ) > 0);
                if (
                    InBounds( pm, attemptLoc ) &&
                    !pm[attemptLoc.y][attemptLoc.x].bVisited &&
                    pm[attemptLoc.y][attemptLoc.x].c == curID &&
                    !bPresent
                ) {
                    locQueue.push_back( attemptLoc );
                }
            }
        }
    }
    rs.push_back( newRegion );
}

// populate plot map (pm) from input data (ds), and analyse all regions from it in rs
void InitPlotStream( DataStream &ds, PlotMap &pm, RegionStream &rs ) {

    // clear old contents (if any)
    for (int y = 0; y < (int)pm.size(); y++) {
        pm[y].clear();   // clear all rows of old pm contents
    }
    rs.clear();

    // create placeholders for all plots in the map, and fill it using input data
    for (int y = 0; y < (int)ds.size(); y++) {
        PlotRow newRow;
        for (int x = 0; x < (int)ds[0].length(); x++) {
            PlotType dummy = { x, y, ds[y][x], false, { false, false, false, false }};
            newRow.push_back( dummy );
        }
        pm.push_back( newRow );
    }

    // now extract all the regions using the plot map
    for (int y = 0; y < (int)pm.size(); y++) {
        for (int x = 0; x < (int)pm[0].size(); x++) {
            PlotType &curPlot = pm[y][x];
            if (!curPlot.bVisited) {
                // we found a new region, so figure out all it's plots
                FindRegion( pm, x, y, rs );
            }
        }
    }
}

// brute force approach - just sets and counts all the flags of bFenced for each of the cells
// of region r. The result is stored in the region r itself
void DeterminePerimeter( PlotMap &pm, RegionType &r ) {
    // iterate over all plots in region
    for (int i = 0; i < (int)r.vLocs.size(); i++) {
        LocType  &curLoc  = r.vLocs[i];
        PlotType &curPlot = pm[curLoc.y][curLoc.x];
        // attempt each direction
        for (int i = UP; i <= LT; i++) {
            LocType attemptLoc = { curLoc.x + vGlbOffsets[i].x, curLoc.y + vGlbOffsets[i].y };
            if (InBounds( pm, attemptLoc )) {
                PlotType &attemptPlot = pm[attemptLoc.y][attemptLoc.x];
                // there must be a fence if neighbour has different id
                curPlot.bFenced[i] = curPlot.c != attemptPlot.c;
            } else {
                // there must be a fence if there's no neighbour (map boundaries)
                curPlot.bFenced[i] = true;
            }
            if (curPlot.bFenced[i]) {
                r.perim += 1;
            }
        }
    }
}

// function populates all bFenced flags and perim values for each region in rs
void AnalyzePerimeters( PlotMap &pm, RegionStream &rs ) {
    for (int i = 0; i < (int)rs.size(); i++) {
        DeterminePerimeter( pm, rs[i] );
    }
}

// ----- PART 2

// function to determine number of external corners of a plot
// we only need to examine the bFenced flag of this plot
int GetExternalCorners( PlotMap &pm, int x, int y ) {
    int nCorners = 0;
    PlotType &curPlot = pm[y][x];
    if (curPlot.bFenced[UP] && curPlot.bFenced[RT]) nCorners += 1;
    if (curPlot.bFenced[RT] && curPlot.bFenced[DN]) nCorners += 1;
    if (curPlot.bFenced[DN] && curPlot.bFenced[LT]) nCorners += 1;
    if (curPlot.bFenced[LT] && curPlot.bFenced[UP]) nCorners += 1;
    return nCorners;
}

// function to determine number of internal corners of a plot
// note that neighbours bFenced values must be checked for this
// which also necessitates checking on map boundaries
int GetInternalCorners( PlotMap &pm, int x, int y ) {
    int nCorners = 0;
    PlotType &curPlot = pm[y][x];
    char curId = curPlot.c;
    LocType locUP = { curPlot.x + vGlbOffsets[UP].x, curPlot.y + vGlbOffsets[UP].y };
    LocType locRT = { curPlot.x + vGlbOffsets[RT].x, curPlot.y + vGlbOffsets[RT].y };
    LocType locDN = { curPlot.x + vGlbOffsets[DN].x, curPlot.y + vGlbOffsets[DN].y };
    LocType locLT = { curPlot.x + vGlbOffsets[LT].x, curPlot.y + vGlbOffsets[LT].y };
    // upper right external corner
    if (
        InBounds( pm, locUP ) && pm[locUP.y][locUP.x].c == curId && pm[locUP.y][locUP.x].bFenced[RT] &&
        InBounds( pm, locRT ) && pm[locRT.y][locRT.x].c == curId && pm[locRT.y][locRT.x].bFenced[UP]
    ) {
        nCorners += 1;
    }
    // lower right external corner
    if (
        InBounds( pm, locRT ) && pm[locRT.y][locRT.x].c == curId && pm[locRT.y][locRT.x].bFenced[DN] &&
        InBounds( pm, locDN ) && pm[locDN.y][locDN.x].c == curId && pm[locDN.y][locDN.x].bFenced[RT]
    ) {
        nCorners += 1;
    }
    // lower left external corner
    if (
        InBounds( pm, locDN ) && pm[locDN.y][locDN.x].c == curId && pm[locDN.y][locDN.x].bFenced[LT] &&
        InBounds( pm, locLT ) && pm[locLT.y][locLT.x].c == curId && pm[locLT.y][locLT.x].bFenced[DN]
    ) {
        nCorners += 1;
    }
    // upper left external corner
    if (
        InBounds( pm, locLT ) && pm[locLT.y][locLT.x].c == curId && pm[locLT.y][locLT.x].bFenced[UP] &&
        InBounds( pm, locUP ) && pm[locUP.y][locUP.x].c == curId && pm[locUP.y][locUP.x].bFenced[LT]
    ) {
        nCorners += 1;
    }
    return nCorners;
}

// function to count all corners of a cell
int GetNrCorners( PlotMap &pm, int x, int y ) {
    return GetExternalCorners( pm, x, y ) + GetInternalCorners( pm, x, y );
}

// function to count all corners of a region (the number of sides == number of corners)
int GetNrCorners( PlotMap &pm, RegionType &r ) {
    int nCorners = 0;
    for (int i = 0; i < (int)r.vLocs.size(); i++) {
        nCorners += GetNrCorners( pm, r.vLocs[i].x, r.vLocs[i].y );
    }
    return nCorners;
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

    PlotMap glbPM;
    RegionStream glbRS;
    InitPlotStream( inputData, glbPM, glbRS );

//    std::cout << "After InitPlotStream()" << std::endl;
//    PrintPlotStream( glbPM );
//    PrintRegionStream( glbRS );

    AnalyzePerimeters( glbPM, glbRS );

//    std::cout << "After AnalyzePerimeters()" << std::endl;
//    PrintPlotStream( glbPM );
//    PrintRegionStream( glbRS );

    int nTotalPrice1 = 0;
    for (int i = 0; i < (int)glbRS.size(); i++) {
        nTotalPrice1 += (GetArea( glbRS[i] ) * GetPerimeter( glbRS[i] ));
    }

    std::cout << std::endl << "Answer to part 1: total fencing price = " << nTotalPrice1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here

    int nTotalPrice2 = 0;
    for (int i = 0; i < (int)glbRS.size(); i++) {
        nTotalPrice2 += (GetArea( glbRS[i] ) * GetNrCorners( glbPM, glbRS[i] ));
    }

    std::cout << std::endl << "Answer to part 2: total fencing (discounted) price = " << nTotalPrice2 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
