#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include "Entidades.h"
#include "Algoritmos.h"
#include "GestionArchivos.h" 

using namespace std;

class GestorPersistencia {
public:
    static void guardarLinea(string archivo, string linea) {
        ofstream f(archivo, ios::app);
        if (f.is_open()) { f << linea << "\n"; f.close(); }
    }
};

class GestorBiblioteca {
private:
    ListaDoble<Cancion*> catalogoCanciones;
    ListaDoble<Podcast*> catalogoPodcasts;

public:
    GestorBiblioteca() { cargarDesdeArchivo(); }
    ~GestorBiblioteca() { catalogoCanciones.vaciar(); catalogoPodcasts.vaciar(); }
    struct Recomendacion {
        double distancia;
        Cancion* cancion;

        bool operator>(const Recomendacion& otra) const {
            return distancia > otra.distancia;
        }

        bool operator<(const Recomendacion& otra) const {
            return distancia < otra.distancia;
        }
    };

    // CARGA DE BIBLIOTECA DESDE ARCHIVO
    void cargarDesdeArchivo() {
        ifstream archivo("biblioteca.txt");
        if (!archivo.is_open()) return;

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            stringstream ss(linea);
            string tag;
            getline(ss, tag, ',');

            if (tag == "CANCION") {
                string id, nom, artistaId, albumId, dur, genId, rep;
                getline(ss, id, ',');
                getline(ss, nom, ',');
                getline(ss, artistaId, ',');
                getline(ss, albumId, ',');
                getline(ss, dur, ',');
                getline(ss, genId, ',');
                getline(ss, rep, ',');

                catalogoCanciones.insertarAlFinal(new Cancion(
                    stoi(id),
                    nom,
                    stoi(artistaId),   // ID — Cancion resuelve el nombre con Utilidades
                    stoi(albumId),
                    stoi(dur),
                    stoi(genId),
                    stoi(rep)
                ));
            }
            else if (tag == "PODCAST") {
                string id, nom, host, rep;
                getline(ss, id, ',');
                getline(ss, nom, ',');
                getline(ss, host, ',');
                getline(ss, rep, ',');
                catalogoPodcasts.insertarAlFinal(new Podcast(stoi(id), nom, host, ""));
            }
            // Las líneas ARTISTA, ALBUM y GENERO ya no existen en biblioteca.txt.
        }
        archivo.close();
    }


    // FUNCIONES DE RECOMENDACION
    bool cancionEscuchada(int idCancion, int idUsuario) {

        GestorArchivos gestor;
        vector<Cancion*> historial = gestor.getHistorialUsuario(idUsuario, catalogoCanciones.toVector());

        for (Cancion* c : historial) {
            if (idCancion == c->getId()) return true;
        }
        return false;
    }

    // Calcula y devuelve la lista de recomendaciones ya ordenadas (Heap Sort) y
    // filtradas (distancia <= 50 y no escuchadas). No imprime nada: la UI decide
    // como mostrar el resultado.
    vector<Recomendacion> obtenerRecomendaciones(vector<double> preferenciasUsuario, int idUsuario) {
        vector<Cancion*> canciones = catalogoCanciones.toVector();
        vector<Recomendacion> listaDistancias;

        if (canciones.empty()) return listaDistancias;

        for (Cancion* cancion : canciones) {
            vector<double> metaDatosCancion = {
                (double)cancion->getDuracion(),
                (double)cancion->getGenero(),
                (double)cancion->getArtista(),
                (double)cancion->getAlbum()
            };

            double dist = DistanciaEuclidiana(preferenciasUsuario, metaDatosCancion);
            listaDistancias.push_back({ dist, cancion });
        }

        Algoritmos alg;
        alg.heapSort(listaDistancias);

        vector<Recomendacion> resultado;
        for (Recomendacion& r : listaDistancias) {
            if (r.distancia <= 50 && !cancionEscuchada(r.cancion->getId(), idUsuario)) {
                resultado.push_back(r);
            }
        }
        return resultado;
    }

    Cancion* buscarCancionId(int id) {
        vector<Cancion*> vista = catalogoCanciones.toVector();
        for (Cancion* c : vista) { if (c->getId() == id) return c; }
        return nullptr;
    }

    // VISTAS PROCESADAS DEL CATALOGO (la UI solo imprime el resultado)
    vector<Cancion*> obtenerCancionesOrdenadasAZ() {
        vector<Cancion*> vista = catalogoCanciones.toVector();
        Algoritmos::insertionSortPorNombre(vista);
        return vista;
    }

    vector<Cancion*> obtenerTopReproducciones() {
        vector<Cancion*> vista = catalogoCanciones.toVector();
        Algoritmos::shellSortPorReproducciones(vista);
        return vista;
    }

    //GETTERS:
    vector<Cancion*> getCatalogoCanciones() { return catalogoCanciones.toVector(); }

    vector<double> calcularPreferencias(int idUsuario) {
        double generoProm = 0, artistaProm = 0, duracionProm = 0, albumProm = 0;

        // Llamas a GestorArchivos pasándole el catálogo actual
        GestorArchivos gestor;
        vector<Cancion*> historial = gestor.getHistorialUsuario(idUsuario, catalogoCanciones.toVector());

        if (historial.empty()) {
            return { 0, 0, 0, 0 }; // O devuelve un vector por defecto si no ha escuchado nada
        }

        for (Cancion* c : historial) {
            duracionProm += c->getDuracion();
            generoProm += c->getGenero();
            artistaProm += c->getArtista();
            albumProm += c->getAlbum();
        }

        int total = historial.size();
        // Construimos el vector promedio: [Duracion, Genero, Artista, Album]
        return { duracionProm / total, generoProm / total, artistaProm / total, albumProm / total };
    }

    ListaDoble<Cancion*>& getListaCanciones() { return catalogoCanciones; }

    ListaDoble<Podcast*>& getListaPodcasts() { return catalogoPodcasts; }


};

