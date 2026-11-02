#define _POSIX_C_SOURCE 200809L
#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "../lib/hash_table_general_key.h"
#include "../lib/hash_table_iterator_general_key.h"
#include "../src/backend/backend.h"
#include "../src/backend/shopping_cart.h"
#include "../src/frontend/actions.h"

//To-Do: Review test validity

//Generated using AI

/* =====================================================================
 * Fake keyboard and captured screen
 *
 * The actions read from stdin and print to stdout. Each test swaps stdin
 * for a string with the "typed" answers (one per line) and stdout for a
 * buffer, so it can check both what happened and what was printed.
 * ===================================================================== */

static FILE *real_stdin;
static FILE *real_stdout;
static char *output = NULL;
static size_t output_size = 0;

// If an action asks for more input than the test gave, ask_question loops
// forever on EOF. This stops the test program instead of hanging.
static void on_timeout(int sig)
{
    (void)sig;
    const char msg[] = "\nTest timed out: an action asked for more input than the test provided\n";
    write(STDERR_FILENO, msg, sizeof msg - 1);
    _exit(EXIT_FAILURE);
}

static void begin_io(const char *input)
{
    real_stdin = stdin;
    real_stdout = stdout;
    stdin = fmemopen((void *)input, strlen(input), "r");
    free(output);
    output = NULL;
    stdout = open_memstream(&output, &output_size);
    alarm(2);
}

static void end_io(void)
{
    alarm(0);
    fclose(stdin);
    fclose(stdout);   // flushes into `output`
    stdin = real_stdin;
    stdout = real_stdout;
}

// Runs `call` with `input` as the keyboard and captures what it prints.
#define WITH_INPUT(input, call) do { begin_io(input); call; end_io(); } while (0)

static bool printed(const char *text)
{
    return output != NULL && strstr(output, text) != NULL;
}

/* =====================================================================
 * Test store
 * ===================================================================== */

typedef struct {
    ioopm_hash_table_t *merch_ht;
    ioopm_hash_table_t *storage_ht;
    ioopm_hash_table_t *carts;
    cart_count_t *count;
} store_t;

// The backend keeps its own copies of every string, so plain literals
// are fine here.
static void add_item(store_t *s, char *name, size_t price, size_t stock, char *location)
{
    backend_create_merch_item(s->merch_ht, name, "test description", price);
    for (size_t i = 0; i < stock; i++) {
        backend_replenish_merch_item(s->storage_ht, s->merch_ht, name, location);
    }
}

// apple: price 10, 5 on A01.  banana: price 3, 2 on B01.
static store_t setup(void)
{
    store_t s;
    s.merch_ht = ioopm_hash_table_create(str_hash, str_eq);
    s.storage_ht = ioopm_hash_table_create(str_hash, str_eq);
    s.carts = ioopm_hash_table_create(int_hash, int_eq);
    s.count = cart_initiate_cart_count();
    add_item(&s, "apple", 10, 5, "A01");
    add_item(&s, "banana", 3, 2, "B01");
    return s;
}

static void teardown(store_t *s)
{
    ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(s->carts);
    while (!ioopm_hash_table_iterator_at_end(it)) {
        ioopm_hash_table_destroy(ioopm_hash_table_iterator_current_value(it).p);
        ioopm_hash_table_iterator_advance(it);
    }
    ioopm_hash_table_iterator_destroy(it);
    ioopm_hash_table_destroy(s->carts);
    cart_destroy_cart_count(s->count);
    backend_destroy_all_merch(s->merch_ht, s->storage_ht);   // also destroys both tables
}

static bool has_merch(store_t *s, char *name)
{
    return ioopm_hash_table_has_key(s->merch_ht, string_elem(name));
}

static size_t reserved_of(store_t *s, char *name)
{
    return backend_get_reserved(backend_find_merch_item(s->merch_ht, name));
}

/* ------------------------------ add_merch ---------------------------- */

static void test_add_merch(void)
{
    store_t s = setup();

    WITH_INPUT("pear\nA green fruit\n7\n", actions_add_merch(s.merch_ht));

    CU_ASSERT_TRUE(has_merch(&s, "pear"));
    CU_ASSERT_EQUAL(backend_get_price(backend_find_merch_item(s.merch_ht, "pear")), 7);

    teardown(&s);
}

