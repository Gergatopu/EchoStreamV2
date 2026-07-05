#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include "Entidades.h"
#include "Algoritmos.h"

using namespace std;

// ============================================================
//  PERSISTENCIA (helpers de archivo) 
// ============================================================
void guardarLinea(string archivo, string linea) {
    ofstream f(archivo, ios::app);
    if (f.is_open()) { f << linea << "\n"; f.close(); }
}

vector<string> partirCSV(const string& linea) {
    vector<string> campos;
    stringstream ss(linea);
    string campo;
    while (getline(ss, campo, ',')) campos.push_back(campo);

    if (!campos.empty() && !campos.back().empty() && campos.back().back() == '\r') {
        campos.back().pop_back();
    }
    return campos;
}


// ============================================================
//  Gestor
// ============================================================
class Gestor {
private:
    // --- Biblioteca ---
    //ListaDoble<Cancion*> catalogoCanciones;
    AVL<Cancion*> catalogoCanciones;

    // --- Usuarios / sesion ---
    ListaDoble<Usuario*> listaUsuarios;
    CGrafo<Usuario*> usuarios;
    Usuario* usuarioLogueado;

    // --- Cola de reproduccion (valida solo durante una sesion activa) ---
    Cola<Cancion*> cancionesEspera;

    // Cancion que esta sonando ahora mismo. Antes esto se inferia indirectamente
    // del frente de la cola o del tope del historial (de ahi parte de la confusion
    // entre Usuario y Gestor); ahora el Gestor es la unica fuente de verdad sobre
    // "que se esta reproduciendo", y coordina la cola (siguientes) y la pila de
    // historial del usuario (anteriores) alrededor de este puntero.
    Cancion* cancionActual = nullptr;



    // ---------------- CARGA DESDE ARCHIVO ----------------

