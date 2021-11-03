
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <numeric>
#pragma pack(1)

#include "storage.cpp"
using namespace std;

static int totalCount = 1;

// maximum number of keys in a node
static int MAX = 0;
static int noOfNode = 1;
static int noOfBlock = 0;
static float ratingSum = 0;
static int ratingCount = 0;

Node::Node()
{
  keys = new int[MAX];
  ptr = new Address[MAX + 1];
  for (int i = 0; i < MAX + 1; i++)
  {
    Address nullAddress{nullptr, 0};
    ptr[i] = {nullptr};
  }
}

// BP tree
class BPTree
{
  Node *root;
  MemoryManager *indexStorage;
  MemoryManager *recordStorage;
  size_t nodeSize;
  void *rootAddress;
  void insertInternal(int, Node *, Node *);
  void removeInternal(int, Node *, Node *);
  Node *findParent(Node *, Node *);
  Node *findParent(Node *, Node *, int);

public:
  BPTree(MemoryManager *, MemoryManager *, size_t);

  void searchKey(int);
  void searchRange(int, int);
  void insert(int, Address);
  int removeKey(int);
  void removeLL(Address LLHeadAddress);
  Address insertExtendedNode(Address, Address, int);
  float searchExtendedNode(Address, int);
  int getNoOfBlockAccessed(Address);
  void display(Node *);
  int getHeight();
  int getNoOfNode(Node *);
  void contentOfRoot();
  void contentOfFirstChild();
  Node *getRoot();
};

BPTree::BPTree(MemoryManager *indexStorage, MemoryManager *recordStorage, size_t nodeSize)
{
  root = NULL;
  rootAddress = nullptr;
  //rootAddress = NULL;
  this->indexStorage = indexStorage;
  this->recordStorage = recordStorage;
  this->nodeSize = nodeSize;
  MAX = (nodeSize - sizeof(Address *) - sizeof(int) - sizeof(bool)) / (sizeof(Address*) + sizeof(int *));

  cout << "MAX: " << MAX << endl;
}
void BPTree::searchRange(int intFrom, int intTo)
{
  if (root == NULL)
  {
    cout << "Tree is empty\n";
  }
  else if (intTo < intFrom)
  {
    cout << "The first parameter must be smaller than the second one\n";
  }
  else
  {
    Node *cursor = root;
    int nodeCount = 0;
    vector<float> averageRating;
    vector<int> blockAccessed;
    int currentKey;
    int currentIndex;
    while (cursor->blLeaf == false)
    {
      for (int i = 0; i < cursor->size; i++)
      {
        if (intFrom < cursor->keys[i])
        {
          nodeCount++;
          cout << "\n"
               << nodeCount << " th node accessed: ";
          for (int i = 0; i < cursor->size; i++)
          {
            cout << cursor->keys[i] << " ";
          }
          Node *indexNode = (Node *)indexStorage->find_item(cursor->ptr[i], nodeSize);
          cursor = indexNode;
          break;
        }
        if (i == cursor->size - 1)
        {
          nodeCount++;
          cout << "\n"
               << nodeCount << " th node accessed: ";
          for (int i = 0; i < cursor->size; i++)
          {
            cout << cursor->keys[i] << " ";
          }
          Node *indexNode = (Node *)indexStorage->find_item(cursor->ptr[i + 1], nodeSize);
          cursor = indexNode;
          break;
        }
      }
    }

    nodeCount++;
    cout << "\n"
         << nodeCount << " th node accessed: ";
    for (int i = 0; i < cursor->size; i++)
    {

      cout << cursor->keys[i] << " ";
    }
    for (int i = 0; i < cursor->size; i++)
    {

      if (cursor->keys[i] >= intFrom)
      {
        currentKey = cursor->keys[i];
        currentIndex = i;
        break;
      }
      if (i == cursor->size - 1)
      {
        Node *indexNode = (Node *)indexStorage->find_item(cursor->ptr[(cursor->size)], nodeSize);
        cursor = indexNode;
        currentKey = cursor->keys[0];
        currentIndex = 0;
        break;
      }
    }

    while (currentKey <= intTo)
    {

      if (currentIndex != cursor->size)
      {
        ratingSum = 0;
        ratingCount = 0;
        averageRating.push_back(searchExtendedNode(cursor->ptr[currentIndex], currentKey));
        noOfBlock = 0;
        blockAccessed.push_back(getNoOfBlockAccessed(cursor->ptr[currentIndex]));
        currentIndex++;
        if (currentIndex < cursor->size)
        {
          currentKey = cursor->keys[currentIndex];
        }
      }
      else
      {

        Node *indexNode = (Node *)indexStorage->find_item(cursor->ptr[cursor->size], nodeSize);

        cursor = indexNode;
        nodeCount++;

        cout << "\n\n"
             << nodeCount << " th node accessed: ";
        for (int i = 0; i < cursor->size; i++)
        {

          cout << cursor->keys[i] << " ";
        }
        currentIndex = 0;
        currentKey = cursor->keys[currentIndex];
      }
    }

    cout << "\n";

    cout << "\nTotal Number of nodes accessed: " << nodeCount;
    cout << "\n";

    cout << "\nAverage of the average of averageRating: ";
    cout << accumulate(averageRating.begin(), averageRating.end(), 0.0) / averageRating.size();
    cout << endl;

    cout << "Total Number of data blocks accessed: " << accumulate(blockAccessed.begin(), blockAccessed.end(), 0.0);
    cout << endl;
  }
}
// Search operation
void BPTree::searchKey(int x)
{
  if (root == NULL)
  {
    cout << "Tree is empty\n";
  }
  else
  {
    Node *cursor = root;
    int nodeCount = 0;
    bool blFound = false;
    int address;
    int blockAccessed;
    while (cursor->blLeaf == false)
    {
      for (int i = 0; i < cursor->size; i++)
      {
        if (x < cursor->keys[i])
        {
          nodeCount++;
          cout << "\n"
               << nodeCount << " th node accessed: ";
          for (int i = 0; i < cursor->size; i++)
          {
            cout << cursor->keys[i] << " ";
          }
          Node *indexNode = (Node *)indexStorage->find_item(cursor->ptr[i], nodeSize);
          cursor = indexNode;
          break;
        }
        if (i == cursor->size - 1)
        {
          nodeCount++;
          cout << "\n"
               << nodeCount << " th node accessed: ";
          for (int i = 0; i < cursor->size; i++)
          {
            cout << cursor->keys[i] << " ";
          }
          Node *indexNode = (Node *)indexStorage->find_item(cursor->ptr[i + 1], nodeSize);
          cursor = indexNode;
          break;
        }
      }
    }

    nodeCount++;
    cout << "\n\n"
         << nodeCount << " th node accessed: ";
    for (int i = 0; i < cursor->size; i++)
    {
      cout << cursor->keys[i] << " ";
    }
    for (int i = 0; i < cursor->size; i++)
    {
      if (cursor->keys[i] == x)
      {
        blFound = true;
        //change to address
        ratingCount = 0;
        ratingSum = 0;
        searchExtendedNode(cursor->ptr[i], x);
        cout << endl;
        noOfBlock = 0;
        blockAccessed = getNoOfBlockAccessed(cursor->ptr[i]);
        cout << "Total Number of data blocks accessed: " << blockAccessed;

      }
    }

    cout << "\n";

    cout << "Total Number of nodes accessed: " << nodeCount;
    cout << "\n";

    if (blFound)
    {
      cout << "Result Found";
    }
    else
    {
      cout << "Result Not Found";
    }
  }
}

