#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <deque>
#include "PredictionAlgorithm.hpp"
// RoboMemory within RoboPredictor class
struct RoboPredictor::RoboMemory {

// Step 1: Add a Short-Term History Buffer
/*
1. Define the Buffer: In RoboMemory, 
add a fixed-size deque to hold recent outcomes (e.g., last 4).
Example: `std::deque<bool> recent_outcomes{true, false, true, false};
*/

// std::deque automatically handles the FIFO (First In, First Out) behavior
/*
    - Add new value
    recent_outcomes.push_back(new_value);

    - Remove oldest if buffer exceeds size
    if (recent_outcomes.size() > 4) {
        recent_outcomes.pop_front();
    }
*/
std::deque<bool> recent_outcomes;
static const size_t HISTORY_SIZE = 10;


// Step 2: Identify Common Patterns in the Buffer
/*
2.1. Create Pattern Match Function: 
  Define a helper function in RoboMemory that 
  compares `recent_outcomes` to common patterns 
*/
  bool matchesPattern(const std::vector<bool>& pattern) const {
    
    if (recent_outcomes.size() < pattern.size()) {
        return false;
    }
    
    for (size_t i = 0; i < pattern.size(); ++i) {
      /*
      Compare:
        recent_outcomes =   {false, true, true}  // Most recent at the end
        pattern =           {false, true, true}  
          pattern[0] (false) with recent_outcomes[1] (false) ✓
          pattern[1] (true) with recent_outcomes[2] (true) ✓
          pattern[2] (true) with recent_outcomes[3] (true) ✓
      */
        size_t history_index = recent_outcomes.size() - pattern.size() + i;
        if (recent_outcomes[history_index] != pattern[i]) {
            return false;
        }
    }
    return true;
  }

   const std::vector<std::pair<std::vector<bool>, bool>> patterns = {
        {{true, true, true}, true},     // 3 days in a row likely leads to another day
        {{false, false, false}, false}, // 3 nights in a row likely leads to another night
        {{true, false, true}, true},    // Alternating pattern ending in day likely continues
        {{false, true, false}, false}   // Alternating pattern ending in night likely continues
    };


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
  float tage_confidence = 1;
  bool use_tage;
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {

/*
2.2. Table Matching
    - Uses fill_table_indices_tags() for lookup
    - get_two_longest_matching_tables() finds matches
    - Banks distributed by path_history_
Variables:
- FIRST_2WAY_TABLE = 9
- LAST_2WAY_TABLE = 22
*/


// Step 3: Adjust Predictions Based on Patterns

/*  
1. Track Matches in Prediction: When a pattern is matched, 
store this fact to influence the next prediction.*/

/*
2. Modify Prediction Based on Match: If a known pattern is matched, modify the prediction based on historical accuracy.
   Example: `if (pattern_match) { prediction = pattern_predicted_outcome; }`*/

  // 1.1
  /*
2.2. Adjust Probabilities Based on Match: 
In `predictTimeOfDayOnNextPlanet`, 
check if a pattern matches; if so, boost the relevant probability.
*/

  bool prediction;
  auto& memory = *roboMemory_ptr;
  float day_to_day = memory.probabilities[0] * (1 + memory.confidence_weight * memory.confidence[0]);
  float day_to_night = memory.probabilities[1] * (1 + memory.confidence_weight * memory.confidence[1]);
  float night_to_night = memory.probabilities[2] * (1 + memory.confidence_weight * memory.confidence[2]);
  float night_to_day = memory.probabilities[3] * (1 + memory.confidence_weight * memory.confidence[3]);

  // Range-based for loop to iterate over each pair
  if (memory.tage_confidence > 0) {
    for (const auto& pattern : memory.patterns){
    // check if pattern matches 100%
    if (memory.matchesPattern(pattern.first)){
      // if day
      if (memory.last_state == 1){
         memory.use_tage = true;
        // if pattern next state = night
        if (pattern.second == false) {
          day_to_night += memory.tage_confidence;
        }
        // if pattern next state = day
        if (pattern.second == true) {
          day_to_day += memory.tage_confidence;
        }
      } 
      else 
      // if pattern next state = night
        if (pattern.second == false) {
          night_to_night += memory.tage_confidence;
        }
      // if pattern next state = day
        if (pattern.second == true) {
          night_to_day += memory.tage_confidence;
        }
    }
    // if so, apply tage_confidence to a value
    // if not, do nothing.
}

  }
  
  // 1.2 Determine prediction based on the 1.1 probabilities
  
    // if last state is day
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

/*
2.3. Update Buffer After Each Outcome: 
In `observeAndRecordTimeofdayOnNextPlanet`, 
add code to push the new outcome and pop the oldest.

   Example: `recent_outcomes.push_back(timeOfDayOnNextPlanet); 
   recent_outcomes.pop_front();`
*/

/*
    - Add new value
    recent_outcomes.push_back(new_value);

    - Remove oldest if buffer exceeds size
    if (recent_outcomes.size() > 4) {
        recent_outcomes.pop_front();
    }
*/


  /*
3. Adjust Confidence of Prediction: If the prediction is correct, increase the confidence for patterns; if incorrect, decrease it.
   Example: `if (prediction == timeOfDayOnNextPlanet) { confidence[pattern] += 0.1; }`
*/
  auto& memory = *roboMemory_ptr;
  memory.recent_outcomes.push_back(timeOfDayOnNextPlanet);
      if (memory.recent_outcomes.size() > memory.HISTORY_SIZE) {
        memory.recent_outcomes.pop_front();
    };
  bool correct_guess;
   if (timeOfDayOnNextPlanet == true){
        memory.last_state = 1;
    } else {
        memory.last_state = 0;
   }
    if (memory.last_prediction == timeOfDayOnNextPlanet) {
        correct_guess = true;
    } else correct_guess = false;
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
   if (memory.use_tage == true) {
      if (correct_guess==true) {
        memory.tage_confidence += 0.5;
      }
      else {
        memory.tage_confidence -= 0.5;
      }
    }
    
   memory.round_count += 1;
   if (memory.round_count > 99){
    memory.round_count = 0;
    if (memory.tage_confidence < 0 ) {
      memory.tage_confidence = 0;
    }

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