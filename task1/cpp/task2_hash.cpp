/*
Exercise 1: Fixed-Size Hash Map for Planet Tags (No STL)

Objective: Design a small, efficient hash map to store and retrieve planet patterns. This will sharpen memory management skills essential for Robo’s limited memory.

Background Knowledge:
	•	Arrays: Learn how to declare and initialize arrays, which will be used for fixed-size storage.
	•	Hashing: Study basic hashing (e.g., mod-based hashing) to map planet IDs to specific memory locations.
	•	Collision Handling: Learn linear probing (moving to the next slot if one is occupied), as it’s simple and doesn’t require extra memory.

Steps:
	1.	Setup Hash Map Structure: Declare a fixed-size array to act as your hash map.
	2.	Implement Basic Hashing: Write a function that maps planet IDs to an index.
	3.	Handle Collisions: Write logic to move to the next available index if a collision occurs.

Getting Started in C++:
	•	Use int hash_map[SIZE]; to declare an array and void insert(int key, int value); for your insert function.
	•	Write a simple main() to test inserting and retrieving values.
*/