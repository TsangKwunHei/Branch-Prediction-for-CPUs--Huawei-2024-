#include <iostream>
#include <deque>
#include <vector>

// Circular buffer for branch history
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
// Step 3: Modify foldHistory() to Allow Dynamic Folding

/* 3.1 Dynamic Folding for Variable History Lengths:
   Modify foldHistory to fold over different history lengths. 
   Instead of XORing every adjacent pair, you can XOR based on the history length size.
   - Example: For 16 bits folded to 3 bits, you might XOR every 4 bits.

   Variable-Length Folding:
   Enable folding based on the required history length.
*/

/* 3.2 Maintain Folded Histories for Each Prediction Table:
   Add a separate folded history for each prediction table (e.g., short, mid, long).
   This ensures each table can operate independently on its own folded history data.

| Folded History    | ->      Compresses each history into a unique index per table for faster access.
*/

// Folded history function
std::vector<bool> folded;
std::vector<bool> short_term_folded(4,0) ;
std::vector<bool> mid_term_folded(4,0);
std::vector<bool> long_term_folded(4,0);
size_t head;
size_t size = 4;

void addToFold_mid(bool value) {
    mid_term_folded[head] = value;
    head = (head + 1) % mid_term_folded.size();
}

void addToFold_short(bool value) {
    short_term_folded[head] = value;
    head = (head + 1) % short_term_folded.size();
}

void addToFold_long(bool value) {
    long_term_folded[head] = value;
    head = (head + 1) % long_term_folded.size();
}

void fold_History_short_term(std::vector<bool> vector) {
    for (int i = 0; i < vector.size(); i += 2) {
        bool x = vector[i] ^ vector[i + 1];
        addToFold_short(x);
    }
}

void fold_History_mid_term(std::vector<bool> vector) {
    for (int i = 0; i < vector.size(); i += 2) {
        bool x = vector[i] ^ vector[i + 1];
        addToFold_mid(x);
    }
}

void fold_History_long_term(std::vector<bool> vector) {
    for (int i = 0; i < vector.size(); i += 2) {
        bool x = vector[i] ^ vector[i + 1];
        addToFold_long(x);
    }
}
// Step 1: Set Up Multiple Prediction Tables for Varying History Lengths

/* 1.1 Define Prediction Tables with Separate History Lengths:
   Define multiple prediction tables (e.g., short, mid, long-term history) 
   with different sizes based on the varying history lengths they represent. 
   Use a separate std::vector<int> for each table to track counters.
   
| Short-Term Prediction Table | ->     Tracks predictions based on recent history (e.g., 4 bits)
| Mid-Term Prediction Table   | ->     Tracks predictions based on moderate history (e.g., 8 bits)
| Long-Term Prediction Table  | ->     Tracks predictions based on extended history (e.g., 16 bits)
*/

// Prediction table setup
std::vector<int> predictionTable(8,0);
std::vector<int> short_term_predictTable(4, 0);
std::vector<int> mid_term_predictTable(8, 0);
std::vector<int> long_term_predictTable(16, 0);

// Step 2: Implement Saturating Counter with Replacement Policy

/* 2.1 Define Saturating Counters:
   Implement counters for each prediction table. The counters should have a range 
   (e.g., -3 to 3) to indicate prediction strength, where positive values favor “taken” and
   negative values favor “not taken.”

   Saturating Counter: 
   A counter that increments/decrements but caps at a maximum and minimum value.
*/
/* 2.2 Add Replacement Policy:
   Create a replacement function that checks each counter’s value after predictions.
   - If a counter is at a low confidence level (e.g., -3), replace it with a new entry.
   - Implement this function to run periodically or whenever confidence is below a certain threshold.

| Replacement Policy | ->      Keeps high-confidence predictions and replaces low-confidence entries. 
*/


void long_term_prediction_update(bool outcome) {
    for (int i = 0; i < long_term_predictTable.size(); i++) {
        if (outcome == long_term_predictTable[i]) {
            std::max(long_term_predictTable[i]+1, 0);
            total_correct_predictions++;
        } else {
            long_term_predictTable[i]--;
            if (long_term_predictTable[i] < -3) {
                long_term_predictTable[i] = 0;
            }
        }
    }
}

void mid_term_prediction_update(bool outcome) {
    for (int i = 0; i < mid_term_predictTable.size(); i++) {
        if (outcome == mid_term_predictTable[i]) {
            std::max(mid_term_predictTable[i]+1, 0);
            total_correct_predictions++;
        } else {
            mid_term_predictTable[i]--;
            if (mid_term_predictTable[i] < -3) {
                mid_term_predictTable[i] = 0;
            }
        }
    }
}
void short_term_prediction_update(bool outcome) {
    for (int i = 0; i < short_term_predictTable.size(); i++) {
        if (outcome == short_term_predictTable[i]) {
            std::max(short_term_predictTable[i]+1,0);
            total_correct_predictions++;
        } else {
            short_term_predictTable[i]--;
            if (short_term_predictTable[i] < -3) {
                short_term_predictTable[i] = 0;
            }
        }
    }
}

// Step 4: Update Prediction Logic with Confidence Management

/* 4.1 Adjust Counter Based on Prediction Outcome:
   Implement a feedback loop that adjusts each prediction table’s counter.
   - If prediction matches outcome, increase the counter (up to max).
   - If prediction mismatches outcome, decrease the counter (down to min).
   
   Confidence-Based Update:
   Keeps predictions adaptable by strengthening correct predictions.
*/

/* 4.2 Set Confidence Thresholds to Manage Entry Replacement:
   Add a check to decide whether to replace an entry based on counter values.
   - If the prediction is incorrect and confidence is low, consider replacing the entry.
   - Track and replace based on a defined confidence threshold.

| Confidence Threshold | ->     Ensures that the prediction tables maintain high-accuracy entries only.
*/



/* 4.3 Implement Final Prediction Based on Longest Matched History:
   Modify getPrediction to consult the longest matching history length table first, 
   then fall back to shorter histories if the match is uncertain or less confident.

   Longest Match First:
   Enhances prediction accuracy by using the deepest (most reliable) history first.
*/



// Retrieve and update predictions
bool getPrediction(int index) {
    if (long_term_predictTable[index] >= 0){
        return long_term_predictTable[index] >= 0;}
    else if (mid_term_predictTable[index] >= 0) {
        return mid_term_predictTable[index] >= 0;
    } 
    else if (short_term_predictTable[index] >= 0) {
        return short_term_predictTable[index] >= 0;
    } 
    else {return false;}
}

int countdown = 0;

void countdown_function() {
    countdown++;
    if (countdown == 100) {
        for (int i = 0; i < predictionTable.size(); i++) {
            std::round(predictionTable[i] *= 0.9);
        }
    }
}

int total_correct_predictions = 0;

// Prediction feedback loop
class observer {
public:
    observer(bool outcome) : outcome(outcome) {}

    void updatePrediction(bool outcome);
    void addToBuffer(bool outcome);
    int counter = +1;
    
private:
    bool outcome;
    std::vector<bool> buffer;
};
