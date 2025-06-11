#define OPTISCOPE_TESTS_NO_MAIN
#include "../tests.c"

static struct lambda_term *
church_ten(void) {
    return applicator(applicator(church_add(), church_five()), church_five());
}

static struct lambda_term *
church_twenty(void) {
    return applicator(applicator(church_add(), church_ten()), church_ten());
}

#define BENCHMARK_TERM                                                         \
    applicator(                                                                \
        applicator(                                                            \
            applicator(                                                        \
                applicator(y_combinator(), y_fibonacci_function()),            \
                church_twenty()),                                              \
            i_combinator()),                                                   \
        i_combinator())

int
main(void) {
    optiscope_open_pools();
    optiscope_algorithm(NULL, BENCHMARK_TERM);
    optiscope_close_pools();
}
