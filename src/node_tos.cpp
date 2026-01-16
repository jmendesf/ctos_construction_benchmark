#include "node_tos.h"
#include <iostream>

Node_tos::Node_tos(const unsigned int id, int altitude, Node_tos *par)
    : alt(altitude), name(id), parent(par), children({}), parent_ct(nullptr), parent_adj({}), children_gos_adj({}), children_gos_ct({}), interval({-1, -1}), interval_parent_adj({})
{
    if (par != nullptr)
    {
        par->children.push_back(this);
    }
}

// Compute node class and interval
void Node_tos::enrich(int highest_alt)
{
    // Node class
    compute_node_class();
    // Interval
    compute_interval(highest_alt);
}

void Node_tos::compute_interval(int highest_alt)
{
    if (node_class == MIN_TREE)
    {
        alt += 1;
    }
    if (root)
    {
        interval[0] = highest_alt + 1;
        interval[1] = alt;
    }
    else if (node_class == parent->node_class)
    {
        if (node_class == MAX_TREE)
        {
            interval[0] = parent->alt + 1;
            interval[1] = alt;
        }
        else
        {
            interval[0] = parent->alt - 1;
            interval[1] = alt;
        }
    }
    else
    {
        interval[0] = parent->alt;
        interval[1] = alt;
    }
}

void Node_tos::compute_node_class()
{
    if (root)
    {
        node_class = MIN_TREE;
    }
    else if (parent->node_class == MAX_TREE)
    {
        if (alt > parent->alt)
        {
            node_class = MAX_TREE;
        }
        else
        {
            node_class = MIN_TREE;
        }
    }
    else if (parent->node_class == MIN_TREE)
    {
        if (alt < parent->alt)
        {
            node_class = MIN_TREE;
        }
        else
        {
            node_class = MAX_TREE;
        }
    }
}

void Node_tos::add_ct_parent(Node_tos *parent)
{
    parent_ct = parent;
    parent->children_gos_ct.push_back(this);
}

void Node_tos::add_adj_parent(std::array<int, 2> adj_interval, Node_tos *adj_parent)
{
    if (node_class == MAX_TREE)
    {
        for (int i = adj_interval[0]; i < adj_interval[1] + 1; i++)
        {
            parent_adj[i] = adj_parent;
        }
        std::array<int, 2> interv = {adj_interval[0], adj_interval[1]};
        interval_parent_adj.emplace_back(adj_parent, interv);
    }
    else
    {
        for (int i = adj_interval[1]; i > adj_interval[1] - 1; i--)
        {
            parent_adj[i] = adj_parent;
        }
        std::array<int, 2> interv = {adj_interval[1], adj_interval[0]};
        interval_parent_adj.emplace_back(adj_parent, interv);
    }
    std::array<int, 2> mirrored_interval = {adj_interval[1], adj_interval[0]};
    adj_parent->children_gos_adj.emplace_back(this, mirrored_interval);
}

Node_tos *Node_tos::get_ct_parent(int alt)
{
    if (root)
    {
        return this;
    }
    if (alt == interval[0])
    {
        return parent_ct;
    }
    if (node_class == MAX_TREE)
    {
        if (interval[0] < alt && interval[1] >= alt)
        {
            return this;
        }
    }
    else
    {
        if (interval[0] > alt && interval[1] <= alt)
        {
            return this;
        }
    }

    return nullptr;
}

void Node_tos::sort_adj_holes()
{
    if (holes_sorted) 
    {
        return;
    }
    if (node_class == MAX_TREE)
    {
        children_gos_adj.sort(
            [](const auto &a, const auto &b)
            {
                return std::get<1>(a)[0] < std::get<1>(b)[0];
            });
    }
    else
    {
        children_gos_adj.sort(
            [](const auto &a, const auto &b)
            {
                return std::get<1>(a)[0] > std::get<1>(b)[0];
            });
    }
    holes_sorted = true;
}

// debugging
void Node_tos::print_children()
{
    if (children.size() == 0)
    {
        std::cout << "Node " << name << " is leaf." << std::endl;
        return;
    }
    std::cout << "Child of " << name << ":" << std::endl;
    for (auto &child : children)
    {
        child->print_node();
    }
}

// debugging
void Node_tos::print_node()
{
    if (parent == nullptr)
    {
        std::cout << "Node " << name << " alt " << alt << " parent " << "gouzi" << std::endl;
    }
    else
    {
        std::cout << "Node " << name << " alt " << alt << " parent " << parent->name << std::endl;
    }
}