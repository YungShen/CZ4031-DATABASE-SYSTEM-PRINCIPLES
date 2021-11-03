#include "common.h"
#include "bPlusTree.cpp"

using namespace std;

// Experiment 1
void experiment_1(size_t disk_size, size_t block_size)
{
    MemoryManager memMgr = MemoryManager(disk_size, block_size);

    ifstream data("./data/data.tsv");
    string line;

    getline(data, line); // skip header column

    vector<Address> addresses;

    while (getline(data, line))
    {

        string item;
        vector<string> row_values;
        stringstream linestream(line);

        while (getline(linestream, item, '\t'))
        {
            row_values.push_back(item);
        }

        Record record = create_record(row_values.at(0), row_values.at(1), row_values.at(2));

        Address addr = memMgr.insert_Record(&record, sizeof(Record));
        addresses.push_back(addr);

        Record test_;
        void *item_ = memMgr.find_item(addr, sizeof(record));
        memcpy(&test_, item_, sizeof(record));

    };
    // Experiment 1 result
    int numblock = memMgr.get_number_of_block();
    cout << "Disk capacity (B): " << disk_size << endl;
    cout << "Block size: " << block_size << endl;
    cout << "Number of blocks: " << numblock << endl;
    cout << "Number of bytes (B) occupied by blocks: " << (numblock * block_size) << endl;
    cout << "Number of bytes (MB) occupied by blocks: " << (numblock * block_size) / (float)(1024 * 1024) << endl;
    // removed the ceil for the full precision
};

bool compareByNumVotes(const Record &a, const Record &b)
{
    return a.numVotes < b.numVotes;
}

void experiment_2(BPTree tree)
{
    tree.display(tree.getRoot());
    cout << endl;

    cout << "No. of Nodes: ";
    cout << tree.getNoOfNode(tree.getRoot()) << endl;
    cout << "Height of the tree: ";
    cout << tree.getHeight() << endl;
    cout << "Content of root node: ";
    tree.contentOfRoot();
    cout << endl;
    cout << "Content of first child: ";
    tree.contentOfFirstChild();
    cout << endl;

}

void experiment_3(BPTree tree)
{
        tree.display(tree.getRoot());
        tree.searchKey(500);

}

void experiment_4(BPTree tree)
{
    tree.display(tree.getRoot());
    tree.searchRange(30000, 40000);
}

void experiment_5(BPTree tree)
{
    tree.display(tree.getRoot());
    tree.removeKey(1000);
    cout << "Number of nodes of the updated B+ tree: " <<tree.getNoOfNode(tree.getRoot())<<endl;
    cout << "Height of the updated B+ tree: " <<tree.getHeight()<< endl;
    cout << "Content of the root node: " << endl;
    tree.contentOfRoot();
    cout <<  endl;
    cout << "Content of 1st child node for root node: " << endl;
    tree.contentOfFirstChild();
    cout <<  endl;
}
int main()
{
    size_t size_of_disk = 100 * (1024 * 1024);
    size_t size_of_index = 100 * (1024 * 1024);
    size_t block_size;
    int x;
    cout << "Enter either 100 or 500 block size (in Bytes): "; 
    cin >> x; 
    if (x == 100) {
        block_size = 100;
    } else if (x == 500) {
        block_size = 500;
    } else{
        cout << "Invalid block size selected. Please rerun the program with the correct Byte size"; 
        exit (0);
    }

    cout << "Which experiment to run?(1/press any key (except 1) to load b+ tree): "; 
    cin >> x; 
    if (x == 1)
    {
        experiment_1(size_of_disk, block_size);
    }
    else
    {
        MemoryManager disk = MemoryManager(size_of_disk, block_size);
        MemoryManager index = MemoryManager(size_of_index, block_size);
        BPTree tree = BPTree(&index, &disk, block_size);
        ifstream data("./data/data.tsv");
        string line;
        vector<Address> addresses;

        getline(data, line); // skip header column

        vector<Record> records;

        while (getline(data, line))
        {
            string item;
            vector<string> row_values;
            stringstream linestream(line);

            while (getline(linestream, item, '\t'))
            {
                row_values.push_back(item);
            }

            Record record = create_record(row_values.at(0), row_values.at(1), row_values.at(2));
            records.push_back(record);

            Address addr = disk.insert_item(&record, sizeof(record));
            addresses.push_back(addr);
            cout << "Inserting: " << endl;
            cout << record.tconst << endl;
            tree.insert(record.numVotes, addr);
        };
        cout<<"tree insert completed"<<endl;
        while (true)
        {
           
            cout << "\nWhich experiment to run?(2/3/4/5): "; // Type a number and press enter
            cin >> x;
            switch (x)
            {
            case 2:
                experiment_2(tree);
                break;
            case 3:
                experiment_3(tree);
                break;
            case 4:
                experiment_4(tree);
                break;
            case 5:
                experiment_5(tree);
                break;
            default:
                cout << "Invalid input" << endl;
                break;
            }
            cout<<"\nCtrl-C to exit"<<endl;
            
        }
    }
    return 0;
};