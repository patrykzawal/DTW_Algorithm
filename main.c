// Install gcc. Compile and execute this source code with "gcc -Wall -o exe main.c && ./exe" command. 

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define X_LENGTH 10                    // length of the x respectively a data set
#define Y_LENGTH 10                    // length of the y respectively b data set
#define MATRIX_SIZE X_LENGTH *Y_LENGTH // length of the cost matrix filled with the dtw cost values of data set x respectively a and data set y respectively b

/* // fill a signed 32 bit integer "data_set" array with random signed 32 bit integer numbers */
void data_set_fill(int32_t *data_set, uint32_t length, uint32_t max)
{
    for (uint32_t i = 0; i < length; i++) // go through each entry of the "data_set" array
    {
        int32_t random = rand();                     // get random number
        bool positive = rand() % 2;                  // determine randomly if the random number is positive or negative
        data_set[i] = (positive                      // if "positive" is true,
                           ? random % max            // the "data_set" array entry at the position "i" gets filled with a positive "random" number between 0 and "max"
                           : (-1) * (random % max)); // otherwise the "data_set" array entry at the position "i" gets filled with a negative "random" number between ((-1) * "max") and max
    }
}

/* print a "label" string respectively character array and a signed 32 bit integer "data_set" array to the console */
void data_set_print(char *label, int32_t *data_set, uint32_t length, uint32_t max)
{
    uint32_t indent = (max / 10) + 2;     // determine indentation based on number of digits
    printf("%s", label);                  // print the "label" string respectively character array
    for (uint32_t i = 0; i < length; i++) // go through each entry of the "data_set" array
    {
        char terminator = (i == length - 1                 // determine if the last entry of the "data_set" array has been reached
                               ? '\n'                      // if true, the "terminator" character is a new line
                               : ',');                     // if false, the "terminator" character is a comma
        printf(" %*d%c", indent, data_set[i], terminator); // print the "data_set" array entry at the position "i" and the "terminator" character to the console
    }
    printf("\n"); // print new line for styling purposes
}

/* determine costs and fill the "cost_matrix" array */
void cost_matrix_fill(
    int32_t *cost_matrix, uint32_t cost_matrix_length, // empty "cost_matrix" and the length respectively size of it
    int32_t *data_set_a, uint32_t data_set_a_length,   // "data_set_a" array and the length of it
    int32_t *data_set_b, uint32_t data_set_b_length)   // "data_set_b" array and the length of it
{
    for (uint32_t i = 0; i < cost_matrix_length; i++)
    {
        uint32_t position_x = i % data_set_a_length; // calculate the current x position
        uint32_t position_y = i / data_set_a_length; // calculate the current y position

        if (i == 0) // check if the current position is the beginning respectively origin of the "cost_matrix" array
        {
            cost_matrix[i] = abs(data_set_a[position_x] - data_set_b[position_y]); // if true, the first "cost_matrix" array entry has the value or cost of the absolute distance between the first value of "data_set_a" and the first value of "data_set_b"
        }
        else if (position_x == 0) // check if there is the danger of a logical and not necessarily technical underflow (the left border)
        {
            int32_t min = cost_matrix[i - data_set_a_length];                            // "down" - the origin is the entry "under" the current position
            cost_matrix[i] = abs(data_set_a[position_x] - data_set_b[position_y]) + min; // the algorithm to calculate the cost
        }
        else if (position_y == 0) // check if there is the danger of a logical and not necessarily technical underflow (the bottom border)
        {
            int32_t min = cost_matrix[i - 1];                                            // "left" - the origin is the entry "left" to the current position
            cost_matrix[i] = abs(data_set_a[position_x] - data_set_b[position_y]) + min; // the algorithm to calculate the cost
        }
        else
        {
            /* determine the minimal value of the left downish neighbors and the origin entry */
            int32_t match = cost_matrix[(i - 1) - data_set_a_length]; // "down left"
            int32_t insertion = cost_matrix[i - data_set_a_length];   // "down"
            int32_t deletion = cost_matrix[i - 1];                    // "left"
            int32_t min = match;
            min = (min < insertion ? min : insertion);
            min = (min < deletion ? min : deletion);
            cost_matrix[i] = abs(data_set_a[position_x] - data_set_b[position_y]) + min; // the algorithm to calculate the cost
        }
    }
}

/* print the "cost_matrix" array */
void cost_matrix_print(
    int32_t *cost_matrix,        // calculated and filled "cost_matrix"
    uint32_t cost_matrix_length, // the length respectively size of "cost_matrix"
    uint32_t data_set_a_length,  // "data_set_a" array length respectively the length of "cost_matrix" x length
    uint32_t max)                // maximum range of the random numbers (min = (-1) * max) in the "data_set_a" array
{
    uint32_t indent = (max % 10) + 2; // determine indentation based on number of digits

    printf("matrix:");                                // print label
    for (uint32_t i = 0; i < cost_matrix_length; i++) // go through each entry of the "cost_matrix" array
    {
        uint32_t position_x = i % data_set_a_length; // calculate the current x position

        char terminator = (position_x == 0                       // if the current x position is 0
                               ? '\n'                            // then the terminator is a new line
                               : ' ');                           // otherwise theterminator is just a space character
        printf("%c| %*d |", terminator, indent, cost_matrix[i]); // print entry
    }
    printf("\n\n"); // print new line for styling purposes
}

