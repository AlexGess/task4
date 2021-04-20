#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h> 
#include <winnt.h>
#include <tchar.h>
#include <unistd.h>
#include <sys/types.h>

#define BUF_SIZE 256


TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

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


int choice_path(int *arr, size_t number){

	for(size_t i = 0; i < number; ++i ){
		rm_max(arr);
	}
	return my_max(arr);
}


void print_matrix_with_element(int *matrix, size_t number_col,
	size_t number_str, size_t x, size_t y)
{
	printf("%s\n", "Map, Selected Path :");
	for (size_t i = 0; i < number_str; ++i) {
		for (size_t j = 0; j < number_col; ++j) {
			if (i == y && j == x) {
				printf("|%d| ", *(matrix + i * number_col + j));
			}
			else {
				printf(" %d  ", *(matrix + i * number_col + j));
			}
		}
		printf("\n");
	}
}
int choice_path_right(int *matrix, size_t number_col, size_t number_str,
	size_t x, size_t y)
{
	int number_1 = 0;
	printf("%s\n", "Map, Selected Path :");
	for (size_t i = 0; i < number_str; ++i) {
		for (size_t j = 0; j < number_col; ++j)
		{
			if (i == y && j == x) {
				printf("|%d| ", *(matrix + i * number_col + j));
			}
			else if (i == y && j > x) {
				printf("*%d* ", *(matrix + i * number_col + j));
				if (*(matrix + i * number_col + j) == 1) {
					number_1++;
				}
			}
			else {
				printf(" %d  ", *(matrix + i * number_col + j));
			}
		}
		printf("\n");
	}
	return number_1;
}

int choice_path_left(int *matrix, size_t number_col, size_t number_str,
	size_t x, size_t y)
{
	int number_1 = 0;
	printf("%s\n", "Map, Selected Path :");
	for (size_t i = 0; i < number_str; ++i) {
		for (size_t j = 0; j < number_col; ++j) {
			if (i == y && j == x) {
				printf("|%d| ", *(matrix + i * number_col + j));
			}
			else if (i == y && j < x) {
				printf("*%d* ", *(matrix + i * number_col + j));
				if (*(matrix + i * number_col + j) == 1) {
					number_1++;
				}
			}
			else {
				printf(" %d  ", *(matrix + i * number_col + j));
			}
		}
		printf("\n");
	}
	return number_1;
}

int choice_path_down(int *matrix, size_t number_col, size_t number_str,
	size_t x, size_t y)
{
	int number_1 = 0;
	printf("%s\n", "Map, Selected Path :");
	for (size_t i = 0; i < number_str; ++i) {
		for (size_t j = 0; j < number_col; ++j)
		{
			if (i == y && j == x) {
				printf("|%d| ", *(matrix + i * number_col + j));
			}
			else if (i > y && j == x) {
				printf("*%d* ", *(matrix + i * number_col + j));
				if (*(matrix + i * number_col + j) == 1) {
					number_1++;
				}
			}
			else {
				printf(" %d  ", *(matrix + i * number_col + j));
			}
		}
		printf("\n");
	}
	return number_1;
}

int choice_path_up(int *matrix, size_t number_col, size_t number_str,
	size_t x, size_t y) {
	int number_1 = 0;
	printf("%s\n", "Map, Selected Path :");
	for (size_t i = 0; i < number_str; ++i) {
		for (size_t j = 0; j < number_col; ++j) {
			if (i == y && j == x) {
				printf("|%d| ", *(matrix + i * number_col + j));
			}
			else if (i < y && j == x) {
				printf("*%d* ", *(matrix + i * number_col + j));
				if (*(matrix + i * number_col + j) == 1) {
					number_1++;
				}
			}
			else {
				printf(" %d  ", *(matrix + i * number_col + j));
			}
		}
		printf("\n");
	}
	return number_1;
}

int main(int argc, char const *argv[])
{
	int shfit;
	sscanf_s(argv[1], "%d", &shfit);
	
	HANDLE hMapFile;
	LPCTSTR pBuf;
	printf("Number process %d\n", shfit + 1);

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL) {
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}
	pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
						FILE_MAP_ALL_ACCESS, // read/write permission
						0, 0, BUF_SIZE);

	if (pBuf == NULL) {
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());
		CloseHandle(hMapFile);
		return 1;
	}

	int *ptr_tmp = (int *) pBuf;
	int len[4] = { ptr_tmp[0], ptr_tmp[1], ptr_tmp[2],ptr_tmp[3] };
	size_t length = ptr_tmp[4];
	size_t widht = ptr_tmp[5];
	int x = ptr_tmp[6];
	int y = ptr_tmp[7];
	int* ptr_matrix = (int*) malloc(length * widht * sizeof(int));
	for (size_t i = 0; i < widht * length; ++i) {
		ptr_matrix[i] = ptr_tmp[i + 8];
	}

	UnmapViewOfFile(pBuf);
	// Конец считывания данных

	printf("Process id: %d\n", _getpid());
	// Выбор направления
	int number_1 = 0;
	int len_max = choice_path(len, shfit);
	if (len_max == 0) {
		number_1 = 0;
		print_matrix_with_element(ptr_matrix, length, widht, x, y);
	}
	else if (len_max == len[0]) {
		number_1 = choice_path_right(ptr_matrix, length, widht, x, y);
	}
	else if (len_max == len[1]) {
		number_1 = choice_path_left(ptr_matrix, length, widht, x, y);
	}

	else if (len_max == len[2]) {
		number_1 = choice_path_down(ptr_matrix, length, widht, x, y);
	}
	else if (len_max == len[3]) {
		number_1 = choice_path_up(ptr_matrix, length, widht, x, y);
	}


	pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0, 0, BUF_SIZE);
	if (pBuf == NULL) {
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());
		CloseHandle(hMapFile);
		return 1;
	}
	int *ptr = (int *) pBuf;
	ptr[  widht * length + 8 + shfit ] = number_1;

	UnmapViewOfFile(pBuf);

	printf("Number of target :  %d\n", number_1);
	printf("-------------------------------------------------\n");

	CloseHandle(hMapFile);
	free(ptr_matrix);
	getchar();

	return 0;
}
