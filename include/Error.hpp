#pragma once

#include <exception>
#include <string>

class InvalidOperation : public std::exception {
public:
  explicit InvalidOperation();

  const char *what() const noexcept override;

private:
  std::string message_;
};