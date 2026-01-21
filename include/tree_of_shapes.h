#pragma once
#include <string>
#include <vector>

#include <higra/image/tree_of_shapes.hpp>
#include "node_tos.h"
#include <opencv2/opencv.hpp>
#include <xtensor/xadapt.hpp>

struct Tree_of_shapes
{
    Tree_of_shapes(const std::string &filename);
    void print_parents() const;
    void pre_process_tos();
    void traverse_tree();
    long nb_nodes();
    
    cv::Mat image;
    long img_size;
    int highest_value;
    
    hg::node_weighted_tree<hg::tree, hg::array_1d<uchar>> tos;
    hg::array_1d<hg::index_t> parents;

    Node_tos *root = nullptr;
    std::unordered_map<long, std::unique_ptr<Node_tos>> nodes;
    std::unordered_map<long, std::vector<long>> proper_parts;
    
    bool enriched = false;
};