// AoC 2024 - Day 17 - Chronospatial Computer
// ==========================================

// date:  2025-01-03
// by:    Joseph21 (Joseph21-6147)

#define DAY_STRING  "day17"

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

// the data (the "program" part of it) consists of numbers
typedef long long DatumType;
typedef std::vector<DatumType> DataStream;

// this day is about a simple processor emulation, having 3 registers and a program counter
long long regA, regB, regC;
int PC = 0;

// ==========   INPUT DATA FUNCTIONS

// hardcoded input - just to get the solution tested
void GetData_EXAMPLE( DataStream &dData ) {

    // hard code your examples here - use the same data for your test input file

//    regA = 0; regB = 0; regC = 9;   // small example 1
//    dData = { 2, 6 };

//    regA = 10; regB = 0; regC = 9;   // small example 2
//    dData = { 5, 0, 5, 1, 5, 4 };

//    regA = 2024; regB = 0; regC = 0;   // small example 3
//    dData = { 0, 1, 5, 4, 3, 0 };

//    regA = 0; regB = 29; regC = 0;   // small example 4
//    dData = { 1, 7 };

//    regA = 0; regB = 2024; regC = 43690;   // small example 5
//    dData = { 4, 0 };

//    regA = 729; regB = 0; regC = 0;   // part 1 example like test file
//    dData = { 0, 1, 5, 4, 3, 0 };

    regA = 117440; regB = 0; regC = 0;   // part 2 example like test file
    dData = { 0, 3, 5, 4, 3, 0 };
}

// file input - this function reads text file content one line at a time - adapt code to match your need for line parsing!
void ReadInputData( const std::string sFileName, DataStream &vData ) {

    std::ifstream dataFileStream( sFileName );
    vData.clear();
    std::string sLine, sIgnore;

    // get register data, three similar lines followed by an empty line
    getline( dataFileStream, sLine );
    sIgnore = get_token_dlmtd( "Register A: ", sLine );
    regA = stoi( sLine );
    getline( dataFileStream, sLine );
    sIgnore = get_token_dlmtd( "Register B: ", sLine );
    regB = stoi( sLine );
    getline( dataFileStream, sLine );
    sIgnore = get_token_dlmtd( "Register C: ", sLine );
    regC = stoi( sLine );

    getline( dataFileStream, sLine );   // parse/remove empty line between register block and program data

    // get program data, all numbers are in one line, separated by commas
    getline( dataFileStream, sLine );
    sIgnore = get_token_dlmtd( "Program: ", sLine );
    while (sLine.length() > 0) {
        std::string sNr = get_token_dlmtd( ",", sLine );
        vData.push_back( stoi( sNr ));
    }

    dataFileStream.close();
}