int BPTree::getNoOfBlockAccessed(Address extendedNodeAddress)
{
  Node *currentFirstNode = (Node *)indexStorage->find_item(extendedNodeAddress, nodeSize);
  noOfBlock += currentFirstNode->size;

  if (currentFirstNode->ptr[currentFirstNode->size].blockAddress == nullptr)
  {
    return noOfBlock;
  }

  else
  {
    getNoOfBlockAccessed(currentFirstNode->ptr[currentFirstNode->size]);
  }
  return noOfBlock;
}

void BPTree::insert(int x, Address address)
{

    if (root == NULL)
    {
      Node *extendedNode = new Node();
      extendedNode->keys[0] = x;
      extendedNode->blLeaf = false;
      extendedNode->size = 1;
      extendedNode->ptr[0] = address;
      extendedNode->ptr[1].blockAddress = nullptr;

      Address extendedNodeAddress = indexStorage->insert_item((void *)extendedNode, nodeSize);
      root = new Node();
      root->keys[0] = x;
      root->blLeaf = true;
      root->size = 1;
      root->ptr[0] = extendedNodeAddress;
      rootAddress = indexStorage->insert_item((void *)root, nodeSize).blockAddress;
      delete extendedNode;
      extendedNode = nullptr;
    }
    else
    {
      Node *cursor = root;
      Node *parent;
      Address rAddress{(void *)root, 0};
      Address cursorAddress = rAddress;
      Address parentAddress = rAddress;

      void *cAddress = rootAddress;
      void *pAddress = rootAddress;
      while (cursor->blLeaf == false)
      {
        parent = cursor;
        parentAddress = cursorAddress;
        pAddress = cAddress;
        for (int i = 0; i < cursor->size; i++)
        {
          if (x < cursor->keys[i])
          {
            Node *indexNode = (Node *)indexStorage->find_item(cursor->ptr[i], nodeSize);
            Address tempp{(unsigned char *)indexNode, 0};
            cursorAddress = cursor->ptr[i];
            cAddress = cursor->ptr[i].blockAddress;
            cursor = indexNode;
            break;
          }
          if (i == cursor->size - 1)
          {
            //cursor = cursor->ptr[i + 1];
            Node *indexNode = (Node *)indexStorage->find_item(cursor->ptr[i + 1], nodeSize);
            cursorAddress = cursor->ptr[i + 1];
            cAddress = cursor->ptr[i + 1].blockAddress;

            cursor = indexNode;
            break;
          }
        }
      }

      if (cursor->size < MAX)
      {
        int i = 0;
        while (x > cursor->keys[i] && i < cursor->size)
        {
          i++;
        }
        if (cursor->keys[i] == x)
        {
          cursor->ptr[i] = insertExtendedNode(cursor->ptr[i], address, x);
        }
        else
        {
          cursor->ptr[cursor->size + 1] = cursor->ptr[cursor->size];

          for (int j = cursor->size; j > i; j--)
          {
            cursor->keys[j] = cursor->keys[j - 1];
            cursor->ptr[j] = cursor->ptr[j - 1];
          }
          Node *extendedNode = new Node();
          extendedNode->keys[0] = x;
          extendedNode->blLeaf = false;
          extendedNode->size = 1;
          extendedNode->ptr[0] = address;
          Address extendedNodeAddress = indexStorage->insert_item((void *)extendedNode, nodeSize);
          delete extendedNode;
          extendedNode = nullptr;
          Node *cur = (Node *)indexStorage->find_item(cursorAddress, nodeSize);

          cursor->keys[i] = x;
          cursor->size = cursor->size + 1;
          cursor->ptr[i] = extendedNodeAddress;
          Address ccccAddress{cAddress, 0};
          indexStorage->update_item(cursor, nodeSize, ccccAddress);
        }
      }
      else
      {
        Node *newLeaf = new Node();
        int tempNode[MAX + 1];
        Address tempPtr[MAX + 2];
        // copy to the remp
        for (int i = 0; i < MAX; i++)
        {
          tempNode[i] = cursor->keys[i];
          tempPtr[i] = cursor->ptr[i];
        }
        int i = 0, j;
        while (x > tempNode[i] && i < MAX)
          i++;

        if (cursor->keys[i] == x)
        {
          cursor->ptr[i] = insertExtendedNode(cursor->ptr[i], address, x);
          return;
        }

        for (int j = MAX; j > i; j--)
        {
          tempNode[j] = tempNode[j - 1];
          tempPtr[j] = tempPtr[j - 1];
        }
        tempNode[i] = x;

        // Create extended node
        Node *extendedNode = new Node();
        extendedNode->keys[0] = x;
        extendedNode->blLeaf = false;
        extendedNode->size = 1;
        extendedNode->ptr[0] = address;
        Address extendedNodeAddress = indexStorage->insert_item((void *)extendedNode, nodeSize);
        delete extendedNode;
        extendedNode = nullptr;
        tempPtr[i] = extendedNodeAddress;

        newLeaf->blLeaf = true;
        cursor->size = (MAX + 1) / 2;

        newLeaf->size = MAX + 1 - (MAX + 1) / 2;
        newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];

        for (i = 0; i < cursor->size; i++)
        {
          cursor->keys[i] = tempNode[i];
          cursor->ptr[i] = tempPtr[i];
        }
        for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++)
        {
          newLeaf->keys[i] = tempNode[j];
          newLeaf->ptr[i] = tempPtr[j];
        }
        Address newLeafAddress = indexStorage->insert_item((void *)newLeaf, nodeSize);

        cursor->ptr[cursor->size] = newLeafAddress;
        for (int i = cursor->size; i < MAX; i++)
        {
          cursor->keys[i] = int();
        }
        for (int i = cursor->size + 1; i < MAX + 1; i++)
        {
          Address nullAddress{nullptr, 0};
          cursor->ptr[i] = nullAddress;
        }
        Address cursorAdd{cAddress, 0};
        indexStorage->update_item((void *)cursor, nodeSize, cursorAdd);

        if (cursor == root)
        {
          Node *newRoot = new Node();
          newRoot->keys[0] = newLeaf->keys[0];
          newRoot->ptr[0] = cursorAdd;
          newRoot->ptr[1] = newLeafAddress;
          newRoot->blLeaf = false;
          newRoot->size = 1;
          rootAddress = indexStorage->insert_item((void *)newRoot, nodeSize).blockAddress;
          Address rtAdd{rootAddress, 0};
          root = (Node *)indexStorage->find_item(rtAdd, nodeSize);

          delete newLeaf;
          newLeaf = nullptr;
          delete newRoot;
          newRoot = nullptr;
        }

        else
        {
          insertInternal(newLeaf->keys[0], (Node *)pAddress, (Node *)newLeafAddress.blockAddress);
        }
      }
    }
 
}

