#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include "MATRIX_METHODS.h" 

typedef unsigned char BYTE;

BYTE* loadBitmapFile(int bytesPerPixel, BYTE* header, int* imgWidth, int*imgHeight, char* filename);
void writeBitmapFile(int bytesPerPixel, BYTE* header, BYTE* output, int imgSize, char* filename);

double** constructHaarMatrixRecursive(int n);
double** concatenateTwoMatrices(double** hl, double** hr, int n);
double** applyKroneckerProduct(double** A, int n, double a, double b);
double** allocateMemory(int m, int n);
void releaseMemory(double** A, int m);
double** constructIdentity(int k);
double** transposeMatrix(double** A, int n);
double** multiplyMatrix(double** A, double** B, int n);
void normalizeColumns(double** A, int n);

int main() {
	/*******************************************************************/
	/*************************** Read image  ***************************/
	/*******************************************************************/
	int imgSize, imgWidth, imgHeight;
	int bytesPerPixel = 3;			//number of bytes per pixel (1 byte for R,G,B respectively)
	BYTE* header = (BYTE*)malloc(54*sizeof(BYTE));

	BYTE* image = loadBitmapFile(bytesPerPixel, header, &imgWidth, &imgHeight, "./input_image_bee_24bit.bmp");
	if (image == NULL) {
		printf("No image has been loaded\n");
		free(header);
		return 0;
	}

	imgSize = imgWidth * imgHeight; // total number of pixels

	/*******************************************************************/
	/************************ Perform HWT/IHWT *************************/
	/*******************************************************************/
	double** A; //original image matrix
	A = (double**)malloc(sizeof(double*) * imgHeight);
	for (int i = 0; i < imgHeight; i++) {
		A[i] = (double*)malloc(sizeof(double) * imgWidth);
	}

    // 1.a 이미지를 읽어서 행렬 A 구성
	for (int i = 0; i < imgHeight; i++)
		for (int j = 0; j < imgWidth; j++)
			A[i][j] = image[(i * imgWidth + j) * bytesPerPixel];

	int n = imgHeight; //이미지가 정사각형(Height==Width)이라고 가정; n = 2^t,t=0,1,2,...
	
    // 1.b n-point Haar matrix H 구성 (Normalized)
    double** H = constructHaarMatrixRecursive(n);
    normalizeColumns(H, n); // H^(-1) = H^T 가 되도록 열 정규화 수행

	//DHWT 수행: 행렬곱 B = H'*A*H
    // 1.c 2-D DHWT 수행
    double** HT = transposeMatrix(H, n);    // H^T
    double** Temp = multiplyMatrix(HT, A, n); // Temp = H^T * A
    double** B = multiplyMatrix(Temp, H, n);  // B = (H^T * A) * H
    
    releaseMemory(Temp, n); // 중간 변수 메모리 해제

	int k = 0;
	printf("Enter the value of k : ");
	scanf("%d", &k);
	if (k > n) {
		printf("The value of k(=%d) can't exceed n(=%d)\n", k, n);
        releaseMemory(A, n); releaseMemory(H, n); releaseMemory(HT, n); releaseMemory(B, n);
        free(image); free(header);
		return 0;
	}
	char fname_out[25], s2[10]; // Buffer size increased for safety
	strcpy(fname_out, "output-");
	sprintf(s2, "%d", k);
	strcat(fname_out, s2);
	strcat(fname_out, ".bmp");

	//행렬 B 자르기: B의 upper left corner(subsquare kxk matrix)를 잘라 Bhat에 저장
	//Bhat은 B와 사이즈가 같으며 B에서 잘라서 저장한 부분 외에는 모두 0으로 채워짐
	
    // 1.d Bhat 구성 (k x k 부분만 남기고 나머지는 0)
    double** Bhat = allocateMemory(n, n);
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            if(i < k && j < k) {
                Bhat[i][j] = B[i][j];
            } else {
                Bhat[i][j] = 0.0;
            }
        }
    }
	
	//IDHWT 수행: Ahat = H*Bhat*H'
    // 1.e Inverse DHWT 수행
    double** Temp2 = multiplyMatrix(H, Bhat, n); // Temp2 = H * Bhat
    double** Ahat = multiplyMatrix(Temp2, HT, n); // Ahat = (H * Bhat) * H^T
    
    releaseMemory(Temp2, n);

	/*******************************************************************/
	/******************* Write reconstructed image  ********************/
	/*******************************************************************/
	//Ahat = [a b;c d]  Are=[a a a b b b c c c d d d]
	//Ahat을 이용해서 위의 image와 같은 형식이 되도록 구성 (즉, Ahat = [a b;c d]면 [a a a b b b c c c d d d]를 만들어야 함)
	BYTE* Are = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSize);
	
    // 1.f 복원된 행렬 Ahat을 이미지 파일 포맷(Are)으로 변환
    for (int i = 0; i < imgHeight; i++) {
        for (int j = 0; j < imgWidth; j++) {
            double val = Ahat[i][j];
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            
            int pixelIndex = (i * imgWidth + j) * bytesPerPixel;
            Are[pixelIndex] = (BYTE)val;     // B
            Are[pixelIndex + 1] = (BYTE)val; // G
            Are[pixelIndex + 2] = (BYTE)val; // R
        }
    }

	writeBitmapFile(bytesPerPixel, header, Are, imgSize, fname_out);

	free(image);
    free(header);
	//free(output); 
	releaseMemory(A, n);
	releaseMemory(H, n);
	releaseMemory(HT, n);
	releaseMemory(B, n);
    releaseMemory(Bhat, n);
	releaseMemory(Ahat, n);
	free(Are);

	return 0;

}


