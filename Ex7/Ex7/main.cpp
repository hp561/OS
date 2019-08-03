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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <algorithm>
#include <sys/wait.h>
using namespace std;

int main(int argc, char * argv[]){
    
    int fd1[2];
    int fd2[2];
    int fd3[2];
    int fd4[2];

    int nums[5];
//    char* convert = argv[1];
    nums[0] = atoi(argv[1]);
    nums[1] = atoi(argv[2]);
     nums[2] = atoi(argv[3]);
     nums[3] = atoi(argv[4]);
     nums[4] = atoi(argv[5]);


    /* Ensure all pipes are created successfully */
    if (pipe(fd1) == -1) {
        cout<<"Pipe Failed";
        return 1;
    }
    if (pipe(fd2) == -1) {
        cout<<"Pipe Failed";
        return 1;
    }
    if (pipe(fd3) == -1) {
        cout<<"Pipe Failed";
        return 1;
    }
    if (pipe(fd4) == -1) {
        cout<<"Pipe Failed";
        return 1;
    }

    pid_t p;
    p=fork();

    if (p<0) {
        cout<<"Fork failed."<<endl;
        return 0;
    }

    else if (p>0){

        
        int numsSorted [5];
        
        
        close(fd1[0]);
        write(fd1[1],nums, 5*sizeof(int));  //Pass numbers to child
        close(fd1[1]);   //close write end
        //wait until child process finishes
        wait(NULL);
//        for (int i =0 ; i!=5; i++) {
//            cout<<nums[i]<<" ";
//        }
        close(fd2[1]);
        read(fd2[0], numsSorted, 5*sizeof(int));
//        sort(numsSorted, numsSorted+5);
//
        cout<<"Sorted List: ";
        for (int i =0 ; i!=5; i++) {
            cout<<numsSorted[i]<<" ";

        }
        cout<<endl;
        //closes parent's read;
        close(fd2[0]);

        p = fork();
        
        if (p<0) {
            cout<<"Failed fork"<<endl;
            return 0;
        }
        
        else if(p>0){
            
            int middle;
            close(fd3[0]);
            write(fd3[1],numsSorted, 5*sizeof(int));  //Pass numbers to child
            close(fd3[1]);
            
            //wait until second child process finishes
            wait(NULL);
            
            close(fd4[1]);
            read(fd4[0], &middle, sizeof(middle));
            cout<<"Median: "<<middle<<endl;
            close(fd4[0]);
            
        }
        else{
            //second child process
            close(fd3[1]);
            int median[5];
            int middle;
            read(fd3[0], median, 5*sizeof(int));
            
            middle = median[2];
            close(fd3[0]);
            close(fd4[0]);
            write(fd4[1], &middle,sizeof(middle));
            close(fd4[1]);
            exit(0);
            
        }
        
    }
    
    else{
        
        //first child process
        close(fd1[1]);
        int numsSorted [5];

        read(fd1[0], numsSorted, 5*sizeof(int));
        
                sort(numsSorted, numsSorted+5);
        
//                for (int i =0 ; i!=5; i++) {
//                    cout<<numsSorted[i]<<" ";
//                }
        
        close(fd1[0]);
        close(fd2[0]);
        
        write(fd2[1], numsSorted,5*sizeof(int));
        close(fd2[1]);
        exit(0);
        
    }
    

    return 0;
}   