// Call this when the extended node already exist
Address BPTree::insertExtendedNode(Address extendedNodeAddress, Address newKeyAddress, int x)
{
  Node *currentFirstNode = (Node *)indexStorage->find_item(extendedNodeAddress, nodeSize);
  if (currentFirstNode->size < MAX)
  {
    currentFirstNode->keys[currentFirstNode->size] = x;
    currentFirstNode->blLeaf = false;
    currentFirstNode->ptr[currentFirstNode->size + 1] = currentFirstNode->ptr[currentFirstNode->size];
    currentFirstNode->ptr[currentFirstNode->size] = newKeyAddress;
    currentFirstNode->size++;
    indexStorage->update_item((void *)currentFirstNode, nodeSize, extendedNodeAddress);
    return extendedNodeAddress;
  }
  else
  {
    Node *newFirstNode = new Node();
    newFirstNode->keys[0] = x;
    newFirstNode->blLeaf = false;
    newFirstNode->ptr[0] = newKeyAddress;
    newFirstNode->size = 1;
    newFirstNode->ptr[1] = extendedNodeAddress;
    Address exNodeAddress = indexStorage->insert_item((void *)newFirstNode, nodeSize);
    delete newFirstNode;
    newFirstNode = nullptr;
    return exNodeAddress;
  }
}

