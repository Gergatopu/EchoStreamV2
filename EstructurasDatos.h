#pragma once
#include <vector>
#include <stdexcept>
#include <cmath>
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

    void eliminar(T valor) {
        if (!cabeza) return;
        Nodo<T>* actual = cabeza;
        do {
            if (actual->dato == valor) {
                if (actual == cabeza && actual->siguiente == cabeza) {
                    delete actual;
                    cabeza = nullptr;
                    total--;
                    return;
                }
                else {
                    actual->anterior->siguiente = actual->siguiente;
                    actual->siguiente->anterior = actual->anterior;
                    if (actual == cabeza) cabeza = actual->siguiente;
                    delete actual;
                    total--;
                    return;
                }
            }
            actual = actual->siguiente;
        } while (actual != cabeza);
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

    void insertarAlInicio(T valor) {
        Nodo<T>* nuevo = new Nodo<T>(valor);
        if (!cabeza && !ultimo) {
            cabeza = nuevo;
            ultimo = nuevo;
            contador++;
            return;
        }
        nuevo->siguiente = cabeza;
        cabeza->anterior = nuevo;
        cabeza = nuevo;
        contador++;
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
    Nodo<T>* getUltimo() const { return ultimo; }
    int getContador() const { return contador; }
};

// PILA (LIFO)
template<typename T>
class Pila {
private:
    Nodo<T>* tope;

public:
    Pila() : tope(nullptr) {}

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

    int obtenerTamano() const {
        int c = 0;
        Nodo<T>* t = tope;
        while (t) { c++; t = t->siguiente; }
        return c;
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

    int getTamano() const { return tamano; }
    bool estaVacia() const { return frente == nullptr; }
};



double DistanciaEuclidiana(const vector<double>& v1, const vector<double>& v2) {
    double sumaTotal = 0.0;
    for (size_t i = 0; i < v1.size() && i < v2.size(); i++) {
        sumaTotal += pow(v1[i] - v2[i], 2);
    }
    return sqrt(sumaTotal);
}