class GestorUsuarios {
private:
    ListaDoble<Usuario*> listaUsuarios;
    Usuario* usuarioLogueado;

public:
    GestorUsuarios() : usuarioLogueado(nullptr) { cargarDesdeArchivo(); }
    ~GestorUsuarios() { listaUsuarios.vaciar(); }

    void cargarDesdeArchivo() {
        ifstream archivo("usuarios.txt");
        if (!archivo.is_open()) return;
        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;
            stringstream ss(linea); string tag;
            getline(ss, tag, ',');

            if (tag == "USUARIO") {
                string id, nom, email, pass;
                getline(ss, id, ','); getline(ss, nom, ',');
                getline(ss, email, ',');


                getline(ss, pass);


                if (!pass.empty() && pass.back() == '\r') pass.pop_back();

                listaUsuarios.insertarAlFinal(new Usuario(stoi(id), nom, email, pass));
            }
            else if (tag == "SUSCRIPCION") {
                string idS, idU, nom, tipo, prec, ini, ven, act;
                getline(ss, idS, ','); getline(ss, idU, ','); getline(ss, nom, ',');
                getline(ss, tipo, ','); getline(ss, prec, ','); getline(ss, ini, ',');
                getline(ss, ven, ',');  getline(ss, act, ',');
                Usuario* due = buscarPorId(stoi(idU));
                if (due) due->activarPremium({ tipo, stod(prec), ini, ven, act == "1" });
            }
            else if (tag == "PLAYLIST") {
                string id, nom, desc, uid;
                getline(ss, id, ','); getline(ss, nom, ','); getline(ss, desc, ','); getline(ss, uid, ',');
                Usuario* due = buscarPorId(stoi(uid));
                if (due) due->agregarPlaylist(new Playlist(stoi(id), nom, desc, stoi(uid)));
            }
        }
        archivo.close();
    }

    // FUNCIONES DE GESTION DE USUARIOS
    Usuario* buscarPorId(int id) {
        vector<Usuario*> v = listaUsuarios.toVector();
        for (Usuario* u : v) { if (u->getId() == id) return u; }
        return nullptr;
    }

    // Devuelve true/false; ya no imprime nada (la UI decide el mensaje).
    bool iniciarSesion(string email, string password) {
        vector<Usuario*> v = listaUsuarios.toVector();
        for (Usuario* u : v) {
            if (u->getEmail() == email && u->getContrasena() == password) {
                usuarioLogueado = u;
                GestorArchivos::cargarHistorialUsuario(usuarioLogueado);
                return true;
            }
        }
        return false;
    }

    // Solo logica de registro/persistencia. La UI imprime los mensajes de exito.
    void registrarNuevoUsuario(int id, string nom, string email, string pass, int plan) {
        Usuario* nuevo = new Usuario(id, nom, email, pass);
        listaUsuarios.insertarAlFinal(nuevo);
        GestorPersistencia::guardarLinea("usuarios.txt", nuevo->toString());

        if (plan == 2) {
            nuevo->activarPremium({ "Premium", 29.90, obtenerFechaHoy(), "2026-12-31", true });
            GestorPersistencia::guardarLinea("usuarios.txt", "SUSCRIPCION,1001," + to_string(id) + ",PlanPremium,Premium,29.90,Hoy,2026-12-31,1");
        }
    }

    void cerrarSesion() { usuarioLogueado = nullptr; }

    // Activa el plan Premium de un usuario y persiste el cambio.
    void activarPlanPremium(Usuario* u) {
        if (!u) return;
        u->activarPremium({ "Premium", 29.90, obtenerFechaHoy(), "2026-12-31", true });
        GestorPersistencia::guardarLinea("usuarios.txt", "SUSCRIPCION,999," + to_string(u->getId()) + ",PlanPremium,Premium,29.90,Hoy,2026-12-31,1");
    }

    // Devuelve los favoritos de un usuario ordenados A-Z (Insertion Sort).
    static vector<Cancion*> obtenerFavoritosOrdenados(Usuario* usuario) {
        vector<Cancion*> favs = usuario->getFavoritos().toVector();
        Algoritmos::insertionSortPorNombre(favs);
        return favs;
    }

    // GETTERS
    Usuario* getUsuarioLogueado() { return usuarioLogueado; }
};

