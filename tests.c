#include "optiscope.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

// The testing machinery
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static void
test_case(
    const char test_case_name[const restrict],
    struct lambda_term *(*f)(void),
    const char expected[const restrict]) {
    assert(f);
    assert(expected);
    assert(strlen(expected) > 0);

    printf("Testing '%s'...\n", test_case_name);

    FILE *const fp = tmpfile();
    if (NULL == fp) {
        perror("tmpfile");
        return;
    }

    optiscope_algorithm(fp, f());

    rewind(fp);
    for (size_t i = 0; i < strlen(expected); i++) {
        int c;
        if (EOF != (c = fgetc(fp)) && (char)c == expected[i]) { continue; }

#define TAB "    "

        fprintf(stderr, "FAILED:\n" TAB "%s\n", test_case_name);
        fprintf(stderr, "Expected:\n" TAB);
        fprintf(stderr, "%s\n", expected);
        fprintf(stderr, "Received:\n" TAB);
        rewind(fp);
        optiscope_redirect_stream(fp, stderr);
        fprintf(stderr, "\n");

#undef TAB

        goto close_fp;
    }

    printf("Good: %s\n", test_case_name);

close_fp:
    if (0 != fclose(fp)) { perror("fclose"); }
}

#define TEST_CASE(f, expected) test_case(#f, f, expected)

// The S, K, I combinators
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
s_combinator(void) {
    struct lambda_term *x, *y, *z;

    return lambda(
        x,
        lambda(
            y,
            lambda(
                z,
                applicator(
                    applicator(var(x), var(z)), applicator(var(y), var(z))))));
}

static struct lambda_term *
k_combinator(void) {
    struct lambda_term *x, *y;

    return lambda(x, lambda(y, var(x)));
}

static struct lambda_term *
i_combinator(void) {
    struct lambda_term *x;

    return lambda(x, var(x));
}

static struct lambda_term *
skk_test(void) {
    return applicator(
        applicator(s_combinator(), k_combinator()), k_combinator());
}

static struct lambda_term *
sksk_test(void) {
    return applicator(
        applicator(applicator(s_combinator(), k_combinator()), s_combinator()),
        k_combinator());
}

static struct lambda_term *
ski_kis_test(void) {
    return applicator(
        applicator(applicator(s_combinator(), k_combinator()), i_combinator()),
        applicator(applicator(k_combinator(), i_combinator()), s_combinator()));
}

static struct lambda_term *
sii_combinator(void) {
    return applicator(
        applicator(s_combinator(), i_combinator()), i_combinator());
}

static struct lambda_term *
sii_test(void) {
    return applicator(
        sii_combinator(), applicator(i_combinator(), i_combinator()));
}

static struct lambda_term *
iota_combinator_test(void) {
    return applicator(
        applicator(
            s_combinator(),
            applicator(
                applicator(s_combinator(), i_combinator()),
                applicator(k_combinator(), s_combinator()))),
        applicator(k_combinator(), k_combinator()));
}

static struct lambda_term *
self_iota_combinator_test(void) {
    return applicator(iota_combinator_test(), iota_combinator_test());
}

// The B, C, W combinators
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
b_combinator(void) {
    struct lambda_term *f, *g, *x;

    return lambda(
        f,
        lambda(g, lambda(x, applicator(var(f), applicator(var(g), var(x))))));
}

static struct lambda_term *
c_combinator(void) {
    struct lambda_term *f, *g, *x;

    return lambda(
        f,
        lambda(g, lambda(x, applicator(applicator(var(f), var(x)), var(g)))));
}

static struct lambda_term *
w_combinator(void) {
    struct lambda_term *f, *x;

    return lambda(f, lambda(x, applicator(applicator(var(f), var(x)), var(x))));
}

static struct lambda_term *
bcw_test(void) {
    return applicator(
        applicator(b_combinator(), applicator(b_combinator(), w_combinator())),
        applicator(applicator(b_combinator(), b_combinator()), c_combinator()));
}

// Unary arithmetic
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// clang-format off
static uint64_t square(const uint64_t x) { return x * x; }

static uint64_t cube(const uint64_t x) { return x * x * x; }

static uint64_t halve(const uint64_t x) { return x / 2; }
// clang-format on

static struct lambda_term *
unary_arithmetic(void) {
    struct lambda_term *f, *x;

    return unary_call(
        halve,
        applicator(
            lambda(f, applicator(var(f), cell(4))),
            lambda(x, unary_call(cube, unary_call(square, var(x))))));
}