BYTE* loadBitmapFile(int bytesPerPixel, BYTE* header, int* imgWidth, int* imgHeight, char* filename) {
	FILE* fp = fopen(filename, "rb");
	if (fp == NULL)	{
		return NULL;
	}
	else {
    	// read the 54-byte header
    	fread(header, sizeof(BYTE), 54, fp);

		// header size
		int hSize = *(int*)&header[14];
		printf("Size of header:%d\n", hSize);

    	// extract image height and width from header
    	*imgWidth = *(int*)&header[18];
    	*imgHeight = *(int*)&header[22];
		printf("W:%d H:%d\n", *imgWidth, *imgHeight);
		

		int imgSizeTemp = (*imgWidth) * (*imgHeight); //이미지 사이즈를 상위 변수에 할당

		BYTE* image = (BYTE*)malloc(bytesPerPixel * sizeof(BYTE) * imgSizeTemp); //이미지크기만큼 메모리할당

		fread(image, bytesPerPixel*sizeof(BYTE), imgSizeTemp, fp); //이미지 크기만큼 파일에서 읽어오기

		fclose(fp);
		return image;
	}
}



void writeBitmapFile(int bytesPerPixel, BYTE* header, BYTE* output, int imgSize, char* filename) {
	FILE* fp = fopen(filename, "wb");

	fwrite(header, sizeof(BYTE), 54, fp);
	fwrite(output, bytesPerPixel*sizeof(BYTE), imgSize, fp);
	fclose(fp);
}

double** allocateMemory(int m, int n) {
	double** A;
	A = (double**) malloc(sizeof(double*) * m);
	for (int i = 0; i < m; i++) {
		A[i] = (double*) malloc(sizeof(double) * n);
	}
	return A;
}

void releaseMemory(double** A, int m) {
	for (int i = 0; i < m; i++)
		free(A[i]);
	free(A);
}

double** constructIdentity(int k) {
 	double** I = allocateMemory(k,k);
 	for (int i = 0; i < k; i++) {
 		for (int j = 0; j < k; j++) {
 			if (i != j)
 				I[i][j] = 0.0;
 			else
 				I[i][j] = 1.0;
 		}
 	}
 	return I;
}

double** concatenateTwoMatrices(double** hl, double** hr, int n) {
	double** H = allocateMemory(n, n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (j < n/2)
				H[i][j] = hl[i][j];
			else
				H[i][j] = hr[i][j-n/2];
		}
	}
	return H; 
}

double** applyKroneckerProduct(double** A, int n, double a, double b) {
	double** h = allocateMemory(n, n/2);
	for (int j = 0; j < n/2; j++) {
		for (int i = 0; i < n/2; i++) {
			h[2*i][j] = A[i][j] * a;
			h[2*i+1][j] = A[i][j] * b;
		}
	}
	return h;
}

double** constructHaarMatrixRecursive(int n) {
	double** h;
	if (n > 2)
		h = constructHaarMatrixRecursive(n/2);
	else {
		h = allocateMemory(2,2);
		h[0][0] = 1; h[0][1] = 1; h[1][0] = 1; h[1][1] = -1; 
		return h; 
	}

	// construct the left half (Kronecker product of h and [1;1])
	double** Hl = applyKroneckerProduct(h, n, 1, 1);
	releaseMemory(h, n/2);

	// construct the right half (Kronecker product of I and [1;-1])
	double** I = constructIdentity(n/2);
	double** Hr = applyKroneckerProduct(I, n, 1, -1); 
	releaseMemory(I, n/2);

	// merge hl and hr
	double** H = concatenateTwoMatrices(Hl, Hr, n); //H = [Hl Hr]
	releaseMemory(Hl, n);
	releaseMemory(Hr, n);

	return H;
}

void normalizeColumns(double** A, int n) {
    for (int j = 0; j < n; j++) {
        double sum = 0.0;
        for (int i = 0; i < n; i++) {
            sum += A[i][j] * A[i][j];
        }
        double norm = sqrt(sum);
        if (norm > 1e-9) {
            for (int i = 0; i < n; i++) {
                A[i][j] /= norm;
            }
        }
    }
}

double** transposeMatrix(double** A, int n) {
    double** T = allocateMemory(n, n);
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            T[i][j] = A[j][i];
        }
    }
    return T;
}

double** multiplyMatrix(double** A, double** B, int n) {
    double** C = allocateMemory(n, n);
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            C[i][j] = 0.0;
            for(int k=0; k<n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}