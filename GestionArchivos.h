#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Entidades.h"
#include "EstructurasDatos.h"


using namespace std;

class GestorArchivos {
public:
    // LECTURA DE BIBLIOTECA

    static void cargarBiblioteca(ListaDoble<Cancion*>& catalogoCanciones, ListaDoble<Podcast*>& catalogoPodcasts) {
        ifstream archivo("biblioteca.txt");
        if (!archivo.is_open()) {
            cout << "  [Advertencia] No se pudo abrir biblioteca.txt" << endl;
            return;
        }

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            stringstream ss(linea);
            string tipo;
            getline(ss, tipo, ',');

            if (tipo == "CANCION") {
                string idStr, nombre, artista, duracionStr, genero, reproStr;
                getline(ss, idStr, ',');
                getline(ss, nombre, ',');
                getline(ss, artista, ',');
                getline(ss, duracionStr, ',');
                getline(ss, genero, ',');
                getline(ss, reproStr, ',');

                Cancion* c = new Cancion(stoi(idStr), nombre, artista, "S/A", stoi(duracionStr), genero, stoi(reproStr));
                catalogoCanciones.insertarAlFinal(c);
            }
            else if (tipo == "PODCAST") {
                string idStr, nombre, host, reproStr;
                getline(ss, idStr, ',');
                getline(ss, nombre, ',');
                getline(ss, host, ',');
                getline(ss, reproStr, ',');

                Podcast* p = new Podcast(stoi(idStr), nombre, host, "S/D");
                // Como el txt marca repros al final en los podcasts, lo seteamos
                catalogoPodcasts.insertarAlFinal(p);
            }
            // Ignoramos ARTISTA, ALBUM y GENERO ya que ahora la arquitectura es plana.
        }
        archivo.close();
    }

    // LECTURA Y ESCRITURA DE HISTORIAL
    static void cargarHistorialUsuario(Usuario* u) {
        ifstream archivo("historial.txt");
        if (!archivo.is_open()) return;

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            stringstream ss(linea);
            string tag, userIdStr, histIdStr, titulo, tipo, artista, fecha;
            getline(ss, tag, ',');

            if (tag == "HIST") {
                getline(ss, userIdStr, ',');
                if (stoi(userIdStr) == u->getId()) {
                    getline(ss, histIdStr, ','); 
                    getline(ss, titulo, ',');
                    getline(ss, tipo, ',');
                    getline(ss, artista, ',');
                    getline(ss, fecha, ',');

                    // Usamos el método nativo del usuario para llenar su pila
                    u->registrarEnHistorial(titulo, tipo, artista);
                }
            }
        }
        archivo.close();
    }

    static void guardarEntradaHistorial(int userId, string titulo, string tipo, string artista, string fecha) {
        ofstream archivo("historial.txt", ios::app);
        if (archivo.is_open()) {
            static int histId = 999;
            archivo << "HIST," << userId << "," << ++histId << "," << titulo << "," << tipo << "," << artista << "," << fecha << "\n";
            archivo.close();
        }
    }

    // ESCRITURA GENÉRICA 
    
    static void guardarEntidad(string rutaArchivo, const EntidadBase* entidad) {
        ofstream archivo(rutaArchivo, ios::app);
        if (archivo.is_open()) {
            archivo << entidad->toString() << "\n";
            archivo.close();
        }
    }
};