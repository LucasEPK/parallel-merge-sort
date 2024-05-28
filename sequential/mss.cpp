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
#include <iomanip>

void merge(int array[], int begin, int mid, int end)
{
    // Obtengo tamaño de subarreglos izq y der
    int sizeLeftArray = mid - begin + 1;
    int sizeRightArray = end - mid;

    int *leftArray = new int[sizeLeftArray];
    int *rightArray = new int[sizeRightArray];

    // Copio el contenido del arreglo en el subarreglo izq
    for (int i = 0; i < sizeLeftArray; i++)
    {
        leftArray[i] = array[begin + i];
    }

    // Copio el contenido del arreglo en el subarreglo der
    for (int j = 0; j < sizeRightArray; j++)
    {
        rightArray[j] = array[mid + 1 + j];
    }

    int leftIndex = 0;
    int rightIndex = 0;
    int arrayIndex = begin;
    int actualLeftNum;
    int actualRightNum;

    while (leftIndex < sizeLeftArray && rightIndex < sizeRightArray)
    {
        actualLeftNum = leftArray[leftIndex];
        actualRightNum = rightArray[rightIndex];
        if (actualLeftNum <= actualRightNum)
        {
            array[arrayIndex] = actualLeftNum;
            leftIndex++;
        }
        else
        {
            array[arrayIndex] = actualRightNum;
            rightIndex++;
        }
        arrayIndex++;
    }

    if (leftIndex >= sizeLeftArray)
    {
        while (rightIndex < sizeRightArray)
        {
            array[arrayIndex] = rightArray[rightIndex];
            rightIndex++;
            arrayIndex++;
        }
    }
    else
    {
        while (leftIndex < sizeLeftArray)
        {
            array[arrayIndex] = leftArray[leftIndex];
            leftIndex++;
            arrayIndex++;
        }
    }

    delete[] leftArray;
    delete[] rightArray;
}

void mergeSort(int array[], int begin, int end)
{

    // Caso base
    if (begin == end)
    {
        return;
    }

    // Caso recursivo
    int mid = (begin + end) / 2;
    mergeSort(array, begin, mid);
    mergeSort(array, mid + 1, end);
    merge(array, begin, mid, end);
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

void checkArraySize(int arraySize)
{
    // Acá chequeo que no se hayan ingresado array de longitudes impares y una cantidad de nodos no exponencial de 2
    if (arraySize % 2 != 0)
    {
        throw std::invalid_argument("ERROR: el tamaño del array no es par");
    }
}

void checkIfOrdered(int array[], int size)
{
    int ordered = 1;

    for (int i = 1; i < size; i++)
    {
        if (array[i] < array[i - 1])
        {
            ordered = 0;
            break;
        }
    }

    if (ordered)
    {
        printf("Arreglo ordenado: VERDADERO\n");
    }
    else
    {
        printf("Arreglo ordenado: FALSO\n");
    }
}

int main(int argc, char **argv)
{
    clock_t start, end;

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
    try
    {
        checkArraySize(sizeInputArray);
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    // Crea un array "random" con el tamaño especificado por el usuario
    int *inputArray;
    inputArray = initRandomNums(sizeInputArray);

    int print = (atoi(argv[2]) == 1);
    if (print)
    {
        printArray(inputArray, sizeInputArray);
        checkIfOrdered(inputArray, sizeInputArray);
    }
    start = clock();
    mergeSort(inputArray, 0, sizeInputArray - 1);
    end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    std::cout << "Tiempo de ejecución: " << std::fixed << std::setprecision(2) << time_taken ;
    std::cout << "s" << std::endl;
    if (print)
    {
        printArray(inputArray, sizeInputArray);
    }
    checkIfOrdered(inputArray,sizeInputArray);

    return 0;
}