class GestorReproduccion {
private:
    Cola<Cancion*> cancionesEspera;
    GestorBiblioteca* biblioteca;
    Usuario* usuarioActual;

public:
    GestorReproduccion(GestorBiblioteca* lib, Usuario* user) : biblioteca(lib), usuarioActual(user) {}
    ~GestorReproduccion() { cancionesEspera.vaciar(); }

    // Devuelve la cancion encolada, o nullptr si no existe. La UI imprime el resultado.
    Cancion* agregarCancionAFila(int idCancion) {
        Cancion* c = biblioteca->buscarCancionId(idCancion);
        if (c) {
            cancionesEspera.encolar(c);
        }
        return c;
    }

    // Devuelve la cancion reproducida, o nullptr si la cola estaba vacia.
    Cancion* reproducirSiguiente() {
        if (cancionesEspera.estaVacia()) return nullptr;

        Cancion* c = cancionesEspera.verFrente();
        c->reproducir();
        usuarioActual->registrarEnHistorial(c->getNombre(), "Cancion", obtenerNombreArtista(c->getArtista()));

        string linea = "HIST," + to_string(usuarioActual->getId()) + ",999," +
            c->getNombre() + ",Cancion," + obtenerNombreArtista(c->getArtista()) + "," + obtenerHoraActual();
        GestorPersistencia::guardarLinea("historial.txt", linea);

        cancionesEspera.desencolar();
        return c;
    }

    // Devuelve true si se pudo mezclar (>=2 elementos), false en caso contrario.
    bool modoAleatorio() {
        vector<Cancion*> vista = cancionesEspera.toVector();
        if (vista.size() < 2) return false;
        Algoritmos::fisherYatesShuffle(vista);
        cancionesEspera.vaciar();
        for (Cancion* c : vista) cancionesEspera.encolar(c);
        return true;
    }

    // Getter puro: la impresion de la cola la hace la UI.
    vector<Cancion*> obtenerCola() {
        return cancionesEspera.toVector();
    }
};