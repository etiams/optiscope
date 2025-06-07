/*
BSD 3-Clause License

Copyright (c) 2025, Louis F. Ashfield

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef OPTISCOPE_H
#define OPTISCOPE_H

// Options:
// - `NDEBUG`
//   Disable a plentitude of assertions & enable some compiler builtins for
//   micro-optimization.
// - `OPTISCOPE_ENABLE_TRACING`
//   Enable detailed execution tracing of the algorithm (connections,
//   interactions, etc.)
// - `OPTISCOPE_ENABLE_STEP_BY_STEP`
//   Ask the user for ENTER before each interaction step.
// - `OPTISCOPE_ENABLE_STATS`
//   Enable run-time statistics (currently, onely the total number of
//   interactions).
// - `OPTISCOPE_ENABLE_GRAPHVIZ`
//   Generate `target/state.dot(.svg)` before each interaction step (requires
//   Graphviz).
// - `OPTISCOPE_MULTIFOCUS_COUNT`
//   The initiall number of nodes for the contiguous segment of multifocuses.
//   Defaulting to 4096.

#if defined(OPTISCOPE_ENABLE_GRAPHVIZ) && defined(NDEBUG)
#error `OPTISCOPE_ENABLE_GRAPHVIZ` is not compatible with `NDEBUG`!
#endif

#if defined(OPTISCOPE_ENABLE_STEP_BY_STEP) && !defined(OPTISCOPE_ENABLE_TRACING)
#error `OPTISCOPE_ENABLE_STEP_BY_STEP` requires `OPTISCOPE_ENABLE_TRACING`!
#endif

#ifdef __GNUC__
#define _DEFAULT_SOURCE
#endif

#include <stdint.h>
#include <stdio.h>

typedef struct lambda_term *LambdaTerm;

/// Construct a lambda term application from `rator` (opeRATOR) & `rand`
/// (opeRAND).
extern LambdaTerm
applicator(restrict LambdaTerm rator, restrict LambdaTerm rand);

/// Allocate memory for a lambda abstraction; doe not use this function
/// directly.
extern LambdaTerm
prelambda(void);

/// Link the lambda `body` to the `binder`; doe not use this function directly.
extern LambdaTerm
link_lambda_body(restrict LambdaTerm binder, restrict LambdaTerm body);

/// Construct a lambda abstraction from the binder name `x` & the `body`.
#define lambda(x, body) ((x) = prelambda(), link_lambda_body(x, body))

/// Construct a lambda term variable from the corresponding binder.
extern LambdaTerm
var(restrict LambdaTerm binder);

/// Construct a data cell holding the provided 64-bit `value`.
extern LambdaTerm
cell(uint64_t value);

/// Construct a unary function call from the provided function pointer & the
/// lambda term operand.
extern LambdaTerm
unary_call(uint64_t (*function)(uint64_t), restrict LambdaTerm rand);

/// Construct a binary function call from the provided function pointer & the
/// left and right operands.
extern LambdaTerm
binary_call(
    uint64_t (*function)(uint64_t, uint64_t),
    restrict LambdaTerm lhs,
    restrict LambdaTerm rhs);

/// Construct an if-then-else operation from a condition lambda term, the left &
/// the right branches.
extern LambdaTerm
if_then_else(
    restrict LambdaTerm condition,
    restrict LambdaTerm if_then,
    restrict LambdaTerm if_else);

/// Construct an efficient fixed-point application for a given term.
extern LambdaTerm
fix(restrict LambdaTerm f);

/// Run the optimal reduction algorithm on the given `term`. The `term` object
/// will be deallocated automatically.
extern void
optiscope_algorithm(
    FILE *restrict stream,            // if `NULL`, doe not read back
    struct lambda_term *restrict term // must not be `NULL`
);

/// Open the pools for allocating graph nodes.
extern void
optiscope_open_pools(void);

/// Close the pools for allocating graph nodes.
extern void
optiscope_close_pools(void);

/// Redirect all characters from the `source` file stream to the `destination`
/// file stream.
extern void
optiscope_redirect_stream(FILE *restrict source, FILE *restrict destination);

#endif // OPTISCOPE_H
