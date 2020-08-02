#ifndef _MACHINE_H__
#define _MACHINE_H__


typedef struct machine Machine;

Machine* machineInit(FILE* input);
void machineDestroy(Machine* machine);



#endif // _MACHINE_H__
