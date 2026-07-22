#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//functions for convenience
double** allocateMemory(int m, int n); 
void releaseMemory(double** A, int m);
void printMatrix(double** A, int m, int n, char name[]);

//functions to implement in prj0
//transposeMatrix, normalizeVector, calculateLength
//scaleMatrix, multiplyTwoMatrices, addTwoMatrices
double** transposeMatrix(double** A, int m, int n);
double** normalizeVector(double** v, int n);
double calculateLength(double** v, int n);
double** scaleMatrix(double** A, int m, int n, double c);
double** multiplyTwoMatrices(double** A, int m, int n, double** B, int l, int k);
double** addTwoMatrices(double** A, int m, int n, double** B, int l, int k);

int main() {
    double** A;
    double** B;
    double** v; //vector
    double** w; //vector
    int m = 5;  //number of rows
    int n = 10; //number of columns

    //Test transposeMatrix
    printf("\n1-(a)\n");
    A = allocateMemory(m,n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            A[i][j] = (double) i*j;

    printMatrix(A,m,n,"A");

    B = transposeMatrix(A,m,n);
    printMatrix(B,n,m,"A^T");

    //Test normalizeVector
    printf("\n1-(b)\n");
    v = allocateMemory(m,1);
    for (int i = 0; i < m; i++)
        v[i][0] = i;
    w = normalizeVector(v,m);
    printMatrix(v,m,1,"v");
    printMatrix(w,m,1,"v_normalized");

    //Test calculateLength
    printf("\n1-(c)\n");
    double len = calculateLength(v,m);
    printf("\nlength(v) = %lf\n", len);

    //Test scaleMatrix
    printf("\n1-(d)\n");
    double** M = allocateMemory(2,2);
    M[0][0]=1; M[0][1]=2;
    M[1][0]=3; M[1][1]=4;
    printMatrix(M,2,2,"M");
    double** M2 = scaleMatrix(M,2,2,2.0);
    printMatrix(M2,2,2,"M scaled by 2");

    //Test multiplyTwoMatrices
    printf("\n1-(e)\n");
    double** A2 = allocateMemory(2,3);
    double** B2 = allocateMemory(3,2);
    A2[0][0]=1; A2[0][1]=2; A2[0][2]=3;
    A2[1][0]=4; A2[1][1]=5; A2[1][2]=6;
    B2[0][0]=7; B2[0][1]=8;
    B2[1][0]=9; B2[1][1]=10;
    B2[2][0]=11;B2[2][1]=12;
    printMatrix(A2,2,3,"A2");
    printMatrix(B2,3,2,"B2");
    double** C2 = multiplyTwoMatrices(A2,2,3,B2,3,2);
    if (C2 == NULL) {
    printf("Error: Matrix multiplication impossible!\n");
    } else {
    printMatrix(C2,2,2,"A2*B2");
    }

    //Test addTwoMatrices
    printf("\n1-(f)\n");
    double** X = allocateMemory(2,2);
    double** Y = allocateMemory(2,2);
    X[0][0]=1; X[0][1]=2; X[1][0]=3; X[1][1]=4;
    Y[0][0]=5; Y[0][1]=6; Y[1][0]=7; Y[1][1]=8;
    printMatrix(X,2,2,"X");
    printMatrix(Y,2,2,"Y");
    double** Z = addTwoMatrices(X,2,2,Y,2,2);
    if (Z == NULL) {
    printf("Error: Matrix addition impossible!\n");
    } else {
    printMatrix(Z,2,2,"X+Y");
    }

    printf("\n------------\n");
    printf("\n2-(a) All the functions wrote in 1 are correct.\n");
    printf("\n2-(b)\n");

    //Example Matrix
    int nn = 2;
    double** A_s = allocateMemory(nn,nn);
    double** Htilde = allocateMemory(nn,nn);

    A_s[0][0]=1; A_s[0][1]=2;
    A_s[1][0]=3; A_s[1][1]=4;
    Htilde[0][0]=1; Htilde[0][1]=1;
    Htilde[1][0]=1; Htilde[1][1]=-1;

    printMatrix(A_s,nn,nn,"A");
    printMatrix(Htilde,nn,nn,"Htilde");

    //H(normalized)
    double** H = allocateMemory(nn,nn);
    for (int j=0;j<nn;j++) {
        double** col = allocateMemory(nn,1);
        for (int i=0;i<nn;i++) col[i][0]=Htilde[i][j];
        double** col_norm = normalizeVector(col,nn);
        for (int i=0;i<nn;i++) H[i][j]=col_norm[i][0];
        releaseMemory(col,nn);
        releaseMemory(col_norm,nn);
    }
    printMatrix(H,nn,nn,"H(normalized)");

    //B = H^T * A * H
    double** Ht = transposeMatrix(H,nn,nn);
    double** temp1 = multiplyTwoMatrices(Ht,nn,nn,A_s,nn,nn);
    double** B_s = multiplyTwoMatrices(temp1,nn,nn,H,nn,nn);
    printMatrix(B_s,nn,nn,"B=H^T*A*H");

    //C = H * B * H^T
    double** temp2 = multiplyTwoMatrices(H,nn,nn,B_s,nn,nn);
    double** C_s = multiplyTwoMatrices(temp2,nn,nn,Ht,nn,nn);
    printMatrix(C_s,nn,nn,"C=H*B*H^T");

    //release memory
    releaseMemory(A,m);
    releaseMemory(B,n);
    releaseMemory(v,m);
    releaseMemory(w,m);
    releaseMemory(M,2);
    releaseMemory(M2,2);
    releaseMemory(A2,2);
    releaseMemory(B2,3);
    releaseMemory(C2,2);
    releaseMemory(X,2);
    releaseMemory(Y,2);
    releaseMemory(Z,2);
    releaseMemory(A_s,nn);
    releaseMemory(Htilde,nn);
    releaseMemory(H,nn);
    releaseMemory(Ht,nn);
    releaseMemory(temp1,nn);
    releaseMemory(B_s,nn);
    releaseMemory(temp2,nn);
    releaseMemory(C_s,nn);

    return 0;
}

//functions for convenience
double** allocateMemory(int m, int n) {
    double** A;
    A = (double**) malloc(sizeof(double*) * m);
    for (int i=0;i<m;i++)
        A[i] = (double*) malloc(sizeof(double) * n);

    return A;
}

void releaseMemory(double** A, int m) {
    for (int i=0;i<m;i++)
        free(A[i]);
    free(A);
}

void printMatrix(double** A, int m, int n, char name[]) {
    printf("\n%s = \n", name);
    for (int i=0;i<m;i++) {
        for (int j=0;j<n;j++)
            printf("%10.6f ", A[i][j]);
        printf("\n");
    }
}

//functions to implement in prj0 
double** transposeMatrix(double **A, int m, int n) {
    double** B = allocateMemory(n,m);

    for (int i=0;i<m;i++)
        for (int j=0;j<n;j++)
            B[j][i] = A[i][j];

    return B;
}

double** normalizeVector(double** v, int n) {
    double len = calculateLength(v,n);
    double** w = allocateMemory(n,1);
    for (int i=0;i<n;i++)
        w[i][0] = v[i][0]/len;

    return w;
}

double calculateLength(double** v, int n) {
    double len=0.0;
    for (int i=0;i<n;i++)
        len += v[i][0]*v[i][0];

    return sqrt(len);
}

double** scaleMatrix(double** A, int m, int n, double c) {
    double** B = allocateMemory(m,n);
    for (int i=0;i<m;i++)
        for (int j=0;j<n;j++)
            B[i][j] = c*A[i][j];

    return B;
}

double** multiplyTwoMatrices(double** A, int m, int n, double** B, int l, int k) {
    if (n!=l) return NULL;
    double** C = allocateMemory(m,k);
    for (int i=0;i<m;i++) {
        for (int j=0;j<k;j++) {
            double sum=0.0;
            for (int t=0;t<n;t++)
                sum += A[i][t]*B[t][j];
            C[i][j]=sum;
        }
    }

    return C;
}

double** addTwoMatrices(double** A, int m, int n, double** B, int l, int k) {
    if (m!=l || n!=k) return NULL;
    double** C = allocateMemory(m,n);
    for (int i=0;i<m;i++)
        for (int j=0;j<n;j++)
            C[i][j] = A[i][j]+B[i][j];

    return C;
}
