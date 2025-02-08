#include "position_modeler.h"

#include <algorithm>
#include <cmath>
#include <limits>

//#include "absl/status/status.h"
//#include "absl/status/statusor.h"
//#include "absl/strings/substitute.h"
#include "internal_types.h"
#include "../params.h"
#include "../types.h"

namespace ink {
namespace stroke_model {

int NumberOfStepsBetweenInputs(
    const TipState& tip_state, const Input& start, const Input& end,
    const SamplingParams& sampling_params,
    const PositionModelerParams& position_modeler_params) {
  Duration delta_t = end.time - start.time;
  float float_delta = delta_t.Value();
  int n_steps =
      std::min(std::ceil(float_delta * sampling_params.min_output_rate),
               static_cast<double>(std::numeric_limits<int>::max()));
  Vec2 estimated_delta_v = (end.position - tip_state.position) /
                           position_modeler_params.spring_mass_constant *
                           float_delta;
  Vec2 estimated_end_v = tip_state.velocity + estimated_delta_v;
  float estimated_angle =
      tip_state.velocity.AbsoluteAngleTo(estimated_end_v);
  if (estimated_angle < 0) {
    return -1;
  }
  if (sampling_params.max_estimated_angle_to_traverse_per_input > 0) {
    int steps_for_angle = std::min(
        std::ceil(estimated_angle /
                  sampling_params.max_estimated_angle_to_traverse_per_input),
        static_cast<double>(std::numeric_limits<int>::max()));
    if (steps_for_angle > n_steps) {
      n_steps = steps_for_angle;
    }
  }
  if (n_steps > sampling_params.max_outputs_per_call) {
    return -1;
  }
  return n_steps;
}

TipState PositionModeler::Update(Vec2 anchor_position, Time time) {
  Duration delta_time = time - state_.time;
  state_.acceleration =
      ((anchor_position - state_.position) / params_.spring_mass_constant -
       params_.drag_constant * state_.velocity);
  state_.velocity += delta_time.Value() * state_.acceleration;
  state_.position += delta_time.Value() * state_.velocity;
  state_.time = time;

  return state_;
}

}  // namespace stroke_model
}  // namespace ink
