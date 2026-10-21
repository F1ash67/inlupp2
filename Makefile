CC     = gcc
FLAGS  = -Wall -Wextra -g
LIBS   = -lcunit

COMMON_H  = common.h
HT_H      = hash_table_general_key.h $(COMMON_H)
HT_IT_H   = hash_table_iterator_general_key.h $(HT_H)
LIST_H    = linked_list.h $(COMMON_H)
LIST_IT_H = linked_list_iterator.h $(LIST_H)
BACKEND_H = backend.h $(HT_H) $(LIST_H)
CART_H    = shopping_cart.h $(HT_H) $(LIST_H)
ACTIONS_H = actions.h $(HT_H) $(CART_H)
UTILS_H   = utils.h

complete: webstore

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

hash_table_general_key.o: $(HT_IT_H)
linked_list.o:            $(LIST_IT_H)
backend.o:                $(BACKEND_H) $(LIST_IT_H) hash_table_iterator.h
shopping_cart.o:          $(CART_H) $(BACKEND_H) $(HT_IT_H)
utils.o:                  $(UTILS_H)
actions.o:                $(ACTIONS_H) $(UTILS_H) $(BACKEND_H) $(HT_IT_H) $(LIST_IT_H)
main.o:                   $(ACTIONS_H) $(BACKEND_H) $(CART_H)
backend_tests.o:          $(BACKEND_H)
shopping_cart_tests.o:    $(CART_H) $(BACKEND_H) $(HT_IT_H)
actions_tests.o:          $(ACTIONS_H) $(BACKEND_H) $(HT_IT_H)

COMMON = hash_table_general_key.o linked_list.o
BACKEND = backend.o $(COMMON)
CART = shopping_cart.o $(BACKEND)
ACTIONS = actions.o utils.o $(CART)

TESTS = backend_tests cart_tests actions_tests
INPUT = webstoretest.txt

webstore: main.o $(ACTIONS)
	$(CC) $(FLAGS) $^ -o $@

run: webstore
	./webstore

run_input: webstore
	./webstore $(INPUT)

run_any_input: clean webstore
	./webstore $(FILE)

backend_tests: backend_tests.o $(BACKEND)
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

cart_tests: shopping_cart_tests.o $(CART)
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

actions_tests: actions_tests.o $(ACTIONS)
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test: $(TESTS)
	./backend_tests
	./cart_tests
	./actions_tests

valgrind: $(TESTS) webstore
	valgrind --leak-check=full --error-exitcode=1 ./backend_tests
	valgrind --leak-check=full --error-exitcode=1 ./cart_tests
	valgrind --leak-check=full --error-exitcode=1 ./actions_tests
	valgrind --leak-check=full --error-exitcode=1 ./webstore $(INPUT) > /dev/null

cov: clean
	$(MAKE) test FLAGS="$(FLAGS) --coverage"
	gcovr -r .

clean:
	rm -f webstore $(TESTS) *.o *.gcda *.gcno *.gcov

.PHONY: complete run run_input test valgrind cov clean