////
////  main.cpp
////  Ex7
////
////  Created by Harsh Patel on 4/1/19.
////  Copyright © 2019 Harsh Patel. All rights reserved.
////
//
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main(void){
    ifstream file{"pipe"};
    string failure;
    string check = "Operation failed.";
    int count = 0;
    while(getline(file, failure)){
        
        //    std::cout << line << '\n';
        if(failure.compare(check)==0){
            cout<<"Program failed on operation "<<++count<<std::endl;
            return 0;
        }
        count++;
    }
    
    
    cout<<"Program failed on operation 1"<< endl;
    
    return 0;
    
}