    // Formato CANCION (8 campos):   CANCION, id, nombre, artistaId, albumId, duracion, generoId, reproducciones
    void cargarBibliotecaDesdeArchivo() {
        ifstream archivo("biblioteca.txt");
        if (!archivo.is_open()) return;

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            vector<string> campos = partirCSV(linea);
            if (campos.empty()) continue;
            const string& tag = campos[0];

            if (tag == "CANCION" && campos.size() >= 8) {

                // 1. Creamos la canción
                Cancion* nuevaCancion = new Cancion(
                    stoi(campos[1]), campos[2], stoi(campos[3]), stoi(campos[4]),
                    stoi(campos[5]), stoi(campos[6]), stoi(campos[7])
                );

                // 2. Definimos la lambda
                auto lambdaComparar = [](Cancion* a, Cancion* b) { return a->getId() < b->getId(); };

                // 3. Insertamos y actualizamos la raíz en una sola línea al AVL
                catalogoCanciones.setRaiz(
                    catalogoCanciones.insertar(catalogoCanciones.getRaiz(), nuevaCancion, lambdaComparar)
                );
            }
        }
        archivo.close();
    }

    // --- Helpers de carga de usuarios.txt: uno por tipo de linea ---

    void procesarLineaUsuario(const vector<string>& campos) {
        if (campos.size() < 5) return;
        Usuario* nuevo = new Usuario(stoi(campos[1]), campos[2], campos[3], campos[4]);
        listaUsuarios.insertarAlFinal(nuevo);

        usuarios.adicionarVertice(nuevo);
        nuevo->setIndiceGrafo(usuarios.cantidadVertices() - 1);
    }

    void procesarLineaSuscripcion(const vector<string>& campos) {
        // SUSCRIPCION, idSuscripcion, idUsuario, nombrePlan, tipo, precio, fechaInicio, fechaVencimiento, activa
        if (campos.size() < 9) return;
        Usuario* due = buscarUsuarioPorId(stoi(campos[2]));
        if (due) due->activarPremium({ campos[4], stod(campos[5]), campos[6], campos[7], campos[8] == "1" });
    }

    void procesarLineaPlaylist(const vector<string>& campos) {
        // PLAYLIST, id, nombre, descripcion, idUsuario
        if (campos.size() < 5) return;
        Usuario* due = buscarUsuarioPorId(stoi(campos[4]));
        if (due) {
            due->agregarPlaylist(new Playlist(stoi(campos[1]), campos[2], campos[3], stoi(campos[4])));
            // Evita que el contador interno de IDs choque con playlists ya existentes.
            due->recalcularNextPlaylistId();
        }
    }

    void procesarLineaPlaylistCancion(const vector<string>& campos) {
        // PLAYLIST_CANCION, idPlaylist, idCancion
        if (campos.size() < 3) return;
        int idPlaylist = stoi(campos[1]);
        int idCancion = stoi(campos[2]);

        // Usuario::crearPlaylist genera el ID como (idUsuario * 100) + n,
        // asi que podemos ubicar al dueño directamente sin recorrer a todos los usuarios.
        Usuario* due = buscarUsuarioPorId(idPlaylist / 100);
        if (!due) return;

        Playlist* pl = due->buscarPlaylistPorId(idPlaylist);
        Cancion* c = buscarCancionId(idCancion);
        if (pl && c) pl->agregarCancion(c);
    }

    void cargarUsuariosDesdeArchivo() {
        ifstream archivo("usuarios.txt");
        if (!archivo.is_open()) return;

        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;

            vector<string> campos = partirCSV(linea);
            if (campos.empty()) continue;
            const string& tag = campos[0];

            if (tag == "USUARIO") procesarLineaUsuario(campos);
            else if (tag == "SUSCRIPCION") procesarLineaSuscripcion(campos);
            else if (tag == "PLAYLIST") procesarLineaPlaylist(campos);
            else if (tag == "PLAYLIST_CANCION") procesarLineaPlaylistCancion(campos);
            else if (tag == "FAVORITO") procesarLineaFavorito(campos);
            else if (tag == "AMIGO") procesarLineaAmigo(campos);
        }
        archivo.close();
    }

    // --- Historial (antes GestionArchivos.h / GestorArchivos) ---

    // Carga la pila de historial del Usuario. Se llama una sola vez, al iniciar sesion.
    void cargarHistorialUsuario(Usuario* u) {
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
                    if (!fecha.empty() && fecha.back() == '\r') fecha.pop_back();

                    // miHistorial ahora es Pila<Cancion*>, asi que resolvemos
                    // el titulo guardado en el archivo a la Cancion* real del catalogo.
                    if (tipo == "Cancion") {
                        for (Cancion* c : catalogoCanciones.toVector()) {
                            if (c->getNombre() == titulo) {
                                u->registrarEnHistorial(c);
                                break;
                            }
                        }
                    }
                }
            }
        }
        archivo.close();
    }

    // Historial de un usuario ya resuelto a punteros Cancion* (para recomendaciones).
    // Antes vivia en GestorArchivos y recibia el catalogo como parametro
    // (catalogoCanciones.toVector()); ahora que es metodo de esta misma clase,
    // usa catalogoCanciones directamente.
    vector<Cancion*> obtenerHistorialCanciones(int idUsuario) {
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
                if (stoi(userIdStr) == idUsuario) {
                    getline(ss, histIdStr, ',');
                    getline(ss, titulo, ',');
                    getline(ss, tipo, ',');
                    getline(ss, artista, ',');

                    getline(ss, fecha);
                    if (!fecha.empty() && fecha.back() == '\r') fecha.pop_back();

                    // Solo procesamos registros de tipo Cancion
                    if (tipo == "Cancion") {
                        for (Cancion* c : catalogoCanciones.toVector()) {
                            if (c->getNombre() == titulo) { historial.push_back(c); break; }
                        }
                    }
                }
            }
        }
        archivo.close();
        return historial;
    }

    bool cancionEscuchada(int idCancion, int idUsuario) {
        for (Cancion* c : obtenerHistorialCanciones(idUsuario)) {
            if (idCancion == c->getId()) return true;
        }
        return false;
    }

    // --- Helpers de favoritos (menor prioridad) ---

    void procesarLineaFavorito(const vector<string>& campos) {
        // FAVORITO, idUsuario, idCancion
        if (campos.size() < 3) return;
        Usuario* due = buscarUsuarioPorId(stoi(campos[1]));
        Cancion* c = buscarCancionId(stoi(campos[2]));
        if (due && c) due->agregarFavorito(c);
    }

    void procesarLineaAmigo(const vector<string>& campos) {
        // AMIGO, idUsuario, idAmigo
        if (campos.size() < 3) return;
        Usuario* due = buscarUsuarioPorId(stoi(campos[1]));
        Usuario* amigo = buscarUsuarioPorId(stoi(campos[2]));
        if (due && amigo) due->agregarAmigo(amigo);
    }

    static bool compararPorNombre(Cancion* a, Cancion* b) {
        return a->getNombre() < b->getNombre();
    }

    static bool compararPorDuracion(Cancion* a, Cancion* b) {
        return a->getDuracion() < b->getDuracion();
    }