// print block content and return record array
float BPTree::searchExtendedNode(Address extendedNodeAddress, int currentKey)
{
  Node *currentFirstNode = (Node *)indexStorage->find_item(extendedNodeAddress, nodeSize);
  cout << "\n\nAccessing the extened data nodes of ";
  cout << currentKey << endl;

  for (int i = 0; i < currentFirstNode->size; i++)
  {
    Record result = *(Record *)(recordStorage->find_item(currentFirstNode->ptr[i], sizeof(Record)));
    ratingSum += result.averageRating;
    ratingCount++;
    std::cout << result.tconst << " | ";
  }
  for (int i = currentFirstNode->size; i < MAX; i++)
  {
    std::cout << "empty | ";
  }

  // End of linked list
  if (currentFirstNode->ptr[currentFirstNode->size].blockAddress == nullptr)
  {
    std::cout << "End of extended list" << endl;
    cout << "Average of averageRating = " << ratingSum / ratingCount;
    return ratingSum / ratingCount;
  }

  else
  {
    searchExtendedNode(currentFirstNode->ptr[currentFirstNode->size], currentKey);
  }
  return ratingSum / ratingCount;
}

// Insert Operation
void BPTree::insertInternal(int x, Node *cursorAdd, Node *childAdd)
{

    Address childAddress{childAdd, 0};
    Node *child = (Node *)indexStorage->find_item(childAddress, nodeSize);
    Address cursorAddress{cursorAdd, 0};
    Node *cursor = (Node *)indexStorage->find_item(cursorAddress, nodeSize);
    if (cursorAdd == rootAddress)
    {
      root = cursor;
    }
    if (cursor->size < MAX)
    {
      int i = 0;
      while (x > cursor->keys[i] && i < cursor->size)
        i++;
      for (int j = cursor->size; j > i; j--)
      {
        cursor->keys[j] = cursor->keys[j - 1];
      }
      for (int j = cursor->size + 1; j > i + 1; j--)
      {
        cursor->ptr[j] = cursor->ptr[j - 1];
      }

      cursor->keys[i] = x;
      cursor->size++;
      Address childAddress2{childAdd, 0};
      cursor->ptr[i + 1] = childAddress2;
      Address cursorAdd2{cursorAdd, 0};

      indexStorage->update_item(cursor, nodeSize, cursorAdd2);
    }
    else
    {
      Node *newInternal = new Node();
      int tempkeys[MAX + 1];
      Address tempPtr[MAX + 2];
      for (int i = 0; i < MAX; i++)
      {
        tempkeys[i] = cursor->keys[i];
      }
      for (int i = 0; i < MAX + 1; i++)
      {
        tempPtr[i] = cursor->ptr[i];
      }
      int i = 0, j;
      while (x > tempkeys[i] && i < MAX)
        i++;
      for (int j = MAX; j > i; j--)
      {
        tempkeys[j] = tempkeys[j - 1];
      }
      tempkeys[i] = x;
      for (int j = MAX + 1; j > i + 1; j--)
      {
        tempPtr[j] = tempPtr[j - 1];
      }
      Address childAddress = {childAdd, 0};
      tempPtr[i + 1] = childAddress;

      newInternal->blLeaf = false;
      cursor->size = (MAX + 1) / 2;
      newInternal->size = MAX - (MAX + 1) / 2;
      for (i = 0, j = 0; i < cursor->size; i++, j++)
      {
        cursor->keys[i] = tempkeys[j];
      }
      for (i = 0, j = 0; i < cursor->size + 1; i++, j++)
      {
        cursor->ptr[i] = tempPtr[j];
      }
      for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++)
      {
        newInternal->keys[i] = tempkeys[j];
      }

      for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++)
      {
        newInternal->ptr[i] = tempPtr[j];
      }
      for (int i = cursor->size + 1; i < MAX + 1; i++)
      {
        Address nullAddress{nullptr, 0};
        cursor->ptr[i] = nullAddress;
      }
      Address cursorAdd3{cursorAdd, 0};
      indexStorage->update_item(cursor, nodeSize, cursorAdd3);
      Address newInternalAddress = indexStorage->insert_item(newInternal, nodeSize);
      delete newInternal;
      newInternal = nullptr;
      if (cursor == root)
      {
        Node *newRoot = new Node();
        newRoot->keys[0] = tempkeys[cursor->size];
        Address cursorAddress3{cursorAdd, 0};
        newRoot->ptr[0] = cursorAddress3;
        newRoot->ptr[1] = newInternalAddress;
        newRoot->blLeaf = false;
        newRoot->size = 1;
        root = newRoot;
        Address newRootAdd = indexStorage->insert_item(newRoot, nodeSize);
        rootAddress = newRootAdd.blockAddress;
        newRoot = nullptr;
      }
      else
      {
        insertInternal(tempkeys[cursor->size], findParent((Node *)rootAddress, cursorAdd, cursor->keys[0]), (Node *)newInternalAddress.blockAddress);
      }
    }
 
}

