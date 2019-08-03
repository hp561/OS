//
//  Memory Manager
//
//  Created by Harsh Patel on 2/26/19.
//  Copyright © 2019 Harsh Patel. All rights reserved.
//

#include "MemoryManager.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>




//public size_t sizeOfArr = 0;


MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator){
    
    this->wordSize = wordSize;
    this->allocator = allocator;
}

void MemoryManager:: initialize(size_t sizeInWords) {
    
    if (sizeInWords > 65536 || sizeInWords <0) {
        
        cout<<"Block of memory is either too big or negative. Program will not exit. Restart."<<endl;
        return;
    }
    


    bitmap = vector<size_t>(sizeInWords);
    
    max = sizeInWords*wordSize;
    mem = new char[max];
}

MemoryManager:: ~MemoryManager(){
    
    if (dCheck == true) {
        
        dCheck = false;
    }
   
    else{
        
        delete [] mem;

        
    }
    
    holeAndLength.clear();
    holeAndOffset.clear();
    bitmap.clear();
    
}

void MemoryManager:: shutdown(){
    
    holeAndLength.clear();
    holeAndOffset.clear();
    bitmap.clear();
    
    if (dCheck == false) {
        delete [] mem;
        dCheck =true;
    }
    
   
}

unsigned MemoryManager:: getWordSize(){
    
    return wordSize;
}



void* MemoryManager:: allocate(size_t sizeInBytes){
    
    
    if (max < sizeInBytes || sizeInBytes <0 || memCount == max){
        
        cout<<"Invalid allocation of memory. Either too big, negative, or full :)"<<endl;
        return nullptr;
    }
    

    int offset = (allocator((unsigned)sizeInBytes/getWordSize(),getList()));

    int byteOffset = offset*(unsigned)sizeInBytes;
//       int offset = (allocator((unsigned)sizeInBytes,getList()));
//    cout<<"Allocate Offset:"<<offset<<endl;
    char *temp = mem;
    
    temp += byteOffset;

    int wordsNeeded = (unsigned)sizeInBytes / getWordSize();
    
    if (wordsNeeded == 0) {
        wordsNeeded = 1;
    }
    for (int i = offset; i!=offset+wordsNeeded; i++) {
        
        bitmap.at(i)=1;
    }
    
//    for (int i = offset; i!=offset+wordsNeeded; i++) {
//
//        cout<<bitmap.at(i);
//    }
    
//    for (int i = offset; i!=sizeInBytes;++i) {
//
//        mem[i]='1';
//    }
    
    //offset
    holeAndLength.push_back(make_pair(temp,sizeInBytes));
    holeAndOffset.push_back(make_pair(temp,offset));

    memCount = memCount + sizeInBytes;
    
    
    //    cout<<"degub"<<endl;
    
    //hole address
    return temp;
}


void MemoryManager::free(void *address){
    size_t freeSpace = 0;
    int offset = 0;
    vector<pair<void*, size_t>>::iterator it;
    vector<pair<void*, int>>::iterator it1;

    it = holeAndLength.begin();
    it1 = holeAndOffset.begin();
    
    for (int i = 0; i!=holeAndLength.size(); i++) {
        if (holeAndLength[i].first == address) {
             freeSpace = holeAndLength[i].second;
//            cout<<"check "<<i<<endl;
            
            holeAndLength.erase(it);
            break;
        }
        else
            it++;
        
    }
    for (int i = 0; i!=holeAndOffset.size(); i++) {
        if (holeAndOffset[i].first == address) {
            offset = holeAndOffset[i].second;
            holeAndOffset.erase(it1);
//            cout<<"check 2 "<<i<<endl;
            break;
        }
        else
            it1++;
        
    }
//    cout<<"F: "<<freeSpace<<endl;
    int wordsNeeds = (unsigned)freeSpace / getWordSize();
//    cout<<"Words: "<<wordsNeeds<<endl;
//    cout<<"Offset: "<<offset<<endl;

    for (int i = offset; i!=offset+wordsNeeds; i++) {
        bitmap.at(i)=0;
    }
    
    
    memCount = memCount - freeSpace;



}

void MemoryManager:: setAllocator(std::function<int (int, void *)> allocator){
    
    this->allocator = allocator;
    
}

unsigned MemoryManager::getMemoryLimit(){
    
    return (unsigned)max;
    
}

void* MemoryManager:: getMemoryStart(){
    
    if (mem) {
        return mem;
    }
    
    return nullptr;
}

