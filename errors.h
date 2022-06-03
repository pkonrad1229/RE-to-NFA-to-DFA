#pragma once
#include <iostream>
#include <optional>

#ifdef PRINT_WITH_COLOR
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define CYAN "\033[1;36m"
#define SMALLRED "\033[0;31m"
#else
#define RESET ""
#define RED ""
#define GREEN ""
#define YELLOW ""
#define CYAN ""
#define SMALLRED ""
#endif

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
