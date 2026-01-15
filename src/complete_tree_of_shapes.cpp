#include "complete_tree_of_shapes.h"

void Complete_tree_of_shapes::build()
{
    if (gos == nullptr)
    {
        throw std::runtime_error("Can't build from graph of shapes: structure is not defined.");
    }

    // std::vector<long> stack = {gos->tos.root->name};
    std::vector<std::tuple<long, Node_ct*>> stack = {{gos->tos.root->name, nullptr}};
    bool is_root = true;
    int id = 0;

    Node_ct *parent_ct_node = nullptr;
    Node_ct *current_ct_node = nullptr;
    while (!stack.empty())
    {
        std::tuple<long, Node_ct*> current_tuple = stack.back();
        long current_id = std::get<0>(current_tuple);
        parent_ct_node = std::get<1>(current_tuple);
        stack.pop_back();

        Node_tos *current = gos->tos.nodes[current_id].get();
        current->sort_adj_holes();

        // Node has no hole
        if (current->children_gos_adj.size() == 0) 
        {
            nodes[id] = std::make_unique<Node_ct>(id, current->interval[1], current->node_class, parent_ct_node);
            current_ct_node = nodes[id].get();
            current_ct_node->id_node_tos = current->name;

            if (is_root) 
            {
                current_ct_node->root = true;
                root = current_ct_node;
                is_root = false;
            }
            id++;
        }
        // Node has at least one hole
        else 
        {
            // Initial first node
            nodes[id] = std::make_unique<Node_ct>(id, -1, current->node_class, parent_ct_node);
            current_ct_node = nodes[id].get();
            current_ct_node->id_node_tos = current->name;
            id++;

            // Assign root information
            if (is_root)
            {
                current_ct_node->root = true;
                root = current_ct_node;
                is_root = false;
            }

            int current_alpha = current->interval[0];
            // Go through all nodes and create new ct node for each interval
            for (auto hole_tuple : current->children_gos_adj)
            {
                Node_tos *hole_node = std::get<0>(hole_tuple);
                std::array<int, 2> hole_interval = std::get<1>(hole_tuple);
                
                if (hole_interval[0] != current_alpha)
                {
                    int omega = current->node_class == MAX_TREE ? hole_interval[0] - 1 : hole_interval[0] + 1;
                    current_ct_node->alt = omega;
                    parent_ct_node = current_ct_node;
                    
                    nodes[id] = std::make_unique<Node_ct>(id, -1, current->node_class, parent_ct_node);
                    current_ct_node = nodes[id].get();
                    current_ct_node->id_node_tos = current->name;    
                    id++;
                    current_alpha = hole_interval[0];
                }
            }

            current_ct_node->alt = current->interval[1];
        }
        
        for (auto child : current->children)
        {
            stack.push_back({child->name, current_ct_node});
        }
    }

    return;
}


void Complete_tree_of_shapes::build_from_ct_and_tos()
{
    
    if (ct == nullptr) 
    {
        throw std::runtime_error("Can't build from component tree: structure is not defined.");
    }
    if (tos == nullptr)
    {
        throw std::runtime_error("Can't build from tree of shapes: structure is not defined.");
    }

    std::vector<std::tuple<long, Node_ct*>> stack = {{tos->root->name, nullptr}};
    long id = 0;

    // to delete
    int nb_loops = 0;

    while (!stack.empty())
    {
        std::tuple<long, Node_ct*> current_tuple = stack.back();
        stack.pop_back();
        long id_node_tos = std::get<0>(current_tuple);
        Node_ct* parent = std::get<1>(current_tuple);
        std::cout << "Treating tos node " << id_node_tos << "\n";
        
        Node_tos *current = tos->nodes.at(id_node_tos).get();

        if (!tos->enriched)
        {
            current->enrich(tos->highest_value);
        }

        std::cout << "Tos node has interval (" << current->interval[0] << "," << current->interval[1] << ') \n';

        long pixel = tos->proper_parts.at(current->name).at(0);
        long ct_node_X_id = -1;
        hg::array_1d<hg::index_t> *parents = current->node_class == MAX_TREE ? &ct->parents_max_tree : &ct->parents_min_tree;   
        hg::array_1d<int> *altitudes = current->node_class == MAX_TREE ? &ct->max_tree.altitudes : &ct->min_tree.altitudes;
        std::string nodetype = current->node_class == MAX_TREE ? "maxtree" : "mintree";
        
        ct_node_X_id = (*parents)[pixel];
        
        std::cout << "Tos node pp corresponds to ct node " << ct_node_X_id << " of altitude " << (*altitudes)[ct_node_X_id] << "\n";

        nodes[id] = std::make_unique<Node_ct>(id, (*altitudes)[ct_node_X_id], current->node_class, nullptr);
        Node_ct *node_x = nodes[id].get();
        node_x->id_node_tos = current->name;
        Node_ct *node_y = node_x;

        long ct_node_Y_id = ct_node_X_id;
        long alpha_y = (*altitudes)[(*parents)[ct_node_X_id]];

        std::cout << "altitude of parent (alpha_y) = " << alpha_y << "\n";

        id++;

        while ((current->node_class == MAX_TREE && alpha_y >= current->interval[0]) || (current->node_class == MIN_TREE && alpha_y <= current->interval[0]))
        {
            std::cout << "parent of ct node is alt " << alpha_y << " and target is " << current->interval[0] << "\n";
            // get the parent of Y node in the component tree
            long ct_node_z_id = (*parents)[ct_node_Y_id];
            if (ct_node_z_id == ct_node_Y_id)
            {
                break;
            }
            std::cout << "parent of ct node is " << ct_node_z_id << "\n";
            
            // create the corresponding node
            nodes[id] = std::make_unique<Node_ct>(id, (*altitudes)[ct_node_z_id], current->node_class, nullptr);
            Node_ct *node_z = nodes.at(id).get();
            node_z->id_node_tos = current->name;
            // update relationship
            node_y->parent = node_z;
            node_z->children.push_back(node_y);

            id++;
            ct_node_Y_id = ct_node_z_id;
            alpha_y = (*altitudes)[(*parents)[ct_node_Y_id]];
            node_y = node_z;
        }
        
        std::cout << "exited because alpha_y = " << alpha_y << "\n";

        if (parent != nullptr)
        {
            node_y->parent = parent;
            parent->children.push_back(node_y);
        } else
        {
            node_y->root = true;
            root = node_y;
        }

        for (auto child : current->children)
        {
            stack.emplace_back(child->name, node_x);
        }
    }
}

long Complete_tree_of_shapes::nb_nodes()
{
    return nodes.size();
}

void Complete_tree_of_shapes::print_tree() 
{
    if (root == nullptr)
    {
        throw std::runtime_error("Root is null.");
    }
    std::vector<long> stack = {root->name};

    while (!stack.empty()) 
    {
        long current_id = stack.back();
        stack.pop_back();
        Node_ct *current = nodes[current_id].get();

        std::cout << "Node " << current->name << " (" << current->alt << ") -";
        if (current->parent != nullptr) 
        {
            std:: cout << " parent: " << current->parent->name << " - ";
        }

        std::cout << " tos node: " << current->id_node_tos << std::endl;

        for (auto& c : current->children)
        {
            stack.push_back(c->name);
        }
    }
}