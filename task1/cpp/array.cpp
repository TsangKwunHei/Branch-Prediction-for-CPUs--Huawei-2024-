#include <iostream>
#include <deque>

int main() {
    /*  1 create an array, 
        2 add elements, 
        3 limit the size to keep only recent values. in cpp */





    //Ex 3 - Deque
     std::vector<int> vector2;
     const size_t limit = 5;
     
     auto add_to_deque = [&](int value){
         vector2.push_back(value);
         if(vector2.size() > limit){
             vector2.erase(vector2.begin());
         }
     };

        add_to_deque(1);
        add_to_deque(2);
        add_to_deque(3);
        add_to_deque(4);
        add_to_deque(5);
        add_to_deque(6); // This will cause the oldest element (1) to be removed


    //Ex 2.1 - Dynamic Array
     std::vector<int> vector1; // Declare a vector of integers

     vector1.push_back(1);
     vector1.push_back(1); // Each push_back call adds a new element to the vector.
     vector1.push_back(1);
     vector1.push_back(2);
     vector1.push_back(3); // push_back method automatically resizes the vector
     vector1.resize(3); // Resized to : 1 1 1
    //Ex 2 - Adding elements to a Array
    //Arrays have a fixed size, can't dynamically add elements beyond the initial size.
    int arr2[3];
    arr2[0] = 1;
    arr2[1] = 2;
    arr2[2] = 3;
    arr2[2] = 2; //The final value at arr2[2] will be 2.

    //Ex 1 - Array
    int arr1[5] = {1,2,3,4,5};
    for(int i=0;i<5;i++){
        std::cout << arr1[i] << " ";
    }
    std::cout << std::endl;
    return 0;


}