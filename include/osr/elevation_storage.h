#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>

#include "cista/mmap.h"
#include "cista/strong.h"

#include "osr/preprocessing/elevation/shared.h"
#include "osr/types.h"
#include "osr/ways.h"

namespace osr {

namespace preprocessing::elevation {
struct provider;

struct elevation_params {
  unsigned max_points_{8U};
  std::optional<std::filesystem::path> stats_file_{};
  elevation_meters_t min_elevation_{0};
  double min_incline_{0};
  double max_incline_{1.0};
};
}  // namespace preprocessing::elevation

struct elevation_storage {
  using data_vec_t = mm_vec32<preprocessing::elevation::elevation_meters_t>;

  struct elevation {
    elevation& operator+=(elevation const&);
    elevation swapped() const;

    std::vector<data_vec_t::value_type> values_{};
    std::uint8_t n_{0};
    elevation_monotonic_t up_{0};
    elevation_monotonic_t down_{0};
  };

  struct encoding {
    bool valid() const { return n_ != 0; }

    way_idx_t::value_t idx_{0};
    std::uint8_t n_{0};
  };

  elevation_storage(std::filesystem::path const&,
                    cista::mmap::protection const mode);
  static std::unique_ptr<elevation_storage> try_open(
      std::filesystem::path const&);
  void set_elevations(ways const&,
                      preprocessing::elevation::provider const&,
                      preprocessing::elevation::elevation_params);
  elevation get_elevations(way_idx_t const way,
                           std::uint16_t const segment) const;

  // (way_idx, internal-idx) -> single elevation sample
  // mm_vecvec<way_idx_t, encoding> elevations_;
  data_vec_t elevations_;

  // (way_idx, segment_idx) -> internal-idx, number of samples
  mm_vecvec<way_idx_t, encoding> offsets_;
};

elevation_storage::elevation get_elevations(elevation_storage const*,
                                            way_idx_t const way,
                                            std::uint16_t const segment);

}  // namespace osr