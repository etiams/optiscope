#include "../optiscope.h"

// clang-format off
static uint64_t is_zero(const uint64_t x) { return 0 == x; }

static uint64_t minus_one(const uint64_t x) { return x - 1; }

static uint64_t add(const uint64_t x, const uint64_t y)
    { return x + y; }

static uint64_t multiply(const uint64_t x, const uint64_t y)
    { return x * y; }
// clang-format on

static struct lambda_term *
scott_nil(void) {
    struct lambda_term *n, *c;

    return lambda(n, lambda(c, var(n)));
}

static struct lambda_term *
scott_cons(void) {
    struct lambda_term *h, *t, *n, *c;

    return lambda(
        h,
        lambda(t, lambda(n, lambda(c, apply(apply(var(c), var(h)), var(t))))));
}

static struct lambda_term *
fix_factorial_function(void) {
    struct lambda_term *f, *n;

    return lambda(
        f,
        lambda(
            n,
            if_then_else(
                unary_call(is_zero, var(n)),
                cell(1),
                binary_call(
                    multiply,
                    var(n),
                    apply(var(f), unary_call(minus_one, var(n)))))));
}

static uint64_t optiscope_inside_optiscope_result = 0;

static uint64_t
extract_result(const uint64_t n) {
    optiscope_inside_optiscope_result = n;
    return 0;
}

static uint64_t
inner_factorial(const uint64_t n) {
    struct lambda_term *const term = unary_call(
        extract_result, apply(fix(fix_factorial_function()), cell(n)));

    optiscope_algorithm(NULL, term);

    return optiscope_inside_optiscope_result;
}

static struct lambda_term *
scott_factorial_sum(void) {
    struct lambda_term *rec, *list, *x, *xs;

    return fix(lambda(
        rec,
        lambda(
            list,
            apply(
                apply(var(list), cell(0)),
                lambda(
                    x,
                    lambda(
                        xs,
                        binary_call(
                            add,
                            unary_call(inner_factorial, var(x)),
                            apply(var(rec), var(xs)))))))));
}

static struct lambda_term *
scott_list_1_2_3_4_5(void) {
    return apply(
        apply(scott_cons(), cell(1)),
        apply(
            apply(scott_cons(), cell(2)),
            apply(
                apply(scott_cons(), cell(3)),
                apply(
                    apply(scott_cons(), cell(4)),
                    apply(apply(scott_cons(), cell(5)), scott_nil())))));
}

static struct lambda_term *
program(void) {
    return apply(scott_factorial_sum(), scott_list_1_2_3_4_5());
}

int
main(void) {
    optiscope_open_pools();
    optiscope_algorithm(stdout, program());
    puts("");
    optiscope_close_pools();
}
