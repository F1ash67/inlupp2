CC     = gcc
FLAGS  = -Wall -Wextra -g
LIBS   = -lcunit

COMMON_H  = ./lib/common.h
HT_H      = ./lib/hash_table_general_key.h $(COMMON_H)
HT_IT_H   = ./lib/hash_table_iterator_general_key.h $(HT_H)
LIST_H    = ./lib/linked_list.h $(COMMON_H)
LIST_IT_H = ./lib/linked_list_iterator.h $(LIST_H)
BACKEND_H = ./src/backend/backend.h $(HT_H) $(LIST_H)
CART_H    = ./src/backend/shopping_cart.h $(HT_H) $(LIST_H)
ACTIONS_H = ./src/frontend/actions.h $(HT_H) $(CART_H)
UTILS_H   = ./lib/utils.h

all: ./build/webstore

vpath %.c lib src/backend src/frontend tests

./build/%.o: %.c
	$(CC) $(FLAGS) -I./lib -I./src/backend -I./src/frontend -c $< -o $@

./build/hash_table_general_key.o: $(HT_IT_H)
./build/linked_list.o:            $(LIST_IT_H)
./build/backend.o:                $(BACKEND_H) $(LIST_IT_H) ./lib/hash_table_iterator.h
./build/shopping_cart.o:          $(CART_H) $(BACKEND_H) $(HT_IT_H)
./build/utils.o:                  $(UTILS_H)
./build/actions.o:                $(ACTIONS_H) $(UTILS_H) $(BACKEND_H) $(HT_IT_H) $(LIST_IT_H)
./build/main.o:                   $(ACTIONS_H) $(BACKEND_H) $(CART_H)
./build/backend_tests.o:          $(BACKEND_H)
./build/shopping_cart_tests.o:    $(CART_H) $(BACKEND_H) $(HT_IT_H)
./build/actions_tests.o:          $(ACTIONS_H) $(BACKEND_H) $(HT_IT_H)

COMMON = ./build/hash_table_general_key.o ./build/linked_list.o
BACKEND = ./build/backend.o $(COMMON)
CART = ./build/shopping_cart.o $(BACKEND)
ACTIONS = ./build/actions.o ./build/utils.o $(CART)

TESTS = ./build/backend_tests ./build/cart_tests ./build/actions_tests
INPUT = webstoretest.txt

./build/webstore: build/main.o $(ACTIONS)
	$(CC) $(FLAGS) $^ -o $@

run: ./build/webstore
	./build/webstore

run_input: ./build/webstore
	./build/webstore $(INPUT)

run_any_input: clean ./build/webstore
	./build/webstore $(FILE)

./build/backend_tests: build/backend_tests.o $(BACKEND)
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

./build/cart_tests: build/shopping_cart_tests.o $(CART)
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

./build/actions_tests: build/actions_tests.o $(ACTIONS)
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

test: $(TESTS)
	./build/backend_tests
	./build/cart_tests
	./build/actions_tests

valgrind: $(TESTS) ./build/webstore
	valgrind --leak-check=full --error-exitcode=1 ./build/backend_tests
	valgrind --leak-check=full --error-exitcode=1 ./build/cart_tests
	valgrind --leak-check=full --error-exitcode=1 ./build/actions_tests
	valgrind --leak-check=full --error-exitcode=1 ./build/webstore $(INPUT) > /dev/null

cov: clean
	$(MAKE) test FLAGS="$(FLAGS) --coverage"
	gcovr -r .

clean:
	rm -f ./build/webstore $(TESTS) ./build/*.o ./build/*.gcda ./build/*.gcno ./build/*.gcov

.PHONY: all run run_input run_any_input test valgrind cov clean