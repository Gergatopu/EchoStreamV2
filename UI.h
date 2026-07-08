#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Gestores.h"
#include "chrono"

using namespace std;
using namespace System;



void pausar() {
    asignarcolor(8);
    cout << "\n  Presiona Enter para continuar...";
    cin.ignore(); cin.get();
    asignarcolor(7);
}

void cabecera(string titulo) {
    system("cls");
    asignarcolor(14); cout << "========================================" << endl;
    asignarcolor(7);  cout << "  ECHOSTREAM | " << titulo << endl;
    asignarcolor(14); cout << "========================================" << endl;
    asignarcolor(7);
}

// --- IMPRESION DE LISTAS DE CANCIONES (sin logica, solo presentacion) ---
void imprimirCanciones(const vector<Cancion*>& canciones, string mensajeVacio = "  (Sin canciones)") {
    if (canciones.empty()) { cout << mensajeVacio << endl; return; }
    for (Cancion* c : canciones) c->mostrarDetalles();
}

void mostrarCancionesOrdenadasA_Z(Gestor* lib) {
    cout << "\n  [Insercion] Canciones (A-Z):" << endl;
    imprimirCanciones(lib->obtenerCancionesOrdenadasAZ());
}

void mostrarTopReproducciones(Gestor* lib) {
    cout << "\n  [Shell Sort] Top Reproducciones:" << endl;
    imprimirCanciones(lib->obtenerTopReproducciones());
}

void mostrarCancionesPorDuracion(Gestor* lib) {
    cout << "\n  [Merge Sort] Canciones por Duracion:" << endl;
    imprimirCanciones(lib->obtenerCancionesPorDuracion());
}

void mostrarSoloCanciones(Gestor* lib) {
    cout << "\n--- CANCIONES DISPONIBLES ---" << endl;
    imprimirCanciones(lib->getCatalogoCanciones());
}

// --- IMPRESION DE LA COLA DE REPRODUCCION ---
void mostrarCola(Gestor* repro) {
    vector<Cancion*> vista = repro->obtenerCola();
    cout << "\n--- Cola (" << vista.size() << ") ---" << endl;
    for (Cancion* c : vista) cout << "  -> " << c->getNombre() << endl;
}

// ============================================================
//    1. STREAMING (pantalla principal, reproduccion)
//    2. PLAYLISTS
//    3. EXPLORAR BIBLIOTECA
//    4. AGREGAR A COLA
//    5. FAVORITOS
//    6. SUSCRIPCION
//    7. ACCESO (login / registro)
// ============================================================
class AppUI {
private:
    Gestor* gestor;

    Usuario* actual = nullptr;

    // ============================================================
    //  STREAMING 
    // ============================================================
    void mostrarCabeceraStreaming() {
        system("cls");

        string borde = "==========================================================";

        asignarcolor(14);
        Console::SetCursorPosition(35, 3);  cout << borde;

        Console::SetCursorPosition(35, 4);
        asignarcolor(14); cout << "| ";
        asignarcolor(7);  cout << "               ECHOSTREAM PLAYER                ";
        asignarcolor(14); cout << " |";

        string nombre = actual->getNombre();
        string tipo = actual->esPremium() ? "Premium" : "Gratuito";

        string contenido = " Usuario: " + nombre + " | Estado: " + tipo;

        int anchoInterior = 54;
        int espaciosNecesarios = anchoInterior - contenido.length();

        Console::SetCursorPosition(35, 5);
        asignarcolor(14); cout << "| ";
        asignarcolor(7);  cout << contenido;

        for (int i = 0; i < espaciosNecesarios; i++) cout << " ";

        asignarcolor(14); cout << "|";

        Console::SetCursorPosition(35, 6);
        cout << borde;

        asignarcolor(7);
    }

