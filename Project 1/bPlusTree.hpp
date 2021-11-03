

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
  //__int8 MAX;

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