#include "Node.h"

Node::Node(int id, int memory, int time_limit, double swap_prob): 
id(id), memory(memory), time_limit(time_limit), swap_prob(swap_prob) {
    memory_table.resize(time_limit, memory);
    // cerr << "memory = " << memory << endl;
}

Node::~Node() {}
int Node::get_id() { return id; }
int Node::get_memory() { return memory; }
int Node::get_memory_at(int t) { return memory_table[t]; }
vector<int> Node::get_neighbot() { return neighbor; }
double Node::get_swap_prob() { return swap_prob; }

void Node::add_neighbor(int neighbor_id) {
    neighbor.push_back(neighbor_id);
}

void Node::reserve_memory(int t, int amount) {
    assert(memory_table[t] >= amount);
    // if(memory_table[t] < amount) {
    //     cerr << "error: memory is not enough" << endl;
    //     exit(1);
    // }
    memory_table[t] -= amount;
}

void Node::reserve_memory(int amount) {
    memory -= amount;
    for(int t = 0; t < time_limit; t++) {
        assert(memory_table[t] >= amount);
        // if(memory_table[t] < amount) {
        //     cerr << "error: memory is not enough" << endl;
        //     exit(1);
        // }
        memory_table[t] -= amount;
    }
}