//Remove
int BPTree::removeKey(int key)
{
  // Number of nodes before remove
  noOfNode = 1;
  int prev_numberofNodes = this->getNoOfNode((*this).getRoot());

  if (rootAddress == nullptr)
  {
    throw std::logic_error("Tree is empty!");
  }
  else
  {
    Address rootDiskAddress{(unsigned char *)rootAddress, 0};
    root = (Node *)indexStorage->find_item(rootDiskAddress, nodeSize);
    void *parentDiskAddress = rootAddress; // Keep track of parent's disk address to update parent in disk.
    void *cursorDiskAddress = rootAddress; // Store current node's disk address in case we need to update it in disk.
    Node *cursor = root;
    Node *parent;

    int leftSibling, rightSibling; // keep track of neighbours keys to borrow

    while (cursor->blLeaf == false)
    {
      parent = cursor;
      parentDiskAddress = cursorDiskAddress;
      for (int i = 0; i < cursor->size; i++)
      {

        leftSibling = i - 1;
        rightSibling = i + 1;
        if (key < cursor->keys[i])
        {
          // Load node in from disk to main memory.
          Node *mainMemoryNode = (Node *)indexStorage->find_item(cursor->ptr[i], nodeSize);

          // Update cursorDiskAddress to maintain address in disk if we need to update nodes.
          cursorDiskAddress = cursor->ptr[i].blockAddress;

          // Move to new node in main memory.
          cursor = (Node *)mainMemoryNode;
          break;
        }
        if (i == cursor->size - 1)
        {
          leftSibling = i;
          rightSibling = i + 2;
          // Load node in from disk to main memory.
          Node *mainMemoryNode = (Node *)indexStorage->find_item(cursor->ptr[i + 1], nodeSize);

          // Update cursorDiskAddress to maintain address in disk if we need to update nodes.
          cursorDiskAddress = cursor->ptr[i + 1].blockAddress;

          // Move to new node in main memory.
          cursor = (Node *)mainMemoryNode;
          break;
        }
      }
    }
    bool found = false;
    int pos;
    for (pos = 0; pos < cursor->size; pos++)
    {
      if (cursor->keys[pos] == key)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      std::cout << "Key " << key << " is not found in b+ tree" << endl;
      // No node is deleted
      noOfNode = 1;
      int numNodesDeleted = prev_numberofNodes - this->getNoOfNode((*this).getRoot());
      noOfNode = 1;

      prev_numberofNodes = this->getNoOfNode((*this).getRoot());
      return numNodesDeleted;
    }
    // remove linked list
    removeLL(cursor->ptr[pos]);

    for (int i = pos; i < cursor->size; i++)
    {
      cursor->keys[i] = cursor->keys[i + 1];
      cursor->ptr[i] = cursor->ptr[i + 1];
    }
    cursor->size--;
    //move last pointer
    cursor->ptr[cursor->size] = cursor->ptr[cursor->size + 1];

    // Set all forward pointers from numKeys onwards to nullptr.
    for (int i = cursor->size + 1; i < MAX + 1; i++)
    {
      Address nullAddress{nullptr, 0};
      cursor->ptr[i] = nullAddress;
    }

    if (cursor == root)
    {
      if (cursor->size == 0)
      {
        cout << "Tree died. Entire tree is deleted\n";
        // Deallocate block used to store root node.
        Address rootDiskAddress{(unsigned char *)rootAddress, 0};
        indexStorage->delete_item(rootDiskAddress, nodeSize);

        // Reset root pointers in the B+ Tree.
        // root = nullptr;
        rootAddress = nullptr;
        root = NULL;
      }
      // update numNodes and numNodesDeleted after deletion
      noOfNode = 1;

      int numNodesDeleted = prev_numberofNodes - this->getNoOfNode((*this).getRoot());
      noOfNode = 1;

      prev_numberofNodes = this->getNoOfNode((*this).getRoot());

      // Save to disk.
      Address cursorAddress = {(unsigned char *)cursorDiskAddress, 0};
      indexStorage->update_item(cursor, nodeSize, cursorAddress);
      cout << "Number of Node Deleted: " << numNodesDeleted << endl;
      return numNodesDeleted;
    }

    // Doesnt need merge
    if (cursor->size >= (MAX + 1) / 2)
    {
      // No node is deleted
      noOfNode = 1;
      int numNodesDeleted = prev_numberofNodes - this->getNoOfNode((*this).getRoot());
      noOfNode = 1;
      prev_numberofNodes = this->getNoOfNode((*this).getRoot());
      // Save to disk.
      Address cursorAddress = {(unsigned char *)cursorDiskAddress, 0};
      indexStorage->update_item(cursor, nodeSize, cursorAddress);
      cout << "Number of Node Deleted: " << numNodesDeleted << endl;
      return numNodesDeleted;
    }

    // Borrow from left
    if (leftSibling >= 0)
    {
      
      Node *leftNode = (Node *)indexStorage->find_item(parent->ptr[leftSibling], nodeSize);
      
      
      for (int i = leftNode->size, j = 0; j < cursor->size; i++, j++)
      {
        leftNode->keys[i] = cursor->keys[j];
        leftNode->ptr[i] = cursor->ptr[j];
      }

      
      leftNode->size += cursor->size;
      leftNode->ptr[leftNode->size] = cursor->ptr[cursor->size];

      
      indexStorage->update_item(leftNode, nodeSize, parent->ptr[leftSibling]);

      
      removeInternal(parent->keys[leftSibling], (Node *)parentDiskAddress, (Node *)cursorDiskAddress);

      
      Address cursorAddress{(unsigned char *)cursorDiskAddress, 0};
      indexStorage->delete_item(cursorAddress, nodeSize);
    }
  
    else if (rightSibling <= parent->size)
    {
      // Load right sibling 
      Node *rightNode = (Node *)indexStorage->find_item(parent->ptr[rightSibling], nodeSize);

      
      for (int i = cursor->size, j = 0; j < rightNode->size; i++, j++)
      {
        cursor->keys[i] = rightNode->keys[j];
        cursor->ptr[i] = rightNode->ptr[j];
      }

       
      cursor->size += rightNode->size;
      cursor->ptr[cursor->size] = rightNode->ptr[rightNode->size];

      
      Address cursorAddress{(unsigned char *)cursorDiskAddress, 0};
      
      indexStorage->update_item(cursor, nodeSize, cursorAddress);

      
      void *rightNodeAddress = parent->ptr[rightSibling].blockAddress;
      removeInternal(parent->keys[rightSibling - 1], (Node *)parentDiskAddress, (Node *)rightNodeAddress);

      
      Address rightNodeDiskAddress{(unsigned char *)rightNodeAddress, 0};
      indexStorage->delete_item(rightNodeDiskAddress, nodeSize);
    }
  }

  
  noOfNode = 1;
  int numNodesDeleted = prev_numberofNodes - this->getNoOfNode((*this).getRoot());
  noOfNode = 1;
  prev_numberofNodes = this->getNoOfNode((*this).getRoot());
  cout << "Number of Node Deleted: " << numNodesDeleted << endl;
  return numNodesDeleted;
}


