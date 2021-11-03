#include "common.h"
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

using namespace std;

// create record
inline Record create_record(string tconst, string averageRating, string numVotes)
{
    Record record;
    strcpy(record.tconst, tconst.c_str());
    record.averageRating = stof(averageRating);
    record.numVotes = stoi(numVotes);

    return record;
}

inline MemoryManager::MemoryManager(size_t disk_size, size_t block_size)
{
    this->size = disk_size;
    this->block_size = block_size;
    this->num_blocks = 0;
    this->num_visitedBlocks = 0;
    this->content = new char[disk_size];
    this->sizeUsed = 0;


    std::memset(content, '\0', disk_size);
    this->block_ptr = nullptr;

    this->used_memory_space = 0;
    this->used_block_space = 0;
};
bool MemoryManager::allocateBlock()
{
    // Only allocate a new block if we don't exceed maxPoolSize.
    if (sizeUsed + block_size <= size)
    {
        // Update variables
        sizeUsed += block_size;
        block_ptr = (char *)content + num_blocks * block_size; 
        used_block_space = 0; 
        num_blocks += 1;
        return true;
    }
    else
    {
        std::cout << "Error: No memory left to allocate new block (" << sizeUsed << "/" << size << " used)." << '\n';
        return false;
    }
}
inline Address MemoryManager::insert_item(void *item, size_t item_size)
{

    if (item_size > block_size)
    {
        std::cout << "Error: Size required larger than block size (" << item_size << " vs " << block_size << ")! Increase block size to store data." << '\n';
        throw std::invalid_argument("Requested size too large!");
    }

    // If no current block, or record can't fit into current block, make a new block.
    if (num_blocks == 0 || (used_block_space + item_size > block_size))
    {
        bool isSuccessful = allocateBlock();
        if (!isSuccessful)
        {
            throw std::logic_error("Failed to allocate new block!");
        }
    }

    // Update variables
    short int offset = used_block_space;

    used_block_space += item_size;
    used_memory_space += item_size;

    // Return the new memory space to put in the record.
    Address recordAddress = {block_ptr, offset};

    memcpy((char *)recordAddress.blockAddress + recordAddress.offset, item, sizeof(Node));

    return recordAddress; // return address

}
inline Address MemoryManager::insert_Record(void *item, size_t item_size)
{

    if (item_size > block_size)
    {
        std::cout << "Error: Size required larger than block size (" << item_size << " vs " << block_size << ")! Increase block size to store data." << '\n';
        throw std::invalid_argument("Requested size too large!");
    }

    // If no current block, or record can't fit into current block, make a new block.
    if (num_blocks == 0 || (used_block_space + item_size > block_size))
    {
        bool isSuccessful = allocateBlock();
        if (!isSuccessful)
        {
            throw std::logic_error("Failed to allocate new block!");
        }
    }

    // Update variables
    short int offset = used_block_space;

    used_block_space += item_size;
    used_memory_space += item_size;

    // Return the new memory space to put in the record.
    Address recordAddress = {block_ptr, offset};

    memcpy((char *)recordAddress.blockAddress + recordAddress.offset, item, item_size);

    return recordAddress; 
}
inline void MemoryManager::update_item(void *item, size_t item_size, Address parentAddress)
{
    memcpy((char *)parentAddress.blockAddress + parentAddress.offset, (char *)item, item_size);
}

inline bool MemoryManager::delete_item(Address address, size_t item_size)
{
    unsigned char *blockAddr = (unsigned char *)address.blockAddress;
    unsigned char *deleteItemAddr = blockAddr + address.offset;
    try
    {
        // delete item
        if (deletedItems[deleteItemAddr] != true)
        {
            memset(deleteItemAddr, '\0', item_size);
            this->deletedItems[deleteItemAddr] = true;
        }

        // decrement the number of block if entire block is empty
        unsigned char *empty = new unsigned char[this->block_size];
        memset(empty, '\0', this->block_size);
        if (memcmp(blockAddr, empty, this->block_size) == 0)
        {
            this->num_blocks -= 1;
        }

        // set block as visit
        if (this->visitedBlocks[blockAddr] == false)
        {
            this->visitedBlocks[blockAddr] = true;
            this->num_visitedBlocks += 1;
        }

        return true;
    }
    catch (...)
    {
        cout << "Exception occurred";
        return false;
    }
}

inline void *MemoryManager::find_item(Address address, size_t item_size)
{
    unsigned char *blockAddr = (unsigned char *)address.blockAddress;
    unsigned char *itemAddr = blockAddr + address.offset;
    try
    {
        void *item = operator new(item_size);
        memcpy(item, (char *)address.blockAddress + address.offset, item_size);

        // set block as visit
        if (this->visitedBlocks[blockAddr] == false)
        {
            this->visitedBlocks[blockAddr] = true;
            this->num_visitedBlocks += 1;
        };

        return item;
    }
    catch (...)
    {
        cout << "Exception occurred";
        return NULL;
    }
};

inline list<unsigned char *> MemoryManager::get_all_visited_blocks()
{
    bool visit_status = true;

    list<unsigned char *> visitedBlocks;

    short int limit = 5; // number of blocks to return

    for (auto &block : this->visitedBlocks)
    {

        if (limit > 0)
        {
            if (block.second == visit_status)
            {
                visitedBlocks.push_back(block.first);
                limit -= 1;
            }
        }
    }
    return visitedBlocks;
};

inline void MemoryManager::reset_visited_blocks()
{
    for (auto &block : this->visitedBlocks)
    {
        block.second = false;
    }
    this->num_visitedBlocks = 0;
}

inline int MemoryManager::get_number_of_block()
{
    return this->num_blocks;
};