void GetData_TEST(   DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.test.txt"   ); ReadInputData( sInputFile, dData ); }
void GetData_PUZZLE( DataStream &dData ) { std::string sInputFile = DAY_STRING; sInputFile.append( ".input.puzzle.txt" ); ReadInputData( sInputFile, dData ); }

// ==========   CONSOLE OUTPUT FUNCTIONS

// output to console for testing
void PrintDataStream( DataStream &dData ) {
    std::cout << "Reg A : " << regA << std::endl;
    std::cout << "Reg B : " << regB << std::endl;
    std::cout << "Reg C : " << regC << std::endl;
    std::cout << "PC    : " << PC   << std::endl;
    std::cout << "Prog  : ";
    for (int i = 0; i < (int)dData.size(); i++) {
        std::cout << dData[i] << ",";
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

// converts combo operands to their integer values
long long Combo2Number( long long nCombo ) {
    long long llResult = -1;
    switch (nCombo) {
        case 0 :
        case 1 :
        case 2 :
        case 3 : llResult = nCombo; break;
        case 4 : llResult = regA;   break;
        case 5 : llResult = regB;   break;
        case 6 : llResult = regC;   break;
        case 7 : std::cout << "ERROR: Combo2Number() --> operand 7 is reserved value!" << std::endl; break;
        default: std::cout << "ERROR: Combo2Number() --> unknown operand value: " << nCombo << std::endl;
    }
    return llResult;
}

long long my_power( long long a, long long b ) {
    long long llResult = 1;
    for (int i = 0; i < b; i++) {
        llResult *= a;
    }
    return llResult;
}

// ===== this is the part where the instructions are implemented =====

void adv( long long operand ) {
    regA = regA / my_power( 2, Combo2Number( operand ));
}

void bxl( long long operand ) {
    regB = regB ^ operand;
}

void bst( long long operand ) {
    regB = Combo2Number( operand ) % 8;
}

void jnz( long long operand ) {
    if (regA != 0) {
        PC = operand;
    } else {
        PC += 2;
    }
}

void bxc( long long operand ) {
    regB = regB ^ regC;
}

void out( long long operand, std::vector<long long> &vOutput ) {
    vOutput.push_back( Combo2Number( operand ) % 8 );
}

void bdv( long long operand ) {
    regB = regA / my_power( 2, Combo2Number( operand ));
}

void cdv( long long operand ) {
    regC = regA / my_power( 2, Combo2Number( operand ));
}

// ===== end of the part where the instructions are implemented =====

// executes one instruction from progData, as indexed by glb var PC
// if any output is done, it will be put in vOutput
// returns true if not halted, false if halted
bool OneInstruction( DataStream &progData, std::vector<long long> &vOutput ) {
    bool bNotHalted = true;
    if (PC >= (int)progData.size()) {
        bNotHalted = false;
    } else {
        int opcode  = progData[PC    ];
        long long operand = progData[PC + 1];
        switch (opcode) {
            case  0: adv( operand          ); PC += 2; break;
            case  1: bxl( operand          ); PC += 2; break;
            case  2: bst( operand          ); PC += 2; break;
            case  3: jnz( operand          );          break;  // PC is updated within the instruction
            case  4: bxc( operand          ); PC += 2; break;
            case  5: out( operand, vOutput ); PC += 2; break;
            case  6: bdv( operand          ); PC += 2; break;
            case  7: cdv( operand          ); PC += 2; break;
            default: std::cout << "ERROR: OneInstruction() --> unknown opcode value: " << opcode << std::endl;
        }
    }
    return bNotHalted;
}

std::vector<long long> RunProgram( DataStream &progData, long long strtA ) {

    // reset status variables
    regA = strtA;
    regB = 0;
    regC = 0;
    PC   = 0;
    // prepare running the program
    std::vector<long long> curOutput;
    // run it until halted
    while (OneInstruction( progData, curOutput )) {
        // execute instruction, store result in curOutput
    }
    return curOutput;
}

// ----- PART 2

// checks on equality of progData and vOutput "at the back": vOutput is assumed to be less than or equal in size
// than progData, so the last element of vOutput is compared with the last element of progData etc.
bool IsSimilar( DataStream &progData, std::vector<long long> &vOutput, int nDigitsToProcess ) {

    int nSizeDifference = progData.size() - vOutput.size();
    // needed this additional check to squash a nasty bug
    bool bStillSimilar = nSizeDifference == nDigitsToProcess;
    // pair wise compare elements
    for (int i = 0; i < (int)vOutput.size() && bStillSimilar; i++) {
        bStillSimilar = (progData[i + nSizeDifference] == vOutput[i]);
    }
    return bStillSimilar;
}

// takes llCurA as initial A value, and looks for extension of llCurA such that the running of progData
// on llCurA produces output that is identical to progData
long long ExtractProgram( DataStream &progData, long long llCurA, int nDigitsToProcess ) {

    long long llResult = -1;  // signals no match
    // attempt to expad current A value and check if running the program on it produces similar output
    // (see IsSimilar() for explanation)
    for (int i = 0; i < 8 && llResult == -1; i++) {
        // assemble next value to examine
        long long llNewA = (llCurA << 3) + i;
        // run program on that new A value and collect output from the run
        std::vector<long long> vOutput = RunProgram( progData, llNewA );
        // check if output resembles the program itself
        if (IsSimilar( progData, vOutput, nDigitsToProcess )) {
            // it does!
            if (nDigitsToProcess == 0) {
                // if all digits are processed, then progData == vOutput: return the resulting A value
                llResult = llNewA;
            } else {
                // recursively look for extension of A to match more digits
                llResult = ExtractProgram( progData, llNewA, nDigitsToProcess - 1 );
            }
        }
    }
    return llResult;
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
    std::vector<long long> glbOutput = RunProgram( inputData, regA );

    std::string sAnswer1;
    for (int i = 0; i < (int)glbOutput.size(); i++) {
        sAnswer1.append( std::to_string( glbOutput[i] ));
        if (i < (int)glbOutput.size() - 1) {
            sAnswer1.append( "," );
        }
    }

    std::cout << std::endl << "Answer to part 1:  = " << sAnswer1 << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 1 - solving puzzle part 1 : " );   // =========================^^^^^vvvvv

    inputData = part2Data; // get fresh untouched copy of input data

    // part 2 code here
    long long resultA = ExtractProgram( inputData, 0, (int)inputData.size() - 1 );

    std::cout << std::endl << "Answer to part 2: = " << resultA << std::endl << std::endl;

/* ========== */   tmr.TimeReport( "    Timing 2 - solving puzzle part 2 : " );   // ==============================^^^^^

    return 0;
}
