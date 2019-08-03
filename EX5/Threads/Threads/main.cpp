//
//  main.cpp
//  Threads
//
//  Created by Harsh Patel on 3/13/19.
//  Copyright © 2019 Harsh Patel. All rights reserved.
//

#include <iostream>
#include <thread>
#include <stdlib.h>
#include <time.h>
#include <mutex>

using namespace std;
mutex waitPrint;


void foo(int id, int arg){
    
    srand (time(NULL));
    
    int random  = rand() % 10000;
    
    while (random!=arg) {
        random = rand() % 10000;
    }
    
    
    if (random==arg) {
        waitPrint.lock();
        cout<<"Thread "<<id<<" completed."<<endl;
        waitPrint.unlock();
        
    }
//    cout<<"Thread "<<id<<" completed."<<endl;
}

int main(int argc, const char * argv[]) {

    
    thread threads[10];
    
    for (int i=0; i!=10; i++) {
        threads[i] = thread(foo, i, stoi(argv[1]));

    }
    for (int i=0; i!=10; i++) {
        threads[i].join();
        
    }
    
    
    cout<<"All threads have finished finding numbers."<<endl;
    
    
    return 0;
}
