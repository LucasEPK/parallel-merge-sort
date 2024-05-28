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
#include <stdexcept>
#include <iostream>

void merge(int array[], int begin, int mid, int end)
{
    // Obtengo tamaño de subarreglos izq y der
    int sizeLeftArray = mid - begin + 1;
    int sizeRightArray = end - mid;

    int leftArray[sizeLeftArray];
    int rightArray[sizeRightArray];

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

void printArray(int array[], int size)
{
    printf("[ ");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", array[i]);
    }
    printf("]\n");
}

void concatenateArrays(int array1[], int array2[], int arrayResult[], int size)
{

    for (int i = 0; i < size; i++)
    {
        arrayResult[i] = array1[i];
        arrayResult[size + i] = array2[i];
    }
}

void checkNodeCountAndArraySize(int nodeCount, int arraySize) {
    // Acá chequeo que no se hayan ingresado array de longitudes impares y una cantidad de nodos no exponencial de 2
    bool nodesArePowOf2 = trunc(log2((double)nodeCount)) == log2((double)nodeCount);
    if (arraySize % 2 != 0) {
        throw std::invalid_argument("ERROR: el tamaño del array no es par");
    } else if (!nodesArePowOf2) {
        throw std::invalid_argument("ERROR: la cantidad de nodos no es una potencia de 2");
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    int nodeCount;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nodeCount);

    int sizeInputArray;

    // Define el tamaño del arreglo inicial en función del argumento
    if (argv[1] == NULL)
    {
        sizeInputArray = 16 * nodeCount;
    }
    else
    {
        sizeInputArray = atoi(argv[1]);
    }

    // Acá chequeo que no se hayan ingresado array de longitudes impares y una cantidad de nodos no exponencial de 2
    try {
        checkNodeCountAndArraySize(nodeCount, sizeInputArray);
    } catch (std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    // Defino el arreglo inicial
    int *inputArray = NULL;

    int subarraySize; // = tamaño de subarreglos
    int *subArray;

    if (rank == 0)
    {
        // Lleno con valores el arreglo inicial
        inputArray = initRandomNums(sizeInputArray);
        printf("El arreglo global es: ");
        printArray(inputArray, sizeInputArray);
    }

    
    subarraySize = sizeInputArray / nodeCount;
    subArray = (int *)malloc(sizeof(int) * subarraySize);

    MPI_Scatter(inputArray, subarraySize, MPI_INT, subArray, subarraySize, MPI_INT, 0, MPI_COMM_WORLD);

    mergeSort(subArray, 0, subarraySize - 1);

    // Cantidad de iteraciones de la última etapa
    int totalLevels = round(log2(nodeCount));
    int actualLevel = 0;


    int currentSubarraySize = subarraySize;
    int *recvArray;
    int *arrayResult;
    int rankSource;
    int rankDest;

    bool isFinished = false;
    while (!isFinished)
    {
        if (rank % (int)pow(2, actualLevel + 1) == 0)
        {                                                                  // Nodo que recibe el array del otro proceso
            recvArray = (int *)malloc(currentSubarraySize * sizeof(int)); // Acá asignamos el tamaño al array
            rankSource = rank + (int)pow(2, actualLevel);
            MPI_Recv(recvArray, currentSubarraySize, MPI_INT, rankSource, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Ahora concatenamos el array recibido y el subarray que ordenamos
            arrayResult = (int *)malloc(currentSubarraySize * 2 * sizeof(int));
            concatenateArrays(subArray, recvArray, arrayResult, currentSubarraySize); // concatenamos los primeros dos arrays

            merge(arrayResult, 0, currentSubarraySize - 1, currentSubarraySize * 2 - 1);

            subArray = (int *)malloc(currentSubarraySize * 2 * sizeof(int));
            // Se sobreescribe el subarreglo para utilizarlo en la próxima iteración
            subArray = arrayResult;
        }
        else
        { 
            // Nodo que envia el array al otro proceso
            rankDest = rank - (int)pow(2, actualLevel);
            MPI_Send(subArray, currentSubarraySize, MPI_INT, rankDest, 0, MPI_COMM_WORLD);
            isFinished = true;
        }

        if (actualLevel + 1 == totalLevels)
        {
            isFinished = true;
        }
        actualLevel += 1;
        currentSubarraySize *= 2; // tamaño del buffer actual
    }

    if (rank == 0)
    {
        printf("\nEl resultado final es:\n");
        printArray(subArray, currentSubarraySize);
    }

    MPI_Finalize();
    return 0;
}