int MemoryManager::dumpMemoryMap(char *filename){
    
    //get the list of holes
    uint16_t * list = (uint16_t*)getList();

    //i will create one big string for all the holes and their lengths with proper format when writing
    string sholes;
    
//    cout<<list[0]<<endl;
    
    sholes.append("[");
    
    //use to make the format of the write
    int forMat = 0;
    
    //will iterate through list of holes and their lengths
    for (int i = 1; i!=(list[0]/2)+1; i++) {
            
        
        //converts the number to a string
        auto s = std::to_string(list[i]);
        
        //formatting if statement. When we have successfully appednded the second number in brackets, format by closing the bracket adding a - and making a new oprning bracket and reset the forMat counter
        if (forMat==2) {
            sholes.append("] - [");
            forMat=0;
        }
        
        //formatting if statement. When we append second element in pair dont add the , to avoid an extra comma and space after the second value is appended
        if (forMat==1) {
            sholes.append(s);
            forMat++;
            continue;
        }
        
        sholes.append(s);
        sholes.append(", ");
        forMat++;
        
    
    }
    
    //we have finished. Close string
    sholes.append("]");

    
//    cout<<sholes<<endl;
    
    //creating buffer to write to document
    char *buff = new char [sholes.length()];
    
    //converts the sholes string that i got earlier to a cstring and places it in buff
    strcpy(buff, sholes.c_str());
    
//    cout<<buff[0]<<" "<<buff[sholes.length()-1]<<endl;
//
//    char *test = new char[5];
//    test[0]='1';
//    test[1]='1';
//    test[2]='1';
//    test[3]='1';
//    test[4]='1';
    int fd = open( filename, O_RDWR );
    
    if( fd == -1 )
    {
        cout<<"Cannot open. Error."<<endl;
        close( fd );
        return -1;
    }
//    ssize_t read_byte;
//    if( ( read_byte = read( fd, buff, sholes.length() ) ) == -1 )
//    {
//        cout<<"Can't read"<<endl;
//        close( fd );
//        return -1;
//    }
    
    if (write(fd, buff, sholes.length())==-1){
        perror( "write()" );

        cout<<"Failed Writing"<<endl;
        
        
        close( fd );
        delete [] list;
        delete[] buff;
        return -1;
    }
    
    delete [] list;
    delete []buff;
    
    return 0;
    
}



int bestFit(int sizeInWords, void *list){
    
    uint16_t * hole2 = (uint16_t*)list;
    
    
    int tracker = -1;
    int sizeTracker = 0;
    
    uint16_t length = hole2[0]+1;
//    uint16_t t1 = hole2[0];
//    uint16_t t2 = hole2[1];
//    uint16_t t3 = hole2[2];

//    cout<<"Length: "<<t1<<endl;
//    cout<<"Offset: "<<t2<<endl;
//    cout<<"Offset Length: "<<t3<<endl;
    for (int i =2; i!= length; i+=2) {

        
        
        if (i > length) {
            break;
        }
        
        if (tracker == -1) {
            
            if (hole2[i] == sizeInWords ) {

                tracker = hole2[i-1];
                break;
            }
            
            if (hole2[i]>sizeInWords ) {
               

                tracker = hole2[i-1];
                sizeTracker = hole2[i];
                
            }
            

        }

        
        else if (tracker!=-1){
            if (hole2[i] == sizeInWords ) {

                tracker = hole2[i-1];
                break;
            }
            if (hole2[i]< sizeTracker && hole2[i]>=sizeInWords) {

                tracker = hole2[i-1];
                sizeTracker = hole2[i];
            }

        }
        
    }
    
    if (tracker!=-1) {
        delete []hole2;

        return tracker;
    }
    
    delete [] hole2;
    
    return -1;
}

void * MemoryManager::getList(){
//    int c = 0;
    //used to track the length of hole
    
    int holelength = 0;
    
    //used to keep track of index in hole array
    int holecounter = 0;
    
    if (holeAndLength.size()==0) {
        
        uint16_t * hole = new uint16_t[3];
        
        
        hole[0] = 2*2;
        hole[1] = 0;
        hole[2] = bitmap.size();
        
        
        return hole;
    }

    uint16_t * hole = new uint16_t[bitmap.size()];

    
    for (int i = 0; i!=bitmap.size(); i++) {
        if (bitmap.at(i) == 0){
            if ((i-1)<0) {
                
                hole[holecounter++] = i;
                ++holelength;
                continue;
            }
            
            if ((i+1)==bitmap.size() && bitmap.at(i) == 0) {
                
                hole[holecounter++] = ++holelength;
                break;
            }
            
            if (bitmap.at(i-1) == 0) {
                ++holelength;
                continue;
            }
            
            hole[holecounter++] = i;
            ++holelength;
        }
        
        
        
       else if (bitmap.at(i) ==1) {
//           cout<<i<<endl;
            if ((i-1)<0) {
                continue;
            }
           
            if ( bitmap.at(i-1)== 0) {
                
                hole[holecounter++] = holelength;
                holelength = 0;
                continue;
            }
           
           holelength = 0;
//           cout<<"check"<<c++<<endl;
           
        }
        
    
    }
//    cout<<"Hole Length in Glist: "<<holelength<<endl;

//    cout<<"Hole counter: "<<holecounter<<endl;
    uint16_t * realList = new uint16_t[holecounter+1];
    
    for (int i = 0; i!=holecounter+1; i++) {
        
        if (i==0) {
            realList[i] = holecounter*2;
            continue;
        }
        
        realList[i] = hole[i-1];
        
    }

    
    delete [] hole;
//    cout<<"reaches here"<<endl;
    
    return realList;
    
}
//

