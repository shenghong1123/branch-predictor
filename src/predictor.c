//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
//#include <math.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID   = "PID";
const char *email       = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint8_t* BHT_local;
uint8_t* BHT_global;
uint8_t* chooser;
uint32_t* PHT;
int GHR;


int** weights;
int* history;
float y;
int mask_perceptron;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
    switch (bpType) {
        case STATIC:
            break;
        case GSHARE:
            init_predictor_GSHARE();
            break;
        case TOURNAMENT:
            init_predictor_TOURNAMENT();
            break;
        case CUSTOM:
            init_predictor_CUSTOM();
            break;
        default:
            break;
    }
}

// get the
int
power(int base, int times) {
    if (times == 0) return 1;
    return base * power(base, times - 1);
}

// helper method for GSHARE initialization
void
init_predictor_GSHARE()
{
    GHR = 0;
    int size = power(2, ghistoryBits);
    BHT_global = (uint8_t*)malloc(size * sizeof(uint8_t));
    for(int i = 0; i < size; i++) {
        BHT_global[i] = WN;
    }
}

// helper method for TOURNAMENT initialization
void
init_predictor_TOURNAMENT()
{
    
    GHR = 0;
     
    int size_local = power(2, lhistoryBits);
    int size_global = power(2, ghistoryBits);
    int size_PHT = power(2, pcIndexBits);
    int size_chooser = size_global;
    
    BHT_local = (uint8_t*)malloc(size_local * sizeof(uint8_t));
    BHT_global = (uint8_t*)malloc(size_global * sizeof(uint8_t));
    PHT = (uint32_t*)malloc(size_PHT * sizeof(uint32_t));
    chooser = (uint8_t*)malloc(size_chooser * sizeof(uint8_t));
    
    for (int i = 0; i < size_local; i++) {
        BHT_local[i] = WN;
    }
    for (int i = 0; i < size_global; i++) {
        BHT_global[i] = WN;
    }
    for (int i = 0; i < size_PHT; i++) {
        PHT[i] = 0;
    }
    for (int i = 0; i < size_chooser; i++) {
        chooser[i] = WG;
    }
    
    
}

// helper method for CUSTOM initialization
void
init_predictor_CUSTOM()
{
    ghistoryBits = 12;
    pcIndexBits = 10;
    
    
    // init mask
    for (int i = 0; i < pcIndexBits; i++) {
        mask_perceptron = (mask_perceptron << 1) + 1;
    }

    // init weight table (perceptron table)
    int size_perceptron = power(2, pcIndexBits);
    weights = (int **)malloc(size_perceptron * sizeof(int *));
    for (int i = 0; i < size_perceptron; i++) {
        weights[i] = (int *)malloc((ghistoryBits + 1) * sizeof(int));
        for (int j = 0; j <= ghistoryBits; j++) {
            weights[i][j] = 0;
        }
    }
    
    // init all history to -1 (not taken)
    history = (int *)malloc(ghistoryBits * sizeof(int));
    for (int i = 0; i < ghistoryBits; i++) {
        history[i] = -1;
    }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
          return TAKEN;
          break;
    case GSHARE:
          return make_prediction_GSHARE(pc);
          break;
    case TOURNAMENT:
          return make_prediction_TOURNAMENT(pc);
          break;
    case CUSTOM:
          return make_prediction_CUSTOM(pc);
          break;
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// helper method for GSHARE predictor
uint8_t
make_prediction_GSHARE(uint32_t pc) {
    
    uint32_t mask = 0;
    for (int i = 0; i < ghistoryBits; i++) {
        mask = (mask << 1) + 1;
    }
    uint32_t result = (pc & mask) ^ (GHR & mask);
    if (BHT_global[result] <= 1) {
        return NOTTAKEN;
    }
    else {
        return TAKEN;
    }
    
}

// helper method for TOURNAMENT predictor
uint8_t
make_prediction_TOURNAMENT(uint32_t pc) {
    

    uint32_t mask_pc = 0;
    uint32_t mask_PHT = 0;
    for (int i = 0; i < pcIndexBits; i++) {
        mask_pc = (mask_pc << 1) + 1;
    }
    for (int i = 0; i < lhistoryBits; i++) {
        mask_PHT = (mask_PHT << 1) + 1;
    }
    
    int index_PHT = pc & mask_pc;                  // get the index for PHT
    int value_PHT = PHT[index_PHT];                // get the index for local predictor
    int prediction_local = BHT_local[value_PHT];   // prediction from local predictor
    int prediction_global = BHT_global[GHR];       // prediction from global predictor
    int choose = chooser[GHR];                     // choose which one to use
    
    // choose prediction
    if (choose <= 1) {
        return prediction_local <= 1? NOTTAKEN: TAKEN;
    }
    else {
        return prediction_global <= 1? NOTTAKEN: TAKEN;
    }
    
}

// helper method to transfer array history to int history
int
transferHistory() {
    int ghr = 0;
    for (int i = 0; i < ghistoryBits; i++) {
        ghr = ghr << 1;
        if (history[i] == 1) {
            ghr = ghr + 1;
        }
    }
    return ghr;
}

uint8_t
make_prediction_CUSTOM(uint32_t pc) {
    
    GHR = transferHistory();
    
    int index_perceptron = (pc & mask_perceptron) ^ (GHR & mask_perceptron);
    
    y = weights[index_perceptron][ghistoryBits];
    
    for (int i = 0; i < ghistoryBits; i++) {
        y += weights[index_perceptron][i] * history[i];
    }
    
    if (y < 0) {
        return NOTTAKEN;
    }
    else return TAKEN;

}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
    switch (bpType) {
        case STATIC:
            break;
        case GSHARE:
            train_predictor_GSHARE(pc, outcome);
            break;
        case TOURNAMENT:
            train_predictor_TOURNAMENT(pc, outcome);
            break;
        case CUSTOM:
            train_predictor_CUSTOM(pc, outcome);
            break;
        default:
            break;
    }
}

