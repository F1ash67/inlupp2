#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hash_table_general_key.h"
#include "hash_table_iterator_general_key.h"
#include "shopping_cart.h"
#include "backend.h"

//To-Do: Review test validity

//Generated using AI

static ioopm_hash_table_t *create_merch_ht(void)
{
    return ioopm_hash_table_create(str_hash, str_eq);
}

static ioopm_hash_table_t *create_storage_ht(void)
{
    return ioopm_hash_table_create(str_hash, str_eq);
}

// Adds a merch item with the given price and puts `stock` of it on one shelf.
// Each item needs its own shelf, since a location can only hold one item.
static void add_test_merch(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht,
                           char *name, size_t price, size_t stock, char *location)
{
    create_merch_item(merch_ht, name, "test description", price);
    for (size_t i = 0; i < stock; i++) {
        replenish_merch_item(storage_ht, merch_ht, name, location);
    }
}

static void destroy_backend(ioopm_hash_table_t *merch_ht, ioopm_hash_table_t *storage_ht)
{
    destroy_all_merch(merch_ht, storage_ht);
    ioopm_hash_table_destroy(merch_ht);
    ioopm_hash_table_destroy(storage_ht);
}

/* ===================================================================== */

// Return codes used by add_to_cart / remove_from_cart
#define NO_SUCH_CART   -1
#define NO_SUCH_MERCH  -2
#define ZERO_AMOUNT    -3
#define SUCCESS        -4

// Defined in hash_table_general_key.c. Delete these two lines once they are
// declared in hash_table_general_key.h.
size_t int_hash(elem_t key);
bool int_eq(elem_t a, elem_t b);

// Everything a test needs, created fresh for every test
typedef struct {
    ioopm_hash_table_t *carts;
    ioopm_hash_table_t *merch_ht;
    ioopm_hash_table_t *storage_ht;
    cart_count_t *count;
} fixture_t;

// Creates a fresh store with two items:
//   "apple"  price 10, stock 5
//   "banana" price 3,  stock 2
static fixture_t setup(void)
{
    fixture_t f;
    f.carts = ioopm_hash_table_create(int_hash, int_eq);
    f.merch_ht = create_merch_ht();
    f.storage_ht = create_storage_ht();
    f.count = initiate_cart_count();

    add_test_merch(f.merch_ht, f.storage_ht, "apple", 10, 5, "A01");
    add_test_merch(f.merch_ht, f.storage_ht, "banana", 3, 2, "B01");
    return f;
}

static void teardown(fixture_t *f)
{
    // Destroy any carts the test left behind
    ioopm_hash_table_iterator_t *it = ioopm_hash_table_iterator_create(f->carts);
    while (!ioopm_hash_table_iterator_at_end(it)) {
        ioopm_hash_table_destroy(ioopm_hash_table_iterator_current_value(it).p);
        ioopm_hash_table_iterator_advance(it);
    }
    ioopm_hash_table_iterator_destroy(it);
    ioopm_hash_table_destroy(f->carts);

    destroy_cart_count(f->count);
    destroy_backend(f->merch_ht, f->storage_ht);
}

static size_t reserved_of(fixture_t *f, char *name)
{
    return get_reserved(find_merch_item(f->merch_ht, name));
}

/* ---------------------------- create_cart ---------------------------- */

static void test_create_cart_gives_increasing_ids(void)
{
    fixture_t f = setup();

    CU_ASSERT_EQUAL(create_cart(f.carts, f.count), 0);
    CU_ASSERT_EQUAL(create_cart(f.carts, f.count), 1);
    CU_ASSERT_EQUAL(create_cart(f.carts, f.count), 2);

    teardown(&f);
}

static void test_new_cart_costs_nothing(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);

    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 0);

    teardown(&f);
}

/* ---------------------------- add_to_cart ---------------------------- */

static void test_add_invalid_input(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);

    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, 999, "apple", 1), NO_SUCH_CART);
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, id, "apple", 0), ZERO_AMOUNT);

    // Nothing should have been reserved or added
    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 0);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 0);

    teardown(&f);
}

static void test_add_reserves_and_costs(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);

    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, id, "apple", 2), SUCCESS);
    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 2);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 20);

    // Adding does not change the actual stock
    CU_ASSERT_EQUAL(get_tot_stock(f.merch_ht, "apple"), 5);

    teardown(&f);
}

static void test_add_same_item_twice_accumulates(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);

    add_to_cart(f.carts, f.merch_ht, id, "apple", 2);
    add_to_cart(f.carts, f.merch_ht, id, "apple", 3);

    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 5);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 50);

    teardown(&f);
}

static void test_add_multiple_items_cost(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);

    add_to_cart(f.carts, f.merch_ht, id, "apple", 1);    // 10
    add_to_cart(f.carts, f.merch_ht, id, "banana", 2);   // 6

    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 16);

    teardown(&f);
}

