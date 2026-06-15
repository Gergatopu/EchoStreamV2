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
    //
    // Formato CANCION en biblioteca.txt (8 campos tras el tag):
    //   CANCION, id, nombre, artista_id, album_id, duracion, genero_id, reproducciones
    //
    // Formato PODCAST en biblioteca.txt (4 campos tras el tag):
    //   PODCAST, id, nombre, host, reproducciones
    //
    // Los strings de artista, álbum y género se resuelven en tiempo de ejecución
    // mediante las funciones obtenerNombreArtista/Album/Genero() de Utilidades.h.

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
                string idStr, nombre, artistaIdStr, albumIdStr, duracionStr, generoIdStr, reproStr;
                getline(ss, idStr, ',');
                getline(ss, nombre, ',');
                getline(ss, artistaIdStr, ',');
                getline(ss, albumIdStr, ',');
                getline(ss, duracionStr, ',');
                getline(ss, generoIdStr, ',');
                getline(ss, reproStr, ',');

                Cancion* c = new Cancion(
                    stoi(idStr),
                    nombre,
                    stoi(artistaIdStr),   // ID numérico — Cancion resuelve el nombre con Utilidades
                    stoi(albumIdStr),
                    stoi(duracionStr),
                    stoi(generoIdStr),
                    stoi(reproStr)
                );
                catalogoCanciones.insertarAlFinal(c);
            }
            else if (tipo == "PODCAST") {
                string idStr, nombre, host, reproStr;
                getline(ss, idStr, ',');
                getline(ss, nombre, ',');
                getline(ss, host, ',');
                getline(ss, reproStr, ',');

                Podcast* p = new Podcast(stoi(idStr), nombre, host, "S/D");
                catalogoPodcasts.insertarAlFinal(p);
            }
            // Las líneas ARTISTA, ALBUM y GENERO ya no existen en biblioteca.txt.
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

                    // SOLUCIÓN: Leer la fecha sin la coma para atrapar hasta el final
                    getline(ss, fecha);

                    // SOLUCIÓN: Limpiar el retorno de carro (\r) de Windows
                    if (!fecha.empty() && fecha.back() == '\r') fecha.pop_back();

                    // Usar el nuevo método
                    u->cargarEnHistorial(titulo, tipo, artista, fecha);
                }
            }
        }
        archivo.close();
    }

    vector<Cancion*> getHistorialUsuario(int userId, const vector<Cancion*>& catalogo) {
        vector<Cancion*> historial;
        ifstream archivo("historial.txt");
        if (!archivo.is_open()) return historial;

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;
            stringstream ss(linea);
            string tag, userIdStr, histIdStr, titulo, tipo, artista, fecha;

            getline(ss, tag, ',');
            if (tag == "HIST") {
                getline(ss, userIdStr, ',');
                if (stoi(userIdStr) == userId) {
                    getline(ss, histIdStr, ',');
                    getline(ss, titulo, ',');
                    getline(ss, tipo, ',');
                    getline(ss, artista, ',');

                    // Leer la fecha sin la coma para atrapar hasta el final
                    getline(ss, fecha);
                    // Limpiar el retorno de carro (\r) de Windows
                    if (!fecha.empty() && fecha.back() == '\r') fecha.pop_back();

                    // 1. Filtrar para asegurarse de procesar únicamente registros de tipo Cancion
                    if (tipo == "Cancion") {
                        // 2. Buscar el puntero de la canción en el catálogo mediante el título
                        for (Cancion* c : catalogo) {
                            if (c->getNombre() == titulo) {
                                historial.push_back(c);
                                break; // Coincidencia encontrada, saltar a la siguiente línea del archivo
                            }
                        }
                    }
                }
            }
        }
        archivo.close();
        return historial;
    }

    static void guardarEntradaHistorial(int userId, string titulo, string tipo, string artista, string fecha) {
        ofstream archivo("historial.txt", ios::app);
        if (archivo.is_open()) {
            static int histId = 999;
            archivo << "HIST," << userId << "," << ++histId << ","
                << titulo << "," << tipo << "," << artista << "," << fecha << "\n";
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