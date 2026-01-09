#include "tree_of_shapes.h"
#include <iostream>
#include <chrono>

Tree_of_shapes::Tree_of_shapes(const std::string &filename)
{
    // Load grayscale
    image = cv::imread(filename, cv::IMREAD_GRAYSCALE);
    if (image.empty())
    {
        throw std::runtime_error("Failed to load image: " + filename);
    }
    img_size = image.rows * image.cols;

    // img to xtensor
    auto img_xt = xt::adapt(
        image.data,
        static_cast<size_t>(image.total()),
        xt::no_ownership(),
        std::vector<size_t>{(size_t)image.rows, (size_t)image.cols});

    // Higra tos computation
    tos = hg::component_tree_tree_of_shapes_image(img_xt);

    // Higra: preprocessing children
    tos.tree.compute_children();
    parents = tos.tree.parents();

    // Higra Tos to this
    pre_process_tos();
}

void Tree_of_shapes::pre_process_tos()
{
    // init root
    long id_root = tos.tree.root() - img_size;
    int alt_root = tos.altitudes[tos.tree.root()];
    nodes[id_root] = std::make_unique<Node_tos>(id_root, alt_root, nullptr);
    root = nodes[id_root].get();
    root->root = true;
    highest_value = root->alt;

    // init rest of the tree
    std::vector<long> queue = {id_root};
    while (!queue.empty())
    {
        long current = queue.back();
        queue.pop_back();
        
        Node_tos *current_node = nodes[current].get();
        for (auto child : tos.tree.children(current + img_size))
        {
            if (child < img_size)
            {
                proper_parts[current].emplace_back(child);
                continue;
            }

            long id_child = child - img_size;
            int child_alt = tos.altitudes[child];

            nodes[id_child] = std::make_unique<Node_tos>(id_child, child_alt, current_node);
            if (nodes[id_child].get()->alt > highest_value) {
                highest_value =  nodes[id_child].get()->alt;
            }
            // Node_tos *child_node = nodes[id_child].get();
            queue.push_back(child - img_size);
        }
    }
}

// Debugging
void Tree_of_shapes::traverse_tree()
{
    std::vector<long> stack = {tos.tree.root() - img_size};

    while (!stack.empty())
    {
        long current_id = stack.back();
        stack.pop_back();

        Node_tos *current = nodes[current_id].get();
        if (current->parent != nullptr)
        {
            std::cout << "Node " << current_id << " parent " << current->parent->name << "\n";
        }

        for (auto child : current->children)
        {
            stack.push_back(child->name);
        }
    }
}

// Debugging
void Tree_of_shapes::print_parents() const
{
    std::cout << "Tree of Shapes -- parents:\n";
    for (size_t i = img_size; i < parents.size(); ++i)
    {
        std::cout << i - img_size << " -> " << parents[i] - img_size << "\n";
    }
}