#pragma once
#include "graph_of_shapes.h"
#include "component_tree.h"

struct Complete_tree_of_shapes
{
    explicit Complete_tree_of_shapes(Graph_of_shapes& g) : gos(&g), ct(nullptr), tos(nullptr) {}
    explicit Complete_tree_of_shapes(Component_tree& ct, Tree_of_shapes& tos) : gos(nullptr), ct(&ct), tos(&tos) {}
    
    // from tos only
    void build();
    // from ct and tos
    void build_from_ct_and_tos();

    // Debugging
    void print_tree();

    long nb_nodes();

    // If built from tree of shapes alone : graph of shapes is needed
    Graph_of_shapes *gos;

    // If built from component tree and tree of shapes
    Component_tree *ct;
    Tree_of_shapes *tos;

    // Tree  nodes
    std::unordered_map<long, std::unique_ptr<Node_ct>> nodes;
    // Nodes proper parts
    Node_ct *root = nullptr;
};