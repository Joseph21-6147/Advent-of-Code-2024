// AoC 2024 - Day 11 - Plutonian Pebbles
// =====================================

// date:  2024-12-11
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day11"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <vector>
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

// the data consists of a number of numbers
typedef long long DatumType;
typedef std::vector<DatumType> DataStream;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file
    DatumType cDatum;
    cDatum = 125; dData.push_back( cDatum );
    cDatum =  17; dData.push_back( cDatum );
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine;
    while (getline( dataFileStream, sLine )) {
        while (sLine.length() > 0) {
            std::string sNumber = get_token_dlmtd( " ", sLine );
            vData.push_back( stoll( sNumber ));
        }
    }
    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDatum( DatumType &iData ) {
    // define your datum printing code here
    std::cout << iData << ", ";
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

// an entry contains the value and the number of times it occurs
typedef std::pair<long long, long long> EntryType;
typedef std::map< long long, long long> EntryMap;
typedef EntryMap::iterator              EntryIter;

void PrintEntry( EntryIter e ) {
    std::cout << "(v: " << (*e).first << ", c: " << (*e).second << "), " << std::endl;
}

void PrintMap( EntryMap &em, std::string sMsg ) {
    std::cout << sMsg << std::endl;
    for (EntryIter iter = em.begin(); iter != em.end(); iter++) {
        PrintEntry( iter );
    }
    std::cout << std::endl;
}

// global var's to prevent parameter passing
EntryMap mapZero, mapEven, mapOthr;

#define UNKN  -1      // type constants
#define ZERO   0
#define EVEN   1
#define OTHR   2

// returns true if value has an even number of digits
bool MustSplit( long long value ) {
    return std::to_string( value ).length() % 2 == 0;
}

// returns one of the type constants depending on value
int GetType( long long value ) {
    int nResult = UNKN;
    if (value == 0) {
        nResult = ZERO;
    } else if (MustSplit( value )) {
        nResult = EVEN;
    } else {
        nResult = OTHR;
    }
    return nResult;
}

// convenience getter functions
long long GetValue(  EntryType mapEntry ) { return mapEntry.first;  }
long long GetCopies( EntryType mapEntry ) { return mapEntry.second; }

// inits the three global maps from the input data. The data is categorized and put
// in the appropriate map as is. Since all data are unique, all entries get copies = 1
void InitMaps( DataStream &dData ) {
    mapZero.clear();
    mapEven.clear();
    mapOthr.clear();

    for (int i = 0; i < (int)dData.size(); i++) {
        switch (GetType( dData[i] )) {
            case ZERO: mapZero.insert( make_pair( dData[i], 1 )); break;
            case EVEN: mapEven.insert( make_pair( dData[i], 1 )); break;
            case OTHR: mapOthr.insert( make_pair( dData[i], 1 )); break;
            default  : std::cout << "ERROR - InitMaps() --> unknown type encountered: " << GetType( dData[i] ) << std::endl;
        }
    }
}

// looks for value in eMap. Returns eMap.end() if not found, iterator to entry otherwise
EntryIter FindEntry( EntryMap &eMap, long long value ) {
    EntryIter result = eMap.find( value );
    return result;
}

// if the entry is already available in eMap, then the copies field is updated
// otherwise it is inserted as additional entry
void MapInsert( EntryMap &eMap, EntryType &entry ) {
    EntryIter searchEntry = FindEntry( eMap, GetValue( entry ));
    if (searchEntry == eMap.end()) {
        eMap.insert( make_pair( entry.first, entry.second ));
    } else {
        (*searchEntry).second += GetCopies( entry );
    }
}

// performs transformation step for ZERO type entries
// transformed entries are always put in OTHR map (since outcome value is always 1)
void OneZeroBlink( EntryType entry ) {
    // create new entry with value 1 and ...
    EntryType newEntry = make_pair( 1, GetCopies( entry ));
    // insert it into other type map
    MapInsert( mapOthr, newEntry );
}

// performs transformation step for OTHR type entries
// transformed entries can't be of type ZERO, but are inserted in type EVEN or OTHR maps,
// depending on the transformed value
void OneOthrBlink( EntryType entry ) {
    // create new entry with value * 2024 and ...
    long long newValue = GetValue( entry ) * 2024;
    EntryType newEntry = make_pair( newValue, GetCopies( entry ));
    if (MustSplit( newValue )) {
        MapInsert( mapEven, newEntry );
    } else {
        MapInsert( mapOthr, newEntry );
    }
}

// performs transformation step for EVEN type entries
// Two new entries result from this transformation. They are independently analyzed for their type
// and put in the appropriate map
void OneEvenBlink( EntryType entry ) {
    std::string sValue = std::to_string( GetValue( entry ));
    int nHalfLen = sValue.length() / 2;
    std::string sLeft = sValue.substr(        0, nHalfLen );
    std::string sRgth = sValue.substr( nHalfLen, nHalfLen );
    long long llLeftVal = stoll( sLeft );
    long long llRghtVal = stoll( sRgth );

    EntryType newLeftEntry = make_pair( llLeftVal, GetCopies( entry ));
    if (llLeftVal == 0) {
        MapInsert( mapZero, newLeftEntry );
    } else if (MustSplit( llLeftVal )) {
        MapInsert( mapEven, newLeftEntry );
    } else {
        MapInsert( mapOthr, newLeftEntry );
    }

    EntryType newRghtEntry = make_pair( llRghtVal, GetCopies( entry ));
    if (llRghtVal == 0) {
        MapInsert( mapZero, newRghtEntry );
    } else if (MustSplit( llRghtVal )) {
        MapInsert( mapEven, newRghtEntry );
    } else {
        MapInsert( mapOthr, newRghtEntry );
    }
}

// performs one transformation step on all of the entries in each three maps
void OneBlink() {
    // make copies of the global maps
    EntryMap cpyZero = mapZero; mapZero.clear();
    EntryMap cpyEven = mapEven; mapEven.clear();
    EntryMap cpyOthr = mapOthr; mapOthr.clear();
    // now process data from the copies and put result into the global maps
    for (EntryIter iter = cpyZero.begin(); iter != cpyZero.end(); iter++) {
        OneZeroBlink( *iter );
    }
    for (EntryIter iter = cpyOthr.begin(); iter != cpyOthr.end(); iter++) {
        OneOthrBlink( *iter );
    }
    for (EntryIter iter = cpyEven.begin(); iter != cpyEven.end(); iter++) {
        OneEvenBlink( *iter );
    }
}

// cumulates all occurrences for all elements for each of the three maps to get a
// total nr of stones
long long GetNrStones() {
    long long nResult = 0;
    for (EntryIter iter = mapZero.begin(); iter != mapZero.end(); iter++) {
        nResult += GetCopies( *iter );
    }
    for (EntryIter iter = mapOthr.begin(); iter != mapOthr.end(); iter++) {
        nResult += GetCopies( *iter );
    }
    for (EntryIter iter = mapEven.begin(); iter != mapEven.end(); iter++) {
        nResult += GetCopies( *iter );
    }
    return nResult;
}

// ----- PART 2

// No new code was needed, I only had to rewrite the copies var from int to long long since
// it overflowed for 75 blinks


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

    // fill global maps from input data
    InitMaps( inputData );
//    PrintMap( mapZero, "ZERO map - after init" );
//    PrintMap( mapEven, "EVEN map - after init" );
//    PrintMap( mapOthr, "OTHR map - after init" );

    for (int i = 1; i <= 25; i++) {
        OneBlink();
//        std::cout << "After " << i << " blinks ... ";
//        if (i <= 6) {
//            PrintMap( mapZero, "ZERO map" );
//            PrintMap( mapEven, "EVEN map" );
//            PrintMap( mapOthr, "OTHR map" );
//        }
//        std::cout << "nr of stones = " << GetNrStones() << std::endl;
    }

    std::cout << std::endl << "Answer to part 1: nr of stones after 25 blinks = " << GetNrStones() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    // part 2 code here
    for (int i = 26; i <= 75; i++) {
        OneBlink();
//        std::cout << "After " << i << " blinks ... ";
//        std::cout << "nr of stones = " << GetNrStones() << std::endl;
    }

    std::cout << std::endl << "Answer to part 2: nr of stones after 75 blinks = " << GetNrStones() << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
