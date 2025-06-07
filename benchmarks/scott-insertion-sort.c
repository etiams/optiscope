#define OPTISCOPE_TESTS_NO_MAIN
#include "../tests.c"

static struct lambda_term *
generate_list(const uint64_t n) {
    struct lambda_term *term = scott_nil();
    for (uint64_t i = 0; i < n; i++) {
        term = applicator(applicator(scott_cons(), cell(i)), term);
    }

    return term;
}

#define BENCHMARK_TERM                                                         \
    applicator(                                                                \
        scott_sum_list(),                                                      \
        applicator(scott_insertion_sort(), generate_list(150)))

int
main(void) {
    optiscope_open_pools();
    optiscope_algorithm(NULL, BENCHMARK_TERM);
    optiscope_close_pools();
}
