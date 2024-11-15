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

struct RoboPredictor::RoboMemory {
    // * ========================   PART 1   =================================
    // * Setting up history tables and speculative branches.

    // Configure TAGE History Sizes (Different Lengths for Geometric Sequence)
    static const int MIN_HISTORY = 4;
    static const int MAX_HISTORY = 100;
    static const int total_number_of_history_tables = 5;

    // History tables containing individual histories
    std::array<std::vector<bool>, total_number_of_history_tables> history_tables;
    std::array<int, total_number_of_history_tables> confidence;
    std::array<int, total_number_of_history_tables> history_tables_sizes;

    // Constructor to initialize member variables
    RoboMemory()
        : head(0),
          size(1024),
          speculative_bits(0),
          fold_history_counter(1),
          last_outcome(false),
          hit_bank(0),
          alt_bank(0),
          first_table_match_j(0),
          second_table_match_j(0),
          finale_prediction(false),
          finale_prediction_bank(0) {
        buffer.resize(size);
        confidence.fill(0);
        history_tables_sizes = {MIN_HISTORY, MIN_HISTORY * 2, MIN_HISTORY * 4, MIN_HISTORY * 8, MIN_HISTORY * 16};
        foleded_history_tables_sizes.fill(0);
    }

    int max_speculative_branches = 5;

    // Long History Register (Circular Buffer for History)
    std::vector<bool> buffer;
    int head;
    int size;

    void addToBuffer(bool value) {
        buffer[head] = value;
        head = (head + 1) % size;
    }

    void add_to_history_tables(bool data) {
        // Push data into all history tables
        for (int i = 0; i < total_number_of_history_tables; i++) {
            history_tables[i].push_back(data);
            if (history_tables[i].size() > history_tables_sizes[i]) {
                history_tables[i].erase(history_tables[i].begin());
            }
        }
    }

    // Rewind (not implemented in this context)
    void rewind(int num_bits_to_remove) {
        // Implementation can be added if needed
    }

    // Retire speculative bits (not fully implemented)
    int speculative_bits;
    void retire(int num_bits_to_retire) {
        int index = head - speculative_bits;
        add_to_history_tables(buffer[index % size]);
        // Additional implementation can be added if needed
    }

    // Hash function for branch address and history
    int entry_size = 3;
    int hashInt1(int branch_num, bool bit) {
        int combined = (branch_num << 1) | bit;
        return combined * 2654435761u;
    }

    // Folding the history tables
    int fold_parameters = 2; // Fold to half size
    std::array<std::vector<std::pair<bool, int>>, total_number_of_history_tables> foleded_history_tables;
    std::array<int, total_number_of_history_tables> foleded_history_tables_sizes;

    std::vector<std::pair<bool, int>> fold_history(
        const std::vector<bool>& list,
        int branch_num,
        bool bit) {
        int compressed_size = list.size() / fold_parameters;
        std::vector<std::pair<bool, int>> compressed(compressed_size);
        int history_hash = hashInt1(list.size(), true);
        int folded_history_hash = hashInt1(compressed_size, true);

        for (int i = 0; i + 1 < list.size(); i += fold_parameters) {
            int folded_tag = hashInt1(branch_num, bit) + folded_history_hash + history_hash;
            compressed[i / fold_parameters] = std::make_pair(list[i] ^ list[i + 1], folded_tag);
        }
        return compressed;
    }

    void fold_all_history(int branch_num, bool bit) {
        for (int i = 0; i < total_number_of_history_tables; i++) {
            foleded_history_tables[i] = fold_history(history_tables[i], branch_num, bit);
            foleded_history_tables_sizes[i] = history_tables_sizes[i] / fold_parameters;
        }
    }

