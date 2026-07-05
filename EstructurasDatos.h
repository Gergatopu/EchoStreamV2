#pragma once
#include <vector>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include "Utilidades.h"

using namespace std;

template<typename T>
struct Nodo {
    T dato;
    Nodo<T>* siguiente;
    Nodo<T>* anterior;

    Nodo(T valor) : dato(valor), siguiente(nullptr), anterior(nullptr) {}
};


// LISTA CIRCULAR DOBLE

template<typename T>
class ListaCircularDoble {
private:
    Nodo<T>* cabeza;
    int total;

public:
    ListaCircularDoble() : cabeza(nullptr), total(0) {}


    ~ListaCircularDoble() {
        vaciar();
    }

    void insertar(T valor) {
        Nodo<T>* nuevo = new Nodo<T>(valor);
        if (!cabeza) {
            cabeza = nuevo;
            cabeza->siguiente = cabeza;
            cabeza->anterior = cabeza;
        }
        else {
            Nodo<T>* ultimo = cabeza->anterior;
            ultimo->siguiente = nuevo;
            nuevo->anterior = ultimo;
            nuevo->siguiente = cabeza;
            cabeza->anterior = nuevo;
        }
        total++;
    }

    void vaciar() {
        if (!cabeza) return;
        Nodo<T>* actual = cabeza;
        do {
            Nodo<T>* temp = actual;
            actual = actual->siguiente;
            delete temp;
        } while (actual != cabeza);
        cabeza = nullptr;
        total = 0;
    }

    // Exportación temporal para ordenamientos y vistas
    vector<T> toVector() const {
        vector<T> v;
        if (!cabeza) return v;
        Nodo<T>* actual = cabeza;
        do {
            v.push_back(actual->dato);
            actual = actual->siguiente;
        } while (actual != cabeza);
        return v;
    }

    Nodo<T>* getCabeza() const { return cabeza; }
    int getTotal() const { return total; }
};

// LISTA DOBLE
template<typename T>
class ListaDoble {
private:
    Nodo<T>* cabeza;
    Nodo<T>* ultimo;
    int contador;

public:
    ListaDoble() : cabeza(nullptr), ultimo(nullptr), contador(0) {}

    ~ListaDoble() {
        vaciar();
    }

    void insertarAlFinal(T valor) {
        Nodo<T>* nuevo = new Nodo<T>(valor);
        if (!cabeza && !ultimo) {
            cabeza = nuevo;
            ultimo = nuevo;
            contador++;
            return;
        }
        nuevo->anterior = ultimo;
        ultimo->siguiente = nuevo;
        ultimo = nuevo;
        contador++;
    }

    void vaciar() {
        Nodo<T>* actual = cabeza;
        while (actual) {
            Nodo<T>* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
        cabeza = nullptr;
        ultimo = nullptr;
        contador = 0;
    }

    vector<T> toVector() const {
        vector<T> v;
        Nodo<T>* actual = cabeza;
        while (actual) {
            v.push_back(actual->dato);
            actual = actual->siguiente;
        }
        return v;
    }

    Nodo<T>* getCabeza() const { return cabeza; }
};

// PILA (LIFO)
template<typename T>
class Pila {
private:
    Nodo<T>* tope;

public:
    Pila() : tope(nullptr) {}

    // La Pila es dueña de una cadena de Nodo<T>* (memoria propia).
    // Copiarla por valor (constructor/operador= por defecto) copiaria
    // solo el puntero 'tope', compartiendo los mismos nodos con el original.
    // Al destruirse la copia, su destructor llamaria a vaciar() y borraria
    // esos nodos, dejando al original con un puntero colgante (use-after-free).
    // Por eso se prohibe copiar: cualquier intento de copia (ej. "Pila<T> x = obj.getPila();")
    // sera ahora un error de compilacion en vez de un crash en tiempo de ejecucion.
    Pila(const Pila&) = delete;
    Pila& operator=(const Pila&) = delete;

    ~Pila() {
        vaciar();
    }

    void push(T valor) {
        Nodo<T>* nuevo = new Nodo<T>(valor);
        nuevo->siguiente = tope;
        tope = nuevo;
    }

    void pop() {
        if (!tope) return;
        Nodo<T>* temp = tope;
        tope = tope->siguiente;
        delete temp;
    }

    T getTope() const {
        if (!tope) throw runtime_error("Pila vacia");
        return tope->dato;
    }

    bool estaVacio() const { return tope == nullptr; }

    void vaciar() {
        while (tope) pop();
    }

    vector<T> toVector() const {
        vector<T> v;
        Nodo<T>* actual = tope;
        while (actual) {
            v.push_back(actual->dato);
            actual = actual->siguiente;
        }
        return v;
    }
};

// COLA (FIFO)
template<typename T>
class Cola {
private:
    Nodo<T>* frente;
    Nodo<T>* fin;
    int tamano;

public:
    Cola() : frente(nullptr), fin(nullptr), tamano(0) {}

