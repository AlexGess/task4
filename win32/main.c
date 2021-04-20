#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h> 
#include <winnt.h>
#include <tchar.h>

#define BUF_SIZE 256

TCHAR szName[] = TEXT("Global\\MyFileMappingObject");

void print_matrix(int *matrix, size_t number_col, size_t number_str)
{
	printf("%s\n", "Map:");
	for (size_t i = 0; i < number_str; ++i) {
		for (size_t j = 0; j < number_col; ++j) {
			printf(" %d  ", *(matrix + i * number_col + j));
		}
		printf("\n");
	}

}
void print_matrix_with_element(int *matrix, size_t number_col,
	size_t number_str, int x, int y)
{
	printf("%s\n", "Map and selected item :");
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

void init_matrix(int *matrix, int number_col, int number_str)
{
	int tmp = 0;
	for (size_t i = 0; i < number_str; ++i) {
		for (size_t j = 0; j < number_col; ++j) {
			tmp = rand() % 2;
			*(matrix + i * number_col + j) = tmp;
		}
	}
}

void select_element(int *matrix, size_t number_col,
	size_t number_str, int *x, int *y)
{
	*x = rand() % number_col;
	*y = rand() % number_str;
	printf("Selected coordinates  X: %d Y: %d \n", *x + 1, *y + 1);
	print_matrix_with_element(matrix, number_col, number_str, *x, *y);
}

int main(void)
{	
	srand(time(NULL));
	const size_t length = 5 + rand() % 6;
	const size_t widht = 5 + rand() % 6; 

	printf("Number of lines :%d\n", widht);
	printf("Number of columns :%d\n", length);

	int *ptr_matrix = (int *) malloc((widht * length) * sizeof(int));


	init_matrix(ptr_matrix, length, widht);
	print_matrix(ptr_matrix, length, widht);
	printf("-------------------------------------------\n");

	int x, y;
	select_element(ptr_matrix, length, widht, &x, &y);
	printf("-------------------------------------------------\n");

	int len[4];
	len[0] = length - (x + 1);
	len[1] = length - (len[0] + 1);
	len[2] = widht - (y + 1);
	len[3] = widht - (len[2] + 1);
	printf("Long paths : %d %d %d %d\n", len[0], len[1], len[2], len[3]);
	printf("-------------------------------------------------\n");

	int *data = (int *) malloc((widht * length  + 2 + 4 + 2) * sizeof(int));
	data[0] = len[0]; data[1] = len[1]; data[2] = len[2]; data[3] = len[3];
	data[4] = length; data[5] = widht; data[6] = x;  data[7] = y;
	for (size_t i = 8; i < widht * length + 8; ++i) {
		data[i] = ptr_matrix[i- 8];
	}
	free(ptr_matrix);

	HANDLE hMapFile;
	LPCTSTR pBuf;
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object
	if (hMapFile == NULL) {
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}
	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);
	if (pBuf == NULL) {
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());
		CloseHandle(hMapFile);
		return 1;
	}
	if (data) {
		CopyMemory((LPVOID)pBuf, data, (length * widht + 8) * sizeof(int));
	}
	UnmapViewOfFile(pBuf);

	int number_proc = rand() % 5;
	printf("Number of scouts %d\n", number_proc);
	getchar();

	PROCESS_INFORMATION arr_inf_proc[number_proc];

	for (size_t i = 0; i < number_proc; ++i) {
		const char *lpszAppName = "./scout.exe";
		char lpszCommandLine[2];
		sprintf(lpszCommandLine, " %d", i);
		STARTUPINFO si;
		PROCESS_INFORMATION piApp;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		if (!CreateProcess(lpszAppName, lpszCommandLine, NULL, NULL, FALSE,
			CREATE_NEW_CONSOLE, NULL, NULL, &si, &piApp))
		{
			_cputs("The process is not created .\n");
			_getch();
			return 0;
		}
		arr_inf_proc[i] = piApp;
		_cputs("The process is created.\n");
	}

	for (size_t i = 0; i < number_proc; ++i) {
		WaitForSingleObject(arr_inf_proc[i].hProcess, INFINITE);
		CloseHandle(arr_inf_proc[i].hThread);
		CloseHandle(arr_inf_proc[i].hProcess);
	}	

	pBuf = (LPTSTR) MapViewOfFile(hMapFile,
						FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
	if (pBuf == NULL) {
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());
		CloseHandle(hMapFile);
		return 1;
	}
	int *ptr = (int *) pBuf;

	printf("Number of target :\n");
	for (size_t i = 0; i < number_proc; ++i) {
		printf("Number of process  %d : ", i + 1);
		printf(" %d \n", ptr[widht * length + 8 + i]);

	}
	printf("\n");
	
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	free(data);
	return 0;
}
