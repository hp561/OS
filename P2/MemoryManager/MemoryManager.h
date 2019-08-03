#include <string>
#include <iostream>
#include <functional>
#include <list>
#include <vector>
#include <bitset>
#include <string>
#include <algorithm>


using namespace std;


int bestFit(int sizeInWords, void *list);
int worstFit(int sizeInWords, void *list);

class MemoryManager{

	public:
	 	MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator);
		~MemoryManager();
		void initialize(size_t sizeInWords);
		void shutdown();
		void *allocate(size_t sizeInBytes);
		void free(void *address);
		void setAllocator(std::function<int(int, void*)> allocator);
		int dumpMemoryMap(char *filename);
		void *getList();
		void *getBitmap();
		unsigned getWordSize();
		void *getMemoryStart();
		unsigned getMemoryLimit();
		

	private:
		unsigned wordSize;
        std::function<int(int, void*)> allocator;
        char* mem;
        bool dCheck = false;
        vector<size_t> bitmap;
        vector<pair<void*,size_t>> holeAndLength;
        vector<pair<void*,int>> holeAndOffset;
        size_t memCount;
        size_t max;
        uint16_t swap_uint16( uint16_t val );

};
