#include "../optiscope.h"

#include <stdlib.h>
#include <string.h>

static uint64_t
my_puts(const uint64_t s, const uint64_t token) {
    (void)token;

    return (uint64_t)puts((const char *)s);
}

static uint64_t
my_gets(const uint64_t token) {
    (void)token;

    char *const s = malloc(256);
    if (NULL == s) { perror("malloc"), abort(); }
    scanf("%255s", s);
    return (uint64_t)s;
}

static uint64_t
my_free(const uint64_t s, const uint64_t token) {
    (void)token;

    free((void *)s);
    return 0;
}

static uint64_t
my_strcmp(const uint64_t s1, const uint64_t s2) {
    return 0 == strcmp((const char *)s1, (const char *)s2) ? 1 : 0;
}

static uint64_t
is_palindrome(uint64_t s) {
    const char *const chars = (const char *)s;
    const size_t len = strlen(chars);

    for (size_t i = 0; i < len / 2; i++) {
        if (chars[i] != chars[len - 1 - i]) { return 0; }
    }

    return 1;
}

static struct lambda_term *
program(void) {
    struct lambda_term *rec, *token, *s;

    return fix(lambda(
        rec,
        lambda(
            token,
            perform(
                binary_call(
                    my_puts,
                    cell((uint64_t)"Enter your palindrome or type 'quit':"),
                    var(token)),
                bind(
                    s,
                    unary_call(my_gets, var(token)),
                    if_then_else(
                        binary_call(my_strcmp, var(s), cell((uint64_t)"quit")),
                        binary_call(my_free, var(s), var(token)),
                        perform(
                            perform(
                                if_then_else(
                                    unary_call(is_palindrome, var(s)),
                                    binary_call(
                                        my_puts,
                                        cell((uint64_t)"This is a palindrome!"),
                                        var(token)),
                                    binary_call(
                                        my_puts,
                                        cell((
                                            uint64_t)"This isn't a palindrome."),
                                        var(token))),
                                binary_call(my_free, var(s), var(token))),
                            apply(var(rec), var(token)))))))));
}

int
main(void) {
    optiscope_open_pools();
    optiscope_algorithm(NULL, apply(program(), cell(0)));
    optiscope_close_pools();
}
