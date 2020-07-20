/* PA2 estimate */

#include <stdio.h>
#include <stdlib.h>

// CS211 Computer Architecture - Spring 2020
//Abhay Saxena

double **callTranspose(double **training, int row, int col);
double **callMultiplication(double **transpose, double **training, int row, int col);
double **callInverse(double **multiplied, int col);
double **calltransMul(double **transpose, double **price, int col, int row);
double **callfinalMul(double **inverse, double **transPrice, int col);
double **callPrice(double **data, double **weights, int row2, int col);

double **callTranspose(double **training, int row, int col){

	double** transpose = (double**)malloc(col*sizeof(double*));
	int i,j;
	for(i = 0; i < col; i++){
		transpose[i]=(double*)malloc(row*sizeof(double));
	}

	for(i = 0; i < col; i++){
		for(j = 0; j < row; j++){
			transpose[i][j]=training[j][i];
		}
	}

	//CHECK
	/*printf("X^T : \n");
	for(i = 0; i < col; i++){
		for(j = 0; j < row; j++){
			printf("%.5f ", transpose[i][j]);
		}
		printf("\n");
	}
	printf("\n");*/
	
	return transpose;
}

double **callMultiplication(double **transpose, double **training, int row, int col){
	
	double **multiplied = calloc(col, col*sizeof(double*));
	
	for (int i = 0; i < col; i++){
	multiplied[i] = (double *) calloc(col, col*sizeof(double));
		for (int j = 0; j < col; j++){				//j < row;
			for (int k = 0; k < row; k++){
				multiplied[i][j] = multiplied[i][j] + (transpose[i][k] * training[k][j]);
			}
		}
	}
	
	//CHECK
	/*printf("X^T*X : \n");
	for(int i = 0; i < col; i++){
		for(int j = 0; j < col; j++){
			printf("%.5f ", multiplied[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	printf("rows: %d\n", col);
	printf("cols: %d\n", col);*/
	
	return multiplied;
}

double **callInverse(double **multiplied, int col){
	
	double **augIdentity = malloc(col * sizeof(double*));
	for (int i = 0; i < col; i++) {
		augIdentity[i] = malloc(col * sizeof(double));
		for (int j = 0; j < col; j++) {
			if (i == j) augIdentity[i][j] = (double) 1;
			else augIdentity[i][j] = (double) 0;
		}
	}
	
	double f;
	for (int p = 0; p < col; p++) {
		f = multiplied[p][p];
		for (int j = 0; j < col; j++) {
			multiplied[p][j] = multiplied[p][j] / f;
			augIdentity[p][j] = augIdentity[p][j] / f;
		}
		for (int i = p + 1; i < col; i++) {
			f = multiplied[i][p];
			for (int j = 0; j < col; j++) {
				multiplied[i][j] -= f*multiplied[p][j];
				augIdentity[i][j] -= f*augIdentity[p][j];
			}
		}
	}
	
	for (int p = col - 1; p >= 0; p--) {
		for (int i = p - 1; i >= 0; i--) {
			f = multiplied[i][p];
			for (int j = 0; j < col; j++) {
				multiplied[i][j] -= f*multiplied[p][j];
				augIdentity[i][j] -= f*augIdentity[p][j];
			}
		}
	}
	
	//CHECK
	/*printf("\nInverse Matrix: \n\n");
	for(int i = 0; i < col; i++){
		for(int j = 0; j < col; j++){
			printf("%.5f ", augIdentity[i][j]);
		}
		printf("\n");
	}
	printf("\n");*/
	
	return augIdentity;
}

double **calltransMul(double **transpose, double **price, int col, int row){
	
	double **transPrice = calloc(col, col*sizeof(double*));
	
	for (int i = 0; i < col; i++){
		transPrice[i] = calloc(1, 1*sizeof(double));
		for (int j = 0; j < row; j++){
			transPrice[i][0] += transpose[i][j] * price[j][0];
		}
	}
	
	//CHECK
	/*printf("X^T*Y : \n");
	for(int i = 0; i < col; i++){
		printf("%.0f ", transPrice[i][0]);
		printf("\n");
	}
	printf("\n");*/
	
	return transPrice;
}

double **callfinalMul(double **inverse, double **transPrice, int col){

	double **weights = calloc(col, col*sizeof(double*));
	
	for (int i = 0; i < col; i++){
		weights[i] = calloc(1, sizeof(double));
		for (int j = 0; j < col; j++){
			weights[i][0] += inverse[i][j] * transPrice[j][0];
		}
	}
	
	//CHECK
	/*printf("((X^T*X)^-1)*X^T*Y : \n");
	for(int i = 0; i < col; i++){
		printf("%5f ", weights[i][0]);
		printf("\n");
	}
	printf("\n");*/
	
	return weights;
}