// Binary arithmetic
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// clang-format off
static uint64_t add(const uint64_t x, const uint64_t y)
    { return x + y; }

static uint64_t multiply(const uint64_t x, const uint64_t y)
    { return x * y; }

static uint64_t subtract(const uint64_t x, const uint64_t y)
    { return x - y; }

static uint64_t divide(const uint64_t x, const uint64_t y)
    { return x / y; }
// clang-format on

static struct lambda_term *
binary_arithmetic(void) {
    struct lambda_term *f, *x;

    return applicator(
        lambda(
            f,
            binary_call(
                divide,
                binary_call(subtract, applicator(var(f), cell(10)), cell(8)),
                cell(2))),
        lambda(
            x,
            binary_call(multiply, binary_call(add, var(x), cell(5)), cell(2))));
}

// Conditional logic with recursion
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// clang-format off
static uint64_t equals(const uint64_t x, const uint64_t y)
    { return x == y; }
// clang-format on

static struct lambda_term *
conditionals(void) {
    struct lambda_term *x;

    return if_then_else(
        applicator(
            lambda(
                x,
                if_then_else(
                    binary_call(equals, var(x), cell(100)), cell(0), cell(1))),
            cell(100)),
        cell(5),
        cell(10));
}

// clang-format off
static uint64_t is_zero(const uint64_t x) { return 0 == x; }

static uint64_t is_one(const uint64_t x) { return 1 == x; }
// clang-format on

static struct lambda_term *
fix_fibonacci_function(void) {
    struct lambda_term *rec, *n;

    // clang-format off
    return lambda(rec, lambda(n,
        if_then_else(
            unary_call(is_zero, var(n)),
            cell(0),
            if_then_else(
                unary_call(is_one, var(n)),
                cell(1),
                binary_call(add,
                    applicator(var(rec),
                        binary_call(subtract, var(n), cell(1))),
                    applicator(var(rec),
                        binary_call(subtract, var(n), cell(2))))))));
    // clang-format on
}

static struct lambda_term *
fix_fibonacci_term(void) {
    return fix(fix_fibonacci_function());
}

static struct lambda_term *
fix_fibonacci_test(void) {
    return applicator(fix_fibonacci_term(), cell(10));
}

// Church booleans
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
church_true(void) {
    struct lambda_term *x, *y;

    return lambda(x, lambda(y, var(x)));
}

static struct lambda_term *
church_false(void) {
    struct lambda_term *x, *y;

    return lambda(x, lambda(y, var(y)));
}

static struct lambda_term *
church_not(void) {
    struct lambda_term *p, *a, *b;

    return lambda(
        p,
        lambda(a, lambda(b, applicator(applicator(var(p), var(b)), var(a)))));
}

static struct lambda_term *
church_and(void) {
    struct lambda_term *p, *q;

    return lambda(p, lambda(q, applicator(applicator(var(p), var(q)), var(p))));
}

static struct lambda_term *
church_or(void) {
    struct lambda_term *p, *q;

    return lambda(p, lambda(q, applicator(applicator(var(p), var(p)), var(q))));
}

static struct lambda_term *
church_xor(void) {
    struct lambda_term *p, *q;

    return lambda(
        p,
        lambda(
            q,
            applicator(
                applicator(
                    var(p),
                    applicator(
                        applicator(var(q), church_false()), church_true())),
                applicator(
                    applicator(var(q), church_true()), church_false()))));
}

static struct lambda_term *
church_if_then_else(void) {
    struct lambda_term *c, *t, *f;

    return lambda(
        c,
        lambda(t, lambda(f, applicator(applicator(var(c), var(t)), var(f)))));
}

#define CHURCH_IF_THEN_ELSE(c, t, f)                                           \
    applicator(applicator(applicator(church_if_then_else(), c), t), f)

static struct lambda_term *
boolean_test(void) {
    return CHURCH_IF_THEN_ELSE(
        applicator(applicator(church_or(), church_true()), church_false()),
        applicator(
            applicator(
                church_xor(),
                applicator(
                    applicator(church_and(), church_true()),
                    applicator(church_not(), church_false()))),
            church_false()),
        church_false());
}

// Church numerals
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
church_zero(void) {
    struct lambda_term *f, *x;

    return lambda(f, lambda(x, var(x)));
}

static struct lambda_term *
church_one(void) {
    struct lambda_term *f, *x;

    return lambda(f, lambda(x, applicator(var(f), var(x))));
}

static struct lambda_term *
church_two(void) {
    struct lambda_term *f, *x;

    return lambda(f, lambda(x, applicator(var(f), applicator(var(f), var(x)))));
}

