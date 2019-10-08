#include "crosspoint.h"

//add a read to trace
int read(unsigned long long bank, int row, int col, int type, FILE * fp) {
	//type = 0 -> row, type = 1 -> col, type = 2 -> cell, type = 3 -> element
	unsigned long long bytes_write;

    unsigned long long rowTwo = 0;
    unsigned long long colTwo = 0;
    unsigned long long req = 0;
    unsigned long long sel = type;
    bytes_write = (sel<<62) | (req<<59) | (bank<<50) | (rowTwo<<35) | (colTwo<<25) | (row<<10) | (col);
    fwrite(&bytes_write, sizeof(unsigned long long), 1, fp);
    return 1;
}

//add a write to trace
int write(unsigned long long bank, int row, int col, int type, FILE * fp) {
	//type = 0 -> row, type = 1 -> col, type = 2 -> cell
	unsigned long long bytes_write;
  	unsigned int data[DATA_SIZE];
  	for(int iter = 0; iter < DATA_SIZE; iter++){
      data[iter] = 0xFFFFFFF0 + iter;
    }

    unsigned long long rowTwo = 0;
    unsigned long long colTwo = 0;
    unsigned long long req = 1;
    unsigned long long sel = type;
    bytes_write = (sel<<62) | (req<<59) | (bank<<50) | (rowTwo<<35) | (colTwo<<25) | (row<<10) | (col);
    fwrite(&bytes_write, sizeof(unsigned long long), 1, fp);
    fwrite(&data[0], sizeof(unsigned int), 16, fp);
    return 1;
}

