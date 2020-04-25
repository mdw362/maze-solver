# Multithreaded maze solver

Generates a maze and uses a mutlithreaded genetic algorithm to attempt to find a solution. Result is a sequence of numbers with each number representing a step through the maze (1 = left, 2 = right, 3 = down, 4 = up).

# Usage
Run the following:

    make
    ./exec_lab4 <number of threads> <value of g> <rows> <cols> <genome length>

For example:
    make
    ./exec_lab4 4 10000 10 10 30