static void test_add_merch_negative_price(void)
{
    store_t s = setup();

    WITH_INPUT("pear\nA green fruit\n-7\n", actions_add_merch(s.merch_ht));

    CU_ASSERT_TRUE(printed("Invalid price"));
    CU_ASSERT_FALSE(has_merch(&s, "pear"));

    teardown(&s);
}

/* -------------------------- list_merch_items ------------------------- */

static void test_list_merch(void)
{
    store_t s = setup();

    WITH_INPUT("", actions_list_merch_items(s.merch_ht));

    CU_ASSERT_TRUE(printed("apple"));
    CU_ASSERT_TRUE(printed("banana"));

    teardown(&s);
}

static void test_list_merch_stops_after_20_on_n(void)
{
    store_t s = setup();
    char name[16];
    char loc[4];
    for (int i = 0; i < 25; i++) {
        snprintf(name, sizeof name, "item%02d", i);
        snprintf(loc, sizeof loc, "C%02d", i);
        add_item(&s, name, 1, 0, loc);
    }

    // 27 items in total: it should ask after 20 and stop on "n"
    WITH_INPUT("n\n", actions_list_merch_items(s.merch_ht));

    CU_ASSERT_TRUE(printed("keep listing"));
    // Count printed names: exactly 20 lines before the question
    int lines = 0;
    char *question = strstr(output, "Do you want");
    for (char *p = output; p < question; p++) {
        if (*p == '\n') lines++;
    }
    CU_ASSERT_EQUAL(lines, 20);

    teardown(&s);
}

/* ---------------------------- remove_merch --------------------------- */

static void test_remove_merch_confirmed(void)
{
    store_t s = setup();

    WITH_INPUT("banana\ny\n", actions_remove_merch(s.merch_ht, s.storage_ht));

    CU_ASSERT_FALSE(has_merch(&s, "banana"));
    // Its shelf is freed up too
    CU_ASSERT_FALSE(ioopm_hash_table_has_key(s.storage_ht, string_elem("B01")));

    teardown(&s);
}

static void test_remove_merch_aborted(void)
{
    store_t s = setup();

    WITH_INPUT("banana\nn\n", actions_remove_merch(s.merch_ht, s.storage_ht));

    CU_ASSERT_TRUE(printed("Aborting removal"));
    CU_ASSERT_TRUE(has_merch(&s, "banana"));

    teardown(&s);
}

static void test_remove_merch_invalid_name(void)
{
    store_t s = setup();

    WITH_INPUT("pear\n", actions_remove_merch(s.merch_ht, s.storage_ht));

    CU_ASSERT_TRUE(printed("Invalid merch"));
    CU_ASSERT_TRUE(has_merch(&s, "apple"));
    CU_ASSERT_TRUE(has_merch(&s, "banana"));

    teardown(&s);
}

static void test_remove_merch_in_cart(void)
{
    store_t s = setup();
    size_t id = cart_create_cart(s.carts, s.count);
    cart_add_to_cart(s.carts, s.merch_ht, id, "apple", 1);

    WITH_INPUT("apple\n", actions_remove_merch(s.merch_ht, s.storage_ht));

    CU_ASSERT_TRUE(printed("shopping cart"));
    CU_ASSERT_TRUE(has_merch(&s, "apple"));

    teardown(&s);
}

/* ----------------------------- edit_merch ---------------------------- */

static void test_edit_merch_confirmed(void)
{
    store_t s = setup();

    WITH_INPUT("apple\ngreen apple\nSour\n12\ny\n", actions_edit_merch(s.merch_ht, s.storage_ht));

    CU_ASSERT_FALSE(has_merch(&s, "apple"));
    CU_ASSERT_TRUE(has_merch(&s, "green apple"));
    CU_ASSERT_EQUAL(backend_get_price(backend_find_merch_item(s.merch_ht, "green apple")), 12);
    // Stock follows the item to its new name
    CU_ASSERT_EQUAL(backend_get_tot_stock(s.merch_ht, "green apple"), 5);

    teardown(&s);
}

static void test_edit_merch_aborted(void)
{
    store_t s = setup();

    WITH_INPUT("apple\ngreen apple\nSour\n12\nn\n", actions_edit_merch(s.merch_ht, s.storage_ht));

    CU_ASSERT_TRUE(printed("Aborting edit"));
    CU_ASSERT_TRUE(has_merch(&s, "apple"));
    CU_ASSERT_FALSE(has_merch(&s, "green apple"));
    CU_ASSERT_EQUAL(backend_get_price(backend_find_merch_item(s.merch_ht, "apple")), 10);

    teardown(&s);
}