void path(
    int32_t *cost_matrix,        // calculated and filled "cost_matrix"
    uint32_t cost_matrix_length, // the length respectively size of "cost_matrix"
    uint32_t data_set_a_length)  // "data_set_a" array length respectively the length of "cost_matrix" x length
{
    int32_t position = cost_matrix_length - 1; // adjust to origin offset 0 and not 1 (arrays go from 0 to (length - 1))
    int32_t step = 0;                          // iterator for the amount of steps of the optimal path

    printf("cost matrix path:\n"); // print label
    while (position >= 0)
    {
        uint32_t position_x = position % data_set_a_length; // calculate the current x position
        uint32_t position_y = position / data_set_a_length; // calculate the current y position

        printf("step %d - position %d, x: %d, y: %d, value: %d\n", step, position, position_x, position_y, cost_matrix[position]); // print the meta data and cost value of the current path entry respectively step

        step++; // iterate step

        if (position == 0)
        {
            position = -1; // exit the while loop
        }
        else if (position_x == 0)
        {
            int32_t origin = position - data_set_a_length; // down
            position = origin;                             // the origin is the entry "under" the current entry
        }
        else if (position_y == 0)
        {
            int32_t origin = position - 1; // left
            position = origin;             // the origin is the entry to "left" to the current entry
        }
        else
        {
            int32_t down_left = cost_matrix[(position - 1) - data_set_a_length]; // down left
            int32_t down = cost_matrix[position - data_set_a_length];            // down
            int32_t left = cost_matrix[position - 1];                            // left
            /* determine the origin entry (the down leftish neighbouring entry with the smallest value) of the current entry */
            int32_t origin = 0;
            if (down_left <= down && down_left <= left)
                origin = (position - 1) - data_set_a_length;
            else if (down <= down_left && down <= left)
                origin = position - data_set_a_length;
            else
                origin = position - data_set_a_length;

            position = origin;
        }
    }
    printf("\n\n\n\n"); // print new line for styling purposes
}

void test()
{
    const uint32_t test_max = 2;               // maximum range of the random numbers (min = (-1) * max)
    const uint32_t test_data_set_a_length = 7; // length of the "test_data_set_a" array
    const uint32_t test_data_set_b_length = 6; // length of the "test_data_set_b" array

    int32_t test_data_set_a[test_data_set_a_length] =
        {0, 0, 1, 2, 0, 1, 0}; // initializing "test_data_set_a"
    int32_t test_data_set_b[test_data_set_b_length] =
        {0, 2, 0, 1, 0, 0}; // initializing "test_data_set_b"

    data_set_print(
        "test_data_set_a:",
        test_data_set_a,
        test_data_set_a_length,
        test_max); // print "data_set_a" to the console
    data_set_print(
        "test_data_set_b:",
        test_data_set_b,
        test_data_set_b_length,
        test_max); // print "data_set_b" to the console

    uint32_t test_cost_matrix_size = test_data_set_a_length * test_data_set_b_length;
    int32_t test_cost_matrix[test_cost_matrix_size]; // declare "test_cost_matrix"

    cost_matrix_fill(
        test_cost_matrix, test_cost_matrix_size,
        test_data_set_a, test_data_set_a_length,
        test_data_set_b, test_data_set_b_length); // fill "test_cost_matrix" with the dtw costs of "test_data_set_a" and "test_data_set_b"

    cost_matrix_print(
        test_cost_matrix,
        test_cost_matrix_size,
        test_data_set_a_length,
        test_max); // print "test_cost_matrix"

    path(test_cost_matrix, test_cost_matrix_size, test_data_set_a_length); // print the optimal path and its costs of the "test_cost_matrix" beginning with the last and ending with the first entry
}

int main(int argc, char const *argv[])
{
    const uint32_t max = 10;                  // maximum range of the random numbers (min = (-1) * max)
    int32_t data_set_a[X_LENGTH];             // initializing "data_set_a"
    int32_t data_set_b[Y_LENGTH];             // initializing "data_set_b"
    srand(time(NULL));                        // setting up random number generator
    data_set_fill(data_set_a, X_LENGTH, max); // fill "data_set_a" with random numbers
    data_set_fill(data_set_b, Y_LENGTH, max); // fill "data_set_b" with random numbers

    data_set_print("data_set_a:", data_set_a, X_LENGTH, max); // print "data_set_a" to the console
    data_set_print("data_set_b:", data_set_b, Y_LENGTH, max); // print "data_set_b" to the console

    int32_t cost_matrix[MATRIX_SIZE]; // declare "cost_matrix"

    cost_matrix_fill(
        cost_matrix, MATRIX_SIZE,
        data_set_a, X_LENGTH,
        data_set_b, Y_LENGTH); // fill "test_cost_matrix" with the dtw costs of "data_set_a" and "data_set_b"

    cost_matrix_print(cost_matrix, MATRIX_SIZE, X_LENGTH, max); // print "test_cost_matrix"

    path(cost_matrix, MATRIX_SIZE, X_LENGTH); // print the optimal path and its costs of the "test_cost_matrix" beginning with the last and ending with the first entry

    test(); // calculate test values and print the results

    return 0;
}