double **callPrice(double **data, double **weights, int row2, int col){
	
	double **finalPrice = calloc(row2, row2*sizeof(double*));
	
	for (int i = 0; i < row2; i++){
		finalPrice[i] = calloc(1, sizeof(double));
		for (int j = 0; j < col; j++){
			finalPrice[i][0] += data[i][j] * weights[j][0];
		}
	}
	
	return finalPrice;
}

void freeMatrix(double **matrix, int row){
	for (int i = 0; i < row; i++){
		free(matrix[i]);
	}
	
	free(matrix);
}

int main(int argc, char **argv){

	char trainString[6];
	char dataString[5];
	int row = 0;
	int col = 0;
	int row2 = 0;
	int col2 = 0;
	
	//Read file pointer
	FILE *ptr = fopen(argv[1],"r");
	if (ptr == NULL){
		printf("error");
		return 0;
	}
	
	//Scan string, number of columns and rows of training matrix
	fscanf(ptr, "%s", trainString);
	fscanf(ptr, "%d\n", &col);
	fscanf(ptr, "%d\n", &row);
	
	col = col + 1;
	
	double **training = malloc(row*sizeof(double*));
	
	double **price = malloc(row*sizeof(double*));
	
	for (int i = 0; i < row; i++){
	training[i] = malloc(col*sizeof(double*));
		for (int j = 0; j < col; j++){
			if (j == 0){
				training[i][0] = 1;
			}
		}
	}
	
	for (int i = 0; i < row; i++){
	price[i] = malloc(1*sizeof(double));
		for (int j = 1; j <= col; j++){ //check this condition
			if (j == col){
				fscanf(ptr, "%lf\n", &price[i][0]);
			} else {
				fscanf(ptr, "%lf\n", &training[i][j]);
			}
		}
	}
	
	//Close file pointer
	fclose(ptr);
	
	//Read file pointer
	FILE *temp = fopen(argv[2], "r");
	if (temp == NULL){
		printf("error\n");
		return 0;
	}
	
	//Scan string, number of columns and rows of second (data) matrix.
	fscanf(temp, "%s", dataString);
	fscanf(temp, "%d\n", &col2);
	fscanf(temp, "%d\n", &row2);
	
	col2 = col2 + 1;
	
	double **data = malloc(row2*sizeof(double*));
	
	for (int i = 0; i < row2; i++){
	data[i] = malloc(col2*sizeof(double*));
		for (int j = 0; j <= col2; j++){
			if (j == 0){
				data[i][0] = 1;
			}
		}
	}
	
	for (int i = 0; i < row2; i++){
		for (int j = 1; j < col2; j++){
			fscanf(temp, "%lf\n", &data[i][j]);
		}
	}
	
	//Close file pointer
	fclose(temp);
	
	//CHECK
	/*for (int i = 0; i < row; i ++){
		for (int j = 0; j < col; j++){
			printf("%5f ", training[i][j]);
		}
		printf("\n");
	}
	
	printf("\n");
	
	for (int i = 0; i < row; i ++){
		printf("%5f ", price[i][0]);
		printf("\n");
	}
	
	printf("\n");
	
	for (int i = 0; i < row2; i ++){
		for (int j = 0; j < col2; j++){
			printf("%5f ", data[i][j]);
		}
		printf("\n");
	}
	
	printf("\n");*/
	
	double **transpose = callTranspose(training, row, col);				//X^T

	double **multiplied = callMultiplication(transpose, training, row, col);	//(X^T*X)

	double **inverse = callInverse(multiplied, col);				//(X^T*X)^-1

	double **transPrice = calltransMul(transpose, price, col, row);			//X^T*Y	

	double **weights = callfinalMul(inverse, transPrice, col);			//(X^T*X)^-1*X^T*Y

	double **finalPrice = callPrice(data, weights, row2, col);			//Y


	for (int i = 0; i < row2; i ++){
		for (int j = 0; j < 1; j++){
			printf("%.0f", finalPrice[i][j]);
		}
		printf("\n");
	}

	freeMatrix(transpose, col);
	freeMatrix(multiplied, col);
	freeMatrix(inverse, col);
	freeMatrix(transPrice, col);
	freeMatrix(weights, col);
	freeMatrix(finalPrice, row2);
	
	freeMatrix(price, row);
	freeMatrix(training, row);
	freeMatrix(data, row2);
	
	return 0;
	
}