    int fold_history_counter;
    bool last_outcome;
    void push_to_history_tables(int branch_num, bool outcome) {
        hashInt1(branch_num, outcome);

        if (fold_history_counter == 1) {
            last_outcome = outcome;
            fold_history_counter += 1;
        } else {
            bool folded_outcome = last_outcome ^ outcome;
            fold_history_counter = 1;
            for (int i = 0; i < total_number_of_history_tables; i++) {
                int history_hash = hashInt1(history_tables_sizes[i], true);
                int folded_tag = hashInt1(branch_num, outcome) + history_hash;

                std::pair<bool, int> pair = std::make_pair(folded_outcome, folded_tag);
                foleded_history_tables[i].push_back(pair);
                if (foleded_history_tables[i].size() > foleded_history_tables_sizes[i]) {
                    foleded_history_tables[i].erase(foleded_history_tables[i].begin());
                }
            }
        }
    }

    int hit_bank; // Longest matching table
    int alt_bank; // Second longest matching table
    int first_table_match_j; // Index in longest matching table
    int second_table_match_j; // Index in second longest matching table

    // * ======================   II PREDICTOR   ============================
    // * Find matching tables and generate predictions

    void find_two_longest_table_matches(int branch_num, bool bit) {
        bool found_first_match = false;
        bool found_second_match = false;
        int current_bit = hashInt1(branch_num, bit);

        for (int i = total_number_of_history_tables - 1; i >= 0 && !found_second_match; i--) {
            for (int j = static_cast<int>(foleded_history_tables[i].size()) - 1; j >= 0; j--) {
                int index = foleded_history_tables[i][j].second;
                if (index == current_bit) {
                    if (!found_first_match) {
                        found_first_match = true;
                        first_table_match_j = j;
                        hit_bank = i;
                    } else {
                        found_second_match = true;
                        second_table_match_j = j;
                        alt_bank = i;
                        break;
                    }
                }
            }
        }
    }

    bool generate_prediction(int bank) {
        int pattern_true = 0;
        int pattern_false = 0;
        int size = static_cast<int>(history_tables[bank].size());
        for (int i = size - 1; i >= 0; i--) {
            if (history_tables[bank][i]) {
                pattern_true += 1;
            } else {
                pattern_false += 1;
            }
        }
        speculative_bits += 1;
        return pattern_true > pattern_false;
    }

    // Making the final prediction
    bool finale_prediction;
    int finale_prediction_bank;

    void choosing_predictions(bool& prediction, int branch_num) {
        int prediction_strength;
        int confidence_longest_table = confidence[hit_bank] - confidence[alt_bank];

        if (confidence_longest_table < 0) {
            prediction_strength = -confidence_longest_table;
            prediction = generate_prediction(alt_bank);
            finale_prediction_bank = alt_bank;
        } else {
            prediction = generate_prediction(hit_bank);
            prediction_strength = confidence_longest_table;
            finale_prediction_bank = hit_bank;
            finale_prediction = prediction;
        }
    }
    // * ===================    III FEEDBACK   =========================
    // * receive results and update tables

    // Feedback loop to learn from prediction outcomes
    std::vector<bool> speculative;
    std::vector<std::vector<bool>> speculative_backups;
    int backup;

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

    int MAX_CONFIDENCE = 7;

    void update_confidence(int table_index, bool wasCorrect) {
        if (wasCorrect) {
            confidence[table_index] = std::min(confidence[table_index] + 1, MAX_CONFIDENCE);
        } else {
            confidence[table_index] = std::max(confidence[table_index] - 1, 0);
        }
    }

    void feedback_loop_main(bool actual, int branch) {
        bool is_correct = compare_predict_outcome(finale_prediction, actual);
        update_confidence(finale_prediction_bank, is_correct);
        if (!is_correct) {
            speculative_rollback();
        } else {
            speculative_checkpoint();
        }
        push_new_bits(actual);
        push_to_history_tables(branch, actual);
    }

    void push_new_bits(bool outcome) {
        add_to_history_tables(outcome);
    }
};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
    bool planet_prediction;
    roboMemory_ptr->find_two_longest_table_matches(nextPlanetID, spaceshipComputerPrediction);
    roboMemory_ptr->choosing_predictions(planet_prediction, nextPlanetID);
    return planet_prediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
    roboMemory_ptr->feedback_loop_main(timeOfDayOnNextPlanet, nextPlanetID);
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
