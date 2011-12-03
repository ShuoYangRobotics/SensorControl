#ifndef COMMUNICATE_H
#define COMMUNICATE_H
#include <string.h>
void executeCommand(char* command, int* messageLength, char* msg)
{
	int i;
	char cmd[50];
	FILE *inFile;
	char ch;
	int count = 0;
	for (i=0;i<50;i++)
	{   
		cmd[i] = 32; 
	} 
	char des[10] = " > tempMsg";
	strcpy (cmd, command);
	strcat (cmd, des);
	fprintf (stderr, "\ncommand: %s\n", cmd);
	int result = system(cmd);
	sleep(1);
	inFile = fopen("tempMsg","r");
    do 
	{
    	ch = fgetc(inFile);
		if (ch == EOF) break;
		msg[count] = ch;
		if (count >=48) break;
		else count++;
    } 
	while (ch != EOF);

	/*
	rewind(inFile);
	count = 0;
	while (1)
	{
		ch = fgetc(inFile);
		if (ch == EOF) break;
		msg[count] = ch;
		if (count >=50) break;
		++ count;
	}
	*/
	fclose(inFile);
	*messageLength = count;
	return msg;
}
#endif