    ~Cola() {
        vaciar();
    }

    void encolar(T valor) {
        Nodo<T>* nuevo = new Nodo<T>(valor);
        if (!fin) {
            frente = fin = nuevo;
        }
        else {
            fin->siguiente = nuevo;
            nuevo->anterior = fin;
            fin = nuevo;
        }
        tamano++;
    }

    void desencolar() {
        if (!frente) return;
        Nodo<T>* temp = frente;
        frente = frente->siguiente;
        if (!frente) {
            fin = nullptr;
        }
        else {
            frente->anterior = nullptr;
        }
        delete temp;
        tamano--;
    }

    T verFrente() const {
        return frente ? frente->dato : nullptr;
    }

    void vaciar() {
        while (frente) {
            Nodo<T>* t = frente;
            frente = frente->siguiente;
            delete t;
        }
        fin = nullptr;
        tamano = 0;
    }

    vector<T> toVector() const {
        vector<T> v;
        Nodo<T>* actual = frente;
        while (actual) {
            v.push_back(actual->dato);
            actual = actual->siguiente;
        }
        return v;
    }

    bool estaVacia() const { return frente == nullptr; }
};



double DistanciaEuclidiana(const vector<double>& v1, const vector<double>& v2) {
    double sumaTotal = 0.0;
    for (size_t i = 0; i < v1.size() && i < v2.size(); i++) {
        sumaTotal += pow(v1[i] - v2[i], 2);
    }
    return sqrt(sumaTotal);
}

template<typename T>
struct NodoArbol {
    T valor;
    NodoArbol<T>* izq;
    NodoArbol<T>* der;
    int altura;

    NodoArbol<T>(T v) : valor(v), izq(nullptr), der(nullptr), altura(1) {}
};


template<typename T>
class AVL {
private:

    NodoArbol<T>* raiz;


    int obtenerAltura(NodoArbol<T>* n) {
        if (n == nullptr) return 0;
        return n->altura;
    }

    // Función auxiliar para calcular el Factor de Balance (FB)
    int obtenerBalance(NodoArbol<T>* n) {
        if (n == nullptr) return 0;
        return obtenerAltura(n->izq) - obtenerAltura(n->der);
    }

