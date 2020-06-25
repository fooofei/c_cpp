/* the file shows merge sub sorted lists to one sorted lists
 */

#include <vector>
#include <queue>
#include <functional>
#include <iostream>
#include <stdio.h>

/*
    https://www.techiedelight.com/merge-m-sorted-lists-containing-n-elements/

    Given M sorted lists each containing N elements, print them in sorted order efficiently.


    For example,

    Input: 5 sorted lists of fixed size 4

    [ 10, 20, 30, 40 ]
    [ 15, 25, 35, 45 ]
    [ 27, 29, 37, 48 ]
    [ 32, 33, 39, 50 ]
    [ 16, 18, 22, 28 ]

    Output:

    10 15 16 18 20 22 25 27 28 29 30 32 33 35 37 39 40 45 48 50

*/

struct node_t {
    int row;
    int col;
    int value;

    node_t(int arg0, int arg1, int arg2) : row(arg0), col(arg1), value(arg2)
    {
        ;
    }
};

struct comp_func_t {
    bool operator () (const node_t &lhs, const node_t &rhs)
    {
        return lhs.value > rhs.value;
    }
};

/*
    O(M * N * logM)
*/
void print_sorted(const int *ar, int row, int col)
{
    std::priority_queue<node_t, std::vector<node_t>, comp_func_t> q;

    for (int i = 0; i < row; ++i) {
        q.push(node_t(i, 0, *(ar + i * col)));
    }

    while (!q.empty()) {
        node_t x = q.top(); // cannot use const node_t &
        q.pop();

        printf("%d ", x.value);

        if (x.col + 1 < col) {
            q.push(node_t(x.row, x.col + 1, *(ar + x.row * col + x.col + 1)));
        }
    }

    printf("\n");
}


int main()
{
    // M lists of size N each in the form of 2D-matrix
    int list[] = {
        10, 20, 30, 40,
        15, 25, 35, 45,
        27, 29, 37, 48,
        32, 33, 39, 50,
        16, 18, 22, 28
    };

    // 10 15 16 18 20 22 25 27 28 29 30 32 33 35 37 39 40 45 48 50
    print_sorted(list, 5, 4);

    return 0;
}