static struct lambda_term *
church_three(void) {
    struct lambda_term *f, *x;

    return lambda(
        f,
        lambda(
            x,
            applicator(
                var(f), applicator(var(f), applicator(var(f), var(x))))));
}

static struct lambda_term *
church_five(void) {
    struct lambda_term *f, *x;

    return lambda(
        f,
        lambda(
            x,
            applicator(
                var(f),
                applicator(
                    var(f),
                    applicator(
                        var(f),
                        applicator(var(f), applicator(var(f), var(x))))))));
}

// The originall showcase test from the Lambdascope paper.
static struct lambda_term *
church_two_two_test(void) {
    return applicator(church_two(), church_two());
}

static struct lambda_term *
church_two_two_two_test(void) {
    return applicator(applicator(church_two(), church_two()), church_two());
}

static struct lambda_term *
church_add(void) {
    struct lambda_term *m, *n, *f, *x;

    return lambda(
        m,
        lambda(
            n,
            lambda(
                f,
                lambda(
                    x,
                    applicator(
                        applicator(var(m), var(f)),
                        applicator(applicator(var(n), var(f)), var(x)))))));
}

static struct lambda_term *
church_multiply(void) {
    struct lambda_term *m, *n, *f, *x;

    return lambda(
        m,
        lambda(
            n,
            lambda(
                f,
                lambda(
                    x,
                    applicator(
                        applicator(var(m), applicator(var(n), var(f))),
                        var(x))))));
}

static struct lambda_term *
church_one_plus_two_times_five_test(void) {
    return applicator(
        applicator(
            church_multiply(),
            applicator(applicator(church_add(), church_one()), church_two())),
        church_five());
}

static struct lambda_term *
church_predecessor(void) {
    struct lambda_term *n, *f, *x, *g, *h, *u, *v;

    return lambda(
        n,
        lambda(
            f,
            lambda(
                x,
                applicator(
                    applicator(
                        applicator(
                            var(n),
                            lambda(
                                g,
                                lambda(
                                    h,
                                    applicator(
                                        var(h), applicator(var(g), var(f)))))),
                        lambda(u, var(x))),
                    lambda(v, var(v))))));
}

static struct lambda_term *
church_predecessor2x(void) {
    struct lambda_term *n;

    return lambda(
        n,
        applicator(
            church_predecessor(), applicator(church_predecessor(), var(n))));
}

static struct lambda_term *
church_five_predecessor2x(void) {
    return applicator(church_predecessor2x(), church_five());
}

// Iterative factorial
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
church_pair(void) {
    struct lambda_term *x, *y, *z;

    return lambda(
        x,
        lambda(y, lambda(z, applicator(applicator(var(z), var(x)), var(y)))));
}

static struct lambda_term *
church_first(void) {
    struct lambda_term *p;

    return lambda(p, applicator(var(p), church_true()));
}

static struct lambda_term *
church_second(void) {
    struct lambda_term *p;

    return lambda(p, applicator(var(p), church_false()));
}

static struct lambda_term *
factorial_step_term(void) {
    struct lambda_term *p;

    return lambda(
        p,
        applicator(
            applicator(
                church_pair(),
                applicator(
                    applicator(
                        church_multiply(), applicator(church_first(), var(p))),
                    applicator(church_second(), var(p)))),
            applicator(
                church_predecessor(), applicator(church_second(), var(p)))));
}

static struct lambda_term *
factorial_term(void) {
    struct lambda_term *n;

    return lambda(
        n,
        applicator(
            church_first(),
            applicator(
                applicator(var(n), factorial_step_term()),
                applicator(applicator(church_pair(), church_one()), var(n)))));
}

static struct lambda_term *
factorial_of_three_test(void) {
    return applicator(factorial_term(), church_three());
}

// The Y combinator
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
y_combinator(void) {
    struct lambda_term *f, *x, *y;

    return lambda(
        f,
        applicator(
            lambda(x, applicator(var(f), applicator(var(x), var(x)))),
            lambda(y, applicator(var(f), applicator(var(y), var(y))))));
}

static struct lambda_term *
church_is_zero(void) {
    struct lambda_term *n, *x;

    return lambda(
        n,
        applicator(
            applicator(var(n), lambda(x, church_false())), church_true()));
}

static struct lambda_term *
church_is_one(void) {
    struct lambda_term *n;

    // Assuming that `n` is positive.
    return lambda(
        n,
        applicator(church_is_zero(), applicator(church_predecessor(), var(n))));
}

