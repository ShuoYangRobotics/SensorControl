#ifndef _CAL_H
#define _CAL_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


double reference = -50.0;
double N = 3;
double DIRECT_THRESHOLD = 0.45;
double SIMILARITY_THRESHOLD = 0.9;
double NOISE = 0.43;

int SIDE = 3;
int MAX_LEN = 15;
int REFLECTION_THRESHOLD = 7;

typedef struct vector {
	double x;
	double y;
} vector;

vector vector1;

double rice[13][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0.607, 0.466, 0.187, 0.033, 0, 0, 0, 0, 0, 0, 0, 0},
    {0.271, 0.374, 0.414, 0.202, 0.039, 0, 0, 0, 0, 0, 0, 0}, 
    {0.033, 0.099, 0.303, 0.405, 0.212, 0.042, 0, 0, 0, 0, 0, 0}, 
    {0, 0, 0.078, 0.282, 0.402, 0.218, 0.044, 0, 0, 0, 0, 0}, 
    {0, 0, 0, 0.070, 0.272, 0.401, 0.222, 0.046, 0, 0, 0, 0}, 
    {0, 0, 0, 0, 0.066, 0.266, 0.400, 0.225, 0.047, 0, 0, 0}, 
    {0, 0, 0, 0, 0, 0.064, 0.262, 0.400, 0.227, 0.048, 0, 0}, 
    {0, 0, 0, 0, 0, 0, 0.063, 0.259, 0.400, 0.229, 0.048, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0.061, 0.257, 0.400, 0.230, 0.049}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0.060, 0.255, 0.399, 0.231}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.060, 0.254, 0.399}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.059, 0.253}
};

vector getVector (double distance, double rssi) {
	int i,j;
	double p1, p2;
	double rd;
	vector rstVector;

	i = (int)distance;
	rd = pow(10, (rssi-reference)/(10*N));

	for (j=0; j<=110;j++) {
		if ((rd - 1.0/(distance * 0.1*j)) >= 0) {
			p1 = 1.0/(0.1*j);
			p2 = (rd - 1.0/(distance * 0.1*j))/(1.0/(distance+1));
			if (p2/(double)rice[i][i+1] > REFLECTION_THRESHOLD)
				continue;
			else {
				rstVector.x = p1;///sqrt((p1*p1)+(p2*p2));
				rstVector.y = p2;///sqrt((p1*p1)+(p2*p2));
				return rstVector;
			}
		}
	}
}

int generate (double rssi, double result[], vector result_vector[], int* len) {
	if ((rssi >=0) || (rssi <=-100)) return 1;

	double i, j;
	int itemItr = 0;
	double rd;
	double p1, p2;

	rd = pow(10, (rssi-reference)/(10*N));
	result[itemItr] = 1.0/rd;
	itemItr++;

	for (i = 2; i<=100; i=i+2) {
		for (j=1; j<=110;j++) {
			if ((rd - 1.0/(0.01*i*j)) >=0) {
				p1 = 1.0/(0.1*j);
				p2 = (rd - 1.0/(0.01*i*j))/(1.0/(0.1*i+1));
				if (p2/(double)rice[(int)(0.1*i)][(int)(0.1*i)+1]
					>REFLECTION_THRESHOLD)
					break;

				if (itemItr >= 2) {
					if ((p1 >= DIRECT_THRESHOLD) &&
						(abs(0.1*i-1.0/rd) < 2) &&
						(((int)(i) - (int)(result[itemItr-1]*10)) !=0)) {
						result[itemItr] = 0.1*i;
						result_vector[itemItr].x = p1;//sqrt((p1*p1)+(p2*p2));
						result_vector[itemItr].y = p2;//sqrt((p1*p1)+(p2*p2));
						itemItr++;
						if (itemItr >=MAX_LEN-1) {
							*len = itemItr-1;
							return 1;
						}
					}
				}
				else {
					if ((p1 >= DIRECT_THRESHOLD) &&
						(abs(0.1*i-1.0/rd) < 2)) {
						result[itemItr] = 0.1*i;
						result_vector[itemItr].x = p1;//sqrt((p1*p1)+(p2*p2));
						result_vector[itemItr].y = p2;//sqrt((p1*p1)+(p2*p2));
						itemItr++;
					}
				}
			}
		}
	}
	*len = itemItr-1;
	return 1;
}

int calculatePosition (double d0, double d1, double d2, double d3) {
	int i,j,k,l;
	double list[4] = {d0,d1,d2,d3};
	double d[4][MAX_LEN];
	vector r[4][MAX_LEN];
	int lenList[4];
	double d_weight[4][MAX_LEN];

	double x = 0;
	double y = 0;
	double posX = 0;
	double posY = 0;
	double counter = 0;
	double aver_posX = 0;
	double aver_posY = 0;

	for (i=0; i<4; i++)
	{
		generate(list[i],d[i], r[i],&lenList[i]);
		/*************debug output*************/ 
		/**/printf("\n");
		/**/printf("rssi %4.2f(%d): ",list[i],lenList[i]);
		/*************debug output*************/ 

		for (j=1;j<lenList[i];j++)
		{
			d_weight[i][j] = 
				(r[i][j].x*vector1.x+r[i][j].y*vector1.y)/
				(sqrt(r[i][j].x*r[i][j].x+r[i][j].y*r[i][j].y)
				*sqrt(vector1.x*vector1.x+vector1.y*vector1.y));
			/*************debug output*************/ 
			/**/printf("%2.1f(%6.5f,%6.5f)-%6.5f\n\t\t ",
			/**/	d[i][j], 
			/**/	r[i][j].x,
			/**/	r[i][j].y,
			/**/	d_weight[i][j]);
			/*************debug output*************/ 
		}
		/*************debug output*************/ 
		printf("\n");
		/*************debug output*************/ 
	}


	for (i=0;i<lenList[0];i++)
	{
		for (j=0;j<lenList[1];j++)
		{
			for (k=0;k<lenList[2];k++)
			{
				for (l=0;l<lenList[3];l++)
				{
					x = (d[3][l]*d[3][l]-
						 d[2][k]*d[2][k]+
						 SIDE*SIDE)
						/(2*SIDE);

					y = (d[3][l]*d[3][l]-
						 d[0][i]*d[0][i]+
						 SIDE*SIDE)
						/(2*SIDE);

					counter += 
						1 *
						d_weight[3][l] *
						d_weight[2][k] *
						d_weight[0][i]; 

					posX += 
						x *
						d_weight[3][l] *
						d_weight[2][k] *
						d_weight[0][i]; 

					posY += 
						y *
						d_weight[3][l] *
						d_weight[2][k] *
						d_weight[0][i]; 
				}
			}
		}
	}

	aver_posX = posX / counter ;
	aver_posY = posY / counter ;

	printf ("position: \tX=%6.5f\n\t\tY=%6.5f\n",
		aver_posX, aver_posY);

	if ((aver_posX>=0 && aver_posX<=SIDE) ||
		(aver_posY>=0 && aver_posY<=SIDE))
		return 1;
	else 
		return 0;
}
#endif

