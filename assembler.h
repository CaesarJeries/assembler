#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__


typedef enum
{
	ASSEMBLER_SUCCESS,
	ASSEMBLER_INVALID_EXT,
	ASSEMBLER_FILE_OPEN_ERR,
	ASSEMBLER_FILE_READ_ERR,
	ASSEMBLER_PARSE_ERR
} AssemblerStatus;


typedef struct assembler Assembler;

Assembler* assemblerInit();
AssemblerStatus assemblerProcess(Assembler* assembler, const char* filename);
void assemblerReset(Assembler* assembler);
void assemblerDestroy(Assembler* assembler);

#endif // __ASSEMBLER_H__
