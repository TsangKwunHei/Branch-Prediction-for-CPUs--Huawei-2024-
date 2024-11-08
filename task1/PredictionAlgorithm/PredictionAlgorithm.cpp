#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <deque>
#include "PredictionAlgorithm.hpp"



struct RoboPredictor::RoboMemory {
  // Place your RoboMemory content here
  // Note that the size of this data structure can't exceed 64KiB!

  int last_state = 0 | 1;
  // {P_day_to_day, P_day_to_night, P_night_to_night, P_night_to_day};
  std::array<float, 4> probabilities = {0.6, 0.6, 0.4, 0.4};
  std::array<float, 4> confidence = {1.0,1.0,1.0,1.0};
  std::vector<int> last_2 = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
  const size_t limit = 5;
  float confidence_weight = 0.1;
  const float increment = 0.05;
  const float decrement = 0.03;
  bool last_prediction;
  int decision = 0 | 1 | 2 | 3;
  int round_count = 0;

  /*
  array long_table 
  circular buffer
  push_bit
  confidence long_table
  */
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
  // 1.1
  // Step 1     Get basic prediction from bimodal 
  // Step 2     Look for better predictions in more advanced tables

  // Step 3     If we found a matching entry 
  //                Get the prediction from our best matching table
  //                Convert the counter to a yes/no prediction (>=0 means predict taken)
  //                If we have a second-best matching table, get its prediction too
  //                Check if this alternate prediction is confident
  //                Calculate which table we should use for our final prediction
  //                Should we use the alternate prediction instead of our best match?
  
  // Step 4         Make the final prediction!
  //                We use the longest match prediction if:
  //                - The selector says not to use alternate (use_alt is false) OR
  //                - The longest match is very confident
  //                    else, use the alternate prediction

  // Step 5         Set confidence levels for our final prediction
  //                If we didn't find any matches, use bimodal prediction

  bool prediction;
  return prediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {

/*
    if last prediction is correct
        increase ++ confidence
    if not 
        confidence -= 1
        replace low corresponding confidence for entry   
 */

// if incorrect ()
// feedback_adjustment()

  auto& memory = *roboMemory_ptr;
   if (timeOfDayOnNextPlanet == true){
        memory.last_state = 1;
    } else {
        memory.last_state = 0;
   }

   if (memory.last_prediction == timeOfDayOnNextPlanet) {
    memory.probabilities[memory.decision] += 0.5;
    memory.confidence[memory.decision] += 1;
    if( memory.decision % 2 == 0  ){
        memory.probabilities[memory.decision + 1] -= 0.5;    
    } else {
        memory.probabilities[memory.decision - 1] -= 0.5;
    }
   }
   else {
    memory.probabilities[memory.decision] -= 0.3;
    memory.confidence[memory.decision] -= 1;
    if( memory.decision % 2 == 0  ){
        memory.probabilities[memory.decision + 1] += 0.3;    
    } else {
        memory.probabilities[memory.decision - 1] += 0.3;
    }
   }

   memory.round_count += 1;
   if (memory.round_count > 99){
    memory.round_count = 0;
    for (auto& probabliity : memory.probabilities){
       if (probabliity > 0.9){
           probabliity = 0.9;
       } else if (probabliity < 0.1){
           probabliity = 0.1;
       }
    }
    for (auto& confidence : memory.confidence){
        confidence *= 0.9;
    }

   }
  // Simple prediction policy: do nothing
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