static struct lambda_term *
y_factorial_function(void) {
    struct lambda_term *f, *n;

    return lambda(
        f,
        lambda(
            n,
            CHURCH_IF_THEN_ELSE(
                applicator(church_is_zero(), var(n)),
                church_one(),
                applicator(
                    applicator(church_multiply(), var(n)),
                    applicator(
                        var(f), applicator(church_predecessor(), var(n)))))));
}

static struct lambda_term *
y_factorial_term(void) {
    return applicator(y_combinator(), y_factorial_function());
}

static struct lambda_term *
y_factorial_test(void) {
    return applicator(y_factorial_term(), church_three());
}

static struct lambda_term *
y_fibonacci_function(void) {
    struct lambda_term *rec, *n;

    // clang-format off
    return lambda(rec, lambda(n,
        CHURCH_IF_THEN_ELSE(
            applicator(church_is_zero(), var(n)),
            church_zero(),
            CHURCH_IF_THEN_ELSE(
                applicator(church_is_one(), var(n)),
                church_one(),
                applicator(applicator(
                    church_add(),
                    applicator(var(rec),
                        applicator(church_predecessor(), var(n)))),
                    applicator(var(rec),
                        applicator(church_predecessor2x(), var(n))))))));
    // clang-format on
}

static struct lambda_term *
y_fibonacci_term(void) {
    return applicator(y_combinator(), y_fibonacci_function());
}

static struct lambda_term *
y_fibonacci_test(void) {
    return applicator(y_fibonacci_term(), church_three());
}

// The WHY combinator
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// Suggested by Marvin Borner <git@marvinborner.de>.
static struct lambda_term *
why_combinator(void) {
    struct lambda_term *f, *u, *a, *f2, *d, *u2, *i, *x;

    return lambda(
        f,
        applicator(
            lambda(
                u,
                applicator(
                    applicator(
                        var(u),
                        lambda(
                            a,
                            lambda(
                                f2,
                                applicator(
                                    applicator(var(f2), var(a)), var(a))))),
                    var(u))),
            lambda(
                d,
                lambda(
                    u2,
                    applicator(
                        var(f),
                        lambda(
                            i,
                            applicator(
                                applicator(applicator(var(i), var(d)), var(u2)),
                                lambda(x, applicator(var(x), var(d))))))))));
}

static struct lambda_term *
why_factorial_function(void) {
    struct lambda_term *rec, *n;

    return lambda(
        rec,
        lambda(
            n,
            CHURCH_IF_THEN_ELSE(
                applicator(church_is_zero(), var(n)),
                church_one(),
                applicator(
                    applicator(church_multiply(), var(n)),
                    applicator(
                        applicator(var(rec), i_combinator()),
                        applicator(church_predecessor(), var(n)))))));
}

static struct lambda_term *
why_factorial_term(void) {
    return applicator(why_combinator(), why_factorial_function());
}

static struct lambda_term *
why_factorial_test(void) {
    return applicator(why_factorial_term(), church_three());
}

// Church lists
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
church_nil(void) {
    struct lambda_term *f, *n;

    return lambda(f, lambda(n, var(n)));
}

static struct lambda_term *
church_cons(void) {
    struct lambda_term *h, *t, *f, *n;

    return lambda(
        h,
        lambda(
            t,
            lambda(
                f,
                lambda(
                    n,
                    applicator(
                        applicator(var(f), var(h)),
                        applicator(applicator(var(t), var(f)), var(n)))))));
}

static struct lambda_term *
church_list_1_2_3(void) {
    return applicator(
        applicator(church_cons(), cell(1)),
        applicator(
            applicator(church_cons(), cell(2)),
            applicator(applicator(church_cons(), cell(3)), church_nil())));
}

static struct lambda_term *
church_sum_list(void) {
    struct lambda_term *list, *x, *y;

    return lambda(
        list,
        applicator(
            applicator(
                var(list),
                lambda(x, lambda(y, binary_call(add, var(x), var(y))))),
            cell(0)));
}

static struct lambda_term *
church_sum_list_test(void) {
    return applicator(church_sum_list(), church_list_1_2_3());
}

static struct lambda_term *
church_reverse(void) {
    struct lambda_term *xs, *x, *cont, *f, *n;

    // clang-format off
    return lambda(
        xs,
        applicator(
            applicator(
                var(xs),
                lambda(x, lambda(cont, lambda(f, lambda(n,
                    applicator(
                        applicator(var(cont), var(f)),
                        applicator(applicator(var(f), var(x)), var(n)))))))),
            church_nil()));
    // clang-format on
}

