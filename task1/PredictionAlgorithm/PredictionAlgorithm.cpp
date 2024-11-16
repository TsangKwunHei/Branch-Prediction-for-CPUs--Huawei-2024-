// PredictionAlgorithm.cpp
#include "PredictionAlgorithm.hpp"
#include <limits.h> // CHAR_BIT
#include <cmath>
#include <iostream>
#include <vector>

// Macro to create a bitmask with the specified number of least significant bits set to 1.
#define BIT_MASK(__TYPE__, __ONE_COUNT__) \
    ((__TYPE__)(-((__ONE_COUNT__) != 0))) & \
    (((__TYPE__)-1) >> ((sizeof(__TYPE__) * CHAR_BIT) - (__ONE_COUNT__)))

struct RoboPredictor::RoboMemory {
    /**
     * Struct representing an entry in the history tables.
     */
    struct Entry {
        uint64_t tag;             // Tag for the entry.
        int confidence_counter;  // Confidence counter for the prediction.
    };

    // Configuration constants
    static constexpr int MAX_TABLE_SIZE = 624;   // Total number of entries in the largest table.
    static constexpr int NUM_TABLES = 1;         // Number of history tables.
    static constexpr int TAG_LENGTH = 12;        // Number of bits used for the tag.
    static constexpr int MAX_CONFIDENCE = 3;     // Maximum value for the confidence counter.

    // Prediction state variables
    bool found = false;                      // Indicates if a matching tag was found.
    int given_prediction_table = -1;         // Index of the table that provided the last prediction.
    bool prev_prediction = false;            // The previous prediction made.
    uint64_t global_history = 0;             // Global history of outcomes as a shift register.
    std::vector<std::vector<Entry>> tables;   // History tables.

    /**
     * Initializes the history tables with default entries.
     */
    void init_tables() {
        for (int table_num = 0; table_num < NUM_TABLES; table_num++) {
            int table_size = MAX_TABLE_SIZE >> table_num;
            tables.emplace_back(std::vector<Entry>(table_size, Entry{0, 0}));
        }
    }

    /**
     * Constructor that initializes the history tables.
     */
    RoboMemory() {
        init_tables();
    }

    /**
     * Makes a prediction based on the current state and branch ID.
     *
     * @param branch_id Unique identifier for the branch (planet ID).
     * @return Predicted time of day (true for day, false for night).
     */
    bool make_prediction(uint64_t branch_id) {
        uint64_t indices[NUM_TABLES];
        uint64_t tags[NUM_TABLES];

        // Compute indices and tags for each table.
        for (int table_num = 0; table_num < NUM_TABLES; table_num++) {
            int table_size = MAX_TABLE_SIZE >> table_num;
            uint64_t mask = (table_num > 0) ? BIT_MASK(uint64_t, (1 << table_num)) : 0;

            indices[table_num] = 
                (branch_id ^ 
                (global_history & mask)) %
                table_size;

            tags[table_num] = 
                (branch_id ^ 
                (global_history & mask)) &
                BIT_MASK(uint64_t, TAG_LENGTH);
        }

        found = false;
        bool final_prediction = false;

        // Check each table for a matching tag and determine the prediction.
        for (int table_num = 0; table_num < NUM_TABLES; table_num++) {
            if (tables[table_num][indices[table_num]].tag == tags[table_num]) {
                if (tables[table_num][indices[table_num]].confidence_counter > (MAX_CONFIDENCE + 1) / 2) {
                    final_prediction = true;
                }
                given_prediction_table = table_num;
                prev_prediction = final_prediction;
                found = true;
                break;
            }
        }

        if (!found) {
            given_prediction_table = -1;
        }

        return final_prediction;
    }

    /**
     * Updates the history tables based on the actual outcome.
     *
     * @param branch_id Unique identifier for the branch (planet ID).
     * @param outcome Actual outcome observed (true for day, false for night).
     */
    void update(uint64_t branch_id, bool outcome) {
        uint64_t indices[NUM_TABLES];
        uint64_t tags[NUM_TABLES];

        // Compute indices and tags for each table.
        for (int table_num = 0; table_num < NUM_TABLES; table_num++) {
            int table_size = MAX_TABLE_SIZE >> table_num;
            uint64_t mask = (table_num > 0) ? BIT_MASK(uint64_t, (1 << table_num)) : 0;

            indices[table_num] = 
                (branch_id ^ 
                (global_history & mask)) %
                table_size;

            tags[table_num] = 
                (branch_id ^ 
                (global_history & mask)) &
                BIT_MASK(uint64_t, TAG_LENGTH);
        }

        // Update the global history with the new outcome.
        global_history = (global_history << 1) | (outcome ? 1 : 0);

        if (given_prediction_table >= 0) {
            if (prev_prediction == outcome) { // Correct prediction.
                int id = given_prediction_table;
                if (outcome && tables[id][indices[id]].confidence_counter < MAX_CONFIDENCE) {
                    tables[id][indices[id]].confidence_counter++;
                } else if (!outcome && tables[id][indices[id]].confidence_counter > 0) {
                    tables[id][indices[id]].confidence_counter--;
                }
            } else { // Incorrect prediction.
                if (given_prediction_table < NUM_TABLES - 1) { // Allocate to next table.
                    int id = given_prediction_table + 1;
                    tables[id][indices[id]].tag = tags[id];
                    tables[id][indices[id]].confidence_counter = outcome ? 2 : 1;
                } else { // Already in the last table; update confidence.
                    int id = given_prediction_table;
                    if (outcome && tables[id][indices[id]].confidence_counter < MAX_CONFIDENCE) {
                        tables[id][indices[id]].confidence_counter++;
                    } else if (!outcome && tables[id][indices[id]].confidence_counter > 0) {
                        tables[id][indices[id]].confidence_counter--;
                    }
                }
            }
        } else { // No prediction was found; allocate to the first table.
            int id = 0;
            tables[id][indices[id]].tag = tags[id];
            tables[id][indices[id]].confidence_counter = outcome ? 2 : 1;
        }
    }
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
    // Currently, the spaceshipComputerPrediction parameter is unused.
    // The prediction is solely based on Robo's internal algorithm.
    return roboMemory_ptr->make_prediction(nextPlanetID);
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
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

// Optional: Add a main function for testing
#ifdef TEST_ROBO_PREDICTOR
int main() {
    RoboPredictor predictor;

    // Example planet IDs and actual outcomes
    std::uint64_t planetIDs[] = {12345, 67890, 13579, 24680, 11223, 44556};
    bool outcomes[] = {true, false, true, true, false, true};

    for (size_t i = 0; i < sizeof(planetIDs)/sizeof(planetIDs[0]); ++i) {
        bool prediction = predictor.predictTimeOfDayOnNextPlanet(planetIDs[i], false);
        std::cout << "Prediction for planet " << planetIDs[i] << ": " 
                  << (prediction ? "True (Day)" : "False (Night)") << std::endl;
        predictor.observeAndRecordTimeofdayOnNextPlanet(planetIDs[i], outcomes[i]);
    }

    return 0;
}
#endif
