#include "graph_of_shapes.h"

Graph_of_shapes::Graph_of_shapes(Tree_of_shapes &tree_of_shapes)
    : tos(tree_of_shapes)
{
}

void Graph_of_shapes::build()
{
    std::vector<long> stack = {tos.root->name};

    while (!stack.empty())
    {
        long current_id = stack.back();
        stack.pop_back();

        Node_tos *current = tos.nodes[current_id].get();

        // Enriching node if necessary
        if (!tos.enriched)
        {
            current->enrich(tos.highest_value);
        }

        // Compute graph of shapes relations
        compute_gos_relations(current);

        for (auto child : current->children)
        {
            stack.push_back(child->name);
        }
    }

    tos.enriched = true;
}

void Graph_of_shapes::compute_gos_relations(Node_tos *current)
{
    if (current->root)
    {
        return;
    }

    // Case 1: node is the child of the root:
    // Node is added to both ct and adj relation of its tos parent (root)
    if (current->parent->root)
    {
        current->add_ct_parent(current->parent);
        current->add_adj_parent(current->interval, current->parent);
        return;
    }

    Node_tos *z = current->parent->parent_adj.at(current->parent->interval[1])->get_ct_parent(current->parent->interval[1]);
    Node_tos *first_adj = nullptr;

    // Case 2: current node and its parent have different node class
    if (current->node_class != current->parent->node_class)
    {
        current->add_ct_parent(z);
        first_adj = current->parent;
    }
    // Case 3: current node has the same ct class
    else
    {
        current->add_ct_parent(current->parent);
        first_adj = z;
    }

    // Common treatment: adding adjacency for all greylevel values of the current node
    compute_comprehensive_adjacency(current, first_adj);
}

void Graph_of_shapes::compute_comprehensive_adjacency(Node_tos *current, Node_tos *first_adj)
{
    int current_alpha = current->interval[0];
    int current_omega = -1;
    int omega = current->interval[1];
    Node_tos *adj_parent = first_adj;
    std::array<int, 2> adj_interval = {-1, -1};

    while (current_omega != omega)
    {
        if (current->node_class == MAX_TREE)
        {
            if (current->interval[1] <= adj_parent->interval[0])
            {
                current_omega = omega;
            }
            else
            {
                current_omega = adj_parent->interval[0];
            }
        }
        else
        {
            if (current->interval[1] >= adj_parent->interval[0])
            {
                current_omega = omega;
            }
            else
            {
                current_omega = adj_parent->interval[0];
            }
        }
        adj_interval = {current_alpha, current_omega};
        current->add_adj_parent(adj_interval, adj_parent);
        current_alpha = current->node_class == MAX_TREE ? current_omega + 1 : current_omega - 1;
        adj_parent = adj_parent->parent_ct;
    }
}

void Graph_of_shapes::print_graph()
{
    std::vector<long> stack = {tos.root->name};

    while (!stack.empty())
    {
        long current_id = stack.back();
        stack.pop_back();
        Node_tos *current = tos.nodes[current_id].get();

        std::cout << "Node " << current->name << " ";
        if (current->node_class == MAX_TREE)
        {
            std::cout << "(Max-T) ";
        }
        else
        {
            std::cout << "(Min-T) ";
        }
        std::cout << "[" << current->interval[0] << "," << current->interval[1] << "] - ";
        if (current->root)
        {
            std::cout << "Root" << std::endl;
        }
        else
        {
            std::cout << "CT-parent: " << current->parent_ct->name << ", ";
            std::cout << "Adj-parents: ";
            for (auto &interv_node : current->interval_parent_adj)
            {
                auto adj_inter = std::get<1>(interv_node);
                std::cout << std::get<0>(interv_node)->name << " [" << adj_inter[0] << "," << adj_inter[1] << "], ";
            }
            std::cout << std::endl;
        }

        for (auto child : current->children)
        {
            stack.push_back(child->name);
        }
    }
}