void * MemoryManager:: getBitmap(){
    
    uint16_t bitSize =0;
//    cout<<bitmap.size()<<endl;
    
    if (bitmap.size()==0) {
        cout<<"Error. There is nothing in the bitMap. Returning nothing."<<endl;
        return nullptr;
    }
    
//    bitset<1> bits;
    
    
    
    if (bitmap.size()/8 ==0) {
        bitSize = 1;
    }
    
    
    if (bitmap.size()%8!=0) {
        bitSize = bitmap.size()%8+1;
    }
    else {
        bitSize = bitmap.size()/8;
    }
    
//    for (int i = 0; i!=bitmap.size(); i++) {
//
//        if (bitmap.at(i)==1) {
//            bitSize++;
//        }
//    }
    
    uint16_t* bMap=  new uint16_t[bitSize];
    
    uint16_t byte = 0;
    uint16_t bTracker = 1;
    int index =1;
    
    
    
    for (int i = 0; i!=bitmap.size(); i++) {
        
        
        if (i==0) {
            bMap[i]=bitSize;
            
             byte = bTracker*(uint16_t)bitmap.at(i)+byte;
             bTracker = bTracker*2;
            
            continue;
        }
        if (i%16==0) {
            
            
            bMap[index++] = byte;
            bTracker = 1;
            byte = 0;
        }
        


//        cout<<"Val: "<<bitmap.at(i)<<endl;


        byte = bTracker*(uint16_t)bitmap.at(i)+byte;
        
   
        bTracker = bTracker*2;
        
//        bMap[i] = bitmap.at(i-1);
       
        
    }
    
//    uint8_t* returnBitMap = new uint8_t(bitSize);
//
//    for (int i =1; i!=bitSize-1; i++) {
//
//        bMap[i] = bits[i-1]
//    }
    
    
    
    return bMap;
}


int worstFit(int sizeInWords, void *list){

    uint16_t * hole2 = (uint16_t*)list;
    int tracker = -1;
    int sizeTracker = 0;
    
    uint16_t length = hole2[0]+1;
//    uint16_t t1 = hole2[0];
//    uint16_t t2 = hole2[1];
//    uint16_t t3 = hole2[2];
    
//    cout<<"Length: "<<t1<<endl;
//    cout<<"Offset: "<<t2<<endl;
//    cout<<"Offset Length: "<<t3<<endl;
    for (int i =2; i!= length; i+=2) {
        
      
        
        if (tracker == -1) {
            if (hole2[i]>= sizeInWords ) {
                
                tracker = hole2[i-1];
                sizeTracker = hole2[i];
                
            }
        }
   
        else if (tracker!=-1){
            
            if (hole2[i]>= sizeTracker ) {
                
                tracker = hole2[i-1];
                sizeTracker = hole2[i];
            }
            
           
        }
        
        if ((i+1)== length) {
            break;
        }
        

    }

    if (tracker!=-1) {
        delete []hole2;
        return tracker;
    }
    
    delete []hole2;
    return -1;
}

uint16_t MemoryManager:: swap_uint16(uint16_t val){
    
    return (val << 8) | (val >> 8 );
}

uint16_t swap16( uint16_t val )
{
    return (val << 8) | (val >> 8 );
}

