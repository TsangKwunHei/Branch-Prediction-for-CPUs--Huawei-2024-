#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <deque>
#include <array>
#include <cstddef>
#include <cassert>
#include <algorithm>
#include "PredictionAlgorithm.hpp"

// Assuming RoboPredictor is declared in PredictionAlgorithm.hpp
// and has a nested struct RoboMemory and member pointer roboMemory_ptr.

struct RoboPredictor::RoboMemory {
    // ======================================================================================================
    // ======================================  I INIT   ===================================================
    // ======================================================================================================

    // Define the Entry struct before using it
    struct Entry {
        bool branch_outcome; // True or False outcome of the branch
        int confidence;      // Confidence counter
        int tag;             // Encoded tag (branch address + recent history)

        // Constructor for initializing an entry
        Entry(bool outcome, int initial_confidence, int tag_value)
            : branch_outcome(outcome), confidence(initial_confidence), tag(tag_value) {}
    };

    // Configure TAGE History Sizes (Different Lengths for Geometric Sequence)
    static const int MIN_HISTORY = 4;
    static const int MAX_HISTORY = 100;
    static const int fold_factor = 4;
    static const int total_number_of_history_tables = 5;

    // History tables containing individual histories
    std::array<std::vector<Entry>, total_number_of_history_tables> history_tables;
    std::array<int, total_number_of_history_tables> history_tables_sizes;
    std::array<std::vector<int>, total_number_of_history_tables> folded_history_tables; // Changed from reference
    std::array<int, total_number_of_history_tables> folded_history_tables_sizes;      // Fixed typo

    // Confidence array
    std::array<int, total_number_of_history_tables> confidence;

    // Long History Register (Circular Buffer for History)
    std::vector<bool> buffer;
    int head;
    int size;

    // Speculative bits
    int speculative_bits;
    bool finale_prediction;
    int finale_prediction_bank;
    int finale_prediction_index;

    // Speculative state
    std::vector<bool> speculative;
    std::vector<std::vector<bool>> speculative_backups;

    // Constructor to initialize member variables
    RoboMemory()
        : head(0),
          size(1024),
          speculative_bits(0),
          finale_prediction(false),
          finale_prediction_bank(0),
          finale_prediction_index(0) {
        buffer.resize(size);
        history_tables_sizes = {MIN_HISTORY, MIN_HISTORY * 2, MIN_HISTORY * 4, MIN_HISTORY * 8, MIN_HISTORY * 16};
        folded_history_tables_sizes.fill(0);
        confidence.fill(0); // Initialize confidence levels to 0
    }
    // Function to add entries to history tables
    void add_to_history_tables(bool data, int branch_address) {
        // Update all history tables
        for (int i = 0; i < total_number_of_history_tables; i++) {
            // Compute the tag for the current entry
            int tag = hashInt1(branch_address, history_tables[i].size());
            // Create a new entry with the outcome and computed tag
            Entry new_entry = {data, 5, tag};
            // Add the new entry to the current table
            history_tables[i].push_back(new_entry);
            // Ensure the table does not exceed its maximum size
            if (history_tables[i].size() > history_tables_sizes[i]) {
                history_tables[i].erase(history_tables[i].begin());
            }
        }
    }

    int hashInt1(int branch_address, int history_size) {
        return (branch_address ^ history_size) * 2654435761u;
    }

    // Rewind (not implemented in this context)
    void rewind(int num_bits_to_remove) {
        // Implementation can be added if needed
    }

    // Retire speculative bits (not fully implemented)
    void retire(int num_bits_to_retire) {
        // Additional implementation can be added if needed
    }

    // Folding history
    void FoldHistory() {
        // Loop through each history table
        for (size_t table_idx = 0; table_idx < total_number_of_history_tables; ++table_idx) {
            const auto& current_table = history_tables[table_idx];
            auto& current_folded_table = folded_history_tables[table_idx];

            // Ensure folded table size matches the expected folded size
            size_t folded_size = current_table.size() / fold_factor;
            if (current_folded_table.size() != folded_size) {
                current_folded_table.resize(folded_size, 0);
            }

            // Update the folded table incrementally
            for (size_t i = 0; i < folded_size; ++i) {
                // Calculate indices for the fold segment
                size_t start_index = i * fold_factor;
                size_t end_index = std::min(start_index + fold_factor, current_table.size());

                // Update the folded entry (using XOR of tags within the fold segment)
                int folded_value = 0;
                for (size_t j = start_index; j < end_index; ++j) {
                    folded_value ^= current_table[j].tag; // XOR tags to compute folded value
                }

                // Store the updated folded value
                current_folded_table[i] = folded_value;
            }
        }
    }

    // Struct to hold match results
    struct MatchResult {
        int hit_bank;    // Index of the longest matching table
        int alt_bank;    // Index of the second-longest matching table
        int hit_index;   // Matching entry index in the longest matching table
        int alt_index;   // Matching entry index in the second-longest matching table
    };

