#pragma once
#include <vector>
#include <cstdlib>
#include <ctime>

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

    // Función para intercambiar dos elementos
    template<typename T>
    static void intercambiar(T& a, T& b) {
        T temp = a;
        a = b;
        b = temp;
    }

    // Heapify genérico configurado como MAX-HEAP (usa >)
    template<typename T>
    static void heapify(vector<T>& arr, int n, int i) {
        int mayor = i; // Inicializamos el mayor como la raíz
        int izq = 2 * i + 1; // Hijo izquierdo
        int der = 2 * i + 2; // Hijo derecho

        // El template funcionará perfecto porque T tiene sobrecargado el operador >
        if (izq < n && arr[izq] > arr[mayor])
            mayor = izq;

        if (der < n && arr[der] > arr[mayor])
            mayor = der;

        if (mayor != i) {
            intercambiar(arr[i], arr[mayor]);
            heapify(arr, n, mayor);
        }
    }

    // HeapSort genérico
    // Usado para el ranking de Recomendaciones por distancia euclidiana (menor a mayor)
    template<typename T>
    static void heapSort(vector<T>& arr) {
        int n = arr.size();

        // Paso 1: Construir el Max-Heap 
        for (int i = n / 2 - 1; i >= 0; i--)
            heapify(arr, n, i);

        // Paso 2: Extraer elementos. El mayor va al final, ordenando de MENOR a MAYOR.
        for (int i = n - 1; i > 0; i--) {
            intercambiar(arr[0], arr[i]);
            heapify(arr, i, 0);
        }
    }

    // QUICK SORT
    template <typename T>
    static int partition(vector<T>& arr, int low, int high, bool (*comparar)(T, T))
    {
        T pivot = arr[high];
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++)
        {
            if (comparar(arr[j], pivot))
            {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        return (i + 1);
    }

    template <typename T>
    static void quickSort(vector<T>& arr, int low, int high, bool (*comparar)(T, T)) {
        if (low < high)
        {
            int pi = partition(arr, low, high, comparar);

            quickSort(arr, low, pi - 1, comparar);
            quickSort(arr, pi + 1, high, comparar);
        }
    }

    // MERGE SORT
    // Recibe un comparador (igual que quickSort) en vez de usar el operador <=
    // directamente sobre T: para T = Cancion*, comparar con <= compararia
    // direcciones de memoria en vez de los datos de la cancion.
    // "comparar(a, b)" debe significar "a va antes que b". Complejidad: O(n log n)
    template<typename T>
    static void merge(vector<T>& A, int left, int mid, int right, bool (*comparar)(T, T)) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        // Crear vectores temporales con el tipo T
        vector<T> L(n1), R(n2);

        for (int i = 0; i < n1; i++)
            L[i] = A[left + i];
        for (int j = 0; j < n2; j++)
            R[j] = A[mid + 1 + j];

        int i = 0, j = 0, k = left;

        // Comparar y fusionar (estable: si son "iguales" se prioriza L)
        while (i < n1 && j < n2) {
            if (!comparar(R[j], L[i])) {
                A[k] = L[i];
                i++;
            }
            else {
                A[k] = R[j];
                j++;
            }
            k++;
        }

        // Copiar elementos restantes de L, si hay
        while (i < n1) {
            A[k] = L[i];
            i++; k++;
        }

        // Copiar elementos restantes de R, si hay
        while (j < n2) {
            A[k] = R[j];
            j++; k++;
        }
    }

    // Usado para ordenar el catalogo por duracion (ascendente)
    template<typename T>
    static void mergeSort(vector<T>& A, int left, int right, bool (*comparar)(T, T)) {
        if (left >= right) return;

        int mid = left + (right - left) / 2;

        mergeSort(A, left, mid, comparar);
        mergeSort(A, mid + 1, right, comparar);

        merge(A, left, mid, right, comparar);
    }
};