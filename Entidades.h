#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "vector"
#include "EstructurasDatos.h"
#include "Utilidades.h" 

using namespace std;

// CLASE BASE 
class EntidadBase {
protected:
    int id;
    string nombre;
public:
    EntidadBase(int id, string nombre) : id(id), nombre(nombre) {}
    virtual ~EntidadBase() {}

    virtual void mostrarDetalles() const = 0;
    virtual string toString() const = 0;

    int getId() const { return id; }
    string getNombre() const { return nombre; }


};

// ============================================================
//  CANCION 
// ============================================================
class Cancion : public EntidadBase {
private:
    int id_artista;
    int id_album;
    int id_genero;

    int duracion;
    int reproducciones;

    bool enReproduccion;


    //El sistema de recomendacion usara los siguientes parámetros para sugerir canciones similares: 
    // { artista, duracion, genero, album }

public:
    Cancion(int id, string nombre, int id_artista, int id_album, int duracion, int id_genero, int repro = 0)
        : EntidadBase(id, nombre), id_artista(id_artista), id_album(id_album),
        duracion(duracion), id_genero(id_genero), reproducciones(repro), enReproduccion(false) {
    }

    // --- Reproduccion ---
    void reproducir(int x, int y, int color) {
        reproducciones++;
        ubicar(x, y); asignarcolor(color); cout << nombre << "  -  " << obtenerNombreArtista(id_artista) << endl;
    }

    bool getEnReproduccion() const { return enReproduccion; }

    void setEnReproduccion(bool estado) { enReproduccion = estado; }

    // --- Getters ---
    int getArtista() const { return id_artista; }
    int getAlbum() const { return id_album; }
    int getGenero() const { return id_genero; }
    int getDuracion() const { return duracion; }
    int getReproducciones() const { return reproducciones; }

    // --- Recomendaciones ---
    vector<double> obtenerVectorComponentes() {

        vector<double> cancionComponentes = {
            (double)id_artista,
            (double)duracion,
            (double)id_genero,
            (double)id_album
        };
        return cancionComponentes;
    }

    // --- Presentacion / persistencia ---
    void mostrarDetalles() const override {
        switch (id_genero)
        {
        case 10: asignarcolor(1); break; //"Metal";
        case 20: asignarcolor(4); break; //"Rock";
        case 30: asignarcolor(13); break;//"Pop";
        case 40: asignarcolor(2); break; //"Electronica"; 
        case 50: asignarcolor(3); break; //"Reggaeton";
        case 60: asignarcolor(6); break; //"Cumbia";
        default: break;  //si es desconocido
        }
        cout << "  [ID:" << id << "] \"" << nombre << "\" | "
            << obtenerNombreArtista(id_artista) << " | "
            << obtenerNombreAlbum(id_album) << " | "
            << obtenerNombreGenero(id_genero) << " | "
            << "Duracion: " << duracion << "s | "
            << "Reprod: " << reproducciones << endl;
    }

    string toString() const override {
        stringstream ss;
        ss << "CANCION," << id << "," << nombre << "," << id_artista << "," << id_album << "," << duracion << "," << id_genero << "," << reproducciones;
        return ss.str();
    }


};

// ============================================================
//  PLAYLIST 
//  Organiza canciones para un usuario. Depende de Cancion.
// ============================================================
class Playlist : public EntidadBase {
private:
    string descripcion;
    int usuarioId;
    ListaCircularDoble<Cancion*> canciones;
public:
    Playlist(int id, string nombre, string descripcion, int usuarioId)
        : EntidadBase(id, nombre), descripcion(descripcion), usuarioId(usuarioId) {
    }

    // --- Canciones de la playlist ---
    void agregarCancion(Cancion* c) { 
    
        //validar que la canción no esté agregada en la playlist
        if(canciones.getCabeza() != nullptr){
        Nodo<Cancion*>* temp = canciones.getCabeza();
        do {
            if (temp->dato->getId() == c->getId()) { return; }
            temp = temp->siguiente;
        } while (temp != canciones.getCabeza());
        //Una vez ya se haya validado, se agrega
        canciones.insertar(c); 
        }
        else {
            canciones.insertar(c);

        }
    }
    ListaCircularDoble<Cancion*>& getCanciones() { return canciones; }

