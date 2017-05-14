#if defined(_UCC)

#include "stdio.h"

#elif defined(_WIN32)

#include "windows.h"

#else

#endif

#include "input.h"
#include "stdio.h"
#include "stdlib.h"

struct input Input;
unsigned char END_OF_FILE = 255;

void ReadSourceFile(char *filename)
{
#if defined(_UCC) //file base size
	

	int len;

	Input.file = fopen(filename, "r");
	if (Input.file == NULL) {
		Fatal("Can't open file : %s", filename);
	}

	fseek(Input.file, 0, SEEK_SET);
	Input.size = fread(Input.base, 1, Input.size, Input.file);
	fclose(Input.file);
	

#elif defined(_WIN32)

	Input.file = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (Input.file == INVALID_HANDLE_VALUE)
	{
		Fatal("Can't open file : %s", filename);
	}

	Input.size = GetFileSize(Input.file, NULL);
	Input.fileMapping = CreateFileMapping(Input.file, NULL, PAGE_READWRITE, 0, Input.size + 1, NULL);
	if (Input.fileMapping == NULL)
	{
		Fatal("Can't create file mapping: %s.", filename);
	}

	Input.base = (unsigned char *)MapViewOfFile(Input.fileMapping, FILE_MAP_WRITE, 0, 0, 0);
	if (Input.base == NULL)
	{
		Fatal("Can't map file : %s", filename);
	}

#endif 
	Input.filename = filename;
	Input.base[Input.size] = END_OF_FILE;
	Input.cursor = Input.lineHead = Input.base;
	Input.line = 1;

	return;

}

void CloseSourceFile(void)
{
#if defined(_UCC)

	free(Input.base);

#elif defined(_WIN32)

	UnmapViewOfFile(Input.base);
	CloseHandle(Input.fileMapping);
	SetFilePointer(Input.file, Input.size, NULL, FILE_BEGIN);
	SetEndOfFile(Input.file);
	CloseHandle(Input.file);

#endif
}



