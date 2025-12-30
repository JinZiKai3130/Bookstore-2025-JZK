#include "../include/Error.hpp"

#include <utility>

InvalidOperation::InvalidOperation() : message_("Invalid") {}

const char *InvalidOperation::what() const noexcept { return message_.c_str(); }