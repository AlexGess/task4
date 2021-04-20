#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>


int my_max(int *arr)
{
    int res = arr[0];
    for (size_t i = 1; i < 4; ++i) {
        if (arr[i] > res) {
            res = arr[i];
        }
    }
    return res;
}

void rm_max(int *arr)
{
    int len_max = my_max(arr);
    for (int i = 0; i < 4; ++i) {
        if (arr[i] == len_max) {
            arr[i] = 0;
            break;
        }
    }
}

int choice_path(int *arr, int number)
{
    for (size_t i = 0; i < number; ++i ){
        rm_max(arr);
    }
    return my_max(arr);
}

void print_matrix(int *matrix, size_t number_col, size_t number_str)
{
    printf("%s\n", "Map:" );
    for (size_t i = 0; i < number_str; ++i){
        for (size_t j = 0; j < number_col; ++j){
            printf(" %d  ", *(matrix + i * number_col + j));
        }
        printf("\n");
    }
}

void print_matrix_with_element(int *matrix, size_t number_col,
    size_t number_str, size_t x , size_t y)
{
    printf("%s\n", "Map and selected item:" );
    for (size_t i = 0; i < number_str; ++i){
        for (size_t j = 0; j < number_col; ++j){
            if (i == y && j ==x){
                printf("|%d| ", *(matrix + i * number_col + j));
            }
            else {
                printf(" %d  ", *(matrix + i * number_col + j));
            }
        }
        printf("\n");
    }
}

void init_matrix(int *matrix, size_t number_col, size_t number_str)
{
    int tmp = 0;
    for (size_t i = 0; i < number_str; ++i) {
        for (size_t j = 0; j < number_col; ++j) {
            tmp = rand() % 2;
            *(matrix + i * number_col + j)  = tmp;
        }
    }
}

void select_element(int *matrix, size_t number_col,
    size_t number_str, int *x , int *y)
{
    *x = rand() % number_col;
    *y = rand() % number_str;
    printf("Selected coordinates X: %d Y: %d \n", *x + 1, *y + 1 );
    print_matrix_with_element(matrix, number_col, number_str , *x, *y);
}

void choice_path_right(int *matrix, size_t number_col, size_t number_str,
    size_t x , size_t y, int *arr, size_t shift)
{
    int number_1 = 0;
    for (size_t i = 0; i < number_str; ++i) {
        for (size_t j = 0; j < number_col; ++j) {
            if (i == y && j > x) {
                if (*(matrix + i * number_col + j)  == 1) {
                    number_1++;
                }
            }
        }
    }
    arr[shift] = number_1;
}

void choice_path_left(int *matrix, size_t number_col, size_t number_str,
    size_t x, size_t y, int *arr, size_t shift)
{
    int number_1 = 0;
    for (size_t i = 0; i < number_str; ++i) {
        for (size_t j = 0; j < number_col; ++j) {
            if (i == y && j < x) {
                if(*(matrix + i * number_col + j)  == 1) {
                    number_1++;
                }
            }
        }
    }
    arr[shift] = number_1;
}

void choice_path_down(int *matrix, size_t number_col, size_t number_str,
    size_t x, size_t y, int *arr, size_t shift)
{
    int number_1 = 0;
    for (size_t i = 0; i < number_str; ++i) {
        for (size_t j = 0; j < number_col; ++j) {
            if (i > y && j == x) {
                if (*(matrix + i * number_col + j)  == 1) {
                    number_1++;
                }
            }
        }
    }
    arr[shift] = number_1;
}

void choice_path_up(int *matrix, size_t number_col, size_t number_str,
    size_t x, size_t y, int *arr, size_t shift)
{
    int number_1 = 0;
    for (size_t i = 0; i < number_str; ++i){
        for (size_t j = 0; j < number_col; ++j) {
            if (i < y && j == x) {
                if (*(matrix + i * number_col + j)  == 1) {
                    number_1++;
                }
            }
        }
    }
    arr[shift] = number_1;
}


int main(void)
{   
    srand(time(NULL));
    int  length = 5 + rand() % 6;
    int  widht = 5 + rand() % 6;
    printf("Number of rows: %d\n", widht);
    printf("Number of columns: %d\n", length);

    int matrix[widht][length];
    int *ptr_matrix = (int *) matrix;

    init_matrix(ptr_matrix, length, widht);
    print_matrix(ptr_matrix, length, widht);
    printf("-------------------------------------------------\n");

    int x, y;
    select_element(ptr_matrix, length, widht, &x, &y);
    printf("-------------------------------------------------\n");

    int len[4];
    len[0] = length - (x + 1);
    len[1] = length - (len[0] + 1);
    len[2] = widht - (y + 1);
    len[3] = widht - (len[2] + 1);
    printf("Lengths of paths: %d %d %d %d\n", len[0], len[1], len[2], len[3]);
    printf("-------------------------------------------------\n");

    int number_proc = rand() % 5;
    int *ptr_arr_number_1 = mmap(NULL, 4 * sizeof(int), PROT_READ|PROT_WRITE,
                                                MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    printf("Number of scouts: %d\n", number_proc);
    for (size_t i = 0; i < number_proc; ++i) {
        pid_t new_proc_copy; 
        int rv = 0;
        switch (new_proc_copy = fork()) {
        case -1:
            fprintf(stderr, "fork() failed\n");
            exit(1);
        case 0: 
            printf("Scout with PID %d\n", getpid());
            size_t len_max = choice_path(len, i);
            if (len_max == len[0]) {
                choice_path_right(ptr_matrix, length, widht, x, y, ptr_arr_number_1, i);
            }
            else if (len_max == len[1]) {
                choice_path_left(ptr_matrix, length, widht, x, y, ptr_arr_number_1, i);
            }
            else if (len_max == len[2]) {
                choice_path_down(ptr_matrix, length, widht, x, y, ptr_arr_number_1, i);
            }
            else if (len_max == len[3]) {
                choice_path_up(ptr_matrix, length, widht, x, y, ptr_arr_number_1, i);
            }
            exit(rv);
        }
    }
    int rv = 0;
    for (size_t i = 0; i < number_proc; ++i) {
        wait(&rv);
    }
    for (size_t i = 0; i < number_proc; ++i) {
        printf("Number of items for process %zu: %d\n", i + 1, ptr_arr_number_1[i]);
    }
    printf("-------------------------------------------------\n");

    munmap(ptr_arr_number_1, 4 * sizeof(int));


    return 0;
}