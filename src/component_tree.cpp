#include "component_tree.h"

Component_tree::Component_tree(const std::string& filename) {
    // Load grayscale 
    cv::Mat image_cv = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if (image_cv.empty()) {
        throw std::runtime_error("Failed to load image: " + filename);
    }

    int height = image_cv.rows;
    int width = image_cv.cols;

    std::vector<int> image;
    image.reserve(height * width);

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            image.push_back(static_cast<int>(image_cv.at<uchar>(i, j)));

    hg::array_1d<int>::shape_type shape = { image.size() };
    hg::array_1d<int> img_flat(shape);
    for (size_t i = 0; i < image.size(); ++i)
        img_flat(i) = image[i];

    auto graph = hg::get_4_adjacency_graph({height, width});
    
    max_tree = hg::component_tree_max_tree(graph, img_flat);
    min_tree = hg::component_tree_min_tree(graph, img_flat);

    parents_max_tree = max_tree.tree.parents();
    parents_min_tree = min_tree.tree.parents();

    img_size = img_flat.size();
}

void Component_tree::print_parents_max_tree() const {
    std::cout << "Max tree -- parents:\n";
    for (size_t i = img_size; i < parents_max_tree.size(); ++i) {
        std::cout << i - img_size << " -> " << parents_max_tree[i] - img_size << " alt " << max_tree.altitudes[i] << "\n";
    }
}

void Component_tree::print_parents_min_tree() const {
    std::cout << "Min tree -- parents:\n";
    for (size_t i = img_size; i < parents_min_tree.size(); ++i) {
        std::cout << i - img_size << " -> " << parents_min_tree[i] - img_size << " alt " << min_tree.altitudes[i] << "\n";
    }
}