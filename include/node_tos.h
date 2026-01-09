#pragma once

#include "node_ct.h"

struct Node_tos
{
    Node_tos(const unsigned int id, int altitude, Node_tos *par);
    
    // Gos functions
    void enrich(int highest_alt);
    void compute_interval(int highest_alt);
    void compute_node_class();
    void add_ct_parent(Node_tos *parent);
    void add_adj_parent(std::array<int,2> interval, Node_tos *parent);
    Node_tos * get_ct_parent(int alt);

    // Ctos functions
    void sort_adj_holes();

    // Debug functions
    void print_node();
    void print_children();

    // shared across all hierarchies
    unsigned int name;
    int alt;
    bool root = false;
    Node_class node_class = NA;
    std::vector<int> proper_part;
    std::array<int, 2> interval;

    // Specific to the tree of shapes
    Node_tos *parent;
    std::list<struct Node_tos *> children;

    // Specific to the graph of shapes
    Node_tos *parent_ct;
    std::unordered_map<int, struct Node_tos*> parent_adj;
    std::vector<std::tuple<struct Node_tos*, std::array<int, 2>>> interval_parent_adj;
    std::list<std::tuple<struct Node_tos*, std::array<int, 2>>> children_gos_adj;
    std::list<struct Node_tos *> children_gos_ct;

    // Specific to complete tree of shapes
    bool holes_sorted = false;
};
