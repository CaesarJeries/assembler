#ifndef __FILE_READER_H__
#define __FILE_READER_H__

#include <stddef.h>	// size_t


#define MAX_LINE_SIZE 128


typedef struct file_reader FileReader;

typedef enum
{
	FILE_READER_SUCCESS,
	FILE_READER_EOF,
	FILE_READER_ERROR

} FileReaderStatus;



FileReader* fileReaderInit(const char* filename);
FileReaderStatus fileReaderGetLine(FileReader* fr, char* dst);

const char* fileReaderGetFilename(const FileReader* fr);
size_t fileReaderGetLineNum(const FileReader* fr);

void fileReaderRewind(FileReader* fr);
void fileReaderDestroy(FileReader* fr);

#endif // __FILE_READER_H__