    // 1. ROTACIÓN SIMPLE A LA DERECHA (Corrige desbalance LL)
    NodoArbol<T>* rotacionDerecha(NodoArbol<T>* y) {
        NodoArbol<T>* x = y->izq;
        NodoArbol<T>* T2 = x->der;

        // Realizar rotación
        x->der = y;
        y->izq = T2;

        // Actualizar alturas (usando std::max de <algorithm>)
        y->altura = std::max(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;
        x->altura = std::max(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;

        // Retornar la nueva raíz del subárbol
        return x;
    }

    // 2. ROTACIÓN SIMPLE A LA IZQUIERDA (Corrige desbalance RR)
    NodoArbol<T>* rotacionIzquierda(NodoArbol<T>* x) {
        NodoArbol<T>* y = x->der;
        NodoArbol<T>* T2 = y->izq;

        // Realizar rotación
        y->izq = x;
        x->der = T2;

        // Actualizar alturas
        x->altura = std::max(obtenerAltura(x->izq), obtenerAltura(x->der)) + 1;
        y->altura = std::max(obtenerAltura(y->izq), obtenerAltura(y->der)) + 1;

        // Retornar la nueva raíz del subárbol
        return y;
    }

    void inorden(NodoArbol<T>* NodoArbol, vector<T>& resultado) const {
        if (NodoArbol == nullptr) return;

        inorden(NodoArbol->izq, resultado);
        resultado.push_back(NodoArbol->valor);
        inorden(NodoArbol->der, resultado);
    }
    void destruirArbol(NodoArbol<T>* NodoArbol) {
        if (NodoArbol == nullptr) return;

        destruirArbol(NodoArbol->izq);
        destruirArbol(NodoArbol->der);
        delete NodoArbol;
    }


public:

    AVL() {
        raiz = nullptr;
    }

    NodoArbol<T>* insertar(NodoArbol<T>* nodoArbol, T valor, bool (*comparar)(T, T)) {

        if (nodoArbol == nullptr) {
            return new NodoArbol<T>(valor);
        }

        if (comparar(valor, nodoArbol->valor)) {
            nodoArbol->izq = insertar(nodoArbol->izq, valor, comparar);
        }
        // Usamos comparar al revés (dato < valor) para simular un "mayor que"
        else if (comparar(nodoArbol->valor, valor)) {
            nodoArbol->der = insertar(nodoArbol->der, valor, comparar);
        }
        else {
            return nodoArbol; // No duplicados
        }

        nodoArbol->altura = max(obtenerAltura(nodoArbol->izq), obtenerAltura(nodoArbol->der)) + 1;
        int balance = obtenerBalance(nodoArbol);

        // Caso Izquierda-Izquierda (LL)
        if (balance > 1 && comparar(valor, nodoArbol->izq->valor)) {
            return rotacionDerecha(nodoArbol);
        }

        // Caso Derecha-Derecha (RR)
        if (balance < -1 && comparar(nodoArbol->der->valor, valor)) {
            return rotacionIzquierda(nodoArbol);
        }

        // Caso Izquierda-Derecha (LR)
        if (balance > 1 && comparar(nodoArbol->izq->valor, valor)) {
            nodoArbol->izq = rotacionIzquierda(nodoArbol->izq);
            return rotacionDerecha(nodoArbol);
        }

        // Caso Derecha-Izquierda (RL)
        if (balance < -1 && comparar(valor, nodoArbol->der->valor)) {
            nodoArbol->der = rotacionDerecha(nodoArbol->der);
            return rotacionIzquierda(nodoArbol);
        }

        return nodoArbol;
    }

    NodoArbol<T>* getRaiz() {
        return raiz;
    }

    void setRaiz(NodoArbol<T>* nuevo) {
        raiz = nuevo;
    }

    vector<T> toVector() const {
        vector<T> resultado;
        inorden(raiz, resultado);
        return resultado;
    }

    void vaciar() {
        destruirArbol(raiz);
        raiz = nullptr;
    }


    T binarySearchID(NodoArbol<T>* nodo, int id) {

        if (nodo == nullptr) {
            return nullptr;
        }

        int idActual = nodo->valor->getId();

        if (idActual == id) {
            return nodo->valor;
        }

        if (idActual < id) {
            return binarySearchID(nodo->der, id);
        }
        else {
            return binarySearchID(nodo->izq, id);
        }
    }
};

template<class T>
class CGrafo {
private:
    //Creando class CArco con el T dato e índice v
    class CArco {
    public:
        //peso en base a la distancia euclidiana entre usuario
        double afinidad;
        int v; //indice del vertice de llegada
        CArco(int vLlegada, double a) { //Constructor
            afinidad = a;
            v = vLlegada; //a donde llegará
        }
    };
    // es el nodo
    class CVertice {
    public:
        T dato; //contiene su información
        vector<CArco*>* ady; //Lista de adyacencia de class CArco, puntero vector
        CVertice() {
            dato = nullptr;
            ady = new vector<CArco*>();
        }
    };
    //Lista de vértices
    vector<CVertice*>* vertices; //creamos un vector de tipo puntero



public:
    CGrafo() {
        vertices = new vector<CVertice*>(); //constructor de CGrafo, realiz la instnacia
    }

    //Operaciones del Grafo
    void adicionarVertice(T dato) {
        CVertice* vert = new CVertice(); //crear puntero temporal *vert tipo CVertice
        vert->dato = dato;  //puntero vert->dato asigno el valor que ingresa a función
        vertices->push_back(vert); //agrego valores en vertices, como es puntero usa ->
        //return vertices->size() - 1; //retorna cantidad de datos en vértices
    }

    int cantidadVertices() {
        return vertices->size(); //retorna cantidad de vértices
    }

    T obtenerVertice(int v) {
        return (vertices->at(v))->dato; //obtiene el valor del vértice
    }
    void modificarVertice(int v, T dato) {
        (vertices->at(v))->dato = dato; //permite modificar el valor dato con el índice del vértice
    }
    //Operaciones del arco
    void adicionarArco(int v, int vLlegada, double afinidad) { //parámetros índice del vértice e índice del vértice de llegada
        CVertice* ver = vertices->at(v); //*ver toma el puntero del índice del vértice de origen
        //Crear el objeto ARCO
        CArco* arc = new CArco(vLlegada, afinidad); //crea temporal arc con el índice de vector de llegada
        ver->ady->push_back(arc); //
        //return ver->ady->size() - 1;
    }

    int cantidadArcos(int v) {
        return (vertices->at(v))->ady->size(); //cantidad de arcos de acuerdo al índice del vértice
    }

    double obtenerArco(int v, int apos) {
        CVertice* ver = vertices->at(v); //obiene el valor del arco de acuerdo al índice del vértice
        return (ver->ady->at(apos))->afinidad;
    }

    void modificarArco(int v, int apos, double afinidad) {
        CVertice* ver = vertices->at(v);
        (ver->ady->at(apos))->afinidad = afinidad;; //modifica el valor del arco
    }

    int obtenerVerticeLlegada(int v, int apos) {
        CVertice* ver = vertices->at(v);
        return (ver->ady->at(apos))->v; //indice del vertice de llegada
    }


};