void BPTree::removeInternal(int key, Node *cursorDiskAddress, Node *childDiskAddress)
{
  
  Address cursorAddress{(unsigned char *)cursorDiskAddress, 0};
  Node *cursor = (Node *)indexStorage->find_item(cursorAddress, nodeSize);

  
  if (cursorDiskAddress == rootAddress)
  {
    root = cursor;
  }

  
  Address childAddress{(unsigned char *)childDiskAddress, 0};

 
  if (cursor == root)
  {
    
    if (cursor->size == 1)
    {
      
      if (cursor->ptr[1].blockAddress == (unsigned char *)childDiskAddress)
      {
        
        indexStorage->delete_item(childAddress, nodeSize);

        
        root = (Node *)indexStorage->find_item(cursor->ptr[0], nodeSize);
        rootAddress = (Node *)cursor->ptr[0].blockAddress;

        
        indexStorage->delete_item(cursorAddress, nodeSize);

        
        std::cout << "Root node changed." << endl;
        return;
      }
      
      else if (cursor->ptr[0].blockAddress == (unsigned char *)childDiskAddress)
      {
        
        indexStorage->delete_item(childAddress, nodeSize);

        
        root = (Node *)indexStorage->find_item(cursor->ptr[1], nodeSize);
        rootAddress = (Node *)cursor->ptr[1].blockAddress;

        
        indexStorage->delete_item(cursorAddress, nodeSize);

        
        std::cout << "Root node changed." << endl;
        return;
      }
    }
  }

  
  int pos;

  
  for (pos = 0; pos < cursor->size; pos++)
  {
    if (cursor->keys[pos] == key)
    {
      break;
    }
  }

  
  for (int i = pos; i < cursor->size; i++)
  {
    cursor->keys[i] = cursor->keys[i + 1];
  }

  
  for (pos = 0; pos < cursor->size + 1; pos++)
  {
    if (cursor->ptr[pos].blockAddress == (unsigned char *)childDiskAddress)
    {
      break;
    }
  }

  
  for (int i = pos; i < cursor->size + 1; i++)
  {
    cursor->ptr[i] = cursor->ptr[i + 1];
  }

  
  cursor->size--;

  
  if (cursor->size >= (MAX + 1) / 2 - 1)
  {
    return;
  }

  
  if ((unsigned char *)cursorDiskAddress == rootAddress)
  {
    return;
  }

  
  Node *parentDiskAddress = findParent((Node *)rootAddress, cursorDiskAddress);
  int leftSibling, rightSibling;

  
  Address parentAddress{(unsigned char *)parentDiskAddress, 0};
  Node *parent = (Node *)indexStorage->find_item(parentAddress, nodeSize);

  
  for (pos = 0; pos < parent->size + 1; pos++)
  {
    if (parent->ptr[pos].blockAddress == (unsigned char *)cursorDiskAddress)
    {
      leftSibling = pos - 1;
      rightSibling = pos + 1;
      break;
    }
  }

  
  if (leftSibling >= 0)
  {
    
    Node *leftNode = (Node *)indexStorage->find_item(parent->ptr[leftSibling], nodeSize);

    
    if (leftNode->size >= (MAX + 1) / 2)
    {
      
      for (int i = cursor->size; i > 0; i--)
      {
        cursor->keys[i] = cursor->keys[i - 1];
      }

      
      cursor->keys[0] = parent->keys[leftSibling];
      parent->keys[leftSibling] = leftNode->keys[leftNode->size - 1];

      
      for (int i = cursor->size + 1; i > 0; i--)
      {
        cursor->ptr[i] = cursor->ptr[i - 1];
      }

      
      cursor->ptr[0] = leftNode->ptr[leftNode->size];

      
      cursor->size++;
      leftNode->size--;

      
      leftNode->ptr[cursor->size] = leftNode->ptr[cursor->size + 1];

      
      Address parentAddress{(unsigned char *)parentDiskAddress, 0};
      
      indexStorage->update_item(cursor, nodeSize, parentAddress);

      
      indexStorage->insert_item(leftNode, nodeSize);

      
      Address cursorAddress = {(unsigned char *)cursorDiskAddress, 0};
      
      indexStorage->update_item(cursor, nodeSize, cursorAddress);
      return;
    }
  }

  
  if (rightSibling <= parent->size)
  {
    
    Node *rightNode = (Node *)indexStorage->find_item(parent->ptr[rightSibling], nodeSize);

    
    if (rightNode->size >= (MAX + 1) / 2)
    {
      
      cursor->keys[cursor->size] = parent->keys[pos];
      parent->keys[pos] = rightNode->keys[0];

      
      for (int i = 0; i < rightNode->size - 1; i++)
      {
        rightNode->keys[i] = rightNode->keys[i + 1];
      }

      
      cursor->ptr[cursor->size + 1] = rightNode->ptr[0];

      
      for (int i = 0; i < rightNode->size; ++i)
      {
        rightNode->ptr[i] = rightNode->ptr[i + 1];
      }

      
      cursor->size++;
      rightNode->size--;

      
      Address parentAddress{(unsigned char *)parentDiskAddress, 0};
      
      indexStorage->update_item(cursor, nodeSize, parentAddress);

      
      indexStorage->insert_item(rightNode, nodeSize);

      
      Address cursorAddress = {(unsigned char *)cursorDiskAddress, 0};
      indexStorage->update_item(cursor, nodeSize, cursorAddress);
      return;
    }
  }

  
  if (leftSibling >= 0)
  {
    
    Node *leftNode = (Node *)indexStorage->find_item(parent->ptr[leftSibling], nodeSize);

    
    leftNode->keys[leftNode->size] = parent->keys[leftSibling];

    
    for (int i = leftNode->size + 1, j = 0; j < cursor->size; j++)
    {
      leftNode->keys[i] = cursor->keys[j];
    }

    
    Address nullAddress{nullptr, 0};
    for (int i = leftNode->size + 1, j = 0; j < cursor->size + 1; j++)
    {
      leftNode->ptr[i] = cursor->ptr[j];
      cursor->ptr[j] = nullAddress;
    }

    
    leftNode->size += cursor->size + 1;
    cursor->size = 0;

    
    indexStorage->insert_item(leftNode, nodeSize);

    
    removeInternal(parent->keys[leftSibling], (Node *)parentDiskAddress, (Node *)cursorDiskAddress);
  }
  
  else if (rightSibling <= parent->size)
  {
    
    Node *rightNode = (Node *)indexStorage->find_item(parent->ptr[rightSibling], nodeSize);

    
    cursor->keys[cursor->size] = parent->keys[rightSibling - 1];

    
    for (int i = cursor->size + 1, j = 0; j < rightNode->size; j++)
    {
      cursor->keys[i] = rightNode->keys[j];
    }

    
    Address nullAddress = {nullptr, 0};
    for (int i = cursor->size + 1, j = 0; j < rightNode->size + 1; j++)
    {
      cursor->ptr[i] = rightNode->ptr[j];
      rightNode->ptr[j] = nullAddress;
    }

    
    cursor->size += rightNode->size + 1;
    rightNode->size = 0;

    
    Address cursorAddress{(unsigned char *)cursorDiskAddress, 0};
    indexStorage->insert_item(cursor, nodeSize);

    
    void *rightNodeAddress = parent->ptr[rightSibling].blockAddress;
    removeInternal(parent->keys[rightSibling - 1], (Node *)parentDiskAddress, (Node *)rightNodeAddress);
  }
}