static struct lambda_term *
church_reverse_test(void) {
    return applicator(church_reverse(), church_list_1_2_3());
}

static struct lambda_term *
church_append(void) {
    struct lambda_term *xs, *ys, *f, *n;

    return lambda(
        xs,
        lambda(
            ys,
            lambda(
                f,
                lambda(
                    n,
                    applicator(
                        applicator(var(xs), var(f)),
                        applicator(applicator(var(ys), var(f)), var(n)))))));
}

static struct lambda_term *
church_list_4_5_6(void) {
    return applicator(
        applicator(church_cons(), cell(4)),
        applicator(
            applicator(church_cons(), cell(5)),
            applicator(applicator(church_cons(), cell(6)), church_nil())));
}

static struct lambda_term *
church_append_test(void) {
    return applicator(
        applicator(church_append(), church_list_1_2_3()), church_list_4_5_6());
}

// Scott numerals
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
scott_zero(void) {
    struct lambda_term *s, *z;

    return lambda(s, lambda(z, var(z)));
}

static struct lambda_term *
scott_one(void) {
    struct lambda_term *s, *z;

    return lambda(s, lambda(z, applicator(var(s), scott_zero())));
}

static struct lambda_term *
scott_two(void) {
    struct lambda_term *s, *z;

    return lambda(s, lambda(z, applicator(var(s), scott_one())));
}

static struct lambda_term *
scott_three(void) {
    struct lambda_term *s, *z;

    return lambda(s, lambda(z, applicator(var(s), scott_two())));
}

static struct lambda_term *
scott_successor(void) {
    struct lambda_term *n, *s, *z;

    return lambda(n, lambda(s, lambda(z, applicator(var(s), var(n)))));
}

static struct lambda_term *
scott_predecessor(void) {
    struct lambda_term *n, *x;

    return lambda(
        n, applicator(applicator(var(n), lambda(x, var(x))), scott_zero()));
}

static struct lambda_term *
scott_three_successor_predecessor2x_test(void) {
    return applicator(
        scott_predecessor(),
        applicator(
            scott_predecessor(), applicator(scott_successor(), scott_three())));
}

// Scott lists
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

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
        lambda(
            t,
            lambda(
                n, lambda(c, applicator(applicator(var(c), var(h)), var(t))))));
}

static struct lambda_term *
scott_singleton(void) {
    struct lambda_term *x;

    return lambda(x, applicator(applicator(scott_cons(), var(x)), scott_nil()));
}

static struct lambda_term *
scott_sum_list(void) {
    struct lambda_term *rec, *list, *x, *xs;

    // clang-format off
    return fix(lambda(rec, lambda(list,
        applicator(
            applicator(var(list), cell(0)),
            lambda(x, lambda(xs,
                binary_call(add, var(x), applicator(var(rec), var(xs)))))))));
    // clang-format on
}

static struct lambda_term *
scott_list_1_2_3_4_5(void) {
    return applicator(
        applicator(scott_cons(), cell(1)),
        applicator(
            applicator(scott_cons(), cell(2)),
            applicator(
                applicator(scott_cons(), cell(3)),
                applicator(
                    applicator(scott_cons(), cell(4)),
                    applicator(
                        applicator(scott_cons(), cell(5)), scott_nil())))));
}

static struct lambda_term *
scott_sum_list_test(void) {
    return applicator(scott_sum_list(), scott_list_1_2_3_4_5());
}

// clang-format off
static uint64_t less_than_or_equal(const uint64_t x, const uint64_t y)
    { return x <= y; }
// clang-format on

static struct lambda_term *
scott_insert(void) {
    struct lambda_term *rec, *y, *list, *z, *zs;

    // clang-format off
    return fix(lambda(rec, lambda(y, lambda(list,
        applicator(applicator(var(list),
            applicator(scott_singleton(), var(y))),
            lambda(z, lambda(zs,
                if_then_else(
                    binary_call(less_than_or_equal, var(y), var(z)),
                    applicator(applicator(scott_cons(), var(y)),
                        applicator(applicator(scott_cons(), var(z)), var(zs))),
                    applicator(applicator(scott_cons(), var(z)),
                        applicator(applicator(
                            var(rec), var(y)), var(zs)))))))))));
    // clang-format on
}

static struct lambda_term *
scott_insertion_sort(void) {
    struct lambda_term *rec, *list, *x, *xs;

    return fix(lambda(
        rec,
        lambda(
            list,
            applicator(
                applicator(var(list), scott_nil()),
                lambda(
                    x,
                    lambda(
                        xs,
                        applicator(
                            applicator(scott_insert(), var(x)),
                            applicator(var(rec), var(xs)))))))));
}

