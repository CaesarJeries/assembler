#ifndef __FILE_READER_H__
#define __FILE_READER_H__

#define MAX_LINE_SIZE 80

typedef struct file_reader FileReader;

typedef enum
{
	FILE_READER_SUCCESS,
	FILE_READER_EOF,
	FILE_READER_ERROR
} FileReaderStatus;

FileReader* fileReaderInit(const char* filename);
FileReaderStatus fileReaderGetLine(FileReader* fr, char** dst);
void fileReaderDestroy(FileReader* fr);

#endif // __FILE_READER_H__