    void dibujarNotaEnCuadrado(int x, int y, int color) {
        asignarcolor(color); Console::SetCursorPosition(x, y);      cout << "+------------------------------+";
        asignarcolor(color); Console::SetCursorPosition(x, y + 1);  cout << "|                              |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 2);  cout << "|       ;;;;;;;;;;;;;;;;;;;;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 3);  cout << "|       ;;;;;;;;;;;;;;;;;;;;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 4);  cout << "|       ;                  ;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 5);  cout << "|       ;                  ;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 6);  cout << "|       ;                  ;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 7);  cout << "|       ;                  ;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 8);  cout << "|       ;                  ;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 9);  cout << "|       ;                  ;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 10); cout << "|       ;                  ;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 11); cout << "|  ,;;;;;             ,;;;;;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 12); cout << "|  ;;;;;;             ;;;;;;   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 13); cout << "|  `;;;;'             `;;;;'   |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 14); cout << "|                              |";
        asignarcolor(color); Console::SetCursorPosition(x, y + 15); cout << "+------------------------------+";
    }

    void verCancionActual(int x, int y, int color) {
        ubicar(x, y); asignarcolor(color); gestor->togglePlayPause();

    }

    void reproducirSiguienteUI() {
        gestor->reproducirSiguiente();
        if (gestor->getCancionActual() == nullptr) { ubicar(50, 25); asignarcolor(5); cout << "  La cola esta vacia.                 " << endl; pausar(); }
    }

    void reproducirAnteriorUI() {
        gestor->reproducirAnterior();
        if (gestor->getCancionActual() == nullptr) { ubicar(50, 25); asignarcolor(5); cout << "Historial vacio. " << endl; pausar(); }
    }

    void gestionarCola() {
        cabecera("GESTIONAR COLA");
        mostrarCola(gestor);

        cout << "\n";
        int currentY = Console::CursorTop;

        vector<string> opcionesCola = {
            "Mezclar aleatoriamente (Shuffle recursivo)",
            "Volver",
            "Agregar cancion a la cola"
        };
        int sub = menuInteractivo(opcionesCola, 5, currentY + 1) + 1;

        if (sub == 1) {
            if (gestor->modoAleatorio()) {
                cout << '\n' << '\n' << "--- Cola mezclada aleatoriamente ---" << endl;
                mostrarCola(gestor);
            }
        }
        else if (sub == 3) {
            menuAgregarACola();
            return;
        }
        pausar();
    }

    void mostrarHistorialUI() {
        cabecera("MI HISTORIAL");
        cout << "\n--- Historial de Reproduccion de " << actual->getNombre() << " ---" << endl;
        vector<Cancion*> historial = gestor->obtenerHistorial(actual->getId());
        if (historial.empty()) {
            cout << "  (Historial vacio - reproduce algo primero)" << endl;
            return;
        }
        for (Cancion* c : historial) {
            c->mostrarDetalles();
        }
        pausar();
    }

    void mostrarRecomendaciones() {
        int idUsu = actual->getId();
        vector<double> perfilUsuario = gestor->calcularPreferencias(idUsu);
        vector<Gestor::Recomendacion> recomendaciones = gestor->obtenerRecomendaciones(perfilUsuario, idUsu);

        cout << "\n--- TUS RECOMENDACIONES ---" << endl;
        if (recomendaciones.empty()) {
            cout << "  (Sin recomendaciones disponibles por ahora)" << endl;
        }
        else {
            for (Gestor::Recomendacion& r : recomendaciones) {

                if (r.distancia <= 25) { asignarcolor(7); }

                else if (r.distancia <= 50 && r.distancia > 25) {
                    asignarcolor(8);
                }
                else if (r.distancia > 50) {
                    asignarcolor(15);
                }

                cout << " [Distancia: " << r.distancia << "]" << '\t' << "->";
                r.cancion->mostrarDetalles();
            }
        }
        pausar();
    }

    void verPlaylist(string playlistNombre) {
        ubicar(5, 8);
        Playlist* pl = actual->buscarPlaylistPorNombre(playlistNombre);
        if (!pl) {
            cabecera("PLAYLIST");
            cout << "  Playlist \"" << playlistNombre << "\" no encontrada." << endl;
            pausar();
            return;
        }

        cout << '\t' << '\t' << "PLAYLIST: " << pl->getNombre() << endl << endl << endl;
        for (Cancion* c : pl->getCanciones().toVector()) {
            c->mostrarDetalles();
        }

        cout << "\n";
        int currentY = Console::CursorTop;

        vector<string> opcionesPlaylist = {
            "Reproducir playlist",
            "Agregar Cancion",
            "Volver"
        };
        int sub = menuInteractivo(opcionesPlaylist, 5, currentY + 1) + 1;

        if (sub == 1) {
            //agregar todas las canciones de la playlist a la cola
            agregarPlaylistCola(pl);
            //reproducir siguiente
            reproducirSiguienteUI();
        }
        else if (sub == 2) {
            mostrarSoloCanciones(gestor);
            int idC;
            cout << "\nID de la cancion: "; asignarcolor(14); cin >> idC; asignarcolor(7);

            agregarCancionAPlaylist(idC, pl);
        }
    }

    void menuStreaming() {
        bool salir = false;
        while (!salir) {
            mostrarCabeceraStreaming();
            Console::CursorVisible = false;
            dibujarNotaEnCuadrado(49, 8, 6);
            if (gestor->getCancionActual() != nullptr) {
                if (gestor->getCancionActual()->getEnReproduccion()) {
                    ubicar(50, 25); asignarcolor(5); cout << "Reproduciendo: " << gestor->getCancionActual()->getNombre(); asignarcolor(7);
                }
                else if (gestor->getCancionActual()->getEnReproduccion() == false || gestor->getCancionActual() == nullptr) {
                    ubicar(50, 25); asignarcolor(5); cout << "PAUSA: " << gestor->getCancionActual()->getNombre(); asignarcolor(7);
                }

            }
            else {
                ubicar(50, 25); asignarcolor(5); cout << "Reproduce algo para empezar "; asignarcolor(7);
            }

            vector<vector<string>> opcionesMenuPrincipal = {
                { "Cerrar Sesion" , ""   , ""  , ""      , "MIX/Essentials",     },
                { ""        , ""   , ""  , ""      , "Tus Me Gusta"  },
                { ""        , ""   , ""  , ""      , "Recomendacion Semanal"  },
                { ""        , ""   , ""  , ""      , "Amigos"  },
                { ""        , ""   , ""  , ""      , "PLAYLISTS +"  },
                { "Historial"  , "|<" , "o" , ">|"    , "COLA"        },
            };

            pair<int, int> sel = menuInteractivoGrid(opcionesMenuPrincipal, 14, 9, 23, 4);
            int fila = sel.first, columna = sel.second;

            // Acciones segun la fila seleccionada
            if (fila == 0) {
                if (columna == 0) {
                    salir = true;
                }
                else if (columna == 4) {
                    system("cls");
                    verPlaylist(opcionesMenuPrincipal[0][4]);
                    pausar();
                }
            }
            else if (fila == 1) {
                if (columna == 4) {
                    menuFavoritos();
                }
            }
            else if (fila == 2) {
                if (columna == 4) {
                    system("cls");
                    mostrarRecomendaciones();
                };
            }
            else if (fila == 3) {
                if (columna == 4) {
                    menuAmigos();
                };
            }
            else if (fila == 4) {
                if (columna == 4) { menuPlaylists(); };
            }
            else if (fila == 5) {
                if (columna == 0) { mostrarHistorialUI(); }
                else if (columna == 1) { reproducirAnteriorUI(); }
                else if (columna == 2) {
                    if (gestor->getCancionActual() != nullptr) {
                        verCancionActual(25, 25, 6);
                    }
                }
                else if (columna == 3) { reproducirSiguienteUI(); }
                else if (columna == 4) { gestionarCola(); }
            }
        }
        gestor->cerrarSesion();
        cout << "  Sesion cerrada." << endl;
    }

    // ============================================================
    //  MIS PLAYLISTS  
    // ============================================================
    void crearPlaylist() {
        string nom, desc;
        cout << "\nNombre de la playlist: ";
        asignarcolor(14);
        getline(cin, nom);
        asignarcolor(7);

        cout << "Descripcion: ";
        asignarcolor(14);
        getline(cin, desc);
        asignarcolor(7);

        Playlist* pl = actual->crearPlaylist(nom, desc);
        guardarLinea("usuarios.txt", pl->toString());
        cout << "  Playlist \"" << nom << "\" creada (ID:" << pl->getId() << ")." << endl;
        pausar();
    }

    void agregarCancionAPlaylist(int idC, Playlist* pl) {


        if (!pl) { cout << "  Playlist no encontrada." << endl; pausar(); return; }

        Cancion* c = gestor->buscarCancionId(idC);


        if (c) {
            pl->agregarCancion(c);
            guardarLinea("usuarios.txt", "PLAYLIST_CANCION," + to_string(pl->getId()) + "," + to_string(idC));
            cout << "  \"" << c->getNombre() << "\" agregada a \"" << pl->getNombre() << "\"." << endl;
        }
        else { cout << "  Cancion no encontrada." << endl; }

        pausar();
    }

    void menuPlaylists() {
        system("cls");

        int currentY = Console::CursorTop;
        vector<Playlist*> misPl = actual->getPlaylists().toVector();

        vector<string> nombresPlaylist;
        for (int i = 0; i < misPl.size(); i++) {
            nombresPlaylist.push_back(misPl[i]->getNombre());
        }
        string crearPlaylistop = "CREAR NUEVA PLAYLIST";
        nombresPlaylist.push_back(crearPlaylistop);

        int sub = menuInteractivo(nombresPlaylist, 5, currentY + 1);
        string op = nombresPlaylist[sub];

        if (sub < 0 || sub > misPl.size()) return;

        else {

            if (sub == misPl.size()) {
                crearPlaylist();
            }
            else {
                verPlaylist(op);
            }
        }
    }



    void menuAmigos() {
        cabecera("MENU AMIGOS");

        int currentY = Console::CursorTop;

        vector<string> opciones = {
            "Ver lista de amigos",
            "Agregar nuevo amigo",
            "Soulmate",
            "Mostrar conexiones",
        };

        int sub = menuInteractivo(opciones, 5, currentY + 1) + 1;

        if (sub == 1) { actual->mostrarAmigos(); pausar(); }
        else if (sub == 2) { menuAgregarAmigo(); pausar(); }
        else if (sub == 3) { mostrarSoulmate(); pausar(); }
        else if (sub == 4) { mostrarConexiones(); pausar(); }

    }

    void mostrarSoulmate() {
        cabecera("TU SOULMATE MUSICAL");

        Usuario* soulmate = gestor->obtenerSoulmate(actual);
        if (!soulmate) {
            asignarcolor(15);
            cout << "\n  Todavia no tenemos suficiente info: agrega amigos y escucha canciones." << endl;
            asignarcolor(7);
            return;
        }

        // Buscamos la afinidad ya calculada para ese amigo en la lista de conexiones
        double afinidad = -1;
        for (Gestor::Conexion& c : gestor->obtenerConexiones(actual)) {
            if (c.usuario->getId() == soulmate->getId()) { afinidad = c.afinidad; break; }
        }

        asignarcolor(7);
        cout << "\n  Tu soulmate musical es: ";
        asignarcolor(6);
        cout << soulmate->getNombre() << " (ID:" << soulmate->getId() << ")" << endl;

        asignarcolor(7);
        cout << "  Distancia de gustos (mientras mas bajo, mas afinidad): ";

        // Igual criterio de colores que en Recomendaciones: mas cerca = mas brillante
        if (afinidad <= 25) asignarcolor(2);
        else if (afinidad <= 50) asignarcolor(6);
        else asignarcolor(8);

        cout << afinidad << endl;
        asignarcolor(7);
    }

    void mostrarConexiones() {
        cabecera("GRAFO DE AMISTADES");

        auto listaCompleta = gestor->obtenerListaAdyacenciaCompleta();
        for (auto& par : listaCompleta) {
            Usuario* u = par.first;
            asignarcolor(14);
            cout << '\n' << u->getNombre() << " (ID:" << u->getId() << "):" << endl;
            asignarcolor(7);

            if (par.second.empty()) {
                asignarcolor(15);
                cout << "   (sin conexiones)" << endl;
                asignarcolor(7);
                continue;
            }
            for (Gestor::Conexion& conexion : par.second) {
                // Mismo criterio que en Recomendaciones/Soulmate: mas cerca = mas brillante
                if (conexion.afinidad <= 25) asignarcolor(2);
                else if (conexion.afinidad <= 50) asignarcolor(6);
                else asignarcolor(8);

                cout << "   -> " << conexion.usuario->getNombre()
                    << " (ID:" << conexion.usuario->getId()
                    << ") | afinidad: " << conexion.afinidad << endl;
            }
            asignarcolor(7);
        }
    }

    void menuAgregarAmigo() {

        cabecera("AGREGAR AMIGO");

        vector<Usuario*> usuarios = gestor->getUsuarios();

        for (Usuario* u : usuarios) {
            u->mostrarDetalles();
        }

        asignarcolor(7);
        int idNueovoAmigo;
        cout << "\nIngresa el ID del usuario que deseas agregar como amigo: ";
        asignarcolor(14);
        cin >> idNueovoAmigo;
        asignarcolor(7);

        gestor->agregarAmigo(idNueovoAmigo);

    }


    // ============================================================
    //  EXPLORAR BIBLIOTECA  
    // ============================================================
    void menuExplorar() {
        int op = 0;
        while (op != 5) {
            cabecera("EXPLORAR BIBLIOTECA");
            vector<string> opciones = {
                "Ver todas las canciones (Sin ordenar)",
                "Ordenar canciones A-Z (Insercion)",
                "Ver Top Reproducciones (Shell Sort)",
                "Ordenar por Duracion (Merge Sort)",
                "Volver"
            };
            op = menuInteractivo(opciones, 5, 4) + 1;

            if (op == 1) { mostrarSoloCanciones(gestor); pausar(); }
            else if (op == 2) { mostrarCancionesOrdenadasA_Z(gestor); pausar(); }
            else if (op == 3) { mostrarTopReproducciones(gestor); pausar(); }
            else if (op == 4) { mostrarCancionesPorDuracion(gestor); pausar(); }
        }
    }

    // ============================================================
    //  AGREGAR A LA COLA  
    // ============================================================
    void agregarCancionIndividual() {
        mostrarSoloCanciones(gestor);
        int idC;
        cout << "\nID de la cancion: "; asignarcolor(14); cin >> idC; asignarcolor(7);

        Cancion* c = gestor->agregarCancionAFila(idC);
        if (c) cout << "  Cancion encolada: " << c->getNombre() << endl;
        else cout << "  Cancion no encontrada." << endl;
        pausar();
    }

    void agregarPlaylistCola(Playlist* pl) {

        Nodo<Cancion*>* temp = pl->getCanciones().getCabeza();
        do {

            Cancion* Ctemp = temp->dato;
            Cancion* c = gestor->agregarCancionAFila(Ctemp->getId());
            if (c) cout << endl << "  Cancion encolada: " << c->getNombre();
            temp = temp->siguiente;

        } while (temp != pl->getCanciones().getCabeza());
        cout << endl;
        pausar();

    }

    void agregarPlaylistCompleta() {
        cout << "\n--- Mis Playlists ---" << endl;
        vector<Playlist*> misPLs = actual->getPlaylists().toVector();
        if (misPLs.empty()) {
            cout << "  Crea una playlist primero desde el menu de Playlists." << endl;
            pausar(); return;
        }
        for (Playlist* p : misPLs) p->mostrarDetalles();

        int idPL;
        cout << "\nID de la playlist: "; asignarcolor(14); cin >> idPL; asignarcolor(7);

        Playlist* pl = actual->buscarPlaylistPorId(idPL);
        if (pl) {
            vector<Cancion*> cans = pl->getCanciones().toVector();
            if (cans.empty()) { cout << "  La playlist esta vacia." << endl; }
            else {
                for (Cancion* c : cans) gestor->agregarCancionAFila(c->getId());
                cout << "  " << cans.size() << " cancion(es) agregadas a la cola." << endl;
            }
        }
        else cout << "  Playlist no encontrada." << endl;
        pausar();
    }

    void menuAgregarACola() {
        int op = 0;
        while (op != 2) {
            cabecera("AGREGAR A COLA");

            vector<string> opciones = {
                "Cancion individual",
                "Volver"
            };
            op = menuInteractivo(opciones, 5, 5) + 1;

            if (op == 1) agregarCancionIndividual();
        }
    }

    // ============================================================
    //  MIS FAVORITOS  
    // ============================================================
    void verFavoritos(vector<Cancion*>& favs) {
        cout << "\n--- Canciones Favoritas ---" << endl;
        if (favs.empty()) cout << "  (Sin canciones favoritas)" << endl;
        else for (Cancion* c : favs) c->mostrarDetalles();
        pausar();
    }

    void agregarFavorito(vector<Cancion*>& favs) {
        //Mostras todas las canciones
        mostrarSoloCanciones(gestor);

        //Ingresa el ID
        int idC; cout << "\nID de la cancion: "; asignarcolor(14); cin >> idC; asignarcolor(7);

        //buscarla en el catalogo con BinarySearch
        Cancion* nueva = gestor->buscarCancionId(idC);

        //Si existe
        if (nueva) {
            //comprobar si ya esta como favoritos
            for (int i = 0; i < favs.size(); i++) {
                if (nueva->getId() == favs[i]->getId()) {
                    cout << "\nNo se puede agregar una cancion mas de 1 vez"; pausar();
                    return;
                }
            }
            actual->agregarFavorito(nueva);
            guardarLinea("usuarios.txt", "FAVORITO," + to_string(actual->getId()) + "," + to_string(nueva->getId()));
            cout << "  Agregado a favoritos." << endl;
        }
        else cout << "  Cancion no encontrada." << endl;
        pausar();
    }

    void verFavoritosOrdenados() {
        vector<Cancion*> favsOrdenados = Gestor::obtenerFavoritosOrdenados(actual);
        cout << "\n  Favoritos ordenados (A-Z):" << endl;
        imprimirCanciones(favsOrdenados, "  (Sin canciones favoritas)");
        pausar();
    }

    void menuFavoritos() {
        int op = 0;
        vector<Cancion*> favs = actual->getFavoritos().toVector();
        while (op != 4) {
            cabecera("MIS FAVORITOS");
            vector<string> opciones = {
                "Ver canciones favoritas",
                "Agregar cancion a favoritos",
                "Ver favoritos ordenados A-Z (Quick Sort)",
                "Volver"
            };
            op = menuInteractivo(opciones, 5, 4) + 1;

            if (op == 1) { verFavoritos(favs); }
            else if (op == 2) agregarFavorito(favs);
            else if (op == 3) verFavoritosOrdenados();
        }
    }

    // ============================================================
    //  MI SUSCRIPCION   
    // ============================================================
    void menuSuscripcion() {
        cabecera("MI SUSCRIPCION");

        if (actual->esPremium()) {
            ubicar(5, 5); cout << "Plan actual: Premium | Vence: " << actual->getSuscripcion().fechaVencimiento;
            ubicar(5, 7); cout << "Ya disfrutas de todos los beneficios de EchoStream.";
            cout << endl;
            pausar();
            return;
        }

        ubicar(5, 4); cout << "Plan actual: Gratuito";
        vector<string> opciones = {
            "Actualizar a Premium (S/29.90/mes)",
            "Volver"
        };
        int op = menuInteractivo(opciones, 5, 6) + 1;

        if (op == 1) {
            gestor->activarPlanPremium(actual);
            cout << "\n  Plan Premium activado exitosamente." << endl;
        }
        pausar();
    }

    // ============================================================
    //  ACCESO (pantalla de entrada: login / registro)
    //  (antes: MenuAcceso)
    // ============================================================
    void mostrarLogo(int x, int y) {
        int ancho = 105;

        asignarcolor(14);

        Console::SetCursorPosition(x, y - 1);
        for (int i = 0; i <= ancho; i++) { cout << "="; }

        string logo[] = {
            " $$$$$$$$\\  $$$$$$\\  $$\\   $$\\  $$$$$$\\   $$$$$$\\ $$$$$$$$\\ $$$$$$$\\  $$$$$$$$\\  $$$$$$\\  $$\\      $$\\ ",
            " $$  _____|$$  __$$\\ $$ |  $$ |$$  __$$\\ $$  __$$\\\\\\__$$  __|$$  __$$\\ $$  _____|$$  __$$\\ $$$\\    $$$ |",
            " $$ |      $$ /  \\__|$$ |  $$ |$$ /  $$ |$$ /  \\__|  $$ |   $$ |  $$ |$$ |      $$ /  $$ |$$$$\\  $$$$ |",
            " $$$$$\\    $$ |      $$$$$$$$ |$$ |  $$ |\\$$$$$$\\    $$ |   $$$$$$$  |$$$$$\\    $$$$$$$$ |$$\\$$\\$$ $$ |",
            " $$  __|   $$ |      $$  __$$ |$$ |  $$ | \\____$$\\   $$ |   $$  __$$< $$  __|   $$  __$$ |$$ \\$$$  $$ |",
            " $$ |      $$ |  $$\\ $$ |  $$ |$$ |  $$ |$$\\   $$ |  $$ |   $$ |  $$ |$$ |      $$ |  $$ |$$ |\\$  /$$ |",
            " $$$$$$$$\\ \\$$$$$$  |$$ |  $$ | $$$$$$  |\\$$$$$$  |  $$ |   $$ |  $$ |$$$$$$$$\\ $$ |  $$ |$$ | \\_/ $$ |",
            " \\________| \\______/ \\__|  \\__| \\______/  \\______/   \\__|   \\__|  \\__|\\________|\\__|  \\__|\\__|     \\__|"
        };

        for (int i = 0; i < 8; i++) {
            Console::SetCursorPosition(x, y + i); cout << "|";
            Console::SetCursorPosition(x + 1, y + i); cout << logo[i];
            Console::SetCursorPosition(x + ancho, y + i); cout << "|";
        }

        Console::SetCursorPosition(x, y + 8);
        for (int i = 0; i <= ancho; i++) { cout << "="; }
    }

    void flujoLogin() {
        system("cls");
        string email, pass;

        asignarcolor(14); Console::SetCursorPosition(55, 8);  cout << "========================================";
        asignarcolor(7);  Console::SetCursorPosition(55, 9);  cout << "             INICIAR SESION             ";
        asignarcolor(14); Console::SetCursorPosition(55, 10); cout << "========================================";

        asignarcolor(7); Console::SetCursorPosition(60, 13); cout << "Email    : ";
        asignarcolor(14); cin >> email;

        asignarcolor(7); Console::SetCursorPosition(60, 15); cout << "Password : ";
        asignarcolor(14); cin >> pass;

        Console::SetCursorPosition(55, 18); asignarcolor(7);
        if (gestor->iniciarSesion(email, pass)) {
            actual = gestor->getUsuarioLogueado();
            cout << "\n  Bienvenido/a, " << actual->getNombre() << "!" << endl;

            pausar();
            menuStreaming();

            actual = nullptr;
        }
        else {
            cout << "  Error: Credenciales incorrectas." << endl;
            pausar();
        }
    }

    void flujoRegistro() {
        system("cls");
        int id, plan;
        string nom, email, pass;

        asignarcolor(14); Console::SetCursorPosition(55, 6);  cout << "========================================";
        asignarcolor(7);  Console::SetCursorPosition(55, 7);  cout << "        REGISTRAR NUEVO USUARIO         ";
        asignarcolor(14); Console::SetCursorPosition(55, 8);  cout << "========================================";

        asignarcolor(7); Console::SetCursorPosition(60, 11); cout << "ID       : ";
        asignarcolor(14); cin >> id;

        asignarcolor(7); Console::SetCursorPosition(60, 13); cout << "Nombre   : ";
        cin.ignore();
        asignarcolor(14); getline(cin, nom);

        asignarcolor(7); Console::SetCursorPosition(60, 15); cout << "Email    : ";
        asignarcolor(14); cin >> email;

        asignarcolor(7); Console::SetCursorPosition(60, 17); cout << "Password : ";
        asignarcolor(14); cin >> pass;

        asignarcolor(7); Console::SetCursorPosition(60, 19); cout << "1. Gratuito | 2. Premium (S/29.90)";
        asignarcolor(14); Console::SetCursorPosition(60, 20); cout << ">> Plan  : ";
        cin >> plan;
        if (plan < 1 || plan > 2) plan = 1;

        Console::SetCursorPosition(55, 23); asignarcolor(7);
        gestor->registrarNuevoUsuario(id, nom, email, pass, plan);

        cout << "  Usuario registrado." << endl;
        if (plan == 2) cout << "  Plan Premium activado." << endl;

        Console::SetCursorPosition(55, 25);
        pausar();
    }

public:
    AppUI(Gestor* gestor) : gestor(gestor) {}

    void ejecutar() {
        int opAcceso = 0;
        while (opAcceso != 3) {
            system("cls");
            mostrarLogo(20, 5);

            vector<string> opcionesAcceso = {
                "Iniciar Sesion",
                "Registrar Nuevo Usuario",
                "Salir"
            };

            opAcceso = menuInteractivo(opcionesAcceso, 55, 23) + 1;

            if (opAcceso == 1) flujoLogin();
            else if (opAcceso == 2) flujoRegistro();
        }

        asignarcolor(7);
        cout << "\n  Hasta pronto! ECHOSTREAM v4.0 " << endl;
    }
};