static void test_edit_merch_invalid_input(void)
{
    store_t s = setup();

    WITH_INPUT("pear\n", actions_edit_merch(s.merch_ht, s.storage_ht));
    CU_ASSERT_TRUE(printed("Invalid merch"));

    WITH_INPUT("apple\ngreen apple\nSour\n-1\n", actions_edit_merch(s.merch_ht, s.storage_ht));
    CU_ASSERT_TRUE(printed("Invalid price"));
    CU_ASSERT_TRUE(has_merch(&s, "apple"));
    CU_ASSERT_EQUAL(backend_get_price(backend_find_merch_item(s.merch_ht, "apple")), 10);

    teardown(&s);
}

static void test_edit_merch_in_cart(void)
{
    store_t s = setup();
    size_t id = cart_create_cart(s.carts, s.count);
    cart_add_to_cart(s.carts, s.merch_ht, id, "apple", 1);

    WITH_INPUT("apple\n", actions_edit_merch(s.merch_ht, s.storage_ht));

    CU_ASSERT_TRUE(printed("shopping cart"));
    CU_ASSERT_TRUE(has_merch(&s, "apple"));

    teardown(&s);
}

/* ----------------------------- show_stock ---------------------------- */

static void test_show_stock(void)
{
    store_t s = setup();

    WITH_INPUT("apple\n", actions_show_stock(s.merch_ht));
    CU_ASSERT_TRUE(printed("A01: 5"));

    WITH_INPUT("pear\n", actions_show_stock(s.merch_ht));
    CU_ASSERT_TRUE(printed("Invalid merch"));

    teardown(&s);
}

/* ------------------------------ replenish ---------------------------- */

static void test_replenish_existing_shelf(void)
{
    store_t s = setup();

    WITH_INPUT("A01\napple\n3\n", actions_replenish(s.merch_ht, s.storage_ht));

    CU_ASSERT_EQUAL(backend_get_tot_stock(s.merch_ht, "apple"), 8);

    teardown(&s);
}

static void test_replenish_new_shelf(void)
{
    store_t s = setup();

    WITH_INPUT("A02\napple\n4\n", actions_replenish(s.merch_ht, s.storage_ht));

    CU_ASSERT_EQUAL(backend_get_tot_stock(s.merch_ht, "apple"), 9);
    CU_ASSERT_TRUE(ioopm_hash_table_has_key(s.storage_ht, string_elem("A02")));

    teardown(&s);
}

static void test_replenish_invalid_input(void)
{
    store_t s = setup();

    WITH_INPUT("A01\npear\n2\n", actions_replenish(s.merch_ht, s.storage_ht));
    CU_ASSERT_TRUE(printed("Invalid merch"));

    WITH_INPUT("A01\napple\n0\n", actions_replenish(s.merch_ht, s.storage_ht));
    CU_ASSERT_TRUE(printed("Invalid amount"));

    WITH_INPUT("A01\napple\n-3\n", actions_replenish(s.merch_ht, s.storage_ht));
    CU_ASSERT_TRUE(printed("Invalid amount"));

    // B01 belongs to banana
    WITH_INPUT("B01\napple\n2\n", actions_replenish(s.merch_ht, s.storage_ht));
    CU_ASSERT_TRUE(printed("Invalid location"));

    WITH_INPUT("XYZ\napple\n2\n", actions_replenish(s.merch_ht, s.storage_ht));
    CU_ASSERT_TRUE(printed("Invalid location"));

    // None of these changed the stock
    CU_ASSERT_EQUAL(backend_get_tot_stock(s.merch_ht, "apple"), 5);
    CU_ASSERT_EQUAL(backend_get_tot_stock(s.merch_ht, "banana"), 2);

    teardown(&s);
}

/* ------------------------- create_sc / remove_sc --------------------- */

static void test_create_sc(void)
{
    store_t s = setup();

    WITH_INPUT("", actions_create_sc(s.carts, s.count));
    CU_ASSERT_TRUE(printed("Created shopping cart 1"));

    WITH_INPUT("", actions_create_sc(s.carts, s.count));
    CU_ASSERT_TRUE(printed("Created shopping cart 2"));

    CU_ASSERT_EQUAL(ioopm_hash_table_size(s.carts), 2);

    teardown(&s);
}

