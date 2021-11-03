#ifndef COMMON_H
#define COMMON_H
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <cstring>
#include <vector>
#include <new>
#include <map>
#include <math.h>
#pragma pack(1)

struct Record
{
    // unsigned char tconst[11];
    char tconst[11];
    float averageRating;
    int numVotes;
};

struct Address
{
    void *blockAddress;
    short int offset;
};
struct Node
{
  // flag for leaf / non-leaf node
  bool blLeaf;
  // pointer to keys & size of keys
  int *keys;
  int size;
  //  pointer to other nodes
  Address *ptr;
  // BPTree class has access to every Node class
  friend class BPTree;

public:
  Node();
};
class MemoryManager
{
private:
    size_t size;           // size of allocated memory space
    size_t block_size;     // size of a block
    int num_blocks;        // number of used block
    int num_visitedBlocks; // number of visited blocks

    std::map<unsigned char *, bool> deletedItems;  //store item address: deleted (T/F) pairs
    std::map<unsigned char *, bool> visitedBlocks; //store block address: visited (T/F) pairs
    short int item_offset;                         // offset value of the last item in the last block
    size_t used_memory_space;                      // size of used memory space
    size_t used_block_space;                       // size of used space in current block
    size_t sizeUsed;

public:
    bool allocateBlock();
    void *content;   // pointer to the content of the memory space
    void *block_ptr; // pointer to the latest block in the memory space
    MemoryManager(size_t disk_size, size_t block_size);
    Address insert_item(void *item, size_t item_size);
    void update_item(void *item, size_t item_size, Address);
    bool delete_item(Address address, size_t item_size);
    void *find_item(Address address, size_t item_size);
    std::list<unsigned char *> get_all_visited_blocks();
    void reset_visited_blocks();
    int get_number_of_block();
    Address insert_Record(void *item, size_t item_size);
};

#endif