/*
assumptions:
ixk X kxj = ixj, colOne == rowTwo required
matrix A = bank(subarray) 0, B = bank 1, product matrix = bank 2
matrix A and C are row-major, B is col-major
matrices fit into the subarrays
subarray sizes as specified in crosspoint.h
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
for (i=0; i<n; i++) {
	read row i
	for (j=0; j<n; j++) {
		read col j
 		for (k=0; k<n; k++)
 			sum += a[i][k] * b[k][j];
 		c[i][j] = sum;
	}
	write row i
} 
*/
int matrix(int rowOne, int colOne, int rowTwo, int colTwo, int numRows, int numCols, int par, int gran, FILE * fp) {
	/*for(int i = 0; i < rowOne; i++) {
		for(int j = 0; j < colTwo; j++) {
			for(int k = 0; k < colOne; k++) {
				printf("read %d %d\n", ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows));
				printf("read %d %d\n", ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows));
				read(0, ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows), fp);
				read(1, ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows), fp);
			}
			printf("write %d %d\n", ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows));
			write(2, ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows), fp);
		}
	}*/

	int oflow1 = 0, oflow2 = 0, oflow3 = 0;
	if((par != 0)&&(gran == 0)){ //row/col operations in parallel
		/*for(int i = 0; i < rowOne; i++) {
			//printf("i = %d, oflow1 = %d\n", i, oflow1);
			if(oflow1 < colOne) {
				read(0, i, 0, 0, fp); //read row i in matrix a
				oflow1 = oflow1 + (par*numCols);
				//printf("read A, oflow1 = %d\n", oflow1);
			}
			oflow1 = oflow1 - (colOne*64);
			while(oflow1 < 0) {
				read(0, i, 0, 0, fp); //issue extra read if necessary (row size < matrix row size)
				oflow1 = oflow1 + (par*numCols);
				//printf("read A, oflow1 = %d\n", oflow1);
			}
		}
		for(int j = 0; j < colTwo; j++) {
			//printf("j = %d, oflow2 = %d\n", j, oflow2);
			if(oflow2 < rowTwo){
				read(1, 0, j, 1, fp); //read col j in matrix b
				oflow2 = oflow2 + (par*numRows);
				//printf("read B, oflow2 = %d\n", oflow2);
			}
			oflow2 = oflow2 - (rowTwo*64);
			while(oflow2 < 0){
				read(1, 0, j, 1, fp); //issue extra read if necessary (col size < matrix col size)
				oflow2 = oflow2 + (par*numRows);
				//printf("read B, oflow2 = %d\n", oflow2);
			}
		}
		for(int k = 0; k < rowOne; k++) {
			if(oflow3 < colTwo){
				write(2, k, 0, 0, fp); //write row k in matrix c
				oflow3 = oflow3 + (par*numCols);
				//printf("write C, oflow3 = %d\n", oflow3);
			}
			oflow3 = oflow3 - (colTwo*64);
			while(oflow3 < 0){
				write(2, k, 0, 0, fp); //issue extra write if necessary (row size < matrix row size)
				oflow3 = oflow3 + (par*numCols);
				//printf("write C, oflow3 = %d\n", oflow3);
			}
		}*/

		//matrix A
		for(int i = 0; i < rowOne; i++) {
			for(int k = 0; k < colOne; k++) {
				if(oflow1 < 64) {
					read(0, ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows), 0, fp);
					//printf("read A %d %d\n", ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows));
					oflow1 = oflow1 + (par * numCols);
				}
				oflow1 = oflow1 - 64;
				while(oflow1 < 0) {
					read(0, ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows), 0, fp);
					//printf("read A %d %d\n", ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows));
					oflow1 = oflow1 + (par * numCols);
				}
			}
		}
		//matrix B
		for(int j = 0; j < colTwo; j++) {
			for(int k = 0; k < colOne; k++) {
				if(oflow2 < 64) {
					read(1, ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows), 1, fp);
					//printf("read B %d %d\n", ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows));
					oflow2 = oflow2 + (par * numRows);
				}
				oflow2 = oflow2 - 64;
				while(oflow2 < 0) {
					read(1, ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows), 1, fp);
					//printf("read B %d %d\n", ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows));
					oflow2 = oflow2 + (par * numRows);
				}
			}
		}
		//matrix C
		for(int i = 0; i < rowOne; i++) {
			for(int j = 0; j < colTwo; j++) {
				if(oflow3 < 64) {
					write(2, ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows), 0, fp);
					//printf("write C %d %d\n", ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows));
					oflow3 = oflow3 + (par * numCols);
				}
				oflow3 = oflow3 - 64;
				while(oflow3 < 0) {
					write(2, ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows), 0, fp);
					//printf("write C %d %d\n", ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows));
					oflow3 = oflow3 + (par * numCols);
				}
			} 
		}
	}
	else if ((par != 0) && (gran == 1)){ //single element operations in parallel
		// for(int i = 0; i < rowOne; i++) {
		// 	for(int k = 0; k < colOne; k++) {
		// 		//printf("read A %d %d\n", ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows));
		// 		read(0, ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows), 2, fp);
		// 	}
		// }

		// for(int j = 0; j < colTwo; j++) {
		// 	for(int k = 0; k < colOne; k++) {
		// 		//printf("read B %d %d\n", ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows));
		// 		read(1, ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows), 2, fp);
		// 	}
		// }

		// for(int i = 0; i < rowOne; i++) {
		// 	for(int j = 0; j < colTwo; j++) {
		// 		//printf("write C %d %d\n", ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows));
		// 		write(2, ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows), 2, fp);
		// 	} 
		// }

		//single element in parallel
		//matrix A
		for(int i = 0; i < rowOne; i++) {
			for(int k = 0; k < colOne; k++) {
				if(oflow1 < 64) {
					read(0, ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows), 2, fp);
					//printf("read A %d %d\n", ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows));
					oflow1 = oflow1 + par;
				}
				oflow1 = oflow1 - 64;
				while(oflow1 < 0) {
					read(0, ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows), 2, fp);
					//printf("read A %d %d\n", ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows));
					oflow1 = oflow1 + par;
				}
			}
		}
		//matrix B
		for(int j = 0; j < colTwo; j++) {
			for(int k = 0; k < colOne; k++) {
				if(oflow2 < 64) {
					read(1, ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows), 2, fp);
					//printf("read B %d %d\n", ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows));
					oflow2 = oflow2 + par;
				}
				oflow2 = oflow2 - 64;
				while(oflow2 < 0) {
					read(1, ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows), 2, fp);
					//printf("read B %d %d\n", ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows));
					oflow2 = oflow2 + par;
				}
			}
		}
		//matrix C
		for(int i = 0; i < rowOne; i++) {
			for(int j = 0; j < colTwo; j++) {
				if(oflow3 < 64) {
					write(2, ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows), 2, fp);
					//printf("write C %d %d\n", ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows));
					oflow3 = oflow3 + par;
				}
				oflow3 = oflow3 - 64;
				while(oflow3 < 0) {
					write(2, ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows), 2, fp);
					//printf("write C %d %d\n", ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows));
					oflow3 = oflow3 + par;
				}
			} 
		}
	}
	else if((par == 0) && (gran == 0)) {//non parallel row/col operations
		//TODO fix this
		for(int i = 0; i < rowOne; i++) {
			//printf("i = %d, oflow1 = %d\n", i, oflow1);
			if(oflow1 < colOne) {
				read(0, i, 0, 0, fp); //read row i in matrix a
				oflow1 = oflow1 + (numCols/64);
				//printf("read A, oflow1 = %d\n", oflow1);
			}
			oflow1 = oflow1 - colOne;
			while(oflow1 < 0) {
				read(0, i, 0, 0, fp); //issue extra read if necessary (row size < matrix row size)
				oflow1 = oflow1 + (numCols/64);
				//printf("extra read A, oflow1 = %d\n", oflow1);
			}
		}
		for(int j = 0; j < colTwo; j++) {
			//printf("j = %d, oflow2 = %d\n", j, oflow2);
			if(oflow2 < rowTwo){
				read(1, 0, j, 1, fp); //read col j in matrix b
				oflow2 = oflow2 + (numRows/64);
				//printf("read B, oflow2 = %d\n", oflow2);
			}
			oflow2 = oflow2 - rowTwo;
			while(oflow2 < 0){
				read(1, 0, j, 1, fp); //issue extra read if necessary (col size < matrix col size)
				oflow2 = oflow2 + (numRows/64);
				//printf("extra read B, oflow2 = %d\n", oflow2);
			}
		}
		for(int k = 0; k < rowOne; k++) {
			if(oflow3 < colTwo){
				write(2, k, 0, 0, fp); //write row k in matrix c
				oflow3 = oflow3 + (numCols/64);
				//printf("write C, oflow3 = %d\n", oflow3);
			}
			oflow3 = oflow3 - colTwo;
			while(oflow3 < 0){
				write(2, k, 0, 0, fp); //issue extra write if necessary (row size < matrix row size)
				oflow3 = oflow3 + (numCols/64);
				//printf("extra write C, oflow3 = %d\n", oflow3);
			}
		}
	}
	else {//par = 0, gran = 1, non parallel single element operations
		for(int i = 0; i < rowOne; i++) {
			for(int k = 0; k < colOne; k++) {
				//printf("read A %d %d\n", ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows));
				read(0, ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows), 3, fp);
			}
		}

		for(int j = 0; j < colTwo; j++) {
			for(int k = 0; k < colOne; k++) {
				//printf("read B %d %d\n", ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows));
				read(1, ((rowTwo*k + j)/numRows), ((rowTwo*k + j)%numRows), 3, fp);
			}
		}

		for(int i = 0; i < rowOne; i++) {
			for(int j = 0; j < colTwo; j++) {
				//printf("write C %d %d\n", ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows));
				write(2, ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows), 3, fp);
			}
		}
	}
	return 1;
}

int main(int argc, char * argv[]) {
	if(argc != 9) {
		cout << "args wrong\n";
		return 0;
	}
	//matrix size arguments, rowone/colone for matrix A, rowtwo/coltwo for matrix B
	int rowOne = atoi(argv[1]);
	int colOne = atoi(argv[2]);
	int rowTwo = atoi(argv[3]);
	int colTwo = atoi(argv[4]);
	int numRows = atoi(argv[5]);
	int numCols = atoi(argv[6]);
	int par = atoi(argv[7]);
	int gran = atoi(argv[8]);
	if(colOne != rowTwo)
		return 0;

	/*float rt = 0.5 * numRows * 2;
  	float ct = 20 * numRows;
  	float lat = (rt*ct/2)*(50000 + rt/3)/(1000000*(50000 + rt));
  	printf("rt %f ct %f lat %f\n", rt, ct, lat);*/

	FILE * fp;
	fp = fopen("mmultrace.bin", "wb");
	matrix(rowOne, colOne, rowTwo, colTwo, numRows, numCols, par, gran, fp);
	fclose(fp);
	return 1;
}

