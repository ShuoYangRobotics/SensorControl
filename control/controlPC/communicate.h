#include <string.h>
#ifndef COMMUNICATE_H
#define COMMUNICATE_H
char* executeCommand(char* command, int* messageLength)
{
	int i;
	char cmd[50];
	for (i=0;i<50;i++)
	{   
		cmd[i] = 32; 
	} 
	char des[10] = " > tempMsg";
	strcpy (cmd, command);
	strcat (cmd, des);
	fprintf (stderr, "command: %s", cmd);
	int result = system(cmd);
	FILE *inFile;
	int ch;
	int count = 0;
	inFile = fopen("tempMsg","r");
	while (1)
	{
		ch = fgetc(inFile);
		if (ch == EOF) break;
		++ count;
	}
	rewind(inFile);
	char* msg = (char *)malloc(count*sizeof(char));
	count = 0;
	while (1)
	{
		ch = fgetc(inFile);
		if (ch == EOF) break;
		msg[count] = ch;
		++ count;
	}
	fclose(inFile);
	*messageLength = count;

	return msg;
}
#endif
