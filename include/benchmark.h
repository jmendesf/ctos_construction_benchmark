#pragma once

#include <filesystem>
#include <string>

namespace bench
{

  struct Options
  {
    std::filesystem::path folder;
    int warmup_per_image = 2;
    int iters_per_image = 10;
    bool recursive = false;

    // output file
    std::filesystem::path csv_path = "results.csv";
  };

  /// Runs pipeline A (ToS -> GoS -> CToS) and pipeline B (ToS + CT -> direct CToS)
  /// over all images in options.folder and writes a CSV of median times per image.
  /// Returns false if no images found or output failed.
  bool run_folder_benchmark(const Options &options);

} // namespace bench