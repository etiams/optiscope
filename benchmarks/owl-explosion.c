#define OPTISCOPE_TESTS_NO_MAIN
#include "../tests.c"

#include <stdint.h>

static struct lambda_term *
owl_combinator(void) {
    struct lambda_term *a, *b;

    return lambda(a, lambda(b, applicator(var(b), applicator(var(a), var(b)))));
}

static struct lambda_term *
owl_explosion(const uint64_t depth) {
    assert(depth > 0);

    struct lambda_term *result = owl_combinator();

    for (uint64_t i = 1; i < depth; i++) {
        result = applicator(result, owl_combinator());
    }

    return result;
}

#define BENCHMARK_TERM applicator(owl_explosion(5000), owl_combinator())

int
main(void) {
    optiscope_open_pools();
    optiscope_algorithm(NULL, BENCHMARK_TERM);
    optiscope_close_pools();
}
