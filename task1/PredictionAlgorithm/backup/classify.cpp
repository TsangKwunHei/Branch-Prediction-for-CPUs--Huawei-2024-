#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <deque>
#include "PredictionAlgorithm.hpp"
#define PO1  0.5
#define PO2  0.5
#define PO3  0.5
#define PO4  0.5
#define TABLE  4
// RoboMemory within RoboPredictor class
struct RoboPredictor::RoboMemory {
  
  uint64_t last_state = 0 | 1;
  // {P_day_to_day, P_day_to_night, P_night_to_night, P_night_to_day};

  float probabilities[TABLE];
  
  std::array<float, 4> confidence;
    void init () {
    for (uint64_t i = 0; i < confidence.size(); i++) {
        confidence[i] = 3;
    }

    probabilities[0] = PO1;
    probabilities[1] = PO1;
    probabilities[2] = PO1;
    probabilities[3] = PO1;
  
  }
  RoboMemory () {
      init () ;
  }

  float confidence_weight = 0.1;
  const float increment = 0.05;
  const float decrement = 0.03;
  bool last_prediction;
  uint64_t decision = 0 | 1 | 2 | 3;
  uint64_t round_count;

  struct States {
    uint64_t last_state_test = 0 | 1;
  };
/*

   */
// turns out no diff between class and struct except class is priv by default 
class Predictor {
  public: 
  void predict (bool& prediction, bool last_state, RoboMemory* robo_memory ){

    bool last_prediction = -1;
    float day_to_day = robo_memory->probabilities[0] * (1 + robo_memory->confidence_weight * robo_memory->confidence[0]);
    float day_to_night = robo_memory->probabilities[1] * (1 + robo_memory->confidence_weight * robo_memory->confidence[1]);
    float night_to_night = robo_memory->probabilities[2] * (1 + robo_memory->confidence_weight * robo_memory->confidence[2]);
    float night_to_day = robo_memory->probabilities[3] * (1 + robo_memory->confidence_weight * robo_memory->confidence[3]);

    // 1.2 Determine prediction based on the 1.1 probabilities

      if (last_state == 1) {
        if (day_to_day > day_to_night) {
            prediction = true;
            robo_memory->decision = 0;
        } else {
            prediction = false;
            robo_memory->decision = 1;
        }} 
      
      else {
        if (night_to_night > night_to_day) {
            prediction = true;
            robo_memory->decision = 2;
        } else {
            prediction = false;
            robo_memory->decision = 3;
        }};

        robo_memory->last_prediction = prediction;

    // Simple prediction policy: follow the spaceship computer's suggestions
    // return spaceshipComputerPrediction;
    // now for i am using my own prediction
      }; 
    
template <class CONFIG>
void feedback_loop(bool timeOfDayOnNextPlanet, RoboMemory* roboMemory_ptr ) {
   if (timeOfDayOnNextPlanet == true){
        roboMemory_ptr->last_state = 1;
    } else {
        roboMemory_ptr->last_state = 0;
   }

   RoboMemory::States last_state_struct;
   last_state_struct.last_state_test= roboMemory_ptr->last_state;
   if (roboMemory_ptr->last_prediction == timeOfDayOnNextPlanet) {
    roboMemory_ptr->probabilities[roboMemory_ptr->decision] += CONFIG::increment;
    roboMemory_ptr->confidence[roboMemory_ptr->decision] += 1;
    if( roboMemory_ptr->decision % 2 == 0  ){
        roboMemory_ptr->probabilities[roboMemory_ptr->decision + 1] -= 0.5;    
    } else {
        roboMemory_ptr->probabilities[roboMemory_ptr->decision - 1] -= 0.5;
    }
   }
   else {
    roboMemory_ptr->probabilities[roboMemory_ptr->decision] -= 0.3;
    roboMemory_ptr->confidence[roboMemory_ptr->decision] -= 1;
    if( roboMemory_ptr->decision % 2 == 0  ){
        roboMemory_ptr->probabilities[roboMemory_ptr->decision + 1] += 0.3;    
    } else {
        roboMemory_ptr->probabilities[roboMemory_ptr->decision - 1] += 0.3;
    }
   }

   roboMemory_ptr->round_count += 1;
   if (roboMemory_ptr->round_count > 99){
    roboMemory_ptr->round_count = 0;
    for (auto& probabliity : roboMemory_ptr->probabilities){
       if (probabliity > 0.9){
           probabliity = 0.9;
       } else if (probabliity < 0.1){
           probabliity = 0.1;
       }
    }
    for (auto& confidence : roboMemory_ptr->confidence){
          confidence *= 0.9;
    }

   };
};
};
  

struct CONFIGS{
  struct CONFIG_SMALL {
    static constexpr  float increment = 0.05;
    static constexpr  float decrement = 0.03;
  };

  struct CONFIG_HEAVY {
    static constexpr  float increment = 0.1;
    static constexpr  float decrement = 0.03;
  };
};


};

bool RoboPredictor::predictTimeOfDayOnNextPlanet(
    std::uint64_t nextPlanetID, bool spaceshipComputerPrediction) {

  bool prediction;
  RoboMemory memory;
  RoboMemory::Predictor predictor_class ;
  RoboMemory::States last_state_in_struct;
  uint64_t last = memory.last_state; // can use memory.last_state directly
  //Adding & makes it a reference, allowing direct access without copying the original object.
  predictor_class.predict(prediction, last_state_in_struct.last_state_test, &memory );

  return prediction;
}

void RoboPredictor::observeAndRecordTimeofdayOnNextPlanet(
    std::uint64_t nextPlanetID, bool timeOfDayOnNextPlanet) {
    RoboMemory::Predictor memory;
    RoboMemory robomemory;
    memory.feedback_loop<RoboMemory::CONFIGS::CONFIG_HEAVY>(timeOfDayOnNextPlanet, &robomemory);
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
