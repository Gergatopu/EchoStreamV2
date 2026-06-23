#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include "Entidades.h"
#include "Algoritmos.h"
#include "GestionArchivos.h"

using namespace std;

// ============================================================
//  PERSISTENCIA GENERICA (escritura + helper de parseo CSV)
// ============================================================
class GestorPersistencia {
public:
    static void guardarLinea(string archivo, string linea) {
        ofstream f(archivo, ios::app);
        if (f.is_open()) { f << linea << "\n"; f.close(); }
    }

    // Parte una linea CSV en sus campos (separados por ',').
    // Si el archivo fue editado/guardado en Windows, limpia el '\r'
    // que puede quedar pegado al ultimo campo.
    static vector<string> partirCSV(const string& linea) {
        vector<string> campos;
        stringstream ss(linea);
        string campo;
        while (getline(ss, campo, ',')) campos.push_back(campo);

        if (!campos.empty() && !campos.back().empty() && campos.back().back() == '\r') {
            campos.back().pop_back();
        }
        return campos;
    }
};


// ============================================================
//  GESTOR DE BIBLIOTECA (catalogo de canciones y podcasts)
// ============================================================
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

        bool operator>(const Recomendacion& otra) const { return distancia > otra.distancia; }
        bool operator<(const Recomendacion& otra) const { return distancia < otra.distancia; }
    };

    // --- CARGA DE BIBLIOTECA DESDE ARCHIVO ---
    // Formato CANCION (8 campos):   CANCION, id, nombre, artistaId, albumId, duracion, generoId, reproducciones
    // Formato PODCAST  (4 campos):  PODCAST, id, nombre, host, reproducciones
    // Los nombres de artista/album/genero se resuelven en runtime con Utilidades.h.
    void cargarDesdeArchivo() {
        ifstream archivo("biblioteca.txt");
        if (!archivo.is_open()) return;

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            vector<string> campos = GestorPersistencia::partirCSV(linea);
            if (campos.empty()) continue;
            const string& tag = campos[0];

            if (tag == "CANCION" && campos.size() >= 8) {
                catalogoCanciones.insertarAlFinal(new Cancion(
                    stoi(campos[1]), campos[2], stoi(campos[3]), stoi(campos[4]),
                    stoi(campos[5]), stoi(campos[6]), stoi(campos[7])
                ));
            }
            else if (tag == "PODCAST" && campos.size() >= 4) {
                catalogoPodcasts.insertarAlFinal(new Podcast(stoi(campos[1]), campos[2], campos[3], ""));
            }
            // Las lineas ARTISTA, ALBUM y GENERO ya no existen en biblioteca.txt.
        }
        archivo.close();
    }

    // --- RECOMENDACIONES ---
    bool cancionEscuchada(int idCancion, int idUsuario) {
        vector<Cancion*> historial = GestorArchivos::getHistorialUsuario(idUsuario, catalogoCanciones.toVector());
        for (Cancion* c : historial) {
            if (idCancion == c->getId()) return true;
        }
        return false;
    }

    // Calcula, ordena (Heap Sort) y filtra (distancia <= 50 y no escuchadas).
    // No imprime nada: la UI decide como mostrar el resultado.
    vector<Recomendacion> obtenerRecomendaciones(vector<double> preferenciasUsuario, int idUsuario) {
        vector<Cancion*> canciones = catalogoCanciones.toVector();
        vector<Recomendacion> listaDistancias;
        if (canciones.empty()) return listaDistancias;

        for (Cancion* cancion : canciones) {
            vector<double> metaDatosCancion = {
                (double)cancion->getDuracion(), (double)cancion->getGenero(),
                (double)cancion->getArtista(),  (double)cancion->getAlbum()
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

    // --- VISTAS PROCESADAS DEL CATALOGO (la UI solo imprime el resultado) ---
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

    // --- GETTERS ---
    vector<Cancion*> getCatalogoCanciones() { return catalogoCanciones.toVector(); }

    vector<double> calcularPreferencias(int idUsuario) {
        double generoProm = 0, artistaProm = 0, duracionProm = 0, albumProm = 0;
        vector<Cancion*> historial = GestorArchivos::getHistorialUsuario(idUsuario, catalogoCanciones.toVector());

        if (historial.empty()) {
            return { 0, 0, 0, 0 }; // No ha escuchado nada todavia
        }

        for (Cancion* c : historial) {
            duracionProm += c->getDuracion();
            generoProm += c->getGenero();
            artistaProm += c->getArtista();
            albumProm += c->getAlbum();
        }

        int total = historial.size();
        return { duracionProm / total, generoProm / total, artistaProm / total, albumProm / total };
    }

    ListaDoble<Cancion*>& getListaCanciones() { return catalogoCanciones; }
    ListaDoble<Podcast*>& getListaPodcasts() { return catalogoPodcasts; }
};


// ============================================================
//  GESTOR DE USUARIOS (cuentas, sesion, playlists, favoritos)
// ============================================================
class GestorUsuarios {
private:
    ListaDoble<Usuario*> listaUsuarios;
    Usuario* usuarioLogueado;
    GestorBiblioteca* biblioteca; // necesario para resolver canciones en PLAYLIST_CANCION

    // --- Helpers de carga: uno por tipo de linea en usuarios.txt ---

    void procesarLineaUsuario(const vector<string>& campos) {
        // USUARIO, id, nombre, email, password
        if (campos.size() < 5) return;
        listaUsuarios.insertarAlFinal(new Usuario(stoi(campos[1]), campos[2], campos[3], campos[4]));
    }

    void procesarLineaSuscripcion(const vector<string>& campos) {
        // SUSCRIPCION, idSuscripcion, idUsuario, nombrePlan, tipo, precio, fechaInicio, fechaVencimiento, activa
        if (campos.size() < 9) return;
        Usuario* due = buscarPorId(stoi(campos[2]));
        if (due) due->activarPremium({ campos[4], stod(campos[5]), campos[6], campos[7], campos[8] == "1" });
    }

    void procesarLineaPlaylist(const vector<string>& campos) {
        // PLAYLIST, id, nombre, descripcion, idUsuario
        if (campos.size() < 5) return;
        Usuario* due = buscarPorId(stoi(campos[4]));
        if (due) {
            due->agregarPlaylist(new Playlist(stoi(campos[1]), campos[2], campos[3], stoi(campos[4])));
            // Evita que el contador interno de IDs choque con playlists ya existentes.
            due->recalcularNextPlaylistId();
        }
    }

    void procesarLineaPlaylistCancion(const vector<string>& campos) {
        // PLAYLIST_CANCION, idPlaylist, idCancion
        if (campos.size() < 3 || !biblioteca) return;
        int idPlaylist = stoi(campos[1]);
        int idCancion = stoi(campos[2]);

        // Usuario::crearPlaylist genera el ID como (idUsuario * 100) + n,
        // asi que podemos ubicar al dueño directamente sin recorrer a todos los usuarios.
        Usuario* due = buscarPorId(idPlaylist / 100);
        if (!due) return;

        Playlist* pl = due->buscarPlaylistPorId(idPlaylist);
        Cancion* c = biblioteca->buscarCancionId(idCancion);
        if (pl && c) pl->agregarCancion(c);
    }

public:
    // Requiere la biblioteca para poder resolver las canciones de cada playlist al cargar.
    GestorUsuarios(GestorBiblioteca* biblioteca) : usuarioLogueado(nullptr), biblioteca(biblioteca) {
        cargarDesdeArchivo();
    }
    ~GestorUsuarios() { listaUsuarios.vaciar(); }

    void cargarDesdeArchivo() {
        ifstream archivo("usuarios.txt");
        if (!archivo.is_open()) return;

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            vector<string> campos = GestorPersistencia::partirCSV(linea);
            if (campos.empty()) continue;
            const string& tag = campos[0];

            if (tag == "USUARIO") procesarLineaUsuario(campos);
            else if (tag == "SUSCRIPCION") procesarLineaSuscripcion(campos);
            else if (tag == "PLAYLIST") procesarLineaPlaylist(campos);
            else if (tag == "PLAYLIST_CANCION") procesarLineaPlaylistCancion(campos);
        }
        archivo.close(); // IMPORTANTE: fuera del while. Antes se cerraba en cada
        // iteracion y solo se llegaba a leer la primera linea del archivo.
    }

    // --- GESTION DE USUARIOS ---
    Usuario* buscarPorId(int id) {
        vector<Usuario*> v = listaUsuarios.toVector();
        for (Usuario* u : v) { if (u->getId() == id) return u; }
        return nullptr;
    }

    // Devuelve true/false; no imprime nada (la UI decide el mensaje).
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

    // --- GETTERS ---
    Usuario* getUsuarioLogueado() { return usuarioLogueado; }
};


// ============================================================
//  GESTOR DE REPRODUCCION (cola de canciones)
// ============================================================
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
        if (c) cancionesEspera.encolar(c);
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