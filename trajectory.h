// Copyright 2015 Google, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
//     distributed under the License is distributed on an "AS IS" BASIS,
//     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include <vision/camera.h>
#include "circular_array.h"

// Represents a 3D trajectory with an initial position and velocity.
template <typename T>
struct trajectory {
  vector3<T> x, v;

  template <typename U>
  vector3<U> position_half_g(float half_g, const U &t) const {
    return vector3<U>(
      v.x*t + x.x,
      v.y*t + x.y,
      half_g*sqr(t) + (v.z*t + x.z));
  }

  template <typename U>
  vector3<U> position(float g, const U &t) const {
    return position_half_g(g*0.5f, t);
  }
};

typedef trajectory<float> trajectoryf;

// This function finds an intersection of the trajectory and the sphere between t_min and t_max.
float intersect_trajectory_sphere(
    float gravity, const trajectoryf &tj,
    const vector3f &s, float r,
    float t_min, float t_max);

// Find the intersection of a trajectory with the z plane. This function computes the
// later (larger t) of the two intersections.
float intersect_trajectory_zplane(float gravity, const trajectoryf &tj, float z);

// Describes a 2D observation/time pair.

// Estimates a trajectory tj given a set of observations from two *unsynchronized*
// cameras. The optimization variable dt is the relative time shift between the cameras.
// The input values of tj and dt are used as an initial guess for the optimization.
// After the optimization is run, observations that are outliers according to the noise
// model are tagged as such.
struct observation {
  float t;
  vector2f f;

  observation() : t(0.0f) {}
  observation(float t, const vector2f &f) : t(t), f(f) {}
};
typedef circular_array<observation, 128> observation_buffer;
float estimate_trajectory(
    float gravity,
    const cameraf &cam0, const cameraf &cam1,
    observation_buffer &obs0, observation_buffer &obs1,
    float &dt, trajectoryf &tj,
    float time_limit = std::numeric_limits<float>::infinity());

#endif