    // --- Getters ---
    int getUsuarioId() const { return usuarioId; }
    string getNombre() {
        return nombre;
    }

    // --- Presentacion / persistencia ---
    void mostrarDetalles() const override {
        cout << '\n' << "Playlist: " << nombre << " | " << canciones.getTotal() << " canciones" << endl;
        vector<Cancion*> cancionesVector = canciones.toVector();
        for (Cancion* c : cancionesVector) {
            cout << '\n' << "-------------------------------------";
            cout << '\n' << '\t' << c->getNombre();
            cout << '\n' << "-------------------------------------";

        }
    }

    string toString() const override {
        stringstream ss;
        ss << "PLAYLIST," << id << "," << nombre << "," << descripcion << "," << usuarioId;
        return ss.str();
    }
};

// ============================================================
//  TABLA HASH DE PLAYLISTS 
//  Estructura de soporte para busqueda rapida de playlists por
//  nombre dentro de un Usuario. Depende de Playlist.
// ============================================================
class TablaHashPlaylist {
private:
    vector<Playlist*> tabla;
    vector<bool> ocupado;
    int tam;

    int hashFunction(const string& clave) {

        unsigned long h = 0;
        for (unsigned char c : clave) h = h * 31 + c;
        return static_cast<int>(h % tam);
    }

public:

    TablaHashPlaylist(int tam) : tam(tam) {
        tabla = vector<Playlist*>(tam, nullptr);
        ocupado.resize(tam, false);
    }

    void insertar(Playlist* pl) {
        int indice = hashFunction(pl->getNombre());
        int inicio = indice;
        while (ocupado[indice]) {
            if (tabla[indice]->getNombre() == pl->getNombre()) {
                tabla[indice] = pl;
                return;
            }
            indice = (indice + 1) % tam;
            if (indice == inicio) {
                cout << "Error: tabla llena" << '\n';
                return;
            }
        }
        tabla[indice] = pl;
        ocupado[indice] = true;
    }

    Playlist* buscar(string& nombre) {
        int indice = hashFunction(nombre);
        int inicio = indice;

        while (ocupado[indice]) {
            if (tabla[indice]->getNombre() == nombre) return tabla[indice];
            indice = (indice + 1) % tam;
            if (indice == inicio) break;
        }
        return nullptr;
    }

    void mostrar() {
        for (int i = 0; i < tam; i++) {
            if (ocupado[i]) {
                cout << i << ": " << tabla[i]->getNombre() << endl;
            }
            else {
                cout << i << ": [vacío]\n";
            }
        }
    }
};

// --- Estructura de soporte para Usuario (Suscripcion) ---

struct Suscripcion {
    string tipo;
    double precio;
    string fechaInicio;
    string fechaVencimiento;
    bool activa;

    bool isPremium() const { return tipo == "Premium" && activa; }
};

// ============================================================
//  USUARIO 
//  Due\u00f1o de playlists, favoritos, historial y suscripcion.
//  Depende de Cancion, Playlist y TablaHashPlaylist.
// ============================================================
class Usuario : public EntidadBase {
private:
    string email;
    string contrasena;
    bool tienePremium;
    Suscripcion plan;

    ListaDoble<Cancion*> misFavoritos;
    ListaDoble<Playlist*> misPlaylists;
    Pila<Cancion*> miHistorialPila;
    vector<Cancion*> historial;

    TablaHashPlaylist* playlistHash; // Para busqueda rapida de playlists por ID

    int nextPlaylistId;

    Cancion* cancionActual;

public:
    Usuario(int id, string nombre, string email, string contrasena)
        : EntidadBase(id, nombre), email(email), contrasena(contrasena),
        tienePremium(false), playlistHash(new TablaHashPlaylist(50)), nextPlaylistId(1), cancionActual(nullptr) {


    }

    ~Usuario() {
        for (Playlist* p : misPlaylists.toVector()) delete p;
        delete playlistHash;
    }

    // --- PLAYLISTS ---
    void recalcularNextPlaylistId() {
        vector<Playlist*> v = misPlaylists.toVector();
        for (Playlist* p : v) {
            int idLocal = p->getId() % 100;
            if (idLocal >= nextPlaylistId) nextPlaylistId = idLocal + 1;

        }
    }