static struct lambda_term *
scott_list_3_1_4_1_5(void) {
    return applicator(
        applicator(scott_cons(), cell(3)),
        applicator(
            applicator(scott_cons(), cell(1)),
            applicator(
                applicator(scott_cons(), cell(4)),
                applicator(
                    applicator(scott_cons(), cell(1)),
                    applicator(
                        applicator(scott_cons(), cell(5)), scott_nil())))));
}

// clang-format off
static uint64_t concatenate_ints(uint64_t x, const uint64_t y) {
    uint64_t z = y;
    do { x *= 10; } while (z /= 10);
    return x + y;
}
// clang-format on

static struct lambda_term *
scott_concatenate_list(void) {
    struct lambda_term *rec, *list, *x, *xs;

    // clang-format off
    return fix(lambda(rec, lambda(list,
        applicator(
            applicator(var(list), cell(0)),
            lambda(x, lambda(xs,
                binary_call(concatenate_ints,
                    var(x), applicator(var(rec), var(xs)))))))));
    // clang-format on
}

static struct lambda_term *
scott_insertion_sort_test(void) {
    return applicator(
        scott_concatenate_list(),
        applicator(scott_insertion_sort(), scott_list_3_1_4_1_5()));
}

// Scott quicksort
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// clang-format off
static uint64_t less_than(const uint64_t x, const uint64_t y)
    { return x < y; }

static uint64_t greater_than_or_equal(const uint64_t x, const uint64_t y)
    { return x >= y; }
// clang-format on

static struct lambda_term *
scott_filter(void) {
    struct lambda_term *rec, *f, *list, *x, *xs;

    // clang-format off
    return fix(lambda(rec, lambda(f, lambda(list,
        applicator(
            applicator(var(list), scott_nil()),
            lambda(x, lambda(xs,
                if_then_else(
                    applicator(var(f), var(x)),
                    applicator(
                        applicator(scott_cons(), var(x)),
                        applicator(applicator(var(rec), var(f)), var(xs))),
                    applicator(applicator(var(rec), var(f)), var(xs))))))))));
    // clang-format on
}

static struct lambda_term *
scott_append(void) {
    struct lambda_term *rec, *xs, *ys, *x, *xss;

    // clang-format off
    return fix(lambda(rec, lambda(xs, lambda(ys,
        applicator(
            applicator(var(xs), var(ys)),
            lambda(x, lambda(xss,
                applicator(applicator(scott_cons(), var(x)),
                    applicator(applicator(var(rec), var(xss)), var(ys))))))))));
    // clang-format on
}

static struct lambda_term *
scott_quicksort(void) {
    struct lambda_term *rec, *list, *x, *xs, *y, *z;

    // clang-format off
    return fix(lambda(rec, lambda(list,
        applicator(applicator(var(list), scott_nil()),
            lambda(x, lambda(xs, applicator(applicator(scott_append(),
                applicator(var(rec),
                    applicator(
                        applicator(scott_filter(),
                            lambda(y, binary_call(less_than, var(y), var(x)))),
                        var(xs)))),
                applicator(applicator(scott_cons(), var(x)),
                    applicator(var(rec),
                        applicator(
                            applicator(scott_filter(),
                                lambda(z, binary_call(greater_than_or_equal,
                                    var(z), var(x)))),
                            var(xs)))))))))));
    // clang-format on
}

static struct lambda_term *
scott_list_9_2_7_3_8_1_4(void) {
    return applicator(
        applicator(scott_cons(), cell(9)),
        applicator(
            applicator(scott_cons(), cell(2)),
            applicator(
                applicator(scott_cons(), cell(7)),
                applicator(
                    applicator(scott_cons(), cell(3)),
                    applicator(
                        applicator(scott_cons(), cell(8)),
                        applicator(
                            applicator(scott_cons(), cell(1)),
                            applicator(
                                applicator(scott_cons(), cell(4)),
                                scott_nil())))))));
}

static struct lambda_term *
scott_quicksort_test(void) {
    return applicator(
        scott_concatenate_list(),
        applicator(scott_quicksort(), scott_list_9_2_7_3_8_1_4()));
}

// Scott binary trees
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
scott_leaf(void) {
    struct lambda_term *v, *leaf, *node;

    return lambda(v, lambda(leaf, lambda(node, applicator(var(leaf), var(v)))));
}

