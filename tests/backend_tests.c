#include <CUnit/Basic.h>
#include "backend.h"
#include "hash_table_general_key.h"

int init_suite(void)
{
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite(void)
{
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}

void test_create_destroy()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create(str_hash, str_eq);
  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  elem_t result;
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(merch_ht, string_elem("shirt"), &result));

  destroy_all_merch(merch_ht, storage_ht);
}

void test_add_item_to_storage()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create(str_hash, str_eq);

  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  create_merch_item(merch_ht, "pants", "a pant", 150);
  elem_t result;
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(merch_ht, string_elem("shirt"), &result));

  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "shirt", "A23"));
  CU_ASSERT_FALSE(replenish_merch_item(storage_ht, merch_ht, "pants", "A23"));

  destroy_all_merch(merch_ht, storage_ht);
}

void test_add_item_to_multiple_storage()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create(str_hash, str_eq);
  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  create_merch_item(merch_ht, "pants", "a pant", 150);

  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "shirt", "A23"));
  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "shirt", "B54"));
  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "shirt", "A23"));
  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "pants", "C89"));

  elem_t result;
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(merch_ht, string_elem("shirt"), &result));

  destroy_all_merch(merch_ht, storage_ht);
}

void test_quantity()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create(str_hash, str_eq);
  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  create_merch_item(merch_ht, "pants", "a pant", 150);

  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "B54");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "pants", "C89");

  int quantity_shirt = get_tot_stock(merch_ht, "shirt");
  int quantity_pants = get_tot_stock(merch_ht, "pants");

  CU_ASSERT_EQUAL(quantity_shirt, 3);
  CU_ASSERT_EQUAL(quantity_pants, 1);

  destroy_all_merch(merch_ht, storage_ht);
}

void test_merch_list()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create(str_hash, str_eq);
  merch_t *shirt = create_merch_item(merch_ht, "shirt", "a shirt", 100);
  merch_t *pants = create_merch_item(merch_ht, "pants", "a pant", 150);

  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "B54");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "pants", "C89");

  ioopm_list_t *merch_list = list_merch(merch_ht);

  CU_ASSERT_TRUE(ioopm_list_head(merch_list).p == shirt || ioopm_list_head(merch_list).p == pants);
  CU_ASSERT_TRUE(ioopm_list_last(merch_list).p == shirt || ioopm_list_last(merch_list).p == pants);
  destroy_all_merch(merch_ht, storage_ht);

  ioopm_list_destroy(merch_list);
}

void test_remove_stock()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create(str_hash, str_eq);
  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  create_merch_item(merch_ht, "pants", "a pant", 150);

  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "B54");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "pants", "C89");

  CU_ASSERT_EQUAL(get_tot_stock(merch_ht, "shirt"), 3);
  remove_stock(merch_ht, storage_ht, "shirt", 2);
  CU_ASSERT_EQUAL(get_tot_stock(merch_ht, "shirt"), 1);

  remove_stock(merch_ht, storage_ht, "pants", 1);
  ioopm_list_t *stock_list = get_stock_list(merch_ht, "pants");
  CU_ASSERT_TRUE(ioopm_list_is_empty(stock_list));

  destroy_all_merch(merch_ht, storage_ht);
}

void test_edit_merch()
{
  ioopm_hash_table_t *merch_ht =
      ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht =
      ioopm_hash_table_create(str_hash, str_eq);

  create_merch_item(merch_ht, "shirt", "A shirt", 100);

  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "B54");

  edit_merch_item(
      merch_ht, storage_ht,
      "shirt", "hoodie", "A hoodie", 250);


  CU_ASSERT_TRUE(merch_item_exists(merch_ht, "hoodie"));
  CU_ASSERT_FALSE(merch_item_exists(merch_ht, "shirt"));
  // The merchandise should be accessible by its new name.
  elem_t result;
  CU_ASSERT_TRUE(
      ioopm_hash_table_lookup(
          merch_ht, string_elem("hoodie"), &result));

  // The item's details should be updated.
  merch_t *hoodie = find_merch_item(merch_ht, "hoodie");
  CU_ASSERT_STRING_EQUAL(get_name(hoodie), "hoodie");
  CU_ASSERT_EQUAL(get_price(hoodie), 250);

  // Existing stock should be preserved.
  CU_ASSERT_EQUAL(get_tot_stock(merch_ht, "hoodie"), 3);

  // Existing storage locations should still be occupied.
  CU_ASSERT_TRUE(
      ioopm_hash_table_has_key(storage_ht, string_elem("A23")));
  CU_ASSERT_TRUE(
      ioopm_hash_table_has_key(storage_ht, string_elem("B54")));

  destroy_all_merch(merch_ht, storage_ht);
}

void test_replenish_same_location()
{
  ioopm_hash_table_t *merch_ht =
      ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht =
      ioopm_hash_table_create(str_hash, str_eq);

  create_merch_item(merch_ht, "shirt", "a shirt", 100);

  CU_ASSERT_TRUE(
      replenish_merch_item(storage_ht, merch_ht, "shirt", "A23"));
  CU_ASSERT_TRUE(
      replenish_merch_item(storage_ht, merch_ht, "shirt", "A23"));
  CU_ASSERT_TRUE(
      replenish_merch_item(storage_ht, merch_ht, "shirt", "A23"));

  CU_ASSERT_EQUAL(get_tot_stock(merch_ht, "shirt"), 3);
  CU_ASSERT_EQUAL(
      ioopm_list_size(get_stock_list(merch_ht, "shirt")), 1);

  destroy_all_merch(merch_ht, storage_ht);
}

