// AoC 2024 - Day 19 - Linen Layout
// ================================

// date:  2024-12-19
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day19"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <deque>
#include <map>

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

// the data consists of tuples (pairs) of numbers

// both the list of designs and the list of patterns are implemented using a datastream type
typedef std::string DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &patternData, DataStream &designData  ) {

    // hard code your examples here - use the same data for your test input file
    patternData.clear();
    // list of patterns
    patternData = { "r", "wr", "b", "g", "bwu", "rb", "gb", "br" };

    DatumType cDatum;
    // possible designs
    cDatum = "brwrr";  designData.push_back( cDatum );
    cDatum = "bggr";   designData.push_back( cDatum );
    cDatum = "gbbr";   designData.push_back( cDatum );
    cDatum = "rrbgbr"; designData.push_back( cDatum );
    cDatum = "ubwu";   designData.push_back( cDatum );
    cDatum = "bwurrg"; designData.push_back( cDatum );
    cDatum = "brgr";   designData.push_back( cDatum );
    cDatum = "bbrgwb"; designData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &patternData, DataStream &designData ) {

    std::ifstream dataFileStream( sFileName );
    patternData.clear();
    designData.clear();
    std::string sLine;

    // line with patterns
    getline( dataFileStream, sLine );
    while (sLine.length() > 0) {
        std::string sPattern = get_token_dlmtd( ", ", sLine );
        patternData.push_back( sPattern );
    }
    // empty line
    getline( dataFileStream, sLine );
    // multiple lines with designs
    while (getline( dataFileStream, sLine )) {
        designData.push_back( sLine );
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &patternData, DataStream &designData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, patternData, designData ); }
void GetData_PUZZLE( DataStream &patternData, DataStream &designData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, patternData, designData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintPatternDatum( DatumType &pDatum ) {
    // define your datum printing code here
    std::cout << pDatum << ", ";
}

// output to console for testing
void PrintPatternDataStream( DataStream &pData ) {
    for (auto &e : pData) {
        PrintPatternDatum( e );
    }
    std::cout << std::endl << std::endl;
}

void PrintDesignDatum( DatumType &dDatum ) {
    // define your datum printing code here
    std::cout << dDatum << std::endl;
}

// output to console for testing
void PrintDesignDataStream( DataStream &dData ) {
    for (auto &e : dData) {
        PrintDesignDatum( e );
    }
    std::cout << std::endl;
}

// ==========   PROGRAM PHASING

// populates input data, by calling the appropriate input function that is associated
// with the global program phase var
void GetInput( DataStream &pData, DataStream &dData, bool bDisplay = false ) {

    switch( glbProgPhase ) {
        case EXAMPLE: GetData_EXAMPLE( pData, dData ); break;
        case TEST:    GetData_TEST(    pData, dData ); break;
        case PUZZLE:  GetData_PUZZLE(  pData, dData ); break;
        default: std::cout << "ERROR: GetInput() --> unknown program phase: " << glbProgPhase << std::endl;
    }
    // display to console if so desired (for debugging)
    if (bDisplay) {
        PrintPatternDataStream( pData );
        PrintDesignDataStream(  dData );
    }
}

// ==========   PUZZLE SPECIFIC SOLUTIONS

// Note that sMainString is adapted if a match is found, so be sure to call it with a copy
bool StringMatch( std::string &sMainString, const std::string &sToMatch ) {
    bool bResult = false;
    // get a substring from sMainstring the same length as sToMatch
    int nLenToMatch = sToMatch.length();
    std::string sSubString = sMainString.substr( 0, nLenToMatch );
    // if that substring is identical, then there is a match
    if (sSubString == sToMatch) {
        bResult = true;

//        std::cout << "INFO: StringMatch() --> before - main string: " << sMainString << " string to match: " << sToMatch;
        // if there's a match, then shorten mainstring with that size
        std::string sIgnore = get_token_sized( nLenToMatch, sMainString );

//        std::cout << " after - main string: " << sMainString << std::endl;
    }

    return bResult;
}

// for memoization - store strings and known outcomes in a std::map
typedef std::pair<std::string, long long> OutcomeType;    // for part 1, a value of 0 means false, a value of 1 means true
typedef std::map< std::string, long long> OutcomesMap;

OutcomesMap mapOutcomes;

void PrintMemoization() {
    for (auto &e : mapOutcomes) {
        std::cout << "(sub)design: _" << e.first << "_ has " << (e.second ? "a " : "NO ") << "solution" << std::endl;
    }
    std::cout << std::endl;
}

bool AttemptOneDesign_recursive( const DataStream &patterns, const DatumType &design, bool bOutput = false ) {
    DatumType partOrg = design;
    bool bFoundFullMatch = false;

    // see if an outcome for this design string is already known
    OutcomesMap::iterator iter = mapOutcomes.find( partOrg );
    if (iter != mapOutcomes.end()) {
        // if it is known, use the result
        bFoundFullMatch = (*iter).second == 1;
        if (bOutput) {
            std::cout << "INFO: AttemptOneDesign_recursive() --> " << (bFoundFullMatch ? "full" : "NO") << " match " << std::endl;
        }
    } else {
        // if there's no outcome known yet...

        // attempt to match each of the patterns to the leading end of the design string
        // the flag helps to quit upon first full match found
        for (int i = 0; i < (int)patterns.size() && !bFoundFullMatch; i++) {

            // work on a copy of the original, because the string is adapted upon a match
            DatumType partCopy = partOrg;
            if (StringMatch( partCopy, patterns[i] )) {

                if (bOutput) {
                    std::cout << "INFO: AttemptOneDesign_recursive() --> remainder = " << partOrg << " splits into " << patterns[i] << " (index " << i << ") and " << partCopy << std::endl;
                }

                // recursively determine if a full match can be found
                bFoundFullMatch = AttemptOneDesign_recursive( patterns, partCopy, bOutput );
                // store that result for memoization
                mapOutcomes.insert( make_pair( partOrg, (bFoundFullMatch ? 1 : 0)));
            }
        }
    }
    return bFoundFullMatch;
}

bool AttemptOneDesign_iterative( DataStream &patterns, DatumType &design, bool bOutput = false ) {
    bool bFoundFullMatch = false;

    std::deque<std::string> dQueue;
    dQueue.push_back( design );

    while (!dQueue.empty() && !bFoundFullMatch) {
        // sort queue on length of string elements
        std::sort( dQueue.begin(), dQueue.end(),
            []( std::string &s1, std::string &s2 ){
                return s1.length() < s2.length();
            }
        );
        dQueue.erase( std::unique( dQueue.begin(), dQueue.end()), dQueue.end());

        // get front element (shortest element) of queue
        std::string sCur = dQueue.front();
        dQueue.pop_front();

        if (bOutput) {
            std::cout << "INFO: AttemptOneDesign_iterative() --> popped from queue: " << sCur << std::endl;
        }

        if (sCur.length() == 0) {
            bFoundFullMatch = true;
            if (bOutput) {
                std::cout << "INFO: AttemptOneDesign_iterative() --> full match :)" << std::endl;
            }
        } else {
            // attempt to match this string on any of the patterns
            for (int i = 0; i < (int)patterns.size(); i++) {
                std::string sPartCopy = sCur;
                if (StringMatch( sPartCopy, patterns[i] )) {
                    dQueue.push_back( sPartCopy );
                    if (bOutput) {
                        std::cout << "INFO: AttemptOneDesign_iterative() --> remainder = " << sCur << " splits into " << patterns[i] << " (index " << i << ") and " << sPartCopy << std::endl;
                    }
                }
            }
        }
    }
    return bFoundFullMatch;
}

int MatchAllDesigns( DataStream &patterns, DataStream &designs ) {

    mapOutcomes.clear();
    mapOutcomes.insert( make_pair( "", 1 ));   // the trivial solution: an empty string always matches
    for (auto &e : patterns) {
        mapOutcomes.insert( make_pair( e, 1 ));   // all patterns are solutions
    }

    int nCnt = 0;
    for (int i = 0; i < (int)designs.size(); i++) {
        if (AttemptOneDesign_recursive( patterns, designs[i], false )) {

//            std::cout << "(index: " << i << ") = design: _" << designs[i] <<  "_ DOES have a match" << std::endl;
            nCnt += 1;
        } else {
//            std::cout << "(index: " << i << ") = design: _" << designs[i] <<  "_ does NOT have a match" << std::endl;
        }
    }
    return nCnt;
}

// ----- PART 2

void PrintMemoization2() {
    for (auto &e : mapOutcomes) {
        std::cout << "(sub)design: _" << e.first << "_ has " << e.second << " solutions" << std::endl;
    }
    std::cout << std::endl;
}


long long AttemptOneDesign_recursive2( const DataStream &patterns, const DatumType &designTrail, bool bOutput = false ) {

    // make a copy of the parameter to prevent it being adapted
    DatumType partOrg = designTrail;
    long long llFoundMatches = 0;

    // see if an outcome for this design string is already known
    OutcomesMap::iterator iter = mapOutcomes.find( partOrg );
    if (iter != mapOutcomes.end()) {
        // if it is known, use the result
        llFoundMatches = (*iter).second;
        if (bOutput) {
            std::cout << "INFO: AttemptOneDesign_recursive2() --> " << llFoundMatches
                      << " match(es) found via memoization " << std::endl;
        }

    } else {
        // if there's no outcome known yet...

        // attempt to match each of the patterns to the leading end of the design string
        for (int i = 0; i < (int)patterns.size(); i++) {

            // work on a copy of the original, because the string is adapted upon a match
            DatumType partCopy = partOrg;
            if (StringMatch( partCopy, patterns[i] )) {

                if (bOutput) {
                    std::cout << "INFO: AttemptOneDesign_recursive2() --> remainder = " << partOrg
                              << " splits into "                                        << patterns[i]
                              << " (index " << i << ") and " << partCopy << std::endl;
                }

                // recursively determine if a full match can be found
                long long llLocalMatches = AttemptOneDesign_recursive2( patterns, partCopy, bOutput );
                // store that result for memoization
                mapOutcomes.insert( make_pair( partCopy, llLocalMatches ));

                llFoundMatches += llLocalMatches;
            }
        }
        mapOutcomes.insert( make_pair( partOrg, llFoundMatches ));

    }
    return llFoundMatches;
}


void ProcessPatterns( DataStream &patterns, bool bOutput = false ) {

    for (int i = 0; i < (int)patterns.size(); i++) {
        long long llNrMatches = AttemptOneDesign_recursive2( patterns, patterns[i], bOutput );
        mapOutcomes.insert( make_pair( patterns[i], llNrMatches ));
    }
}

long long MatchAllDesigns2( DataStream &patterns, DataStream &designs, bool bOutput = false ) {

    mapOutcomes.clear();
    mapOutcomes.insert( make_pair( "", 1 ));   // the trivial solution: an empty string always matches
    ProcessPatterns( patterns, bOutput );
    if (bOutput) {
        PrintMemoization2();
    }

    long long llCnt = 0;
    for (int i = 0; i < (int)designs.size(); i++) {
        long long localCount = AttemptOneDesign_recursive2( patterns, designs[i], bOutput );
        llCnt += localCount;
//        std::cout << "design with index: " << i << " = " << designs[i]
//                  << " yields " << localCount << " possibilities, total nr = " << llCnt << " (memo size: " << mapOutcomes.size() << ")" << std::endl;
    }
    return llCnt;
}

// ==========   MAIN()

int main()
{
    glbProgPhase = PUZZLE;     // program phase to EXAMPLE, TEST or PUZZLE
    std::cout << "Phase: " << ProgPhase2string() << std::endl << std::endl;
    flcTimer tmr;

/* ========== */   tmr.StartTiming();   // ============================================vvvvv

    // get input data, depending on the glbProgPhase (example, test, puzzle)
    DataStream patternData, designData;
    GetInput( patternData, designData, glbProgPhase != PUZZLE );
    DataStream part2PatternData = patternData;
    DataStream part2DesignData  = designData;
    std::cout << "Data stats - size of data streams: " << patternData.size() << " patterns and " << designData.size() << " designs" << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 0 - input data preparation: " );   // =========================^^^^^vvvvv

    // part 1 code here
    int nDesignsPossible = MatchAllDesigns( patternData, designData );
//    PrintMemoization();


    std::cout << std::endl << "Answer to part 1: number of designs possible = " << nDesignsPossible << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    patternData = part2PatternData; // get fresh untouched copy of input data
    designData  = part2DesignData;

    // part 2 code here
    long long nCombinationsPossible = MatchAllDesigns2( patternData, designData, false );

    std::cout << std::endl << "Answer to part 2: number of combinations possible = " << nCombinationsPossible << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
