//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>

//
// Student Information
//
extern const char *studentName;
extern const char *studentID;
extern const char *email;

//------------------------------------//
//      Global Predictor Defines      //
//------------------------------------//
#define NOTTAKEN  0
#define TAKEN     1

// The Different Predictor Types
#define STATIC      0
#define GSHARE      1
#define TOURNAMENT  2
#define CUSTOM      3
extern const char *bpName[];

// Definitions for 2-bit counters
#define SN  0			// predict NT, strong not taken
#define WN  1			// predict NT, weak not taken
#define WT  2			// predict T, weak taken
#define ST  3			// predict T, strong taken
#define SL  0           // use local predictor strongly
#define WL  1           // use local predictor weakly
#define WG  2           // use global predictor weakly
#define SG  3           // use global predictor strongly

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//
extern int ghistoryBits; // Number of bits used for Global History
extern int lhistoryBits; // Number of bits used for Local History
extern int pcIndexBits;  // Number of bits used for PC index
extern int bpType;       // Branch Prediction Type
extern int verbose;

//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void init_predictor();

// helper method for GSHARE predictor initialization
void init_predictor_GSHARE();

// helper method for TOURNAMENT predictor initialization
void init_predictor_TOURNAMENT();

// helper method for CUSTOM predictor initialization
void init_predictor_CUSTOM();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc);

// helper method for GSHARE predictor
uint8_t make_prediction_GSHARE(uint32_t pc);

// helper method for TOURNAMENT predictor
uint8_t make_prediction_TOURNAMENT(uint32_t pc);

// helper method for CUSTOM predictor
uint8_t make_prediction_CUSTOM(uint32_t pc);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome);

// helper method to train GSHARE predictor
void train_predictor_GSHARE(uint32_t pc, uint8_t outcome);

// helper method to train TOURNAMENT predictor
void train_predictor_TOURNAMENT(uint32_t pc, uint8_t outcome);

// helper method to train CUSTOM predictor
void train_predictor_CUSTOM(uint32_t pc, uint8_t outcome);


#endif
