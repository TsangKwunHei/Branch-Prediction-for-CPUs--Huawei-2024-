#include <iostream>
#include <deque>

int main() {
    /*  1 Use a dictionary to store counts just two patterns, 0 and 1. 
        2 Add new values and update counts as needed.  */

        std::unordered_map<int,int> pattern_count;
        
        pattern_count[0]=0;
        pattern_count[1]=0;

        int data[] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};

        for (int value : data) {
            pattern_count[value]++;
        }

        // Print the counts
        std::cout << "Count of pattern 0: " << pattern_count[0] << std::endl;
        std::cout << "Count of pattern 1: " << pattern_count[1] << std::endl;

        return 0;

}