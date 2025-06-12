#define OPTISCOPE_TESTS_NO_MAIN
#include "../tests.c"

static struct lambda_term *
multiply_by_two(void) {
    struct lambda_term *x;

    return lambda(x, binary_call(multiply, var(x), cell(2)));
}

static struct lambda_term *
generate_tree(const uint64_t n) {
    if (1 == n) { return apply(scott_leaf(), cell(1)); }

    return apply(
        apply(scott_node(), generate_tree(n / 2)), generate_tree(n / 2));
}

#define BENCHMARK_TERM                                                         \
    apply(                                                                     \
        scott_tree_sum(),                                                      \
        apply(                                                                 \
            apply(scott_tree_map(), multiply_by_two()),                        \
            generate_tree(65536 /* 2^16 */)))

int
main(void) {
    optiscope_open_pools();
    optiscope_algorithm(NULL, BENCHMARK_TERM);
    optiscope_close_pools();
}
