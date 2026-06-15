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

    // Formato CANCION en biblioteca.txt (8 campos tras el tag):
    //   CANCION, id, nombre, artista_id, album_id, duracion, genero_id, reproducciones
    //
    // Formato PODCAST (4 campos tras el tag):
    //   PODCAST, id, nombre, host, reproducciones
    //
    // Los strings de artista, álbum y género se resuelven mediante
    // obtenerNombreArtista/Album/Genero() de Utilidades.h dentro de Cancion.
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

    // --- VISTAS Y ALGORITMOS (Sin alterar la ListaDoble original) ---
    void mostrarCancionesOrdenadasA_Z() {
        vector<Cancion*> vista = catalogoCanciones.toVector();
        Algoritmos::insertionSortPorNombre(vista);
        cout << "\n  [Insercion] Canciones (A-Z):" << endl;
        for (Cancion* c : vista) c->mostrarDetalles();
    }

    void mostrarTopReproducciones() {
        vector<Cancion*> vista = catalogoCanciones.toVector();
        Algoritmos::shellSortPorReproducciones(vista);
        cout << "\n  [Shell Sort] Top Reproducciones:" << endl;
        for (Cancion* c : vista) c->mostrarDetalles();
    }

    void mostrarSoloCanciones() {
        cout << "\n--- CANCIONES DISPONIBLES ---" << endl;
        vector<Cancion*> vista = catalogoCanciones.toVector();
        if (vista.empty()) cout << "  (Sin canciones)" << endl;
        for (Cancion* c : vista) c->mostrarDetalles();
    }

    Cancion* buscarCancionId(int id) {
        vector<Cancion*> vista = catalogoCanciones.toVector();
        for (Cancion* c : vista) { if (c->getId() == id) return c; }
        return nullptr;
    }

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

    bool cancionEscuchada(int idCancion, int idUsuario) {

        GestorArchivos gestor;
        vector<Cancion*> historial = gestor.getHistorialUsuario(idUsuario, catalogoCanciones.toVector());

        for (Cancion* c : historial) {
            if (idCancion == c->getId()) return true;
        }
        return false;
    }

    void mostrarCancionesSugeridas(vector<double> usuarioPreferencias, int idUsuario) {
        vector<Cancion*> canciones = catalogoCanciones.toVector();
        vector<Recomendacion> listaDistancias; 

        if (canciones.empty()) return;

        for (Cancion* cancion : canciones) {



            vector<double> metaDatosCancion = {
                (double)cancion->getDuracion(),
                (double)cancion->getGenero(),
                (double)cancion->getArtista(),
                (double)cancion->getAlbum()
            };

            double dist = DistanciaEuclidiana(usuarioPreferencias, metaDatosCancion);

            // Insertamos el objeto
            
            listaDistancias.push_back({ dist, cancion });
        }
        
        
        Algoritmos alg;
        alg.heapSort(listaDistancias);

        // Mostramos los resultados ya ordenados
        cout << "\n--- TUS RECOMENDACIONES ---" << endl;
        for (int i = 0; i < listaDistancias.size(); i++) {
            if(listaDistancias[i].distancia <= 50 && !cancionEscuchada(listaDistancias[i].cancion->getId(),idUsuario) ){
            cout << " [Distancia: " << listaDistancias[i].distancia << "] ";
            listaDistancias[i].cancion->mostrarDetalles();
            }
        }
    }

	vector<Cancion*> getCatalogoCanciones() { return catalogoCanciones.toVector(); }

    vector<double> calcularPreferencias(int idUsuario) {
        double generoProm = 0, artistaProm = 0, duracionProm = 0, albumProm = 0;

        // Llamas a GestorArchivos pasándole el catálogo actual
        GestorArchivos gestor;
        vector<Cancion*> historial =  gestor.getHistorialUsuario(idUsuario, catalogoCanciones.toVector());

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

    Usuario* buscarPorId(int id) {
        vector<Usuario*> v = listaUsuarios.toVector();
        for (Usuario* u : v) { if (u->getId() == id) return u; }
        return nullptr;
    }

    bool iniciarSesion(string email, string password) {
        vector<Usuario*> v = listaUsuarios.toVector();
        for (Usuario* u : v) {
            if (u->getEmail() == email && u->getContrasena() == password) {
                usuarioLogueado = u;
                cout << "\n  Bienvenido/a, " << u->getNombre() << "!" << endl;

                GestorArchivos::cargarHistorialUsuario(usuarioLogueado);

                return true;
            }
        }
        cout << "  Error: Credenciales incorrectas." << endl;
        return false;
    }

    void registrarNuevoUsuario(int id, string nom, string email, string pass, int plan) {
        Usuario* nuevo = new Usuario(id, nom, email, pass);
        listaUsuarios.insertarAlFinal(nuevo);
        GestorPersistencia::guardarLinea("usuarios.txt", nuevo->toString());

        if (plan == 2) {
            nuevo->activarPremium({ "Premium", 29.90, obtenerFechaHoy(), "2026-12-31", true });
            GestorPersistencia::guardarLinea("usuarios.txt", "SUSCRIPCION,1001," + to_string(id) + ",PlanPremium,Premium,29.90,Hoy,2026-12-31,1");
            cout << "  Plan Premium activado." << endl;
        }
        cout << "  Usuario registrado." << endl;
    }

    Usuario* getUsuarioLogueado() { return usuarioLogueado; }
    void cerrarSesion() { usuarioLogueado = nullptr; cout << "  Sesion cerrada." << endl; }
};

class GestorReproduccion {
private:
    Cola<Cancion*> filaEspera;
    GestorBiblioteca* biblioteca;
    Usuario* usuarioActual;

public:
    GestorReproduccion(GestorBiblioteca* lib, Usuario* user) : biblioteca(lib), usuarioActual(user) {}
    ~GestorReproduccion() { filaEspera.vaciar(); }

    void agregarCancionAFila(int idCancion) {
        Cancion* c = biblioteca->buscarCancionId(idCancion);
        if (c) {
            filaEspera.encolar(c);
            cout << "  Cancion encolada: " << c->getNombre() << endl;
        }
        else {
            cout << "  Cancion no encontrada." << endl;
        }
    }
    
    void reproducirSiguiente() {
        if (filaEspera.estaVacia()) { cout << "  La cola esta vacia." << endl; return; }

        Cancion* c = filaEspera.verFrente();
        c->reproducir();
        usuarioActual->registrarEnHistorial(c->getNombre(), "Cancion", obtenerNombreArtista(c->getArtista()));

        string linea = "HIST," + to_string(usuarioActual->getId()) + ",999," +
            c->getNombre() + ",Cancion," + obtenerNombreArtista(c->getArtista()) + "," + obtenerHoraActual();
        GestorPersistencia::guardarLinea("historial.txt", linea);

        filaEspera.desencolar();
    }
  

    void modoAleatorio() {
        vector<Cancion*> vista = filaEspera.toVector();
        if (vista.size() < 2) return;
        Algoritmos::fisherYatesShuffle(vista);
        filaEspera.vaciar();
        for (Cancion* c : vista) filaEspera.encolar(c);
        cout << "  [Fisher-Yates] Cola mezclada aleatoriamente." << endl;
    }

    void mostrarCola() {
        vector<Cancion*> vista = filaEspera.toVector();
        cout << "\n--- Cola (" << vista.size() << ") ---" << endl;
        for (Cancion* c : vista) cout << "  -> " << c->getNombre() << endl;
    }
};