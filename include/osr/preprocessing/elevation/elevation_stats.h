#pragma once

#include <sstream>

#include "osr/types.h"

namespace osr::preprocessing::elevation {

struct elevation_stats {
  static auto const kNBuckets = 8U;

  elevation_stats(unsigned max_points) : max_points_{max_points} {
    for (auto const _ : buckets_) {
      n_per_length_.add_back_sized(max_points);
    }
  }

  void clear() {
    n_segments_ = 0U;
    min_abs_threshold_ = 0;
    min_incline_ = 0;
    max_incline_ = 0;

    for (auto i = 0U; i < kNBuckets; ++i) {
      for (auto j = 0U; j < max_points_; ++j) {
        n_per_length_[i][j] = 0U;
      }
    }
  }

  void add(elevation_stats const& other) {
    n_segments_ += other.n_segments_;
    min_abs_threshold_ += other.min_abs_threshold_;
    min_incline_ += other.min_incline_;
    max_incline_ += other.max_incline_;

    for (auto i = 0U; i < kNBuckets; ++i) {
      for (auto j = 0U; j < max_points_; ++j) {
        n_per_length_[i][j] += other.n_per_length_[i][j];
      }
    }
  }

  std::string to_str() {
    std::stringstream ss{};

    ss << "Number of segments: " << n_segments_ << std::endl;
    ss << "Number of points per segment length (max=" << max_points_
       << "): " << std::endl;
    for (auto i = 0U; i < kNBuckets; ++i) {
      ss << buckets_[i] << "m: " << n_per_length_[i][0];
      for (auto j = 1U; j < max_points_; ++j) {
        ss << ", " << n_per_length_[i][j];
      }
      ss << std::endl;
    }
    ss << std::endl;

    ss << "below 'min_elevation': " << min_abs_threshold_ << std::endl;
    ss << "below 'min_incline': " << min_incline_ << std::endl;
    ss << "greater 'max_incline': " << max_incline_ << std::endl;

    return ss.str();
  }

  double const buckets_[kNBuckets] = {0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 10.0, 50.0};
  vecvec<std::size_t, unsigned> n_per_length_{};
  unsigned const max_points_;
  unsigned n_segments_{0}, min_abs_threshold_{0}, min_incline_{0},
      max_incline_{0};
};

}  // namespace osr::preprocessing::elevation
