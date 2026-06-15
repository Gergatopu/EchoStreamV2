#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "vector"
#include "EstructurasDatos.h"
#include "Utilidades.h" 

using namespace std;

// CLASE ABSTRACTA BASE 
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

// ESTRUCTURAS LIGERAS ESENCIALES
struct EntradaHistorial {
    string titulo;
    string tipo;
    string artista;
    string fechaHora;
};

struct Suscripcion {
    string tipo;
    double precio;
    string fechaInicio;
    string fechaVencimiento;
    bool activa;

    bool isPremium() const { return tipo == "Premium" && activa; }
};

struct Episodio {
    string nombre;
    int duracion;
};

// ENTIDADES DE NEGOCIO 
class Cancion : public EntidadBase {
private:
    int id_artista;
    int id_album;
    int id_genero;

    int duracion;
    int reproducciones;

	//El sistema de recomendacion usara los siguientes parámetros para sugerir canciones similares: 
    // { artista, duracion, genero, album }

public:
    Cancion(int id, string nombre, int id_artista, int id_album, int duracion, int id_genero, int repro = 0)
        : EntidadBase(id, nombre), id_artista(id_artista), id_album(id_album),
        duracion(duracion), id_genero(id_genero), reproducciones(repro) {
    }

    int getArtista() const { return id_artista; }
    int getAlbum() const { return id_album; }
    int getGenero() const { return id_genero; }
    int getDuracion() const { return duracion; }
    int getReproducciones() const { return reproducciones; }

    
    void reproducir() {
        reproducciones++;
        cout << "  >> Reproduciendo: \"" << nombre << "\" - " << obtenerNombreArtista(id_artista) << endl;
    }
    


    void mostrarDetalles() const override {
        cout << "  [ID:" << id << "] \"" << nombre << "\" | "
            << obtenerNombreArtista(id_artista) << " | "
            << obtenerNombreAlbum(id_album) << " | "
            << obtenerNombreGenero(id_genero) << " | "
            << "Duracion: " << duracion << "s | "
            << "Reprod: " << reproducciones << endl;
	}

    vector<double> obtenerVectorComponentes() {

        vector<double> cancionComponentes = {
            (double)id_artista,
            (double)duracion,
            (double)id_genero,
            (double)id_album
        };
        return cancionComponentes;
    }

    string toString() const override {
        stringstream ss;
        ss << "CANCION," << id << "," << nombre << "," << id_artista << "," << id_album << "," << duracion << "," << id_genero << "," << reproducciones;
        return ss.str();
    }
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

class Playlist : public EntidadBase {
private:
    string descripcion;
    int usuarioId;
    ListaCircularDoble<Cancion*> canciones;
public:
    Playlist(int id, string nombre, string descripcion, int usuarioId)
        : EntidadBase(id, nombre), descripcion(descripcion), usuarioId(usuarioId) {
    }

    void agregarCancion(Cancion* c) { canciones.insertar(c); }
    ListaCircularDoble<Cancion*>& getCanciones() { return canciones; }
    int getUsuarioId() const { return usuarioId; }

    void mostrarDetalles() const override {
        cout << "  [ID:" << id << "] Playlist: \"" << nombre
            << "\" | " << canciones.getTotal() << " canciones" << endl;
    }

    string toString() const override {
        stringstream ss;
        ss << "PLAYLIST," << id << "," << nombre << "," << descripcion << "," << usuarioId;
        return ss.str();
    }
};

class Usuario : public EntidadBase {
private:
    string email;
    string contrasena;
    bool tienePremium;
    Suscripcion plan;

    ListaDoble<Cancion*> misFavoritos;
    ListaDoble<Playlist*> misPlaylists;
    Pila<EntradaHistorial> miHistorial;
    
    int nextPlaylistId;

public:
    Usuario(int id, string nombre, string email, string contrasena)
        : EntidadBase(id, nombre), email(email), contrasena(contrasena),
        tienePremium(false), nextPlaylistId(1) {
    }

    void recalcularNextPlaylistId() {
        vector<Playlist*> v = misPlaylists.toVector();
        for (Playlist* p : v) {
            int idLocal = p->getId() % 100;
            if (idLocal >= nextPlaylistId) nextPlaylistId = idLocal + 1;
        }
    }

    string getEmail() const { return email; }
    string getContrasena() const { return contrasena; }

    bool esPremium() const { return tienePremium && plan.isPremium(); }
    void activarPremium(Suscripcion s) {
        plan = s;
        tienePremium = true;
    }

    Suscripcion getSuscripcion() const { return plan; }

    // --- FAVORITOS ---
    void agregarFavorito(Cancion* c) { misFavoritos.insertarAlFinal(c); }
    ListaDoble<Cancion*>& getFavoritos() { return misFavoritos; }

    // --- HISTORIAL ---
    void registrarEnHistorial(string titulo, string tipo, string artista) {
        miHistorial.push({ titulo, tipo, artista, obtenerHoraActual() });
    }

    void cargarEnHistorial(string titulo, string tipo, string artista, string fechaOriginal) {
        miHistorial.push({ titulo, tipo, artista, fechaOriginal });
    }

    void mostrarHistorial() const {
        cout << "\n--- Historial de Reproduccion de " << nombre << " ---" << endl;
        vector<EntradaHistorial> historial = miHistorial.toVector();
        if (historial.empty()) {
            cout << "  (Historial vacio - reproduce algo primero)" << endl;
            return;
        }
        for (const auto& h : historial) {
            cout << "  [" << h.tipo << "] " << h.titulo << " - " << h.artista << " (" << h.fechaHora << ")" << endl;
        }
    }

    // --- PLAYLISTS ---
    Playlist* crearPlaylist(string nombrePL, string descPL) {
        int pid = (id * 100) + nextPlaylistId++;
        Playlist* p = new Playlist(pid, nombrePL, descPL, id);
        misPlaylists.insertarAlFinal(p);
        return p;
    }

    void agregarPlaylist(Playlist* p) { misPlaylists.insertarAlFinal(p); }
    ListaDoble<Playlist*>& getPlaylists() { return misPlaylists; }

    Playlist* buscarPlaylistPorId(int pid) const {
        vector<Playlist*> v = misPlaylists.toVector();
        for (Playlist* p : v) {
            if (p->getId() == pid) return p;
        }
        return nullptr;
    }

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
	
};