static struct lambda_term *
scott_node(void) {
    struct lambda_term *lhs, *rhs, *leaf, *node;

    return lambda(
        lhs,
        lambda(
            rhs,
            lambda(
                leaf,
                lambda(
                    node,
                    applicator(applicator(var(node), var(lhs)), var(rhs))))));
}

static struct lambda_term *
scott_tree_sum(void) {
    struct lambda_term *rec, *tree, *v, *lhs, *rhs;

    return fix(lambda(
        rec,
        lambda(
            tree,
            applicator(
                applicator(var(tree), lambda(v, var(v))),
                lambda(
                    lhs,
                    lambda(
                        rhs,
                        binary_call(
                            add,
                            applicator(var(rec), var(lhs)),
                            applicator(var(rec), var(rhs)))))))));
}

static struct lambda_term *
scott_tree_map(void) {
    struct lambda_term *rec, *f, *tree, *v, *lhs, *rhs;

    // clang-format off
    return fix(lambda(rec, lambda(f, lambda(tree,
        applicator(
            applicator(
                var(tree),
                lambda(v, applicator(scott_leaf(), applicator(var(f), var(v))))),
            lambda(lhs, lambda(rhs,
                applicator(
                    applicator(
                        scott_node(),
                        applicator(applicator(var(rec), var(f)), var(lhs))),
                    applicator(
                        applicator(var(rec), var(f)), var(rhs))))))))));
    // clang-format on
}

static struct lambda_term *
scott_example_tree(void) {
    return applicator(
        applicator(
            scott_node(),
            applicator(
                applicator(scott_node(), applicator(scott_leaf(), cell(1))),
                applicator(scott_leaf(), cell(2)))),
        applicator(
            applicator(scott_node(), applicator(scott_leaf(), cell(3))),
            applicator(scott_leaf(), cell(4))));
}

static struct lambda_term *
scott_tree_sum_test(void) {
    return applicator(scott_tree_sum(), scott_example_tree());
}

static struct lambda_term *
scott_tree_map_and_sum_test(void) {
    struct lambda_term *x;

    return applicator(
        scott_tree_sum(),
        applicator(
            applicator(
                scott_tree_map(),
                lambda(x, binary_call(multiply, var(x), cell(2)))),
            scott_example_tree()));
}

// The Ackermann function
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// clang-format off
static uint64_t plus_one(const uint64_t x) { return x + 1; }

static uint64_t minus_one(const uint64_t x) { return x - 1; }
// clang-format on

static struct lambda_term *
fix_ackermann(void) {
    struct lambda_term *rec, *m, *n;

    // clang-format off
    return fix(lambda(rec, lambda(m, lambda(n,
        if_then_else(
            unary_call(is_zero, var(m)),
            unary_call(plus_one, var(n)),
            if_then_else(
                unary_call(is_zero, var(n)),
                applicator(
                    applicator(var(rec), unary_call(minus_one, var(m))), cell(1)),
                applicator(
                    applicator(var(rec), unary_call(minus_one, var(m))),
                    applicator(
                        applicator(var(rec), var(m)),
                        unary_call(minus_one, var(n))))))))));
    // clang-format on
}

static struct lambda_term *
fix_ackermann_test(void) {
    return applicator(applicator(fix_ackermann(), cell(3)), cell(3));
}

// Examples from the literature
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

static struct lambda_term *
lamping_example(void) { // Originally Lévy's
    struct lambda_term *g, *x, *h, *f, *z, *w, *y;

    return applicator(
        lambda(g, applicator(var(g), applicator(var(g), lambda(x, var(x))))),
        lambda(
            h,
            applicator(
                lambda(
                    f,
                    applicator(var(f), applicator(var(f), lambda(z, var(z))))),
                lambda(
                    w,
                    applicator(
                        var(h), applicator(var(w), lambda(y, var(y))))))));
}

static struct lambda_term *
lamping_example_2(void) {
    struct lambda_term *g, *x, *h, *f, *z, *y;

    return applicator(
        lambda(g, applicator(var(g), applicator(var(g), lambda(x, var(x))))),
        lambda(
            h,
            applicator(
                lambda(
                    f,
                    applicator(var(f), applicator(var(f), lambda(z, var(z))))),

                applicator(var(h), lambda(y, var(y))))));
}

static struct lambda_term *
asperti_guerrini_example(void) {
    struct lambda_term *z, *v, *w, *x, *y;

    struct lambda_term *once = lambda(v, var(v));
    struct lambda_term *twice = lambda(w, applicator(var(w), var(w)));

    return lambda(
        z,
        applicator(
            lambda(x, applicator(var(x), once)),
            lambda(y, applicator(twice, applicator(var(y), var(z))))));
}

