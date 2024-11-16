// This file contains a template for the implementation of Robo prediction
// algorithm

#include "PredictionAlgorithm.hpp"

struct RoboPredictor::RoboMemory {
    //struct
    struct Entry {
        //Contains uint64_t tag and int confidence counter. 
        int tag;
    };
    /*
    1 #define BITMASK() \
    max_table_size
    num_history_table = 1
    const int tag_length = 12;
    max_confidence = 3;
    


    Define integer number of table given prediction. 
Define Boolean  prev_prediction 
set both to 0. 

Define a vector which is called table. It contains a vector of table entry. 

Define global history. It's a shift register with uint64_t. 

void Initialize tables, 
loop through each history table, 
assign them incremental table_ids, 
and decremental table size by a power of 2 from MAX_HISTORY_TABLE_SIZE
then set the table size : push to the tables an entire factor of table entry which has table size defined, 
and inside the for loop of for each entry in the table size, 
	define the corresponding tag and counter as 0.


    */

    void make_prediction(uint64_t pc) {

        /*
indicies [table_num] =
(planetID ^ // XOR planetiD with a portion of global history 
(ghist & 
((table_num >0) ? 
BIT_MASK(uint64_t, (1 << table_num)) : 0))) % 
tableSize;  // modulo tableSize to ensure index falls within the table’s */

        /*
        Final_prediction = false
Found = false
Perform prediction and compute final prediction
Prediction
For each table    
 If tables[table_num][indicies[table_num]].tag == tags[table_num] 
  Access confidence with tables[table_num][indicies[table_num].counter
   If confidence >= MAX_CONFIDENCE + ½
    make it a prediction : final_prediction = true
Save table_num that made the pred
prev_prediction = final_prediction;
Found = true;
Break;
If (!found) num_of_table_given_prev_prediction = -1;
Return final_prediction;

        */
    };
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
