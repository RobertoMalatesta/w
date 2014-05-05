#pragma once
#ifndef WAYWARD_DEV_SERVER_RECOMPILER_HPP_INCLUDED
#define WAYWARD_DEV_SERVER_RECOMPILER_HPP_INCLUDED

#include <wayward/support/error.hpp>

namespace wayward {
  struct CompilationError : Error {
    CompilationError(const std::string& message) : Error(message) {}
  };

  struct Recompiler {
    explicit Recompiler(const std::string path);

    // This internally invokes `make -q` in the path.
    bool needs_rebuild();

    void rebuild();
  };
}

#endif // WAYWARD_DEV_SERVER_RECOMPILER_HPP_INCLUDED
