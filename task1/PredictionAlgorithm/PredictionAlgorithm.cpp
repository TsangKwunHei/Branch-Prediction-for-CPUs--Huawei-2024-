#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <deque>
#include "PredictionAlgorithm.hpp"

struct RoboPredictor::RoboMemory {
    // 1 History History_Management
    class History_Management {
      // not yet done 13.11 : function to Initalize history table and confidence value
      /* i   Determine the minimum and maximum history lengths for tables */ 
      int MIN_HISTORY = 4;
      static const size_t numTables = 5;
      /* ii   Use a geometric sequence to set intermediate history lengths 
              between the minimum and maximum, creating a diverse set of history tables. */
      
      //vector (all tables) contains vector (indivial table)
      std::array<std::vector<bool>, numTables>history_tables;
      /* tables
      history_tables[0] = smallest
      history_tables[4] = largest
      */
      int history_tables_sizes[numTables] = {MIN_HISTORY, MIN_HISTORY*2, MIN_HISTORY*4, MIN_HISTORY*8, MIN_HISTORY*16};

      /* iii For each table, initialize a structure to 
      i   store history bits 
      ii  confidence counters representing the probability of transitions.*/
      void store_data(bool data) {
        // for table 1-5, push same data, 
        for (size_t i = 0; i < numTables; i++)  {
          history_tables[i].push_back(data);
          if(history_tables[i].size() > history_tables_sizes[i]){
            // for each table if it's reach defined size then remove index 0
              history_tables[i].erase(history_tables[i].begin());};};}

      //Not yet done : entry levels
      std::array<int, numTables> confidence;
      int MAX_CONFIDENCE = 7;
      void update_confidence(int table_index, bool wasCorrect){
        if (wasCorrect == true){
          confidence[table_index] = std::min(confidence[table_index] + 1, MAX_CONFIDENCE);
        } else {
          confidence[table_index] = std::max(confidence[table_index] - 1, MAX_CONFIDENCE);
        }};
      };
    
    // 2. Prediction Tables
    struct Prediction_Tables {
        int prediction_counter;  // Stores how strongly we predict taken/not taken
        int useful_counter;      // Tracks how useful this entry has been
        int tag;                 // Used to identify matching patterns

    /*Call : 
    roboMemory_ptr::History_Management

    */
    };

    // Different types of prediction tables:
    Prediction_Tables* bimodal_table;         // Simplest predictor (fallback)
    Prediction_Tables* short_history_tables;   // For recent patterns
    Prediction_Tables* long_history_tables;    // For older patterns
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
        bool prediction;
        
        // 1. Get base prediction from bimodal
        //bool prediction = get_bimodal_prediction(nextPlanetID);

        void find_matching_entries();{};
        // 2. Look for matching patterns
        // auto matches = find_matching_entries(nextPlanetID);
        
        // 3. Use best matching prediction if found
        /*if (matches.hit_bank != 0) {
            prediction = get_prediction_from_match(matches);
        }
        */
        
    
  return prediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {


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