static void test_add_more_than_available(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);

    // Only 5 apples exist: returns how many are available, adds nothing
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, id, "apple", 6), 5);
    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 0);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 0);

    teardown(&f);
}

static void test_two_carts_share_stock(void)
{
    fixture_t f = setup();
    size_t a = create_cart(f.carts, f.count);
    size_t b = create_cart(f.carts, f.count);

    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, a, "apple", 3), SUCCESS);
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, b, "apple", 3), 2);   // only 2 left
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, b, "apple", 2), SUCCESS);
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, b, "apple", 1), 0);   // sold out

    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 5);

    teardown(&f);
}

/* -------------------------- remove_from_cart ------------------------- */

static void test_remove_from_cart_invalid_input(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);
    add_to_cart(f.carts, f.merch_ht, id, "apple", 2);

    CU_ASSERT_EQUAL(remove_from_cart(f.carts, f.merch_ht, 999, "apple", 1), NO_SUCH_CART);
    CU_ASSERT_EQUAL(remove_from_cart(f.carts, f.merch_ht, id, "apple", 0), ZERO_AMOUNT);

    // More than the cart holds: returns how many are in the cart
    CU_ASSERT_EQUAL(remove_from_cart(f.carts, f.merch_ht, id, "apple", 3), 2);
    // An item that exists but is not in the cart
    CU_ASSERT_EQUAL(remove_from_cart(f.carts, f.merch_ht, id, "banana", 1), 0);

    // Nothing changed
    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 2);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 20);

    teardown(&f);
}

static void test_remove_part_of_item(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);
    add_to_cart(f.carts, f.merch_ht, id, "apple", 4);

    CU_ASSERT_EQUAL(remove_from_cart(f.carts, f.merch_ht, id, "apple", 1), SUCCESS);
    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 3);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 30);

    teardown(&f);
}

static void test_remove_all_of_item(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);
    add_to_cart(f.carts, f.merch_ht, id, "apple", 4);

    CU_ASSERT_EQUAL(remove_from_cart(f.carts, f.merch_ht, id, "apple", 4), SUCCESS);
    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 0);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 0);

    // The item is gone from the cart, so removing again finds nothing
    CU_ASSERT_EQUAL(remove_from_cart(f.carts, f.merch_ht, id, "apple", 1), 0);

    teardown(&f);
}

static void test_removed_items_become_available(void)
{
    fixture_t f = setup();
    size_t a = create_cart(f.carts, f.count);
    size_t b = create_cart(f.carts, f.count);

    add_to_cart(f.carts, f.merch_ht, a, "apple", 5);
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, b, "apple", 1), 0);

    remove_from_cart(f.carts, f.merch_ht, a, "apple", 2);
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, b, "apple", 3), 2);
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, b, "apple", 2), SUCCESS);

    teardown(&f);
}

/* ----------------------------- remove_cart --------------------------- */

static void test_remove_cart(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);
    add_to_cart(f.carts, f.merch_ht, id, "apple", 3);
    add_to_cart(f.carts, f.merch_ht, id, "banana", 1);

    CU_ASSERT_TRUE(remove_cart(f.carts, f.merch_ht, id));

    // Reservations are released
    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 0);
    CU_ASSERT_EQUAL(reserved_of(&f, "banana"), 0);

    // The cart no longer exists
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, id, "apple", 1), NO_SUCH_CART);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), -1);
    CU_ASSERT_FALSE(remove_cart(f.carts, f.merch_ht, id));

    teardown(&f);
}

static void test_remove_cart_keeps_other_carts(void)
{
    fixture_t f = setup();
    size_t a = create_cart(f.carts, f.count);
    size_t b = create_cart(f.carts, f.count);
    add_to_cart(f.carts, f.merch_ht, b, "apple", 2);

    remove_cart(f.carts, f.merch_ht, a);

    // b still works and keeps its id
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, b), 20);
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, b, "apple", 1), SUCCESS);

    // New carts don't reuse old ids
    CU_ASSERT_EQUAL(create_cart(f.carts, f.count), 2);

    teardown(&f);
}

/* ------------------------------ checkout ----------------------------- */

static void test_checkout_nonexistent_cart(void)
{
    fixture_t f = setup();

    CU_ASSERT_FALSE(checkout(f.carts, f.merch_ht, f.storage_ht, 999));
    CU_ASSERT_EQUAL(get_tot_stock(f.merch_ht, "apple"), 5);

    teardown(&f);
}

static void test_checkout_reduces_stock(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);
    add_to_cart(f.carts, f.merch_ht, id, "apple", 3);
    add_to_cart(f.carts, f.merch_ht, id, "banana", 1);

    CU_ASSERT_TRUE(checkout(f.carts, f.merch_ht, f.storage_ht, id));

    CU_ASSERT_EQUAL(get_tot_stock(f.merch_ht, "apple"), 2);
    CU_ASSERT_EQUAL(get_tot_stock(f.merch_ht, "banana"), 1);
    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 0);
    CU_ASSERT_EQUAL(reserved_of(&f, "banana"), 0);

    // The cart is gone after checkout
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), -1);

    teardown(&f);
}

