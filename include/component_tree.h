#pragma once
#include <string>
#include <vector>

#include <higra/image/graph_image.hpp>
#include <higra/hierarchy/component_tree.hpp>
#include <opencv2/opencv.hpp>
#include <xtensor/xadapt.hpp>
#include <iostream>

struct Component_tree {
    Component_tree(const std::string& filename);
    void print_parents_max_tree() const;
    void print_parents_min_tree() const;

    cv::Mat image;
    hg::node_weighted_tree<hg::tree, hg::array_1d<int>> max_tree;
    hg::node_weighted_tree<hg::tree, hg::array_1d<int>> min_tree;
    hg::array_1d<hg::index_t> parents_max_tree;
    hg::array_1d<hg::index_t> parents_min_tree;
    int img_size;
};