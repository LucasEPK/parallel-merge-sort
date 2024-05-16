#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <array>
#include <mpi.h>

void merge(int array[], int begin, int mid, int end){
    // Obtengo tamaño de subarreglos izq y der
    int sizeLeftArray = mid - begin + 1;
    int sizeRightArray = end - mid;

    int leftArray[sizeLeftArray];
    int rightArray[sizeRightArray];

    // Copio el contenido del arreglo en el subarreglo izq 
    for (int i=0;i<sizeLeftArray;i++){
        leftArray[i] = array[begin + i];
    }

    // Copio el contenido del arreglo en el subarreglo der 
    for (int j=0;j<sizeRightArray;j++){
        rightArray[j] = array[mid + 1 + j];
    }

    int leftIndex = 0; int rightIndex = 0; int arrayIndex = begin;
    int actualLeftNum; int actualRightNum;

    while (leftIndex < sizeLeftArray && rightIndex < sizeRightArray) {
        actualLeftNum = leftArray[leftIndex];
        actualRightNum = rightArray[rightIndex];
        if (actualLeftNum <= actualRightNum) {
            array[arrayIndex] = actualLeftNum;
            leftIndex++;
        } else {
            array[arrayIndex] = actualRightNum;
            rightIndex++;
        }
        arrayIndex++;
    }

    if (leftIndex >= sizeLeftArray) {
        while (rightIndex < sizeRightArray)
        {
            array[arrayIndex] = rightArray[rightIndex];
            rightIndex++; arrayIndex++;
        }
    } else {
        while (leftIndex < sizeLeftArray)
        {
            array[arrayIndex] = leftArray[leftIndex];
            leftIndex++; arrayIndex++;
        }
    }
}

void mergeSort(int array[],int begin, int end){

    // Caso base
    if (begin == end) {
        return;
    }
    
    // Caso recursivo
    int mid = (begin + end) / 2;
    mergeSort(array,begin,mid);
    mergeSort(array,mid + 1, end);
    merge(array,begin,mid,end);
}

int *initRandomNums(int size){
    int *randomArray = (int *)malloc(sizeof(int)* size);
    int i;
    for (i = 0; i < size; i++) {
        randomArray[i] = (((i+1) * 3) % size) + 1;
    }
    return randomArray;
}

void printArray(int array[], int size)
{
    printf("[ ");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", array[i]);
    }
    printf("]\n");
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    int size;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int sizeInputArray = 64;
    int *inputArray = NULL;
    int sendCount;
    int *subArray;

    if (rank == 0)
    {
        inputArray = initRandomNums(sizeInputArray);
        printf("El arreglo global es: "); printArray(inputArray,sizeInputArray);
    }

    sendCount = sizeInputArray / size;
    subArray = (int *)malloc(sizeof(int)*sendCount);

    MPI_Scatter(inputArray, sendCount, MPI_INT, subArray, sendCount, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Soy el proceso %d y recibí el array: ", rank);
    printArray(subArray, sendCount);

    MPI_Barrier(MPI_COMM_WORLD); // Para debugging
    
    mergeSort(subArray,0,sendCount - 1);
    printf("Soy el proceso %d y ordené el array: ", rank);
    printArray(subArray, sendCount);

    MPI_Finalize();
    return 0;
}