#include <math.h>
#include "cal.h"
int main ()
{
	int measurement[4];
	measurement[0] = -62;
	measurement[1] = -62;
	measurement[2] = -62;
	measurement[3] = -80;

	//double temp[MAX_LEN];
	//int len;
	//generate (-80, temp, &len);

	vector1 = getVector(4,-72);
	printf("(4,-72) --> (%6.5f,%6.5f)",vector1.x,vector1.y);

	calculatePosition(measurement[0],measurement[1],measurement[2],measurement[3]);

	return 0;
}
