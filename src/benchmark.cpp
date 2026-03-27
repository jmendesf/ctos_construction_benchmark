#include "benchmark.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "complete_tree_of_shapes.h"

namespace fs = std::filesystem;

namespace bench
{

    using clock_t = std::chrono::steady_clock;
    using ns = std::chrono::nanoseconds;

    static inline double to_ms(ns d)
    {
        return std::chrono::duration<double, std::milli>(d).count();
    }

    struct StepResult
    {
        std::string name;
        double ms;
    };
    struct RunResult
    {
        std::vector<StepResult> steps;
        double total_ms = 0.0;
    };

    struct ScopedStepTimer
    {
        std::string name;
        clock_t::time_point start;
        std::vector<StepResult> &out;
        ScopedStepTimer(std::string n, std::vector<StepResult> &o)
            : name(std::move(n)), start(clock_t::now()), out(o) {}
        ~ScopedStepTimer()
        {
            auto end = clock_t::now();
            out.push_back({name, to_ms(std::chrono::duration_cast<ns>(end - start))});
        }
    };

    static double median(std::vector<double> v)
    {
        if (v.empty())
            return 0.0;
        std::sort(v.begin(), v.end());
        size_t m = v.size() / 2;
        return (v.size() % 2) ? v[m] : 0.5 * (v[m - 1] + v[m]);
    }

    static bool is_image_ext(std::string ext)
    {
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext == ".png" || ext == ".jpg" || ext == ".jpeg" ||
               ext == ".bmp" || ext == ".tif" || ext == ".tiff";
    }

    static std::vector<fs::path> list_images(const fs::path &folder, bool recursive)
    {
        std::vector<fs::path> out;

        if (!fs::exists(folder))
            return out;

        if (recursive)
        {
            for (auto const &e : fs::recursive_directory_iterator(folder))
            {
                if (!e.is_regular_file())
                    continue;
                if (is_image_ext(e.path().extension().string()))
                    out.push_back(e.path());
            }
        }
        else
        {
            for (auto const &e : fs::directory_iterator(folder))
            {
                if (!e.is_regular_file())
                    continue;
                if (is_image_ext(e.path().extension().string()))
                    out.push_back(e.path());
            }
        }

        std::sort(out.begin(), out.end());
        return out;
    }

    template <class T, class... Args>
    static std::unique_ptr<T> timed_new(std::vector<StepResult> &steps,
                                        const std::string &step_name,
                                        Args &&...args)
    {
        T *p = nullptr;
        {
            ScopedStepTimer t(step_name, steps);
            p = new T(std::forward<Args>(args)...);
        }
        return std::unique_ptr<T>(p);
    }

    static RunResult run_pipeline_A(const std::string &filename)
    {
        RunResult rr;
        auto t0 = clock_t::now();

        auto tos = timed_new<Tree_of_shapes>(rr.steps, "Constructor ToS", filename);
        Graph_of_shapes gos(*tos);

        {
            ScopedStepTimer t("Construction GoS", rr.steps);
            gos.build();
        }

        Complete_tree_of_shapes ctos(gos);
        {
            ScopedStepTimer t("Construction CToS (GoS)", rr.steps);
            ctos.build();
        }

        rr.total_ms = to_ms(std::chrono::duration_cast<ns>(clock_t::now() - t0));
        return rr;
    }

    static RunResult run_pipeline_B(const std::string &filename)
    {
        RunResult rr;
        auto t0 = clock_t::now();

        auto tos = timed_new<Tree_of_shapes>(rr.steps, "Constructor ToS", filename);
        auto ct = timed_new<Component_tree>(rr.steps, "Constructor CTs", filename);

        Complete_tree_of_shapes ctos(*ct, *tos);
        {
            ScopedStepTimer t("Construction CToS (ToS + CTs)", rr.steps);
            ctos.build_from_ct_and_tos();
        }

        rr.total_ms = to_ms(std::chrono::duration_cast<ns>(clock_t::now() - t0));
        return rr;
    }

    static double find_step_ms(const RunResult &r, const std::string &name)
    {
        for (auto const &s : r.steps)
            if (s.name == name)
                return s.ms;
        return 0.0;
    }

