#ifndef INK_STROKE_MODELER_INTERNAL_VALIDATION_H_
#define INK_STROKE_MODELER_INTERNAL_VALIDATION_H_

//#include "absl/status/status.h"
//#include "absl/strings/string_view.h"
//#include "absl/strings/substitute.h"

template <typename T>
int ValidateIsFiniteNumber(T value, std::string label) {
  // std::isnan(integer value) fails to compile with Lexan C++20
  // (b/329239835), so only call std::isnan for floating point values.
  if constexpr (std::is_floating_point_v<T>) {
    if (std::isnan(value)) {
      return -1;
//      return absl::InvalidArgumentError(absl::Substitute("$0 is NaN", label));
    }
    if (std::isinf(value)) {
      return -1;
//      return absl::InvalidArgumentError(
//          absl::Substitute("$0 is infinite", label));
    }
  }
  return 0;
}

template <typename T>
int ValidateGreaterThanZero(T value, std::string label) {
  if constexpr (std::is_floating_point_v<T>) {
    if (int status = ValidateIsFiniteNumber(value, label);
        status < 0) {
      return status;
    }
  }
  if (value <= 0) {
    return -1;
//    return absl::InvalidArgumentError(absl::Substitute(
//        "$0 must be greater than zero. Actual value: $1", label, value));
  }
  return 0;
}

template <typename T>
int ValidateGreaterThanOrEqualToZero(T value,
                                     std::string label) {
  if (int status = ValidateIsFiniteNumber(value, label);
      status < 0) {
    return status;
  }
  if (value < 0) {
    return -1;
//    return absl::InvalidArgumentError(absl::Substitute(
//        "$0 must be greater than or equal to zero. Actual value: $1", label,
//        value));
  }
  return 0;
}

#endif  // INK_STROKE_MODELER_INTERNAL_VALIDATION_H_
