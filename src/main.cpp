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

    bench::Options opt;
    opt.folder = (argc >= 2) ? argv[1] : "../ressource/benchmark/unsplash_png/";
    //opt.folder = (argc >= 2) ? argv[1] : "../ressource/benchmark/";
    opt.warmup_per_image = (argc >= 3) ? std::stoi(argv[2]) : 2;
    opt.iters_per_image = (argc >= 4) ? std::stoi(argv[3]) : 10;
    opt.csv_path = (argc >= 5) ? argv[4] : "results.csv";

    if (!bench::run_folder_benchmark(opt))
    {
        return 1;
    }

    return 0;
}
