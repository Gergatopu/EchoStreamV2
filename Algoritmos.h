#pragma once
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Entidades.h"

class Algoritmos {
public:
    
    //INSERTION SORT
  
    // Ideal para listas pequeñas o cadenas de texto. 
    // Lo usamos para ordenar alfabéticamente (A-Z). Complejidad: O(n^2)
    static void insertionSortPorNombre(std::vector<Cancion*>& arr) {
        int n = arr.size();
        for (int i = 1; i < n; i++) {
            Cancion* key = arr[i];
            int j = i - 1;
            // Desplaza los elementos mayores hacia la derecha
            while (j >= 0 && arr[j]->getNombre() > key->getNombre()) {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }
    }

    // SHELL SORT
    // Optimización de la inserción usando saltos (gaps).
    // Lo usamos para el Top Reproducciones (Descendente). Complejidad: ~O(n^2)
    static void shellSortPorReproducciones(std::vector<Cancion*>& arr) {
        int n = arr.size();
        for (int gap = n / 2; gap > 0; gap /= 2) {
            for (int i = gap; i < n; i++) {
                Cancion* temp = arr[i];
                int j;
                // Orden descendente: los mayores van primero
                for (j = i; j >= gap && arr[j - gap]->getReproducciones() < temp->getReproducciones(); j -= gap) {
                    arr[j] = arr[j - gap];
                }
                arr[j] = temp;
            }
        }
    }

    // FISHER-YATES SHUFFLE
    // Permutación aleatoria uniforme.
    // Usado para el Modo Aleatorio de la cola. Complejidad: O(n)
    template<typename T>
    static void fisherYatesShuffle(std::vector<T>& arr) {
        srand(time(0));
        int n = arr.size();
        for (int i = n - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(arr[i], arr[j]);
        }
    }
};