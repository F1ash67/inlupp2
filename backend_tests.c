#include <CUnit/Basic.h>
#include "backend.h"
#include "hash_table.h"

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
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create();
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create();
  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  elem_t result;
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(merch_ht, "shirt", &result));

  remove_merch_item(storage_ht, merch_ht, "shirt");
  ioopm_hash_table_destroy(storage_ht);
  ioopm_hash_table_destroy(merch_ht);
}

void test_add_item_to_storage()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create();
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create();

  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  create_merch_item(merch_ht, "pants", "a pant", 150);
  elem_t result;
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(merch_ht, "shirt", &result));

  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "shirt", "A23"));
  CU_ASSERT_FALSE(replenish_merch_item(storage_ht, merch_ht, "pants", "A23"));

  destroy_all_merch(merch_ht, storage_ht);

  ioopm_hash_table_destroy(merch_ht);
  ioopm_hash_table_destroy(storage_ht);
}

void test_add_item_to_multiple_storage()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create();
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create();
  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  create_merch_item(merch_ht, "pants", "a pant", 150);

  
  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "shirt", "A23"));
  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "shirt", "B54"));
  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "shirt", "A23"));
  CU_ASSERT_TRUE(replenish_merch_item(storage_ht, merch_ht, "pants", "C89"));

  elem_t result;
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(merch_ht, "shirt", &result));

  destroy_all_merch(merch_ht, storage_ht);

  ioopm_hash_table_destroy(merch_ht);
  ioopm_hash_table_destroy(storage_ht);
}

void test_quantity()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create();
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create();
  create_merch_item(merch_ht, "shirt", "a shirt", 100);
  create_merch_item(merch_ht, "pants", "a pant", 150);

  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "B54");
  replenish_merch_item(storage_ht, merch_ht, "shirt", "A23");
  replenish_merch_item(storage_ht, merch_ht, "pants", "C89");

  int quantity_shirt = get_tot_quantity(merch_ht, "shirt");
  int quantity_pants = get_tot_quantity(merch_ht, "pants");

  CU_ASSERT_EQUAL(quantity_shirt, 3);
  CU_ASSERT_EQUAL(quantity_pants, 1);

  destroy_all_merch(merch_ht, storage_ht);

  ioopm_hash_table_destroy(merch_ht);
  ioopm_hash_table_destroy(storage_ht);
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