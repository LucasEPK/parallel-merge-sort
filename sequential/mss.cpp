#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <array>

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
    
    // Caso recursivoo
    int mid = (begin + end) / 2;
    mergeSort(array,begin,mid);
    mergeSort(array,mid + 1, end);
    merge(array,begin,mid,end);
}

void printArray(int array[], int size){
    
    for (int i=0;i<size;i++){
        printf("%d -> ",array[i]);
    }
    printf("\n");
}

int main(){

    int inputArray[] = {6};
    int sizeInputArray = sizeof(inputArray) / sizeof(inputArray[0]); // 20bytes:4bytes = 5

    printArray(inputArray,sizeInputArray);
    mergeSort(inputArray,0,sizeInputArray - 1);
    printArray(inputArray,sizeInputArray);

    return 0;
}