public:
    struct Recomendacion {
        double distancia;
        Cancion* cancion;

        bool operator>(const Recomendacion& otra) const { return distancia > otra.distancia; }
        bool operator<(const Recomendacion& otra) const { return distancia < otra.distancia; }
    };

    Gestor() : usuarioLogueado(nullptr) {
        cargarBibliotecaDesdeArchivo();
        cargarUsuariosDesdeArchivo();
        // Las aristas de amistad no se guardan en disco (serian datos derivados
        // y quedarian obsoletas apenas alguien escuche una cancion nueva). En
        // su lugar se recalculan aqui, una vez que ya estan todos los usuarios
        // y sus relaciones de amistad (AMIGO) cargados en memoria.
        reconstruirGrafoAmistades();
    }

    ~Gestor() {
        catalogoCanciones.vaciar();
        listaUsuarios.vaciar();
        cancionesEspera.vaciar();
    }

    // ---------------- USUARIOS / SESION ----------------

    Usuario* buscarUsuarioPorId(int id) {
        for (Usuario* u : listaUsuarios.toVector()) { if (u->getId() == id) return u; }
        return nullptr;
    }

    // Devuelve true/false; no imprime nada (la UI decide el mensaje).
    bool iniciarSesion(string email, string password) {
        vector<Usuario*> v = listaUsuarios.toVector();

        auto credencialesValidas = [email, password](Usuario* u) {
            return u->getEmail() == email && u->getContrasena() == password;
            };

        for (Usuario* u : v) {
            if (credencialesValidas(u)) {
                usuarioLogueado = u;
                cargarHistorialUsuario(usuarioLogueado);
                return true;
            }
        }

        return false;
    }

    void cerrarSesion() {
        usuarioLogueado = nullptr;
        cancionesEspera.vaciar(); // la cola era propia de la sesion (antes vivia y moria con GestorReproduccion)
        cancionActual = nullptr;
    }

    // Solo logica de registro/persistencia. La UI imprime los mensajes de exito.
    void registrarNuevoUsuario(int id, string nom, string email, string pass, int plan) {
        Usuario* nuevo = new Usuario(id, nom, email, pass);
        listaUsuarios.insertarAlFinal(nuevo);

        // Sin esto, un usuario creado en tiempo de ejecucion (no cargado desde
        // usuarios.txt) se queda con indiceGrafo == -1 y jamas podria formar
        // aristas de amistad, aunque agregue amigos despues.
        usuarios.adicionarVertice(nuevo);
        nuevo->setIndiceGrafo(usuarios.cantidadVertices() - 1);

        guardarLinea("usuarios.txt", nuevo->toString());

        if (plan == 2) {
            nuevo->activarPremium({ "Premium", 29.90, obtenerFechaHoy(), "2026-12-31", true });
            guardarLinea("usuarios.txt", "SUSCRIPCION,1001," + to_string(id) + ",PlanPremium,Premium,29.90,Hoy,2026-12-31,1");
        }
    }

    // Activa el plan Premium de un usuario y persiste el cambio.
    void activarPlanPremium(Usuario* u) {
        if (!u) return;
        u->activarPremium({ "Premium", 29.90, obtenerFechaHoy(), "2026-12-31", true });
        guardarLinea("usuarios.txt", "SUSCRIPCION,999," + to_string(u->getId()) + ",PlanPremium,Premium,29.90,Hoy,2026-12-31,1");
    }

    Usuario* getUsuarioLogueado() { return usuarioLogueado; }

    vector<Usuario*> getUsuarios() {
        return listaUsuarios.toVector();
    }



    // --- Amigos ---
    void agregarAmigo(int idNuevo) {
        Usuario* amigo = buscarUsuarioPorId(idNuevo);
        if (!amigo) {
            ubicar(25, 25); cout << "No existe un usuario con ese ID" << endl;
            return;
        }
        if (amigo->getId() == usuarioLogueado->getId()) {
            ubicar(25, 25); cout << "No puedes agregarte a ti mismo como amigo" << endl;
            return;
        }

        // getAmigos() ahora devuelve una referencia al vector real del Usuario,
        // asi que esta comprobacion consulta el estado verdadero (no una copia).
        for (Usuario* u : usuarioLogueado->getAmigos()) {
            if (u->getId() == idNuevo) {
                ubicar(25, 25); cout << "Ya tienes a " << amigo->getNombre() << " como amigo" << endl;
                return;
            }
        }

        usuarioLogueado->agregarAmigo(amigo);
        guardarLinea("usuarios.txt", "AMIGO," + to_string(usuarioLogueado->getId()) + "," + to_string(amigo->getId()));
        crearAristaAmistad(usuarioLogueado, amigo);

        ubicar(25, 25); cout << amigo->getNombre() << " agregado correctamente como amigo" << endl;
    }

    // ---------------- COLA DE REPRODUCCION ----------------

    // Devuelve la cancion encolada, o nullptr si no existe. La UI imprime el resultado.
    Cancion* agregarCancionAFila(int idCancion) {
        Cancion* c = buscarCancionId(idCancion);
        if (c) cancionesEspera.encolar(c);
        return c;
    }

    void reproducirAnterior() {
        if (!usuarioLogueado) return;

        // El tope de la pila es la cancion anterior. Si no hay historial, no hacemos nada.
        Cancion* previa = usuarioLogueado->verTopeHistorial();
        if (previa == nullptr) return;
        usuarioLogueado->quitarTopeHistorial();


        if (cancionActual != nullptr) {
            cancionActual->setEnReproduccion(false);
        }

        cancionActual = previa;
        cancionActual->setEnReproduccion(true);
    }

    // Alterna play/pausa de la cancion que esta sonando actualmente.
    void togglePlayPause() {
        if (cancionActual == nullptr) return;
        cancionActual->setEnReproduccion(!cancionActual->getEnReproduccion());
    }

    // Avanza a la siguiente cancion de la cola. La que estaba sonando
    // (cancionActual) pasa al historial (pila) del usuario.
    void reproducirSiguiente() {
        if (cancionesEspera.estaVacia() || !usuarioLogueado) return;

        Cancion* siguiente = cancionesEspera.verFrente();
        cancionesEspera.desencolar();

        if (cancionActual != nullptr) {
            cancionActual->setEnReproduccion(false);
            usuarioLogueado->registrarEnHistorial(cancionActual);

            guardarLinea("historial.txt", "HIST," + to_string(usuarioLogueado->getId()) + ",999," +
                cancionActual->getNombre() + ",Cancion," + obtenerNombreArtista(cancionActual->getArtista()) + "," + obtenerHoraActual());
        }

        cancionActual = siguiente;
        cancionActual->setEnReproduccion(true);
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
    vector<Cancion*> obtenerCola() { return cancionesEspera.toVector(); }

    vector<Cancion*> obtenerHistorial(int id) { return obtenerHistorialCanciones(id); }

    Cancion* getCancionActual() { return cancionActual; }

    // ---------------- BIBLIOTECA ----------------
    Cancion* buscarCancionId(int id) {
        return catalogoCanciones.binarySearchID(catalogoCanciones.getRaiz(), id);
    }

    vector<Cancion*> getCatalogoCanciones() { return catalogoCanciones.toVector(); }

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

    // Catalogo ordenado por duracion ascendente (MERGE SORT)
    vector<Cancion*> obtenerCancionesPorDuracion() {
        vector<Cancion*> vista = catalogoCanciones.toVector();
        Algoritmos::mergeSort(vista, 0, (int)vista.size() - 1, compararPorDuracion);
        return vista;
    }

    // ---------------- RECOMENDACIONES ----------------

    vector<double> calcularPreferencias(int idUsuario) {
        double generoProm = 0, artistaProm = 0, duracionProm = 0, albumProm = 0;
        vector<Cancion*> historial = obtenerHistorialCanciones(idUsuario);

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



    // Calcula, ordena (Heap Sort) y filtra (distancia <= 80 y no escuchadas).
    // No imprime nada: la UI decide como mostrar el resultado.
    vector<Recomendacion> obtenerRecomendaciones(vector<double> preferenciasUsuario, int idUsuario) {
        vector<Cancion*> canciones = catalogoCanciones.toVector();
        vector<Recomendacion> listaDistancias;
        if (canciones.empty()) return listaDistancias;

        for (Cancion* cancion : canciones) {
            double dist = DistanciaEuclidiana(preferenciasUsuario, cancion->obtenerVectorComponentes());
            listaDistancias.push_back({ dist, cancion });
        }

        Algoritmos::heapSort(listaDistancias);

        vector<Recomendacion> resultado;
        for (Recomendacion& r : listaDistancias) {
            if (r.distancia <= 80 && !cancionEscuchada(r.cancion->getId(), idUsuario)) {
                resultado.push_back(r);
            }
        }
        return resultado;
    }


    void crearAristaAmistad(Usuario* a, Usuario* b) {
        if (!a || !b) return;
        int iA = a->getIndiceGrafo();
        int iB = b->getIndiceGrafo();
        if (iA < 0 || iB < 0) return;

        vector<double> vecA = calcularPreferencias(a->getId());
        vector<double> vecB = calcularPreferencias(b->getId());
        double afinidad = DistanciaEuclidiana(vecA, vecB);

        usuarios.adicionarArco(iA, iB, afinidad);
        usuarios.adicionarArco(iB, iA, afinidad);
    }

    double obtenerAfinidadDesdeIndice(int iA, int iB) {
        Usuario* uA = usuarios.obtenerVertice(iA);
        Usuario* uB = usuarios.obtenerVertice(iB);
        if (!uA || !uB) return -1;

        vector<double> vecA = calcularPreferencias(uA->getId());
        vector<double> vecB = calcularPreferencias(uB->getId());
        return DistanciaEuclidiana(vecA, vecB);
    }

    // Reconstruye todas las aristas de amistad a partir de las listas de
    // amigos ya cargadas (cada relacion AMIGO,A,B basta una vez: crearAristaAmistad
    // ya agrega el arco en ambos sentidos del grafo).
    void reconstruirGrafoAmistades() {
        for (Usuario* u : listaUsuarios.toVector()) {
            for (Usuario* amigo : u->getAmigos()) {
                crearAristaAmistad(u, amigo);
            }
        }
    }

    // Par (usuario, afinidad) para representar una conexion en la lista de adyacencia
    struct Conexion {
        Usuario* usuario;
        double afinidad;
    };

    // Lista de adyacencia de un solo vertice: con quienes esta conectado y su afinidad
    vector<Conexion> obtenerConexiones(Usuario* u) {
        vector<Conexion> resultado;
        if (!u) return resultado;
        int i = u->getIndiceGrafo();
        if (i < 0) return resultado;

        int total = usuarios.cantidadArcos(i);
        for (int k = 0; k < total; k++) {
            int destino = usuarios.obtenerVerticeLlegada(i, k);
            double afinidad = usuarios.obtenerArco(i, k);
            resultado.push_back({ usuarios.obtenerVertice(destino), afinidad });
        }
        return resultado;
    }

    // Lista de adyacencia de TODO el grafo: cada usuario junto con sus conexiones
    vector<pair<Usuario*, vector<Conexion>>> obtenerListaAdyacenciaCompleta() {
        vector<pair<Usuario*, vector<Conexion>>> resultado;
        int total = usuarios.cantidadVertices();
        for (int i = 0; i < total; i++) {
            Usuario* u = usuarios.obtenerVertice(i);
            resultado.push_back({ u, obtenerConexiones(u) });
        }
        return resultado;
    }

    // El "soulmate" es el amigo con la MENOR distancia euclidiana de gustos
    // musicales (menor distancia = mayor afinidad). nullptr si no tiene amigos.
    Usuario* obtenerSoulmate(Usuario* u) {
        if (!u) return nullptr;
        int i = u->getIndiceGrafo();
        if (i < 0) return nullptr;

        int totalArcos = usuarios.cantidadArcos(i);
        if (totalArcos == 0) return nullptr;

        int mejorDestino = -1;
        double mejorAfinidad = -1;
        for (int k = 0; k < totalArcos; k++) {
            double afinidad = usuarios.obtenerArco(i, k);
            if (mejorDestino == -1 || afinidad < mejorAfinidad) {
                mejorAfinidad = afinidad;
                mejorDestino = usuarios.obtenerVerticeLlegada(i, k);
            }
        }
        if (mejorDestino == -1) return nullptr;
        return usuarios.obtenerVertice(mejorDestino);
    }

    // ---------------- FAVORITOS ----------------

    // Devuelve los favoritos de un usuario ordenados A-Z (QUICK SORT).
    static vector<Cancion*> obtenerFavoritosOrdenados(Usuario* usuario) {
        vector<Cancion*> favs = usuario->getFavoritos().toVector();
        Algoritmos::quickSort(favs, 0, (int)favs.size() - 1, Gestor::compararPorNombre);
        return favs;
    }
};