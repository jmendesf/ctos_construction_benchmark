#include <iostream>
#include <utility>
#include <vector>

#include <higra/hierarchy/component_tree.hpp>
#include <higra/image/graph_image.hpp>
#include <higra/image/tree_of_shapes.hpp>
#include <complete_tree_of_shapes.h>
#include "component_tree.h"
#include "node_tos.h"

#include <opencv2/opencv.hpp>
#include "benchmark.h"

int main(int argc, char** argv)
{
    
    /*
    bench::Options opt;
    opt.folder = (argc >= 2) ? argv[1] : "/home/jmendesf/datasets/unsplash_padded2";
    opt.warmup_per_image = (argc >= 3) ? std::stoi(argv[2]) : 2;
    opt.iters_per_image = (argc >= 4) ? std::stoi(argv[3]) : 10;
    opt.csv_path = (argc >= 5) ? argv[4] : "results.csv";

    if (!bench::run_folder_benchmark(opt))
    {
        return 1;
    }
    */

    //std::string filename = "/home/jmendesforte/ctos_construction_benchmark/ressource/benchmark/unsplash_png/unsplash_000000.png";
    //std::string filename = "/home/jmendesf/implementations/ctos_construction_benchmark/ressource/benchmark/Input.png";
    //std::string filename = "/home/jmendesforte/ctos_construction_benchmark/tests/test_double_khalimsky/image_kh_max.png";
    //std::string filename = "/home/jmendesforte/ctos_construction_benchmark/tests/test_double_khalimsky/img_khal_max_min.png";
    std::string filename = "/home/jmendesforte/ctos_construction_benchmark/tests/test_double_khalimsky/image_kh_max_quantif_huge.png";
    //std::string filename = "/home/jmendesforte/ctos_construction_benchmark/random_quantized2.png";


    std::cout << "Building tos\n";
    Tree_of_shapes tos = Tree_of_shapes(filename);
    std::cout << tos.nb_nodes() << " tos nodes\n";
    //tos.print_parents();
    //tos.traverse_tree();

    Graph_of_shapes gos = Graph_of_shapes(tos);
    gos.build();

    std::cout << "nb adj edges gos: " << gos.nb_adj_edges << "\n";
    std::cout << "nb edges gos: " << gos.nb_edges() << "\n";

    Complete_tree_of_shapes ctos = Complete_tree_of_shapes(gos);
    ctos.build();
    std::cout << "nb nodes ctos: " << ctos.nb_nodes() << "\n";
    std::cout << "printing ctos ----- \n" ;
    //ctos.print_tree();
    std::cout << "Built with tos only. \n"; 

    
    Component_tree ct = Component_tree(filename);
    std::cout << "maxt size: " << ct.max_tree.tree.num_vertices() - tos.img_size << "\n";
    std::cout << "mint size: " << ct.min_tree.tree.num_vertices() - tos.img_size << "\n";
    
    Complete_tree_of_shapes ctos2 = Complete_tree_of_shapes(ct, tos);
    ctos2.build_from_ct_and_tos();
    std::cout << "nb nodes ctos2: " << ctos2.nb_nodes() << "\n";
    std::cout << "printing ctos2 ----- \n\n\n" ;
    //ctos2.print_tree();

    if (ctos2.nb_nodes() != ctos.nb_nodes())
    {
        std::cout << "UNEVEN NB OF NODES \n";
    } else 
    {
        std::cout << "Same number of nodes \n";
    }

    std::cout << "\n";
    
    return 0;
}