//int main(int argc, const char * argv[]) {
//
//    uint16_t z  = 2;
//    auto s = std::to_string(z);
//    string x = s;
//    cout<<x<<endl;
//
//    std::string str ("Please split this sentence into tokens");
//
//    char cstr [str.length()];
//    std::strcpy (cstr, str.c_str());
//    cout<<cstr[str.length()-2];
//
//
////    // insert code here..
////   // std::cout << "Hello, World!\n"<<endl;
//////    vector<int> shit;
//////
//////    shit.push_back(0);
////
////
////
//////    MemoryManager memoryManager(8, bestFit);
//////    memoryManager.initialize(100);
//////    uint32_t* testArray1 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
//////    uint32_t* testArray2 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
//////    uint32_t* testArray3 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
//////    uint32_t* testArray4 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
//////    uint32_t* testArray5 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
//////
////////
//////    memoryManager.free(testArray2);
//////    memoryManager.free(testArray4);
//////    memoryManager.free(testArray5);
//////    memoryManager.free(testArray1);
//////    memoryManager.free(testArray3);
//    MemoryManager memoryManager(8, worstFit);
//    memoryManager.initialize(96);
////     allocate
//    uint32_t* testArray1 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 50));
////    uint32_t* testArray2 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));
////    uint32_t* testArray3 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
////    uint32_t* testArray4 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));
////    uint32_t* testArray5 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));
////    uint32_t* testArray6 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
//
////     free specific allocations to create holes
////    memoryManager.free(testArray3);
////    memoryManager.free(testArray5);
////     change allocator
//    memoryManager.setAllocator(bestFit);
//
//    memoryManager.getBitmap();
//
//////    cout<<"ho";
////
////
////
////
//////
//////     vector<pair<int,int>> holeAndLength;
//////    vector<pair<int, int>>::iterator it;
//////
//////    holeAndLength.push_back(make_pair(1,2));
//////    holeAndLength.push_back(make_pair(2,3));
//////
////////4 0 0 0 0
//////    it = holeAndLength.begin()+1;
//////
//////    holeAndLength.erase(it);
//////
//////    for (int i = 0; i!=holeAndLength.size(); i++) {
//////
//////        cout<<holeAndLength[i].first<<" "<<holeAndLength[i].second<<endl;
//////    }
//////    cout<<"ends here"<<endl;
//////
////////    cout<<holeAndLength[0].first<<" "<<holeAndLength[0].second<<endl;
//////
//////    char * hi= new char[12]();
//////
//////
//////    hi[0] = 'i';
////////    hi[1] = '3';
//////    cout<<&hi<<endl;
//////    char *addr0 = &hi[0];
//////    char *addr00 = &hi[1];
//////
//////    cout<<&addr0<<endl;
//////    cout<<&addr00<<endl;
//////    void* list = hi;
//////    cout<<list<<endl;
//////
////////    cout<<list<<endl;
//////
//////    char *ba = (char*)list;
//////
//////    char *addr = &ba[0];
//////    char *addr2 = &ba[1];
//////
//////
//////    cout<<&addr<<endl;
//////    cout<<&addr2<<endl;
//////
//////    int c = 0;
//////
//////
//////
//////
////////    char *a = new char[10];
////////
//////////
//////    for (int i = 0; i<12; i++) {
//////        if (hi[i]=='\0') {
//////            c++;
//////        }
//////        ba[i]= '7';
//////        addr = &ba[i];
//////        cout<<hi[i]<<" "<<i<<endl;
//////    }
//////    cout<<endl<<c<<endl;
//////
////////
////////    for (int i = -1; i!=3;++i) {
////////
////////        if (i==-1) {
////////            i++;
////////        }
////////
////////        hi[i]='1';
////////         cout<<hi[i]<<" "<<i<<endl;
////////    }
//////
//////    cout<<endl<<endl<<endl;
//////
//////    uint16_t * hole = new uint16_t[34];
//////
//////
//////        for (int i = 0; i!=34;i++) {
//////
////////            if (i==-1) {
////////                i++;
////////            }
//////
//////            hole[i]=i+2;
//////             cout<<hole[i]<<" "<<i<<endl;
//////        }
//////
//////    void* test = hole;
//////
//////    uint16_t * hole2 = (uint16_t*)test;
//////    //    char* arr = static_cast<char*>(list);
//////    uint16_t leg = hole2[10];
//////
//////    hole2 +=4;
//////     void *off = hole2;
////////    cout<<*hole2[2]<<"hello"<<endl;
//////
//////
//////    vector<size_t> testMap;
////
//////    bitset<1> realbitMap;
////
//////    realbitMap[0] = 1;
//////    testMap.push_back(realbitMap[0]);
//////    realbitMap[1] = testMap.at(1);
//////    cout<<testMap.at(0)<<endl;
//////    cout<<*hole2<<endl;
//////
//////    uint16_t * hole3 = (uint16_t*)off;
////////
////////    uint16_t * hole3 = hole2;
////////    hole3+=1;
////////
//////    if (hole3==off) {
//////        cout<<"yes"<<endl;
//////    }
//////    else{
//////        cout<<"nopeee"<<endl;
//////    }
//////    cout<<*hole3;
////////    a = a +1;
////////    unsigned word = 51;
//////
////
////    uint16_t x = swap16(96);
////
//////    cout<<x<<endl;
//////    uint16_t xy = (unsigned long)bitset<16>(500);
//////
////    cout<<(bitset<16>(96))<<endl;
////    cout<<(bitset<16>(x));
//////    auto x = std::bitset<32>(10);
//////    std::cout << x << std::endl;
//////
//////    auto str = x.to_string();
//////    std::reverse(str.begin(), str.end());
//////    auto y = std::bitset<32>(str);
//////    std::cout << y << std::endl;
////    return 0;
////
//}
