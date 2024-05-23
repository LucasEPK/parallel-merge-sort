/*
Trabajo Final Integrador - Versión Paralela de Merge Sort
- Integrantes
    - Moyano, Lucas
    - Yornet de Rosas, Agustín
- Asignatura: Programación Paralela y Distribuida 2024
- Fecha: 23/05/2024
- Etapa: Diseño y Desarrollo de la Versión Paralelo/Distribuida
- Compilación:
    $ mpic++ -o msp.out msp.cpp
- Ejecución:
    $ mpirun -np 4 ./msp.out
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <array>
#include <math.h>
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
        randomArray[i] = ((((i+1) * 3) % size) + 1)*2;
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

void concatenateArrays(int array1[], int array2[], int arrayResult[], int size) {

    for(int i=0; i<size; i++){
        arrayResult[i] = array1[i];
        arrayResult[size + i] = array2[i];
    }
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

    int totalLevels = round(log2(size));
    int actualLevel = 0;
    bool isFinished = false;

    int actualSubarraySize = sendCount;
    int *arrayBuffer;
    int rankSource;
    int rankDest;
    int *arrayResult;

    while( !isFinished){
        if(rank%(int)pow(2, actualLevel+1) == 0){ // Nodo que recibe el array del otro proceso
            arrayBuffer =  (int *)malloc(actualSubarraySize * sizeof(int));// Acá asignamos el tamaño al array
            rankSource = rank+(int)pow(2, actualLevel);
            MPI_Recv(arrayBuffer, actualSubarraySize, MPI_INT, rankSource, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("soy el rango %d y recibo el array : \n", rank);
            printArray(arrayBuffer, actualSubarraySize);

            // Ahora concatenamos el array recibido y el subarray que ordenamos
            arrayResult = (int *)malloc(actualSubarraySize*2*sizeof(int));
            concatenateArrays(subArray, arrayBuffer, arrayResult, actualSubarraySize); // concatenamos los primeros dos arrays

            merge(arrayResult, 0, actualSubarraySize-1 ,actualSubarraySize*2-1);
            printf("arrayResult en el rank %d: \n", rank);
            printArray(arrayResult, actualSubarraySize*2);
            subArray = (int *)malloc(actualSubarraySize*2*sizeof(int));
            subArray = arrayResult;
            printf("subArray en el rank %d: \n", rank);
            printArray(subArray, actualSubarraySize*2);
        } else { // Nodo que envia el array al otro proceso
            rankDest = rank - (int)pow(2, actualLevel);
            MPI_Send(subArray, actualSubarraySize, MPI_INT,rankDest, 0, MPI_COMM_WORLD);
            isFinished = true;
        }



        if(actualLevel+1 == totalLevels){
            isFinished = true;
        }
        actualLevel+=1;
        actualSubarraySize *= 2; // tamaño del buffer actual
    }

    if(rank==0){
        printf("El resultado final es:\n");
        printArray(subArray, actualSubarraySize);
    }

    MPI_Finalize();
    return 0;
}