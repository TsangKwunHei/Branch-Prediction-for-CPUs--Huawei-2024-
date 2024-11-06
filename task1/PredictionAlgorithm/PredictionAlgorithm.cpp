#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <deque>

// This file contains a template for the implementation of Robo prediction
// algorithm

#include "PredictionAlgorithm.hpp"

struct RoboPredictor::RoboMemory {
  // Place your RoboMemory content here
  // Note that the size of this data structure can't exceed 64KiB!

  int last_state = 0 | 1;
  // {P_day_to_day, P_day_to_night, P_night_to_night, P_night_to_day};
  std::array<float, 4> probabilities = {0.6, 0.6, 0.4, 0.4};
  float P_day_night_to_day = 0.5;
  float P_day_night_to_night = 0.5;
  float P_night_day_to_day = 0.5;
  float P_night_day_to_night = 0.5;
  std::array<float, 4> confidence = {1.0,1.0,1.0,1.0};
  std::vector<int> last_2 = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
  const size_t limit = 5;
  float confidence_weight = 0.1;
  const float increment = 0.05;
  const float decrement = 0.03;
  bool last_prediction;
  int decision = 0 | 1 | 2 | 3;
  int round_count = 0;
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
    
    /* TODO STEP 1 
    The predictor gathers information based on the 
        1 branch’s history (past branch outcomes) 
        2 the program counter (PC), 
        which identifies where the branch is in the code. */

    /* STEP 2 
    TAGE starts by looking at a simple, backup prediction table called the bimodal table. 
    This table uses a basic strategy to predict if the branch is likely to be taken.
    */
   // Look at SpaceShipComputerPrediction : 77% or current_prototype : 79%


   // Exercise 1 Long term table Update
   // Inialize variables
   // Prediction 
   // Feedback loop


  // 1.1
  auto& memory = *roboMemory_ptr;
  float day_to_day = memory.probabilities[0] * (1 + memory.confidence_weight * memory.confidence[0]);
  float day_to_night = memory.probabilities[1] * (1 + memory.confidence_weight * memory.confidence[1]);
  float night_to_night = memory.probabilities[2] * (1 + memory.confidence_weight * memory.confidence[2]);
  float night_to_day = memory.probabilities[3] * (1 + memory.confidence_weight * memory.confidence[3]);

  // 1.2 Determine prediction based on the 1.1 probabilities
  bool prediction;
    if (memory.last_state == 1) {
      if (day_to_day > day_to_night) {
          prediction = true;
          memory.decision = 0;
      } else {
          prediction = false;
          memory.decision = 1;
      }} 
    
    else {
      if (night_to_night > night_to_day) {
          prediction = true;
          memory.decision = 2;
      } else {
          prediction = false;
          memory.decision = 3;
      }};

      memory.last_prediction = prediction;

/*
	TAGE makes a final prediction by choosing 
    1 the most confident
    2 longest history match available.


	If the prediction is “taken” (meaning the branch will be executed),
        TAGE predicts that path; 
    if “not taken,” 
        it predicts the branch will be skipped.
*/

// Combine predictions process
// Longest history match()
// most_confident_prediction() 
// decision()

  return prediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {

/*
The CPU executes the branch, and we see if TAGE’s prediction was correct.

    If correct: 
    TAGE reinforces this prediction,
    by increasing the confidence for that particular history pattern.

    If incorrect: 
    TAGE updates its records, noting the wrong prediction. 

        This also triggers a potential feedback adjustment:
            TAGE can either :
            replace the incorrect prediction entry if it was weak, 
            or 
            reduce its confidence if it was confident but wrong.

    Over time, TAGE learns which history patterns are more reliable,
    by keeping entries that are frequently correct and adjusting, 
    or replacing those that are frequently wrong.
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
