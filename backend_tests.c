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
  merch_t *shirt = create_merch_item("shirt");
  add_merch_item(merch_ht, shirt);
  elem_t result;
  CU_ASSERT_PTR_NOT_NULL(shirt);
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(merch_ht, "shirt", &result));

  destroy_merch_item(merch_ht, shirt);
  ioopm_hash_table_destroy(merch_ht);
}

void test_add_item_to_storage()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create();
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create();

  merch_t *shirt = create_merch_item("shirt");
  merch_t *pants = create_merch_item("pants");
  shelf_t *shelf = create_shelf("A23", 10);

  add_merch_item(merch_ht, shirt);
  add_merch_item(merch_ht, pants);

  elem_t result;
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(merch_ht, "shirt", &result));

  CU_ASSERT_TRUE(add_merch_loc(storage_ht, shirt, shelf));
  CU_ASSERT_FALSE(add_merch_loc(storage_ht, pants, shelf));

  destroy_all_merch(merch_ht, storage_ht);

  ioopm_hash_table_destroy(merch_ht);
  ioopm_hash_table_destroy(storage_ht);
}

void test_add_item_to_multiple_storage()
{
  ioopm_hash_table_t *merch_ht = ioopm_hash_table_create();
  ioopm_hash_table_t *storage_ht = ioopm_hash_table_create();

  merch_t *shirt = create_merch_item("shirt");
  merch_t *pants = create_merch_item("pants");
  shelf_t *shelf1 = create_shelf("A23", 10);
  shelf_t *shelf2 = create_shelf("B54", 20);
  shelf_t *shelf3 = create_shelf("C89", 30);

  add_merch_item(merch_ht, shirt);
  add_merch_item(merch_ht, pants);

  
  CU_ASSERT_TRUE(add_merch_loc(storage_ht, shirt, shelf1));
  CU_ASSERT_TRUE(add_merch_loc(storage_ht, shirt, shelf2));
  CU_ASSERT_TRUE(add_merch_loc(storage_ht, pants, shelf3));

  elem_t result;
  CU_ASSERT_TRUE(ioopm_hash_table_lookup(merch_ht, "shirt", &result));

  

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