    // Function to check the longest matching tables
    MatchResult CheckLongestMatchingTable(int branch_address, int global_history_bits) {
        MatchResult result = {-1, -1, -1, -1}; // Initialize result with invalid indices
        int best_match_length = -1;              // Best history length for the longest match
        int second_best_match_length = -1;       // Best history length for the second-longest match

        // Iterate through each table, starting from the longest history table
        for (int table_idx = total_number_of_history_tables-1; 0 < total_number_of_history_tables; --table_idx) {
            const auto& current_table = history_tables[table_idx];
            int history_length = history_tables_sizes[table_idx];

            // Search through the entries in the current table
            for (size_t entry_idx = 0; entry_idx < current_table.size(); ++entry_idx) {
                const Entry& entry = current_table[entry_idx];

                // Check if the tag matches the incoming branch address and global history
                if (entry.tag == HashTag(branch_address, global_history_bits, history_length)) {
                    if (history_length > best_match_length) {
                        // Update second-best match to the current best match
                        second_best_match_length = best_match_length;
                        result.alt_bank = result.hit_bank;
                        result.alt_index = result.hit_index;

                        // Update the longest match
                        best_match_length = history_length;
                        result.hit_bank = static_cast<int>(table_idx);
                        result.hit_index = static_cast<int>(entry_idx);
                    } else if (history_length > second_best_match_length) {
                        // Update the second-best match
                        second_best_match_length = history_length;
                        result.alt_bank = static_cast<int>(table_idx);
                        result.alt_index = static_cast<int>(entry_idx);
                    }
                }
            }
        }

        return result;
    }
    // Hash function for tagging 
    int HashTag(int branch_address, int global_history_bits, int history_length) {
        return branch_address ^ (global_history_bits & ((1 << history_length) - 1));
    }



    // Making the final prediction
    void choosing_predictions(bool& prediction, int branch_num) {
        
        // Example placeholder for global_history_bits; replace with actual history retrieval
        int global_history_bits = folded_history_tables[total_number_of_history_tables-1].size();

        // Find two longest matching tables
        MatchResult match = CheckLongestMatchingTable(branch_num, global_history_bits);

        int hit_bank = match.hit_bank;
        int hit_index = match.hit_index;
        int alt_bank = match.alt_bank;
        int alt_index = match.alt_index;

        if (hit_bank > 0) {
            // Access the tagged entry in the hit_bank at the computed index
            prediction = history_tables[hit_bank][hit_index].branch_outcome;
            finale_prediction_bank = hit_bank;
            finale_prediction_index = hit_index;
        } else if (hit_bank <= 0) { // No match found
            prediction = false; // default prediction
            finale_prediction_bank = -1;
            return;
        }
        /*
        if (alt_bank > 0) {
            // Access the tagged entry in the hit_bank at the computed index
            prediction = history_tables[alt_index][alt_index].branch_outcome; };
        */

        int prediction_strength;
        int confidence_longest_table = (hit_bank != -1 && alt_bank != -1) ? (confidence[hit_bank] - confidence[alt_bank]) : confidence[hit_bank];

    }

    // Feedback loop to learn from prediction outcomes
    void speculative_checkpoint() {
        speculative_backups.push_back(speculative);
        if (speculative_backups.size() > 5) {
            speculative_backups.erase(speculative_backups.begin());
        }
    }

    void speculative_rollback() {
        if (!speculative_backups.empty()) {
            speculative = speculative_backups.back();
            speculative_backups.pop_back();
        }
    }

    bool compare_predict_outcome(bool prediction, bool actual) {
        return prediction == actual;
    }

    const int MAX_CONFIDENCE = 7;

    void update_confidence(int table_index, int entry_index, bool wasCorrect) {
        if (table_index < 0 || table_index >= total_number_of_history_tables) {
            return; // Invalid index, do nothing
        }
        if (wasCorrect) {
            history_tables[table_index][entry_index].confidence = 
            std::min(history_tables[table_index][entry_index].confidence + 1, MAX_CONFIDENCE);
        } else {
            history_tables[table_index][entry_index].confidence = 
            std::max(history_tables[table_index][entry_index].confidence - 1, 0);
        }
    }

    void feedback_loop_main(bool actual, int branch) {
        bool is_correct = compare_predict_outcome(finale_prediction, actual);
        if (finale_prediction_bank != -1) {
            update_confidence(finale_prediction_bank,finale_prediction_index , is_correct);
        }
        if (!is_correct) {
            speculative_rollback();
        } else {
            speculative_checkpoint();
        }
        add_to_history_tables(actual, branch);
        FoldHistory();
    }

    // Function to add to circular buffer
    void addToBuffer(bool value) {
        buffer[head] = value;
        head = (head + 1) % size;
    }

};

// Implementation of RoboPredictor methods
bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
    bool planet_prediction;
    roboMemory_ptr->choosing_predictions(planet_prediction, static_cast<int>(nextPlanetID));
    return planet_prediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
    roboMemory_ptr->feedback_loop_main(timeOfDayOnNextPlanet, static_cast<int>(nextPlanetID));
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
