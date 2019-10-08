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

int matrix(int rowOne, int colOne, int rowTwo, int colTwo, int numRows, int numCols, int par, int gran, FILE * fp) {
    //row/col one is matrix A
    //row/col two is matrix B
    //numrows/cols is dram array size
    //all matrices stored row-major
    //each row fits at least one element for non-parallel
    //par is number of parallel arrays
    //if transpose = 1, tranpose matrix B,
        /*for n = 0 to N - 2
        for m = n + 1 to N - 1
            swap A(n,m) with A(m,n)*/
    int oflow1 = 0, oflow2 = 0, oflow3 = 0;
    if(par == 0) {//single array, no transpose, worst case (one read per element matrix b)
        //matrix A
        for(int i = 0; i < rowOne; i++) {
            for(int k = 0; k < colOne; k++) {
                //oflow counts # of bits read, each iteration of loop is 1 element = 64 bits 
                if(oflow1 < 64) {
                    read(0, ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows), 0, fp);
                    //printf("read A %d %d\n", ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows));
                    oflow1 = oflow1 + (numCols);
                }
                oflow1 = oflow1 - 64;
                while(oflow1 < 0) {
                    read(0, ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows), 0, fp);
                    //printf("read A %d %d\n", ((rowOne*i + k)/numRows), ((rowOne*i + k)%numRows));
                    oflow1 = oflow1 + (numCols);
                }
            }
        }
        //matrix B
        if((64*rowTwo) <= numCols) {
            for(int j = 0; j < rowTwo; j++) {
                for(int h = 0; h < 64; h++) {
                    read(1, j, h, 0, fp); //number of reads is accurate, locations aren't
                    //printf("read B %d %d\n", j, h);
                }
            }
        }
        else {
            for(int n = 0; n < numRows; n++) {
                read(1, n, 0, 0, fp);
                //printf("read B %d %d\n", n, 0);
            }
        }
        //matrix C
        for(int i = 0; i < rowOne; i++) {
            for(int j = 0; j < colTwo; j++) {
                if(oflow3 < 64) {
                    write(2, ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows), 0, fp);
                    //printf("write C %d %d\n", ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows));
                    oflow3 = oflow3 + (numCols);
                }
                oflow3 = oflow3 - 64;
                while(oflow3 < 0) {
                    write(2, ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows), 0, fp);
                    //printf("write C %d %d\n", ((rowOne*i + j)/numRows), ((rowOne*i + j)%numRows));
                    oflow3 = oflow3 + (numCols);
                }
            } 
        }
    }
    else {//par parallel arrays, current assumption 64 for matrix B
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
        for(int j = 0; j < rowTwo; j++) {
            for(int k = 0; k < colOne; k++) {
                //printf("read B %d %d\n", j, k);
                read(1, j, k, 0, fp);
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

    FILE * fp;
    fp = fopen("dramtrace.bin", "wb");
    matrix(rowOne, colOne, rowTwo, colTwo, numRows, numCols, par, gran, fp);
    fclose(fp);
    return 1;
}