    Playlist* crearPlaylist(string nombrePL, string descPL) {
        int pid = (id * 100) + nextPlaylistId++;
        Playlist* p = new Playlist(pid, nombrePL, descPL, id);
        misPlaylists.insertarAlFinal(p);
        playlistHash->insertar(p);

        return p;
    }

    void agregarPlaylist(Playlist* p) { misPlaylists.insertarAlFinal(p); playlistHash->insertar(p); }

    ListaDoble<Playlist*>& getPlaylists() { return misPlaylists; }

    Playlist* buscarPlaylistPorNombre(string nombre) const {
        return playlistHash->buscar(nombre);
    }

    Playlist* buscarPlaylistPorId(int idPlaylist) const {
        vector<Playlist*> v = misPlaylists.toVector();
        for (Playlist* p : v) {
            if (p->getId() == idPlaylist) return p;
        }
        return nullptr;
    }

    // --- REPRODUCCION ---
    void reproducirCancion(Cancion* c, int x, int y, int color) {
        if (!c) return;
        c->reproducir(x, y, color);
        registrarEnHistorial(c);
        cancionActual = c;
        c->setEnReproduccion(true);
    }

    // --- HISTORIAL ---
    // miHistorial es una Pila<Cancion*>: el tope siempre es la ultima cancion
    // reproducida, lo cual es justo lo que necesita "reproducir anterior".
    void registrarEnHistorial(Cancion* c) {
        if (c) miHistorialPila.push(c);
    }
    

    // --- PREMIUM / SUSCRIPCION ---
    bool esPremium() const { return tienePremium && plan.isPremium(); }
    void activarPremium(Suscripcion s) {
        plan = s;
        tienePremium = true;
    }

    Suscripcion getSuscripcion() const { return plan; }

    // --- DATOS DE CUENTA ---
    string getEmail() const { return email; }
    string getContrasena() const { return contrasena; }

    // --- FAVORITOS ---
    void agregarFavorito(Cancion* c) { misFavoritos.insertarAlFinal(c); }
    ListaDoble<Cancion*>& getFavoritos() { return misFavoritos; }

    // --- METODOS VIRTUALES ---
    void mostrarDetalles() const override {
        cout << "  [ID:" << id << "] " << nombre << " | " << email << endl;
        if (esPremium()) cout << "  Plan: Premium" << endl;
        else cout << "  Plan: Gratuito" << endl;
    }

    string toString() const override {
        stringstream ss;
        ss << "USUARIO," << id << "," << nombre << "," << email << "," << contrasena;
        return ss.str();
    }

    // --- HISTORIAL: acceso seguro sin copiar la Pila ---
    Cancion* verTopeHistorial() const {
        if (miHistorialPila.estaVacio()) return nullptr;
        return miHistorialPila.getTope();
    }

    void quitarTopeHistorial() {
        if (!miHistorialPila.estaVacio()) miHistorialPila.pop();
    }

    bool historialVacio() const { return miHistorialPila.estaVacio(); }
};

// ============================================================
//  PODCAST 
//  Entidad secundaria: por ahora no tiene menus propios en la UI.
// ============================================================
struct Episodio {
    string nombre;
    int duracion;
};

class Podcast : public EntidadBase {
private:
    string host;
    string descripcion;
    ListaDoble<Episodio> episodios;
    int reproducciones;
public:
    Podcast(int id, string nombre, string host, string descripcion)
        : EntidadBase(id, nombre), host(host), descripcion(descripcion), reproducciones(0) {
    }

    void agregarEpisodio(string nom, int dur) { episodios.insertarAlFinal({ nom, dur }); }
    string getHost() const { return host; }

    void reproducir() {
        reproducciones++;
        cout << "  >> Reproduciendo podcast: \"" << nombre << "\" | Host: " << host << endl;
    }

    void mostrarDetalles() const override {
        cout << "  [ID:" << id << "] Podcast: \"" << nombre << "\" | Host: " << host << endl;
    }

    string toString() const override {
        stringstream ss;
        ss << "PODCAST," << id << "," << nombre << "," << host << "," << descripcion;
        return ss.str();
    }


};