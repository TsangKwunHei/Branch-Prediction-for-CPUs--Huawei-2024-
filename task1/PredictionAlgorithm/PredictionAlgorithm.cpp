#include <map>
#include <vector>
#include <string>
#include <iostream>


// This file contains a template for the implementation of Robo prediction
// algorithm

#include "PredictionAlgorithm.hpp"

struct RoboPredictor::RoboMemory {
  // Place your RoboMemory content here
  // Note that the size of this data structure can't exceed 64KiB!
  int last_state = 0 | 1;
  // {P_day_to_day, P_day_to_night, P_night_to_night, P_night_to_day};
  std::array<float, 4> probabilities = {0.6, 0.6, 0.4, 0.4};
  bool last_prediction;
  int decision = 0 | 1 | 2 | 3;
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
  // Robo can consult data structures in its memory while predicting.
  // Example: access Robo's memory with roboMemory_ptr-><your RoboMemory
  // content>

  
  bool prediction;
    if (roboMemory_ptr->last_state == 1) {
      if (roboMemory_ptr->probabilities[0] > roboMemory_ptr->probabilities[1]) {
          prediction = true;
          roboMemory_ptr->decision = 0;
      } else {
          prediction = false;
          roboMemory_ptr->decision = 1;
      }} 
    
    else {
      if (roboMemory_ptr->probabilities[2] > roboMemory_ptr->probabilities[3]) {
          prediction = true;
          roboMemory_ptr->decision = 2;
      } else {
          prediction = false;
          roboMemory_ptr->decision = 3;
      }};

      roboMemory_ptr->last_prediction = prediction;


  // Simple prediction policy: follow the spaceship computer's suggestions
  // return spaceshipComputerPrediction;
  // now for i am using my own prediction
  return prediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
  // Robo can consult/update data structures in its memory
  // Example: access Robo's memory with roboMemory_ptr-><your RoboMemory
  // content> 

/*  2.1 Record the last observed state in last_state - day (true) or night (false) 
    2.2 Retrieve transition probabilities based on last_state.
    2.3 Predict the next state using the higher probability 
    (e.g., if P_day_to_day > P_day_to_night, predict “day”). */

   if (timeOfDayOnNextPlanet == true){
        roboMemory_ptr->last_state = 1;
    } else {
        roboMemory_ptr->last_state = 0;
   }

   if (roboMemory_ptr->last_prediction == timeOfDayOnNextPlanet) {
   }
   else {
    roboMemory_ptr->probabilities[roboMemory_ptr->decision] -= 0.1;
    if( roboMemory_ptr->decision % 2 == 0  ){
        roboMemory_ptr->probabilities[roboMemory_ptr->decision + 1] += 0.1;
    } else {
        roboMemory_ptr->probabilities[roboMemory_ptr->decision - 1] += 0.1;
    }
   }

/*  3.1 Compare the prediction with the actual observed state. */

  // It is important not to exceed the computation cost threshold while making
  // predictions and updating RoboMemory. The computation cost of prediction and
  // updating RoboMemory is calculated by the playground automatically and
  // printed together with accuracy at the end of the evaluation (see main.cpp
  // for more details).

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
