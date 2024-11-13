#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <deque>
#include "Utils.hpp" // Include the utility file

// This file contains a template for the implementation of Robo prediction
// algorithm

#include "PredictionAlgorithm.hpp"

// RoboMemory within RoboPredictor class
struct RoboPredictor::RoboMemory {
  // Place your RoboMemory content here
  // Note that the size of this data structure can't exceed 64KiB!

// 1 IS DAY 0 IS NIGHT
  int last_state = 0 | 1;
  // {P_day_to_day, P_day_to_night, P_night_to_night, P_night_to_day};

  std::array<float, 4> probabilities = {0.6, 0.6, 0.4, 0.4};
  std::array<float, 4> confidence = {1.0,1.0,1.0,1.0};
  float confidence_weight = 0.1;
  bool last_prediction;
  int decision = 0 | 1 | 2 | 3;
  int round_count = 0;
  //////////////////////////////////////////////////////////////

/*
1.1. Circular Buffer (Long_History_Register class)
    - Purpose: Stores branch outcomes in a fixed-size circular buffer
    - Key variables:
      * buffer_size_ = power of 2 size to fit history_size + max_in_flight_bits
      * history_bits_ = vector storing actual bits
      * head_ = current position in buffer
    - Why circular: Allows efficient storage/access without shifting entire history
*/
  class CircularBuffer {
  public:
      CircularBuffer(size_t size) : buffer(size), head(0), size(size) {} 

      void addToBuffer(bool value) {
          buffer[head] = value;
          head = (head + 1) % size;
      }
      
  private:
      std::vector<bool> buffer;
      size_t head;
      size_t size;
  };


/*
1.2. Speculative Bits Management
    - num_speculative_bits_ = tracks predictions not yet confirmed
    - max_num_speculative_bits_ = buffer_size_ - history_size
    - Operations:
      * push_bit(): Adds new prediction (increments num_speculative_bits_)
      * rewind(): Removes wrong speculative predictions
      * retire(): Confirms correct predictions
*/

/*
1.3. Random Access (operator[])
    - Purpose: Access any position in history directly
    - Usage: history_register[i] returns bit from i steps ago
    - Implementation:
      * Returns history_bits_[(head_ + i) & buffer_access_mask_]
      * buffer_access_mask_ ensures circular wraparound
*/

/*
1.4. Path History
    - PATH_HISTORY_WIDTH = 27 (not number of tables)
    - Purpose: Tracks sequence of branch addresses
    - Used to:
      * Index into prediction tables
      * Create unique patterns for different code paths
    - Folded history: Compresses longer histories into smaller bits
      using XOR operations
*/

/*
1.5. Speculative State
    - Not about number of guesses per timestep
    - Tracks predictions made but not yet confirmed
    - Allows:
      * Making multiple predictions ahead
      * Rolling back if predictions were wrong
      * Committing when predictions confirmed correct
*/


  static const size_t NUM_TABLES = 3;
static const size_t MAX_HISTORY_LENGTH = 100;

  struct TransitionCounts {
    int day_to_day = 0;
    int day_to_night = 0;
    int night_to_night = 0;
    int night_to_day = 0;
    float confidence = 1.0;
  };

  std::array<std::deque<bool>, NUM_TABLES> history_tables;
  std::array<TransitionCounts, NUM_TABLES> transition_counts;
  std::array<size_t, NUM_TABLES> 
  table_sizes = {4, 8, 16};


};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {



  auto& memory = *roboMemory_ptr;
  bool prediction;

/*
2. Prediction Process
Variables:
- tagged_table_ptrs_ (Array of table pointers)
- tage_histories_ (Stores all history information)
*/


/*
2.1. Base Prediction
    - Uses bimodal_table_ for base prediction
    - Stores in Tage_Prediction_Info structure
Variables:
- BIMODAL_LOG_TABLES_SIZE = 13
- BIMODAL_HYSTERESIS_SHIFT = 2
*/

/*
2.2. Table Matching
    - Uses fill_table_indices_tags() for lookup
    - get_two_longest_matching_tables() finds matches
    - Banks distributed by path_history_
Variables:
- FIRST_2WAY_TABLE = 9
- LAST_2WAY_TABLE = 22
*/


/*
2.3. Final Prediction Selection
    - Uses longest match or alternate based on confidence
Variables:
- ALT_SELECTOR_LOG_TABLE_SIZE = 4
- ALT_SELECTOR_ENTRY_WIDTH = 5
- alt_selector_table_ (For choosing between predictions)
*/
  // 1.1
  
//////////////////////////////////////////////////////////////

  float day_to_day = memory.probabilities[0] * (1 + memory.confidence_weight * memory.confidence[0]);
  float day_to_night = memory.probabilities[1] * (1 + memory.confidence_weight * memory.confidence[1]);
  float night_to_night = memory.probabilities[2] * (1 + memory.confidence_weight * memory.confidence[2]);
  float night_to_day = memory.probabilities[3] * (1 + memory.confidence_weight * memory.confidence[3]);

  // 1.2 Determine prediction based on the 1.1 probabilities
  
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
3. Learning and Update Process
Variables:
- tick_ (Counter for useful bit management)
*/

/*
3.1. Entry Allocation
    - Allocates on misprediction
Variables:
- EXTRA_ENTRIES_TO_ALLOCATE = 1
- random_number_gen_ (For allocation randomization)
*/

/*
3.2. Prediction Counter Updates
    - Updates prediction counters and usefulness
Variables:
- PRED_COUNTER_WIDTH = 3 (Counter size)
- USEFUL_BITS = 1 (Usefulness tracking)
*/


/*
3.3. History Management
    - Manages path_history_ updates
    - Updates folded histories
Variables:
- TICKS_UNTIL_USEFUL_SHIFT = 1024 (Aging interval)
- folded_histories_for_indices_
- folded_histories_for_tags_0_
- folded_histories_for_tags_1_
*/

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