    bool run_folder_benchmark(const Options &opt)
    {
        auto images = list_images(opt.folder, opt.recursive);
        if (images.empty())
        {
            std::cerr << "No images found in: " << opt.folder << "\n";
            return false;
        }

        std::ofstream csv(opt.csv_path);
        if (!csv)
        {
            std::cerr << "Failed to open output CSV: " << opt.csv_path << "\n";
            return false;
        }

        // Fixed columns
        const std::vector<std::string> colsA = {
            "Constructor ToS", "Construction GoS", "Construction CToS (GoS)"};
        const std::vector<std::string> colsB = {
            "Constructor ToS", "Constructor CTs", "Construction CToS (ToS + CTs)"};

        // Header
        csv << "image"
            << ",num_pix"
            << ",TOTAL_GOS(A)";
        for (auto &c : colsA)
            csv << ",A_" << c;
        csv << ",Nb_nodes_ToS";
        csv << ",Nb_ajd_edges_GoS";
        csv << ",Nb_nodes_CToS";
        csv << ",TOTAL_CT(B)";
        for (auto &c : colsB)
            csv << ",B_" << c;
        csv << ",Nb_nodes_maxT";
        csv << ",Nb_nodes_minT";
        csv << ",Nb_total_nodes_ct";
        csv << ",Nb_nodes_ctos";
        csv << "\n";

        for (auto const &p : images)
        {
            const std::string file = p.string();
            std::cerr << "Benchmarking: " << file << "\n";
            try
            {
                Tree_of_shapes tos(file);
                Graph_of_shapes gos(tos);
                Component_tree ct(file);
                gos.build();
                Complete_tree_of_shapes ctos_gos(gos);
                Complete_tree_of_shapes ctos_ct(ct, tos);

                ctos_gos.build();
                ctos_ct.build_from_ct_and_tos();

                // warmup (per image)
                for (int i = 0; i < opt.warmup_per_image; ++i)
                {
                    (void)run_pipeline_A(file);
                    (void)run_pipeline_B(file);
                }

                std::vector<double> A_totals, B_totals;
                std::vector<std::vector<double>> A_steps(colsA.size()), B_steps(colsB.size());

                for (int i = 0; i < opt.iters_per_image; ++i)
                {
                    auto ra = run_pipeline_A(file);
                    for (auto &s : ra.steps)
                    {
                        if (!std::isfinite(s.ms))
                        {
                            std::cerr << "Non-finite in A step '" << s.name << "' on " << file << "\n";
                        }
                    }
                    auto rb = run_pipeline_B(file);

                    A_totals.push_back(ra.total_ms);
                    for (size_t k = 0; k < colsA.size(); ++k)
                        A_steps[k].push_back(find_step_ms(ra, colsA[k]));

                    B_totals.push_back(rb.total_ms);
                    for (size_t k = 0; k < colsB.size(); ++k)
                        B_steps[k].push_back(find_step_ms(rb, colsB[k]));
                }

                csv << std::quoted(file)
                    << "," << tos.img_size
                    << "," << median(A_totals);
                for (size_t k = 0; k < colsA.size(); ++k)
                    csv << "," << median(A_steps[k]);
                csv << "," << tos.nb_nodes();
                csv << "," << gos.nb_adj_edges;
                csv << "," << ctos_gos.nb_nodes();
                csv << "," << median(B_totals);
                for (size_t k = 0; k < colsB.size(); ++k)
                    csv << "," << median(B_steps[k]);
                csv << "," << ct.max_tree.tree.num_vertices() - tos.img_size;
                csv << "," << ct.min_tree.tree.num_vertices() - tos.img_size;
                csv << "," << (ct.max_tree.tree.num_vertices() - tos.img_size) + ct.min_tree.tree.num_vertices() - tos.img_size;
                csv << "," << ctos_ct.nb_nodes();
                csv << "\n";
                csv.flush();
            }
            catch (const std::exception &e)
            {
                std::cerr << "ERROR on " << file << ": " << e.what() << "\n";
                csv << std::quoted(file) << ",ERROR," << std::quoted(e.what()) << "\n";
                csv.flush();
                continue;
            }
            catch (...)
            {
                std::cerr << "UNKNOWN ERROR on " << file << "\n";
                csv << std::quoted(file) << ",ERROR,UNKNOWN\n";
                csv.flush();
                continue;
            }
        }
        std::cerr << "Wrote " << opt.csv_path << "\n";
        return true;
    }
}