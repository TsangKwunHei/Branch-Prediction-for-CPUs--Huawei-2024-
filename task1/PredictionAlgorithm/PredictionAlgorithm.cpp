// This file contains a template for the implementation of Robo prediction
// algorithm
#include "PredictionAlgorithm.hpp"
#include <limits.h> /* CHAR_BIT */
#include <cmath>
#include <iostream>
#include <vector>



#define BIT_MASK(__TYPE__, __ONE_COUNT__) \
  ((__TYPE__)(-((__ONE_COUNT__) != 0))) & \
      (((__TYPE__)-1) >> ((sizeof(__TYPE__) * CHAR_BIT) - (__ONE_COUNT__)))


struct RoboPredictor::RoboMemory {
    //struct
    struct Entry {
        //Contains uint64_t tag and int confidence counter. 
        int64_t tag;
        int64_t confidence_counter;
    };
    static constexpr int MAX_TABLE_SIZE = 624;
    static constexpr int NUM_TABLES = 3;
    static constexpr int tag_length = 12;
    static constexpr int max_confidence = 3;
    

    bool found = false;
    int given_prediction_table = 0;
    bool prev_prediction = false;
    int64_t global_history = 0;
    std::vector<std::vector<Entry>> tables;


void init_tables() {
    for (int table_num = 0; table_num < NUM_TABLES; table_num++ ) {
        int table_size = MAX_TABLE_SIZE >> table_num;
        tables.push_back(std::vector<Entry>(table_size));
        for (int entry_id = 0; entry_id < table_size; entry_id++) {
            tables[table_num][entry_id].tag=0;
            tables[table_num][entry_id].confidence_counter=0;
        }
    };
};


    RoboMemory() {
        init_tables();
    }
bool make_prediction(uint64_t branch_id) {
    uint64_t indicies[NUM_TABLES];
    uint64_t tags[NUM_TABLES];


    for (int table_num = 0; table_num < NUM_TABLES; table_num++ ) {
        int table_size = MAX_TABLE_SIZE >> table_num;
        BIT_MASK(uint64_t, tag_length);

        indicies[table_num] = 
            (branch_id ^ 
            (global_history & 
            ((table_num>0)? 
            BIT_MASK(uint64_t, (1 << table_num)) : 0))) %
            table_size;

        tags[table_num] = 
            (branch_id ^ 
            (global_history & 
            ((table_num>0)? 
            BIT_MASK(uint64_t, (1 << table_num)) : 0))) &
            BIT_MASK(uint64_t, tag_length);
    };
    found = false;

    bool finale_prediction = false;
    for (int table_num = 0; table_num < NUM_TABLES; table_num++ ) {
        if (tables[table_num][indicies[table_num]].tag == tags[table_num]) {
            if (tables[table_num][indicies[table_num]].confidence_counter > (max_confidence+1)/2) {
                finale_prediction = true;
            };
            given_prediction_table = table_num;
            prev_prediction = finale_prediction;
            found = true;
            break;
            };
    };
    if (found == false) {
    given_prediction_table = -1 ;
    };
    return finale_prediction;
    };

    void update(uint64_t branch_id, bool outcome) {
        
        uint64_t indicies[NUM_TABLES];
        uint64_t tags[NUM_TABLES];

        for (int table_num = 0; table_num < NUM_TABLES; table_num++ ) {
        int table_size = MAX_TABLE_SIZE >> table_num;
        BIT_MASK(uint64_t, tag_length);
        
        indicies[table_num] = 
            (branch_id ^ 
            (global_history & 
            ((table_num>0)? 
            BIT_MASK(uint64_t, (1 << table_num)) : 0))) %
            table_size;

        tags[table_num] = 
            (branch_id ^ 
            (global_history & 
            ((table_num>0)? 
            BIT_MASK(uint64_t, (1 << table_num)) : 0))) &
            BIT_MASK(uint64_t, tag_length);
    };


    int id = 0; 
    global_history = (global_history << 1) | (outcome ? 1:0);
    
    if (given_prediction_table>=0) {
        if (prev_prediction == outcome) {
                id = given_prediction_table;
        if (outcome == true and tables[id][indicies[id]].confidence_counter < max_confidence) {
            tables[id][indicies[id]].confidence_counter ++ ;
        } else if (outcome == false and tables[id][indicies[id]].confidence_counter > 0) {
            tables[id][indicies[id]].confidence_counter -- ;
        } 
    } else {
        if (id < NUM_TABLES-1) {
            id = given_prediction_table +1;
        tables[id][indicies[id]].tag = tags[id];
        tables[id][indicies[id]].confidence_counter = (outcome) ? 2:1;
        } else { 
            if (outcome == true and tables[id][indicies[id]].confidence_counter < max_confidence) {
                tables[id][indicies[id]].confidence_counter ++ ;
            } else 
            if (outcome == false and tables[id][indicies[id]].confidence_counter > 0) {
                tables[id][indicies[id]].confidence_counter -- ;
            };
        } 
    };
    } else { 
        id = 0; 
        tables[id][indicies[id]].tag = tags[id];
        tables[id][indicies[id]].confidence_counter = (outcome) ? 2:1;
    }
    };
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
  // Robo can consult data structures in its memory while predicting.
  // Example: access Robo's memory with roboMemory_ptr-><your RoboMemory
  // content>
  // Simple prediction policy: follow the spaceship computer's suggestions
  return roboMemory_ptr->make_prediction(nextPlanetID);
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
  // Robo can consult/update data structures in its memory
  // Example: access Robo's memory with roboMemory_ptr-><your RoboMemory
  // content>
  roboMemory_ptr->update(nextPlanetID, timeOfDayOnNextPlanet);

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