void BPTree::removeLL(Address LLHeadAddress)
{
  // Load in first node from disk.
  Node *head = (Node *)indexStorage->find_item(LLHeadAddress, nodeSize);


  // Deallocate the current node.
  indexStorage->delete_item(LLHeadAddress, nodeSize);

  // End of linked list
  if (head->ptr[head->size].blockAddress == nullptr)
  {
    std::cout << "End of linked list";
    return;
  }

  if (head->ptr[head->size].blockAddress != nullptr)
  {

    removeLL(head->ptr[head->size]);
  }
}

// Find the parent
Node *BPTree::findParent(Node *cursor, Node *child)
{
  Node *parent;
  if (cursor->blLeaf || ((Node *)indexStorage->find_item(cursor->ptr[0], nodeSize))->blLeaf)
  {
    return NULL;
  }
  for (int i = 0; i < cursor->size + 1; i++)
  {
    if ((Node *)indexStorage->find_item(cursor->ptr[i], nodeSize) == child)
    {
      parent = cursor;
      return parent;
    }
    else
    {
      parent = findParent((Node *)indexStorage->find_item(cursor->ptr[i], nodeSize), child);
      if (parent != NULL)
        return parent;
    }
  }
  return parent;
}

Node *BPTree::findParent(Node *cursorDiskAddress, Node *childDiskAddress, int min)
{
  // Load in cursor into main memory, starting from root.
  Address cursorAddress{cursorDiskAddress, 0};
  Node *cursor = (Node *)indexStorage->find_item(cursorAddress, nodeSize);

  // If the root cursor passed in is a leaf node, there is no children, therefore no parent.
  if (cursor->blLeaf)
  {
    return nullptr;
  }

  // Maintain parentDiskAddress
  Node *parentDiskAddress = cursorDiskAddress;

  // While not leaf, keep following the nodes to correct key.
  while (cursor->blLeaf == false)
  {
    // Check through all pointers of the node to find match.
    for (int i = 0; i < cursor->size + 1; i++)
    {
      if (cursor->ptr[i].blockAddress == childDiskAddress)
      {
        return parentDiskAddress;
      }
    }

    for (int i = 0; i < cursor->size; i++)
    {
      // If key is lesser than current key, go to the left pointer's node.
      if (min < cursor->keys[i])
      {
        // Load node in from disk to main memory.
        Node *mainMemoryNode = (Node *)indexStorage->find_item(cursor->ptr[i], nodeSize);

        // Update parent address.
        parentDiskAddress = (Node *)cursor->ptr[i].blockAddress;

        // Move to new node in main memory.
        cursor = (Node *)mainMemoryNode;
        break;
      }

      // Else if key larger than all keys in the node, go to last pointer's node (rightmost).
      if (i == cursor->size - 1)
      {
        // Load node in from disk to main memory.
        Node *mainMemoryNode = (Node *)indexStorage->find_item(cursor->ptr[i + 1], nodeSize);

        // Update parent address.
        parentDiskAddress = (Node *)cursor->ptr[i + 1].blockAddress;

        // Move to new node in main memory.
        cursor = (Node *)mainMemoryNode;
        break;
      }
    }
  }

  // If we reach here, means cannot find already.
  return nullptr;
}
// Print the tree
void BPTree::display(Node *cursor)
{
  if (cursor != NULL)
  {
    for (int i = 0; i < cursor->size; i++)
    {
      cout << cursor->keys[i] << " ";
    }
    cout << "\n";
    if (cursor->blLeaf != true)
    {
      for (int i = 0; i < cursor->size + 1; i++)
      {
        display((Node *)indexStorage->find_item(cursor->ptr[i], nodeSize));
      }
    }
  }
}

int BPTree::getHeight()
{
  if (root == NULL)
  {
    return 0;
  }
  else
  {
    int height = 1;
    Node *cursor = root;
    while (!cursor->blLeaf)
    {
      height++;
      cursor = (Node *)indexStorage->find_item(cursor->ptr[0], nodeSize);
    }
    return height;
  }
}

int BPTree::getNoOfNode(Node *cursor)
{
  if (!cursor->blLeaf)
  {
    for (int i = 0; i < cursor->size + 1; i++)
    {
      noOfNode++;
      getNoOfNode((Node *)indexStorage->find_item(cursor->ptr[i], nodeSize));
    }
  }
  return noOfNode;
}

void BPTree::contentOfRoot()
{
  for (int i = 0; i < root->size; i++)
  {
    cout << root->keys[i] << " ";
  }
}

void BPTree::contentOfFirstChild()
{
  Node *firstChild = (Node *)indexStorage->find_item(root->ptr[0], nodeSize);
  for (int i = 0; i < firstChild->size; i++)
  {
    cout << firstChild->keys[i] << " ";
  }
}
// Get the root
Node *BPTree::getRoot()
{
  return root;
}
