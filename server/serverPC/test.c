#include <stdio.h>
#include <stdlib.h>
#include "communicate.h"
int main ()
{
	char *haha = "ls";
	char *result;
	int msg_len;
	result = executeCommand(haha,&msg_len);
	return 0;
}
