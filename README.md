**Authors: Otis Hildebrand & Edvin Falt**

## Building the program
To build the program, run `make all`

To run all tests, run `make test`

## Running the program
To run the program normally, run `make run`

To run the program with the given test 
input, run `make run_input`

To run the program with any .txt file as the input, run `make run_any_input FILE=(your filename).txt`

You don't have to run the program with any file, then you can just add things manually, but if you do run the program with a file as an argument to populate the web store, you will no longer be able to use the program normally. Therefore you have to make sure to end the file with Q and Y on their own lines, otherwise the program is stuck in an endless loop.

## Coverage report
|File           |Lines  |Exec   |Cover    |Missing            |
|---------------|-------|-------|---------|-------------------|
|actions.c      |205    |202    |98%      |250-251,319        |
|backend.c      |144    |143    |99%      |40                 |
|shopping_cart.c|106    |96     |90%      |41-46,48-49,82,109 |

Coverage report generated using gcov

There are clearly more files that exist in this program but since the tests dont add anything to the program and the hash table, linked list and utils are libraries from other work in the course their coverage is not interesting to this task and is not shown.

