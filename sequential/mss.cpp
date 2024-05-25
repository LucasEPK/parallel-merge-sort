/*
Trabajo Final Integrador - Versión secuencial de Merge Sort
- Integrantes
    - Moyano, Lucas
    - Yornet de Rosas, Agustín
- Asignatura: Programación Paralela y Distribuida 2024
- Fecha: 16/05/2024
- Etapa: Versión Secuencial y Análisis de Paralelización
- Compilación:
    $ g++ -o [nombre_ejecutable] mss.cpp
    $ g++ -o mss mss.cpp
- Ejecución:
    $ ./mss
*/

#include <stdexcept>
#include <iostream>
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
    
    // Caso recursivo
    int mid = (begin + end) / 2;
    mergeSort(array,begin,mid);
    mergeSort(array,mid + 1, end);
    merge(array,begin,mid,end);
}

void printArray(int array[], int size){
    printf("[ ");
    for (int i=0;i<size;i++){
        printf("%d ",array[i]);
    }
    printf("]\n");
}

int *initRandomNums(int size)
{
    int *randomArray = (int *)malloc(sizeof(int) * size);
    int i;
    for (i = 0; i < size; i++)
    {
        randomArray[i] = ((((i + 1) * 3) % size) + 1) * 2;
    }
    return randomArray;
}

void checkArraySize(int arraySize) {
    // Acá chequeo que no se hayan ingresado array de longitudes impares y una cantidad de nodos no exponencial de 2
    if (arraySize % 2 != 0) {
        throw std::invalid_argument("ERROR: el tamaño del array no es par");
    }
}

int main(int argc, char **argv){

    int sizeInputArray;
    // Define el tamaño del arreglo inicial en función del argumento
    if (argv[1] == NULL)
    {
        sizeInputArray = 16;
    }
    else
    {
        sizeInputArray = atoi(argv[1]);
    }

    // Acá chequeo que no se hayan ingresado array de longitudes impares
    try {
        checkArraySize(sizeInputArray);
    } catch (std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    // Crea un array "random" con el tamaño especificado por el usuario
    int *inputArray;
    inputArray = initRandomNums(sizeInputArray);

    printArray(inputArray,sizeInputArray);
    mergeSort(inputArray,0,sizeInputArray - 1);
    printArray(inputArray,sizeInputArray);

    return 0;
}