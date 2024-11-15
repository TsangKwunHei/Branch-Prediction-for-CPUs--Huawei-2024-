// This file contains a template for the implementation of Robo prediction
// algorithm
#include <bitset>
#include "PredictionAlgorithm.hpp"

struct RoboPredictor::RoboMemory {
    //Steps
    //1 import class PREDICTOR
    bitset<131> GHR;           // global history register
  	UINT16 PHR; 		   //path history
	// Bimodal
	bimodVal_t *bimodal;       //bimodal table
	UINT32  numBimodalEntries; //number of entries in bimodal table
	tagVal_t *tagTables[NUM_TAGE_TABLES]; //TAGE table
	UINT32 tageTableSize;	              //number of entries in TAGE table

	UINT32 tageHistory[NUM_TAGE_TABLES];  //number ofGHR bits examined by CSR to index a given table
	csr_t *csrIndex;                      //circular shift register for indices 
	csr_t *csrTag[2];                     //2 circular shift registers for tags
	 
	prediction_t pred;                    //global prediction
	
 	UINT32 tageIndex[NUM_TAGE_TABLES];    //index calculated for a given table 
	UINT32 tageTag[NUM_TAGE_TABLES];      //tag calculated for a given table
	UINT32 clock;                         //global clock
  	bool clockState;                      //clocl flip it
  	INT32 altBetterCount;                 //number of times altpred is better than prd


  	// The interface to the four functions below CAN NOT be changed

  	PREDICTOR(void);
  	bool    GetPrediction(UINT32 PC);  

  	void    UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget);
  	void    TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget);
  	
	//void    steal(UINT32 PC, UINT32 table, UINT32 index, UINT32 bimodalIndex, bool predDir);

	UINT32  getTag(UINT32 PC, int table, UINT32 tagSize);
	UINT32  getIndex(UINT32 PC, int table, UINT32 tagSize, UINT32 phrOffset);
	void    initFold(csr_t *shift, UINT32 origLen, UINT32 newLen);
	void    fold(csr_t *shift);

    //struct initialize, import functions
    // deal with the #define
    
    //2 import logs and helper functions

    //3 import PREDICTOR::PREDICTOR(void) 
    //4 import bool PREDICTOR::GetPrediction
    // call it in  predictTimeOfDayOnNextPlanet to get predict
    //5 import void PREDICTOR::UpdatePredictor
    // call it in observeAndRecordTimeofdayOnNextPlanet to activate
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
  // Robo can consult data structures in its memory while predicting.
  // Example: access Robo's memory with roboMemory_ptr-><your RoboMemory
  // content>

  // Simple prediction policy: follow the spaceship computer's suggestions
  return spaceshipComputerPrediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
  // Robo can consult/update data structures in its memory
  // Example: access Robo's memory with roboMemory_ptr-><your RoboMemory
  // content>


}


//------------------------------------------------------------------------------
// Please don't modify this file below
//
// Check if RoboMemory does not exceed 64KiB
static_assert(
    sizeof(RoboPredictor::RoboMemory) <= 65536,
    "Robo's memory exceeds 65536 bytes (64KiB) in your implementation. "
    "Prediction algorithms using so much "
    "memory are ineligible. Please reduce the size of your RoboMemory struct.");

// Declare constructor/destructor for RoboPredictor
RoboPredictor::RoboPredictor() {
  roboMemory_ptr = new RoboMemory;
}
RoboPredictor::~RoboPredictor() {
  delete roboMemory_ptr;
}