static void test_remove_sc(void)
{
    store_t s = setup();
    size_t id = cart_create_cart(s.carts, s.count);
    cart_add_to_cart(s.carts, s.merch_ht, id, "apple", 2);

    WITH_INPUT("1\nn\n", actions_remove_sc(s.carts, s.merch_ht));
    CU_ASSERT_TRUE(printed("Aborting removal"));
    CU_ASSERT_EQUAL(ioopm_hash_table_size(s.carts), 1);

    WITH_INPUT("1\ny\n", actions_remove_sc(s.carts, s.merch_ht));
    CU_ASSERT_EQUAL(ioopm_hash_table_size(s.carts), 0);
    CU_ASSERT_EQUAL(reserved_of(&s, "apple"), 0);

    WITH_INPUT("1\ny\n", actions_remove_sc(s.carts, s.merch_ht));
    CU_ASSERT_TRUE(printed("Invalid id"));

    teardown(&s);
}

/* ----------------------- add_to_sc / remove_from_sc ------------------ */

static void test_add_to_sc(void)
{
    store_t s = setup();
    cart_create_cart(s.carts, s.count);

    WITH_INPUT("1\napple\n3\n", actions_add_to_sc(s.carts, s.merch_ht));

    CU_ASSERT_EQUAL(reserved_of(&s, "apple"), 3);
    CU_ASSERT_EQUAL(cart_calculate_cost(s.carts, s.merch_ht, 1), 30);

    teardown(&s);
}

static void test_add_to_sc_invalid_input(void)
{
    store_t s = setup();
    cart_create_cart(s.carts, s.count);

    WITH_INPUT("7\napple\n1\n", actions_add_to_sc(s.carts, s.merch_ht));
    CU_ASSERT_TRUE(printed("Invalid id"));

    WITH_INPUT("1\napple\n0\n", actions_add_to_sc(s.carts, s.merch_ht));
    CU_ASSERT_TRUE(printed("Invalid amount"));

    WITH_INPUT("1\napple\n6\n", actions_add_to_sc(s.carts, s.merch_ht));
    CU_ASSERT_TRUE(printed("Only 5 available"));

    CU_ASSERT_EQUAL(reserved_of(&s, "apple"), 0);
    CU_ASSERT_EQUAL(cart_calculate_cost(s.carts, s.merch_ht, 1), 0);

    teardown(&s);
}

static void test_remove_from_sc(void)
{
    store_t s = setup();
    size_t id = cart_create_cart(s.carts, s.count);
    cart_add_to_cart(s.carts, s.merch_ht, id, "apple", 3);

    WITH_INPUT("1\napple\n2\n", actions_remove_from_sc(s.carts, s.merch_ht));
    CU_ASSERT_EQUAL(reserved_of(&s, "apple"), 1);
    CU_ASSERT_EQUAL(cart_calculate_cost(s.carts, s.merch_ht, 1), 10);

    WITH_INPUT("1\napple\n5\n", actions_remove_from_sc(s.carts, s.merch_ht));
    CU_ASSERT_TRUE(printed("You only have 1 items"));

    WITH_INPUT("7\napple\n1\n", actions_remove_from_sc(s.carts, s.merch_ht));
    CU_ASSERT_TRUE(printed("Invalid id"));

    CU_ASSERT_EQUAL(reserved_of(&s, "apple"), 1);

    teardown(&s);
}

/* ------------------------ calculate_sc / checkout_sc ----------------- */

static void test_calculate_sc(void)
{
    store_t s = setup();
    size_t id = cart_create_cart(s.carts, s.count);
    cart_add_to_cart(s.carts, s.merch_ht, id, "apple", 2);
    cart_add_to_cart(s.carts, s.merch_ht, id, "banana", 1);

    WITH_INPUT("1\n", actions_calculate_sc(s.carts, s.merch_ht));
    CU_ASSERT_TRUE(printed("Your items cost 23kr"));

    WITH_INPUT("7\n", actions_calculate_sc(s.carts, s.merch_ht));
    CU_ASSERT_TRUE(printed("Invalid id"));
    CU_ASSERT_FALSE(printed("cost"));

    teardown(&s);
}