static struct lambda_term *
wadsworth_example(void) { // Asperti & Guerrini
    struct lambda_term *v, *w, *x, *y;

    struct lambda_term *once = lambda(v, var(v));
    struct lambda_term *twice = lambda(w, applicator(var(w), var(w)));

    return lambda(
        y,
        applicator(
            twice, applicator(lambda(x, applicator(var(x), var(y))), once)));
}

static struct lambda_term *
wadsworth_counterexample(void) { // Asperti & Guerrini
    struct lambda_term *v, *w, *x, *y, *z;

    struct lambda_term *once = lambda(v, var(v));

    return lambda(
        y,
        lambda(
            z,
            applicator(
                lambda(
                    x,
                    applicator(
                        applicator(var(x), var(y)),
                        applicator(var(x), var(z)))),
                lambda(w, applicator(once, var(w))))));
}

// The test driver
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#ifndef OPTISCOPE_TESTS_NO_MAIN

int
main(void) {
    optiscope_open_pools();

    puts("Running the test cases...");

    TEST_CASE(skk_test, "(λ 0)");
    TEST_CASE(sksk_test, "(λ (λ 1))");
    TEST_CASE(ski_kis_test, "(λ 0)");
    TEST_CASE(sii_test, "(λ 0)");
    TEST_CASE(
        iota_combinator_test, "(λ ((0 (λ (λ (λ ((2 0) (1 0)))))) (λ (λ 1))))");
    TEST_CASE(self_iota_combinator_test, "(λ 0)");
    TEST_CASE(bcw_test, "(λ (λ (λ ((2 0) (1 0)))))");
    TEST_CASE(unary_arithmetic, "cell[2048]");
    TEST_CASE(binary_arithmetic, "cell[11]");
    TEST_CASE(conditionals, "cell[10]");
    TEST_CASE(fix_fibonacci_test, "cell[55]");
    TEST_CASE(boolean_test, "(λ (λ 1))");
    TEST_CASE(church_two_two_test, "(λ (λ (1 (1 (1 (1 0))))))");
    TEST_CASE(
        church_two_two_two_test,
        "(λ (λ (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 0))))))))))))))))))");
    TEST_CASE(
        church_one_plus_two_times_five_test,
        "(λ (λ (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 (1 0)))))))))))))))))");
    TEST_CASE(church_five_predecessor2x, "(λ (λ (1 (1 (1 0)))))");
    TEST_CASE(factorial_of_three_test, "(λ (λ (1 (1 (1 (1 (1 (1 0))))))))");
    TEST_CASE(y_factorial_test, "(λ (λ (1 (1 (1 (1 (1 (1 0))))))))");
    TEST_CASE(y_fibonacci_test, "(λ (λ (1 (1 0))))");
    TEST_CASE(why_factorial_test, "(λ (λ (1 (1 (1 (1 (1 (1 0))))))))");
    TEST_CASE(church_sum_list_test, "cell[6]");
    TEST_CASE(
        church_reverse_test,
        "(λ (λ ((1 cell[3]) ((1 cell[2]) ((1 cell[1]) 0)))))");
    TEST_CASE(
        church_append_test,
        "(λ (λ ((1 cell[1]) ((1 cell[2]) ((1 cell[3]) ((1 cell[4]) ((1 cell[5]) ((1 cell[6]) 0))))))))");
    TEST_CASE(
        scott_three_successor_predecessor2x_test,
        "(λ (λ (1 (λ (λ (1 (λ (λ 0))))))))");
    TEST_CASE(scott_sum_list_test, "cell[15]");
    TEST_CASE(scott_insertion_sort_test, "cell[113450]");
    TEST_CASE(scott_quicksort_test, "cell[12347890]");
    TEST_CASE(scott_tree_sum_test, "cell[10]");
    TEST_CASE(scott_tree_map_and_sum_test, "cell[20]");
    TEST_CASE(fix_ackermann_test, "cell[61]");
    TEST_CASE(lamping_example, "(λ 0)");
    TEST_CASE(lamping_example_2, "(λ 0)");
    TEST_CASE(asperti_guerrini_example, "(λ (0 0))");
    TEST_CASE(wadsworth_example, "(λ (0 0))");
    TEST_CASE(wadsworth_counterexample, "(λ (λ (1 0)))");

    optiscope_close_pools();
}

#endif // OPTISCOPE_TESTS_NO_MAIN
