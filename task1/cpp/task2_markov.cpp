/*
Exercise 2: Simple Markov Chain for Day/Night Prediction

Objective: Implement a Markov chain that tracks the last two outcomes to predict the next day/night cycle. This builds skills in probability-based pattern prediction with minimal memory.

Background Knowledge:
	•	Bit Manipulation: Study bitwise operations (&, |, <<, >>) for efficient state encoding.
	•	Structs: Learn about struct to group data together, which is useful for storing transition counts.
	•	Conditionals and Loops: Understand if statements and for loops for counting and updating predictions.

Steps:
	1.	Design State Representation: Use two bits to represent the last two outcomes (e.g., 00 for two nights, 01 for night-day).
	2.	Track Counts: For each two-bit state, count transitions to Day/Night in a struct.
	3.	Predict Outcome: Use the counts to decide the most likely next state.

Getting Started in C++:
	•	Practice with bitwise operations, e.g., int current_state = (prev << 1) | next;.
	•	Use a struct like struct Markov { int day_count; int night_count; }; to store transition counts.
	•	Test with std::cout to verify the transitions.
*/