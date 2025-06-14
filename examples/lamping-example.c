#include "../optiscope.h"

static struct lambda_term *
lamping_example(void) { // Originally Lévy's
    struct lambda_term *g, *x, *h, *f, *z, *w, *y;

    return apply(
        lambda(g, apply(var(g), apply(var(g), lambda(x, var(x))))),
        lambda(
            h,
            apply(
                lambda(f, apply(var(f), apply(var(f), lambda(z, var(z))))),
                lambda(w, apply(var(h), apply(var(w), lambda(y, var(y))))))));
}

int
main(void) {
    optiscope_open_pools();
    optiscope_algorithm(stdout, lamping_example());
    puts("");
    optiscope_close_pools();
}
