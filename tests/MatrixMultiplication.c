#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void main()
{

	srand(time(0));

	int A[2][2];
	int B[2][2];
	int C[2][2];

	printf("-----------------------------------------------------------");
	printf("\nThe input tables are: \n\n");

	for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			A[i][j] = rand() % 5;
			printf("%d\t", A[i][j]);
		}
		printf("\n");
	}

	printf("\n");

	for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			B[i][j] = rand() % 5;
			printf("%d\t", B[i][j]);
		}
		printf("\n");
	}

	printf("\nThe Multiplication of the tables is:\n\n");

	for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			C[i][j] = 0;
			for(int k=0; k<2; k++){
				C[i][j] += A[i][k]*B[k][j];
			}
		}
	}

    for(int i=0; i<2; i++){
		for(int j=0; j<2; j++){
			printf("%d\t", C[i][j]);
		}
		printf("\n");
	}

	printf("\n-----------------------------------------------------------\n");

}
