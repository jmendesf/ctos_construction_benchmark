#pragma once
#include <string>
#include <vector>

#include "tree_of_shapes.h"

struct Graph_of_shapes
{
    explicit Graph_of_shapes(Tree_of_shapes &tree_of_shapes);
    void build();
    void compute_gos_relations(Node_tos *current);
    void compute_comprehensive_adjacency(Node_tos *current, Node_tos *first_adj);
    void print_graph();
    long nb_edges();

    void print_node(hg::tree_internal::tree::children_list_t &stack);
    long nb_adj_edges = 0;
    Tree_of_shapes &tos;
};