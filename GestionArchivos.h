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

                    // Leer la fecha sin la coma para atrapar hasta el final
                    getline(ss, fecha);

                    // Limpiar el retorno de carro (\r) de Windows
                    if (!fecha.empty() && fecha.back() == '\r') fecha.pop_back();

                    u->cargarEnHistorial(titulo, tipo, artista, fecha);
                }
            }
        }
        archivo.close();
    }

    // Estatica para mantener consistencia con el resto de la clase (no necesita
    // estado propio, solo lee el archivo y filtra por usuario).
    static vector<Cancion*> getHistorialUsuario(int userId, const vector<Cancion*>& catalogo) {
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

                    // Solo procesamos registros de tipo Cancion
                    if (tipo == "Cancion") {
                        // Buscamos el puntero de la cancion en el catalogo por su titulo
                        for (Cancion* c : catalogo) {
                            if (c->getNombre() == titulo) {
                                historial.push_back(c);
                                break; // coincidencia encontrada, vamos a la siguiente linea
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

    // ESCRITURA GENERICA
    static void guardarEntidad(string rutaArchivo, const EntidadBase* entidad) {
        ofstream archivo(rutaArchivo, ios::app);
        if (archivo.is_open()) {
            archivo << entidad->toString() << "\n";
            archivo.close();
        }
    }
};