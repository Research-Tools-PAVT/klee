//===-- Constraints.h -------------------------------------------*- C++ -*-===//
//
//                     The KLEE Symbolic Virtual Machine
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef KLEE_CONSTRAINTS_H
#define KLEE_CONSTRAINTS_H

#include "klee/Expr/Expr.h"
#include "klee/json.hpp"
#include <string.h>
#include <vector>

using json = nlohmann::json;

namespace klee {

/// Resembles a set of constraints that can be passed around
///
class ConstraintSet {
  friend class ConstraintManager;

public:
  using constraints_ty = std::vector<ref<Expr>>;
  using iterator = constraints_ty::iterator;
  using const_iterator = constraints_ty::const_iterator;

  using constraint_iterator = const_iterator;

  bool empty() const;
  constraint_iterator begin() const;
  constraint_iterator end() const;
  size_t size() const noexcept;

  explicit ConstraintSet(constraints_ty cs) : constraints(std::move(cs)) {}
  ConstraintSet() = default;

  void push_back(const ref<Expr> &e);

  bool operator==(const ConstraintSet &b) const {
    return constraints == b.constraints;
  }

  /// COMMENT : Stream the Constraint Set to stringstream.
  // Extra Function. Just to dirty print the constraints in an ExecutionState.
  std::vector<std::string> printConstraintSetTY() {

    std::vector<std::string> constraints_mix;

    if (constraints.size() <= 0) {
      return constraints_mix;
    }

    int counter = 0;
    std::stringstream temp;
    for (const ref<Expr> &conditions : constraints) {
      counter++;
      temp.str(std::string());
      temp << conditions;
      std::string cond;
      cond = temp.str();

      /* COMMENT : Remove extra spaces and '\n' character. */
      cond.erase(std::remove(cond.begin(), cond.end(), '\n'), cond.end());
      cond.erase(std::unique(std::begin(cond), std::end(cond),
                             [](unsigned char a, unsigned char b) {
                               return std::isspace(a) && std::isspace(b);
                             }),
                 std::end(cond));
      constraints_mix.emplace_back(cond);
    }

    return constraints_mix;
  }

private:
  constraints_ty constraints;
};

class ExprVisitor;

/// Manages constraints, e.g. optimisation
class ConstraintManager {
public:
  /// Create constraint manager that modifies constraints
  /// \param constraints
  explicit ConstraintManager(ConstraintSet &constraints);

  /// Simplify expression expr based on constraints
  /// \param constraints set of constraints used for simplification
  /// \param expr to simplify
  /// \return simplified expression
  static ref<Expr> simplifyExpr(const ConstraintSet &constraints,
                                const ref<Expr> &expr);

  /// Add constraint to the referenced constraint set
  /// \param constraint
  void addConstraint(const ref<Expr> &constraint);

private:
  /// Rewrite set of constraints using the visitor
  /// \param visitor constraint rewriter
  /// \return true iff any constraint has been changed
  bool rewriteConstraints(ExprVisitor &visitor);

  /// Add constraint to the set of constraints
  void addConstraintInternal(const ref<Expr> &constraint);

  ConstraintSet &constraints;
};

} // namespace klee

#endif /* KLEE_CONSTRAINTS_H */