static void test_checkout_sc(void)
{
    store_t s = setup();
    size_t id = cart_create_cart(s.carts, s.count);
    cart_add_to_cart(s.carts, s.merch_ht, id, "apple", 2);

    WITH_INPUT("1\n", actions_checkout_sc(s.carts, s.merch_ht, s.storage_ht));
    CU_ASSERT_EQUAL(backend_get_tot_stock(s.merch_ht, "apple"), 3);
    CU_ASSERT_EQUAL(reserved_of(&s, "apple"), 0);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(s.carts), 0);

    WITH_INPUT("1\n", actions_checkout_sc(s.carts, s.merch_ht, s.storage_ht));
    CU_ASSERT_TRUE(printed("Invalid id"));

    teardown(&s);
}

/* ------------------------- quit / event_loop ------------------------- */

static void test_quit(void)
{
    int result;

    WITH_INPUT("y\n", result = actions_quit());
    CU_ASSERT_EQUAL(result, -1);

    WITH_INPUT("n\n", result = actions_quit());
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_TRUE(printed("Aborting exit"));
}

static void test_event_loop(void)
{
    store_t s = setup();
    int result;

    // Lowercase menu choices work too
    WITH_INPUT("c\n", result = actions_event_loop(s.merch_ht, s.storage_ht, s.carts, s.count));
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_EQUAL(ioopm_hash_table_size(s.carts), 1);

    // An invalid choice shows the menu again until a valid one is given
    WITH_INPUT("x\n+\n1\nbanana\n1\n", result = actions_event_loop(s.merch_ht, s.storage_ht, s.carts, s.count));
    CU_ASSERT_TRUE(printed("Please enter a valid option"));
    CU_ASSERT_EQUAL(reserved_of(&s, "banana"), 1);

    WITH_INPUT("q\ny\n", result = actions_event_loop(s.merch_ht, s.storage_ht, s.carts, s.count));
    CU_ASSERT_EQUAL(result, -1);

    teardown(&s);
}

/* -------------------------------- main ------------------------------- */

int main(void)
{
    signal(SIGALRM, on_timeout);

    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }

    CU_pSuite suite = CU_add_suite("Frontend actions", NULL, NULL);
    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        CU_add_test(suite, "add_merch", test_add_merch) == NULL ||
        CU_add_test(suite, "add_merch: negative price", test_add_merch_negative_price) == NULL ||
        CU_add_test(suite, "list_merch_items", test_list_merch) == NULL ||
        CU_add_test(suite, "list_merch_items: stop after 20", test_list_merch_stops_after_20_on_n) == NULL ||
        CU_add_test(suite, "remove_merch: confirmed", test_remove_merch_confirmed) == NULL ||
        CU_add_test(suite, "remove_merch: aborted", test_remove_merch_aborted) == NULL ||
        CU_add_test(suite, "remove_merch: invalid name", test_remove_merch_invalid_name) == NULL ||
        CU_add_test(suite, "remove_merch: in a cart", test_remove_merch_in_cart) == NULL ||
        CU_add_test(suite, "edit_merch: confirmed", test_edit_merch_confirmed) == NULL ||
        CU_add_test(suite, "edit_merch: aborted", test_edit_merch_aborted) == NULL ||
        CU_add_test(suite, "edit_merch: invalid input", test_edit_merch_invalid_input) == NULL ||
        CU_add_test(suite, "edit_merch: in a cart", test_edit_merch_in_cart) == NULL ||
        CU_add_test(suite, "show_stock", test_show_stock) == NULL ||
        CU_add_test(suite, "replenish: existing shelf", test_replenish_existing_shelf) == NULL ||
        CU_add_test(suite, "replenish: new shelf", test_replenish_new_shelf) == NULL ||
        CU_add_test(suite, "replenish: invalid input", test_replenish_invalid_input) == NULL ||
        CU_add_test(suite, "create_sc", test_create_sc) == NULL ||
        CU_add_test(suite, "remove_sc", test_remove_sc) == NULL ||
        CU_add_test(suite, "add_to_sc", test_add_to_sc) == NULL ||
        CU_add_test(suite, "add_to_sc: invalid input", test_add_to_sc_invalid_input) == NULL ||
        CU_add_test(suite, "remove_from_sc", test_remove_from_sc) == NULL ||
        CU_add_test(suite, "calculate_sc", test_calculate_sc) == NULL ||
        CU_add_test(suite, "checkout_sc", test_checkout_sc) == NULL ||
        CU_add_test(suite, "quit", test_quit) == NULL ||
        CU_add_test(suite, "event_loop", test_event_loop) == NULL
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    unsigned int failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    free(output);
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}