static void test_checkout_with_other_cart_active(void)
{
    fixture_t f = setup();
    size_t a = create_cart(f.carts, f.count);
    size_t b = create_cart(f.carts, f.count);
    add_to_cart(f.carts, f.merch_ht, a, "apple", 3);
    add_to_cart(f.carts, f.merch_ht, b, "apple", 2);

    checkout(f.carts, f.merch_ht, f.storage_ht, a);

    // 2 apples left in stock, and all of them are still reserved by b
    CU_ASSERT_EQUAL(get_tot_stock(f.merch_ht, "apple"), 2);
    CU_ASSERT_EQUAL(reserved_of(&f, "apple"), 2);
    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, b, "apple", 1), 0);

    // b can still check out its 2
    CU_ASSERT_TRUE(checkout(f.carts, f.merch_ht, f.storage_ht, b));
    CU_ASSERT_EQUAL(get_tot_stock(f.merch_ht, "apple"), 0);

    teardown(&f);
}

static void test_checkout_empty_cart(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);

    CU_ASSERT_TRUE(checkout(f.carts, f.merch_ht, f.storage_ht, id));
    CU_ASSERT_EQUAL(get_tot_stock(f.merch_ht, "apple"), 5);

    teardown(&f);
}

/* ------------------------- nonexistent merch ------------------------- */

// These need find_merch_item to return NULL for a name that doesn't exist.
// With the current assert version they abort the whole test program, so they
// are off by default. Turn them on with -DTEST_MISSING_MERCH once
// find_merch_item is changed.
#ifdef TEST_MISSING_MERCH
static void test_add_nonexistent_merch(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);

    CU_ASSERT_EQUAL(add_to_cart(f.carts, f.merch_ht, id, "pear", 1), NO_SUCH_MERCH);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 0);

    teardown(&f);
}

static void test_remove_nonexistent_merch(void)
{
    fixture_t f = setup();
    size_t id = create_cart(f.carts, f.count);
    add_to_cart(f.carts, f.merch_ht, id, "apple", 2);

    CU_ASSERT_EQUAL(remove_from_cart(f.carts, f.merch_ht, id, "pear", 1), NO_SUCH_MERCH);
    CU_ASSERT_EQUAL(calculate_cost(f.carts, f.merch_ht, id), 20);

    teardown(&f);
}
#endif

/* -------------------------------- main ------------------------------- */

int main(void)
{
    if (CU_initialize_registry() != CUE_SUCCESS) {
        return CU_get_error();
    }

    CU_pSuite suite = CU_add_suite("Shopping cart", NULL, NULL);
    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
        CU_add_test(suite, "create_cart: increasing ids", test_create_cart_gives_increasing_ids) == NULL ||
        CU_add_test(suite, "create_cart: new cart costs 0", test_new_cart_costs_nothing) == NULL ||
        CU_add_test(suite, "add: invalid input", test_add_invalid_input) == NULL ||
        CU_add_test(suite, "add: reserves and costs", test_add_reserves_and_costs) == NULL ||
        CU_add_test(suite, "add: same item twice", test_add_same_item_twice_accumulates) == NULL ||
        CU_add_test(suite, "add: multiple items", test_add_multiple_items_cost) == NULL ||
        CU_add_test(suite, "add: more than available", test_add_more_than_available) == NULL ||
        CU_add_test(suite, "add: two carts share stock", test_two_carts_share_stock) == NULL ||
        CU_add_test(suite, "remove_from_cart: invalid input", test_remove_from_cart_invalid_input) == NULL ||
        CU_add_test(suite, "remove_from_cart: part of item", test_remove_part_of_item) == NULL ||
        CU_add_test(suite, "remove_from_cart: all of item", test_remove_all_of_item) == NULL ||
        CU_add_test(suite, "remove_from_cart: frees stock", test_removed_items_become_available) == NULL ||
        CU_add_test(suite, "remove_cart: releases and removes", test_remove_cart) == NULL ||
        CU_add_test(suite, "remove_cart: other carts unaffected", test_remove_cart_keeps_other_carts) == NULL ||
        CU_add_test(suite, "checkout: nonexistent cart", test_checkout_nonexistent_cart) == NULL ||
        CU_add_test(suite, "checkout: reduces stock", test_checkout_reduces_stock) == NULL ||
        CU_add_test(suite, "checkout: other cart active", test_checkout_with_other_cart_active) == NULL ||
        CU_add_test(suite, "checkout: empty cart", test_checkout_empty_cart) == NULL
#ifdef TEST_MISSING_MERCH
        || CU_add_test(suite, "add: nonexistent merch", test_add_nonexistent_merch) == NULL
        || CU_add_test(suite, "remove_from_cart: nonexistent merch", test_remove_nonexistent_merch) == NULL
#endif
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    unsigned int failures = CU_get_number_of_failures();
    CU_cleanup_registry();
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}