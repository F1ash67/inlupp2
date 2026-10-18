CC = gcc
FLAGS = -Wall -Wextra -g --coverage

clean:
	rm -f *.o *.gcda *.gcno *.gcov

backend_tests: backend_tests.c backend.c hash_table_general_key.c linked_list.c
	$(CC) $(FLAGS) $^ -o $@ -lcunit

cov_backend: backend_tests
	./$^
	gcov -abcf backend_tests-backend.gcda

cov: cov_backend
	gcovr -r .