void test_remove_stock_across_shelves()
{
  ioopm_hash_table_t *merch_ht =
      ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht =
      ioopm_hash_table_create(str_hash, str_eq);

  create_merch_item(merch_ht, "shirt", "a shirt", 100);

  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "B54");

  CU_ASSERT_EQUAL(get_tot_stock(merch_ht, "shirt"), 3);

  // A23 has two units; B54 has one.
  CU_ASSERT_TRUE(remove_stock(merch_ht, storage_ht, "shirt", 2));

  CU_ASSERT_EQUAL(get_tot_stock(merch_ht, "shirt"), 1);

  // A23 should now be empty and freed.
  CU_ASSERT_FALSE(
      ioopm_hash_table_has_key(storage_ht, string_elem("A23")));

  // B54 should still be occupied.
  CU_ASSERT_TRUE(
      ioopm_hash_table_has_key(storage_ht, string_elem("B54")));

  destroy_all_merch(merch_ht, storage_ht);
}

void test_remove_merch_with_stock()
{
  ioopm_hash_table_t *merch_ht =
      ioopm_hash_table_create(str_hash, str_eq);
  ioopm_hash_table_t *storage_ht =
      ioopm_hash_table_create(str_hash, str_eq);

  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  create_merch_item(merch_ht, "pants", "a pair of pants", 150);


  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "B54");

  remove_merch_item(storage_ht, merch_ht, "shirt");

  // Merchandise should no longer exist.
  CU_ASSERT_FALSE(
      ioopm_hash_table_has_key(merch_ht, string_elem("shirt")));

  // Its former locations should be available.
  CU_ASSERT_FALSE(
      ioopm_hash_table_has_key(storage_ht, string_elem("A23")));
  CU_ASSERT_FALSE(
      ioopm_hash_table_has_key(storage_ht, string_elem("B54")));

  // Other merchandise should remain untouched.
  CU_ASSERT_TRUE(
      ioopm_hash_table_has_key(merch_ht, string_elem("pants")));

  destroy_all_merch(merch_ht, storage_ht);
}

void test_merch_getters()
{
  ioopm_hash_table_t *merch_ht =
      ioopm_hash_table_create(str_hash, str_eq);

  ioopm_hash_table_t *storage_ht =
      ioopm_hash_table_create(str_hash, str_eq);

  merch_t *shirt =
      create_merch_item(merch_ht, "shirt", "a shirt", 100);

  CU_ASSERT_STRING_EQUAL(get_name(shirt), "shirt");
  CU_ASSERT_EQUAL(get_price(shirt), 100);

  destroy_all_merch(merch_ht, storage_ht);

}

int main()
{
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite backend_test_suite = CU_add_suite("Backend test suite", init_suite, clean_suite);
  if (backend_test_suite == NULL)
  {
    // If the test suite could not be added, tear down CUnit and exit
    CU_cleanup_registry();
    return CU_get_error();
  }

  // This is where we add the test functions to our test suite.
  // For each call to CU_add_test we specify the test suite, the
  // name or description of the test, and the function that runs
  // the test in question. If you want to add another test, just
  // copy a line below and change the information
  if (
      (CU_add_test(backend_test_suite, "Create destroy merch", test_create_destroy) == NULL) ||
      (CU_add_test(backend_test_suite, "Add merch to storage", test_add_item_to_storage) == NULL) ||
      (CU_add_test(backend_test_suite, "Add multiple merch to storage", test_add_item_to_multiple_storage) == NULL) ||
      (CU_add_test(backend_test_suite, "Check quantity of merch", test_quantity) == NULL) ||
      (CU_add_test(backend_test_suite, "List all merch", test_merch_list) == NULL) ||
      (CU_add_test(backend_test_suite, "Remove stock", test_remove_stock) == NULL) ||
      (CU_add_test(backend_test_suite, "Edit merch", test_edit_merch) == NULL) ||
      (CU_add_test(backend_test_suite, "Replenish same shelf", test_replenish_same_location) == NULL) ||
      (CU_add_test(backend_test_suite, "Remove stock across shelves", test_remove_stock_across_shelves) == NULL) ||
      (CU_add_test(backend_test_suite, "Remove merchandise with stock", test_remove_merch_with_stock) == NULL) ||
      (CU_add_test(backend_test_suite, "Test merchandise getters", test_merch_getters) == NULL) ||
      0)
  {
    // If adding any of the tests fails, we tear down CUnit and exit
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
  // Use CU_BRM_NORMAL to only print errors and a summary
  CU_basic_set_mode(CU_BRM_VERBOSE);

  // This is where the tests are actually run!
  CU_basic_run_tests();

  // Tear down CUnit before exiting
  CU_cleanup_registry();
  return CU_get_error();
}