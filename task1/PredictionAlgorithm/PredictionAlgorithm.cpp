#include "PredictionAlgorithm.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <deque>
#include <cmath>
#include <cassert>
#include <string>
#include <chrono>

// RoboMemory within RoboPredictor class
struct RoboPredictor::RoboMemory {
  class Perceptron {
  public:
      std::vector<int> weights;
      int N;
      int bias;
      int threshold;

      Perceptron(int N_) : N(N_), bias(0), threshold(2 * N + 14), weights(N_, 0) {}

      // Predict function returns a pair of prediction and running_sum
      std::pair<int, int> predict(const std::deque<int>& global_branch_history) const {
          int running_sum = bias;
          for (int i = 0; i < N; ++i) {
              running_sum += global_branch_history[i] * weights[i];
          }
          int prediction = (running_sum < 0) ? -1 : 1;
          return {prediction, running_sum};
      }

      // Update function modifies weights and bias based on prediction and actual outcome
      void update(int prediction, int actual, const std::deque<int>& global_branch_history, int running_sum) {
          if ((prediction != actual) || (std::abs(running_sum) < threshold)) {
              bias += actual;
              for (int i = 0; i < N; ++i) {
                  weights[i] += actual * global_branch_history[i];
              }
          }
      }
  };

  // Type alias for branch trace
  using Branch = std::pair<std::string, int>;

  // perceptron_pred function
  std::pair<int, int> perceptron_pred(
    const std::vector<Branch>& trace, 
    int l = 1,
    size_t tablesize = 0) {

    std::deque<int> global_branch_history;
    global_branch_history.assign(l, 0);

    std::unordered_map<size_t, Perceptron> p_list;
    int num_correct;

    for (int br = 0; br < trace.size(); br++) {

    }

  
  }

  // Function to calculate table size based on ratio and k
  int getTableSize(double ratio, int k) {
      return static_cast<int>(ratio * k);
  }
};
bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {
 
  return  spaceshipComputerPrediction;
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