// helper method to train GSHARE predictor
void train_predictor_GSHARE(uint32_t pc, uint8_t outcome) {
    
    uint32_t mask = 0;
    
    for (int i = 0; i < ghistoryBits; i++) {
        mask = (mask << 1) + 1;
    }
    
    uint32_t result = (pc & mask) ^ (GHR & mask);
    
    if (outcome == NOTTAKEN) {
        GHR = (GHR << 1) & mask;
        if (BHT_global[result] >= 1) {
            BHT_global[result]--;
        }
    }
    else if (outcome == TAKEN) {
        GHR = ((GHR << 1) + 1) & mask;
        if (BHT_global[result] <= 2) {
            BHT_global[result]++;
        }
    }
}

// helper method to train TOURNAMENT predictor
void train_predictor_TOURNAMENT(uint32_t pc, uint8_t outcome) {
    
    
    // record the original prediction
    uint32_t mask_pc = 0;
    uint32_t mask_PHT = 0;
    uint32_t mask_GHR = 0;
    for (int i = 0; i < pcIndexBits; i++) {
        mask_pc = (mask_pc << 1) + 1;
    }
    for (int i = 0; i < lhistoryBits; i++) {
        mask_PHT = (mask_PHT << 1) + 1;
    }
    for (int i = 0; i < ghistoryBits; i++) {
        mask_GHR = (mask_GHR << 1) + 1;
    }
    
    int index_PHT = pc & mask_pc;
    int value_PHT = PHT[index_PHT];
    int prediction_local = BHT_local[value_PHT];
    int prediction_global = BHT_global[GHR];
    int choose = chooser[GHR];
    
    if (outcome == NOTTAKEN) {
        
        // prefer the correct prediction whenever the local and global predictions differ
        if ((prediction_local <= 1 && prediction_global >= 2) || (prediction_local >= 2 && prediction_global <= 1)) {
            if (prediction_local <= 1 && choose > 0) {chooser[GHR]--;}
            else if (prediction_global <= 1 && choose < 3) {chooser[GHR]++;}
        }
        if (prediction_local >= 1) BHT_local[value_PHT]--;   // update the local predictor
        if (prediction_global >= 1) BHT_global[GHR]--;       // update the global predictor
    
        PHT[index_PHT] = (value_PHT << 1) & mask_PHT;        // update the local history
        GHR = (GHR << 1) & mask_GHR;                         // update the global history
    }
    else {
        
        if ((prediction_local <= 1 && prediction_global >= 2) || (prediction_local >= 2 && prediction_global <= 1)) {
            if (prediction_local >= 2 && choose > 0) {chooser[GHR]--;}
            else if (prediction_global >= 2 && choose < 3) {chooser[GHR]++;}
        }
        if (prediction_local <= 2) BHT_local[value_PHT]++;
        if (prediction_global <= 2) BHT_global[GHR]++;
        
        PHT[index_PHT] = ((value_PHT << 1) + 1) & mask_PHT; // update the local history
        GHR = ((GHR << 1) + 1) & mask_GHR;                  // update the global history
    }
    
}

// helper method to get absolute value
float
absolute(float x) {
    return x > 0? x: -x;
}

// helper method to train CUSTOM predictor
void train_predictor_CUSTOM(uint32_t pc, uint8_t outcome) {
    
    int result = (outcome == TAKEN? 1: -1);
    
    // set the threshold
    int weight_max = power(2, 5) - 1;
    int weight_min = -weight_max - 1;
    
    int index_perceptron = (pc & mask_perceptron) ^ (GHR & mask_perceptron);
    GHR = transferHistory();
    
    uint8_t prediction = make_prediction_CUSTOM(pc);
    
    if (prediction != outcome || (y > weight_min && y < weight_max)) {
        weights[index_perceptron][ghistoryBits] += (outcome == TAKEN)? 1: -1;
        
        for (int i = 0; i < ghistoryBits; i++) {
            weights[index_perceptron][i] += history[i] * result;
        }
    }
    
    // check if weights exceed threshold
    /**
    for (int i = 0; i <= ghistoryBits; i++) {
        if (weights[index_perceptron][i] > weight_max) {
            weights[index_perceptron][i] = weight_max;
        }
        if (weights[index_perceptron][i] < weight_min) {
            weights[index_perceptron][i] = weight_min;
        }
    }
    */
    
    // shift bits to update history
    for (int i = ghistoryBits; i >0; i--) {
        history[i] = history[i - 1];
    }
    history[0] = result;
    
    
}
