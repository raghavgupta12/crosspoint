#include "crosspoint.h"

using namespace std;

//add a read to trace
int read(unsigned long long bank, int row, int col, FILE * fp) {
	unsigned long long bytes_write;

    unsigned long long rowTwo = 0;
    unsigned long long colTwo = 0;
    unsigned long long req = 0;
    unsigned long long sel = 0;
    bytes_write = (sel<<62) | (req<<59) | (bank<<50) | (rowTwo<<35) | (colTwo<<25) | (row<<10) | (col);
    fwrite(&bytes_write, sizeof(unsigned long long), 1, fp);
    return 1;
}

//add a write to trace
int write(unsigned long long bank, int row, int col, FILE * fp) {
	unsigned long long bytes_write;
  	unsigned int data[DATA_SIZE];
  	for(int iter = 0; iter < DATA_SIZE; iter++){
      data[iter] = 0xFFFFFFF0 + iter;
    }

    unsigned long long rowTwo = 0;
    unsigned long long colTwo = 0;
    unsigned long long req = 1;
    unsigned long long sel = 0;
    bytes_write = (sel<<62) | (req<<59) | (bank<<50) | (rowTwo<<35) | (colTwo<<25) | (row<<10) | (col);
    fwrite(&bytes_write, sizeof(unsigned long long), 1, fp);
    fwrite(&data[0], sizeof(unsigned int), 16, fp);
    return 1;
}

/*
assumptions:
ixk X kxj = ixj, colOne == rowTwo required
matrix 1 = bank(subarray) 0, 2 = bank 1, product matrix = bank 2
rows and cols in matrix mapped directly to rows and cols in subarrays
write data doesn't mater so its just 0xffffffff
algo:
for (i=0; i<n; i++) {
	for (j=0; j<n; j++) {
 	sum = 0.0;
 		for (k=0; k<n; k++)
 		sum += a[i][k] * b[k][j];
 	c[i][j] = sum;
	}
} 
TODO: fix layout of matrix in memory, change reads to pull entire row/col in parallel
*/
int matrix(int rowOne, int colOne, int rowTwo, int colTwo, FILE * fp) {
	for(int i = 0; i < rowOne; i++) {
		for(int j = 0; j < colTwo; j++) {
			for(int k = 0; k < colOne; k++) {
				printf("read %x %x\n", ((rowOne*i + k)/NUM_ROWS), ((rowOne*i + k)%NUM_ROWS));
				printf("read %x %x\n", ((rowTwo*k + j)/NUM_ROWS), ((rowTwo*k + j)%NUM_ROWS));
				read(0, ((rowOne*i + k)/NUM_ROWS), ((rowOne*i + k)%NUM_ROWS), fp);
				read(1, ((rowTwo*k + j)/NUM_ROWS), ((rowTwo*k + j)%NUM_ROWS), fp);
			}
			printf("write %x %x\n", ((rowOne*i + j)/NUM_ROWS), ((rowOne*i + j)%NUM_ROWS));
			write(2, ((rowOne*i + j)/NUM_ROWS), ((rowOne*i + j)%NUM_ROWS), fp);
		}
	}
	return 1;
}

int main() {
	FILE * fp;
	fp = fopen("mmultrace.bin", "wb");
	matrix(2, 2, 2, 2, fp);
	fclose(fp);
	return 1;
}