#pragma once
#include <iostream>
#include <optional>

struct Error {
  std::string msg;

  Error() = default;

  explicit Error(std::string msg) : msg(std::move(msg)) {}
};

typedef std::optional<Error> OErr;

template <typename T>
struct ErrOr {
  OErr err;
  std::optional<T> data;
  bool error_happened;

  ErrOr() = default;
  ErrOr(const Error& err) : err(err) {}
  ErrOr(const T& data) : data(data) {}
  ErrOr(T&& data) : data(std::move(data)) {}
};
#define ERROR_WITH_FILE(msg) Error((msg) + std::string(" at ") + __FILE__ + ":" + std::to_string(__LINE__))
