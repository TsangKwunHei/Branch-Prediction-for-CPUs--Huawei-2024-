#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <deque>
#include "PredictionAlgorithm.hpp"
// RoboMemory within RoboPredictor class
struct RoboPredictor::RoboMemory {
  int last_state = 0 | 1;
  // {P_day_to_day, P_day_to_night, P_night_to_night, P_night_to_day};
  std::array<float, 4> probabilities = {0.6, 0.6, 0.4, 0.4};
  std::array<float, 4> confidence = {1.0,1.0,1.0,1.0};
  float confidence_weight = 0.1;
  const float increment = 0.05;
  const float decrement = 0.03;
  bool last_prediction;
  int decision = 0 | 1 | 2 | 3;
  int round_count = 0;
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {

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


  // Simple prediction policy: follow the spaceship computer's suggestions
  // return spaceshipComputerPrediction;
  // now for i am using my own prediction
  return prediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {

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