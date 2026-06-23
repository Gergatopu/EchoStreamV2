#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Gestores.h"

using namespace std;
using namespace System;

// ============================================================
//  UTILIDADES DE PRESENTACION
//  Compartidas por todas las clases de menu de este archivo.
//  IMPORTANTE: aqui solo se imprime. Toda la logica (ordenar,
//  buscar, filtrar, calcular) vive en Gestores.h.
// ============================================================

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

void mostrarCancionesOrdenadasA_Z(GestorBiblioteca* lib) {
    cout << "\n  [Insercion] Canciones (A-Z):" << endl;
    imprimirCanciones(lib->obtenerCancionesOrdenadasAZ());
}

void mostrarTopReproducciones(GestorBiblioteca* lib) {
    cout << "\n  [Shell Sort] Top Reproducciones:" << endl;
    imprimirCanciones(lib->obtenerTopReproducciones());
}

void mostrarSoloCanciones(GestorBiblioteca* lib) {
    cout << "\n--- CANCIONES DISPONIBLES ---" << endl;
    imprimirCanciones(lib->getCatalogoCanciones());
}

// --- IMPRESION DE LA COLA DE REPRODUCCION ---
void mostrarCola(GestorReproduccion* repro) {
    vector<Cancion*> vista = repro->obtenerCola();
    cout << "\n--- Cola (" << vista.size() << ") ---" << endl;
    for (Cancion* c : vista) cout << "  -> " << c->getNombre() << endl;
}

// ============================================================
//  MENU: EXPLORAR BIBLIOTECA
// ============================================================
class MenuExplorar {
private:
    GestorBiblioteca* lib;

public:
    MenuExplorar(GestorBiblioteca* lib) : lib(lib) {}

    void ejecutar() {
        int op = 0;
        while (op != 4) {
            cabecera("EXPLORAR BIBLIOTECA");
            vector<string> opciones = {
                "Ver todas las canciones (Sin ordenar)",
                "Ordenar canciones A-Z (Insercion)",
                "Ver Top Reproducciones (Shell Sort)",
                "Volver"
            };
            op = menuInteractivo(opciones, 5, 4) + 1;

            if (op == 1) { mostrarSoloCanciones(lib); pausar(); }
            else if (op == 2) { mostrarCancionesOrdenadasA_Z(lib); pausar(); }
            else if (op == 3) { mostrarTopReproducciones(lib); pausar(); }
        }
    }
};


// ============================================================
//  MENU: AGREGAR A LA COLA
// ============================================================
class MenuAgregarACola {
private:
    GestorBiblioteca* lib;
    GestorReproduccion* repro;
    Usuario* usuario;
    MenuExplorar* menuExplorar;

    void agregarCancionIndividual() {
        mostrarSoloCanciones(lib);
        int idC;
        cout << "\nID de la cancion: "; asignarcolor(14); cin >> idC; asignarcolor(7);

        Cancion* c = repro->agregarCancionAFila(idC);
        if (c) cout << "  Cancion encolada: " << c->getNombre() << endl;
        else cout << "  Cancion no encontrada." << endl;
        pausar();
    }

    void agregarPlaylistCompleta() {
        cout << "\n--- Mis Playlists ---" << endl;
        vector<Playlist*> misPLs = usuario->getPlaylists().toVector();
        if (misPLs.empty()) {
            cout << "  Crea una playlist primero desde el menu de Playlists." << endl;
            pausar(); return;
        }
        for (Playlist* p : misPLs) p->mostrarDetalles();

        int idPL;
        cout << "\nID de la playlist: "; asignarcolor(14); cin >> idPL; asignarcolor(7);

        Playlist* pl = usuario->buscarPlaylistPorId(idPL);
        if (pl) {
            vector<Cancion*> cans = pl->getCanciones().toVector();
            if (cans.empty()) { cout << "  La playlist esta vacia." << endl; }
            else {
                for (Cancion* c : cans) repro->agregarCancionAFila(c->getId());
                cout << "  " << cans.size() << " cancion(es) agregadas a la cola." << endl;
            }
        }
        else cout << "  Playlist no encontrada." << endl;
        pausar();
    }

public:
    MenuAgregarACola(GestorBiblioteca* lib, GestorReproduccion* repro, Usuario* usuario)
        : lib(lib), repro(repro), usuario(usuario) {
    }

    void ejecutar() {
        int op = 0;
        while (op != 3) {
            cabecera("AGREGAR A COLA");
            ubicar(5, 4); cout << "Que quieres agregar?";

            vector<string> opciones = {
                "Cancion individual",
                "Playlist completa",
                "Volver"
            };
            op = menuInteractivo(opciones, 5, 6) + 1;

            if (op == 1) agregarCancionIndividual();
            else if (op == 2) agregarPlaylistCompleta();
        }
    }
};


// ============================================================
//  MENU: MIS PLAYLISTS
// ============================================================
class MenuPlaylists {
private:
    GestorBiblioteca* lib;
    Usuario* usuario;


    void verPlaylists() {
        cout << "\n--- Mis Playlists ---" << endl;
        vector<Playlist*> misPLs = usuario->getPlaylists().toVector();
        if (misPLs.empty()) cout << "  (Sin playlists creadas)" << endl;
        else for (Playlist* p : misPLs) p->mostrarDetalles();
        pausar();
    }

    void crearPlaylist() {
        string nom, desc;
        cout << "\nNombre de la playlist: "; cin.ignore(); asignarcolor(14); getline(cin, nom); asignarcolor(7);
        cout << "Descripcion: "; asignarcolor(14); getline(cin, desc); asignarcolor(7);
        Playlist* pl = usuario->crearPlaylist(nom, desc);
        GestorPersistencia::guardarLinea("usuarios.txt", pl->toString());
        cout << "  Playlist \"" << nom << "\" creada (ID:" << pl->getId() << ")." << endl;
        pausar();
    }

    void agregarCancionAPlaylist() {
        cout << "\n--- Mis Playlists ---" << endl;
        vector<Playlist*> misPLs = usuario->getPlaylists().toVector();
        if (misPLs.empty()) { cout << "  No tienes playlists."; pausar(); return; }
        for (Playlist* p : misPLs) p->mostrarDetalles();

        int idPL; cout << "ID de la playlist: "; asignarcolor(14); cin >> idPL; asignarcolor(7);
        Playlist* pl = usuario->buscarPlaylistPorId(idPL);
        if (!pl) { cout << "  Playlist no encontrada." << endl; pausar(); return; }

        mostrarSoloCanciones(lib);
        int idC; cout << "\nID de la cancion a agregar: "; asignarcolor(14); cin >> idC; asignarcolor(7);
        Cancion* c = lib->buscarCancionId(idC);
        if (c) {
            pl->agregarCancion(c);
            GestorPersistencia::guardarLinea("usuarios.txt", "PLAYLIST_CANCION," + to_string(pl->getId()) + "," + to_string(idC));
            cout << "  \"" << c->getNombre() << "\" agregada a \"" << pl->getNombre() << "\"." << endl;
        }
        else { cout << "  Cancion no encontrada." << endl; }
        pausar();
    }

    void verCancionesDePlaylist() {
        cout << "\n--- Mis Playlists ---" << endl;
        vector<Playlist*> misPLs = usuario->getPlaylists().toVector();
        if (misPLs.empty()) { cout << "  No tienes playlists."; pausar(); return; }
        for (Playlist* p : misPLs) p->mostrarDetalles();

        int idPL; cout << "ID de la playlist: "; asignarcolor(14); cin >> idPL; asignarcolor(7);
        Playlist* pl = usuario->buscarPlaylistPorId(idPL);
        if (pl) {
            cout << "\n--- Canciones de \"" << pl->getNombre() << "\" ---" << endl;
            vector<Cancion*> cans = pl->getCanciones().toVector();
            if (cans.empty()) { cout << "  (Playlist vacia)" << endl; }
            else {
                int i = 1;
                for (Cancion* c : cans) { cout << "  " << i++ << ". "; c->mostrarDetalles(); }
            }
        }
        else { cout << "  Playlist no encontrada." << endl; }
        pausar();
    }

public:
    MenuPlaylists(GestorBiblioteca* lib, Usuario* usuario) : lib(lib), usuario(usuario) {}

    void ejecutar() {
        int op = 0;
        while (op != 5) {
            cabecera("MIS PLAYLISTS");
            vector<string> opciones = {
                "Ver mis playlists",
                "Crear nueva playlist",
                "Agregar cancion a una playlist",
                "Ver canciones de una playlist",
                "Volver"
            };
            op = menuInteractivo(opciones, 5, 4) + 1;

            if (op == 1) verPlaylists();
            else if (op == 2) crearPlaylist();
            else if (op == 3) agregarCancionAPlaylist();
            else if (op == 4) verCancionesDePlaylist();
        }
    }
};


// ============================================================
//  MENU: MIS FAVORITOS
// ============================================================
class MenuFavoritos {
private:
    GestorBiblioteca* lib;
    Usuario* usuario;

    void verFavoritos(vector<Cancion*>& favs) {
        cout << "\n--- Canciones Favoritas ---" << endl;
        if (favs.empty()) cout << "  (Sin canciones favoritas)" << endl;
        else for (Cancion* c : favs) c->mostrarDetalles();
        pausar();
    }

    void agregarFavorito() {
        mostrarSoloCanciones(lib);
        int idC; cout << "\nID de la cancion: "; asignarcolor(14); cin >> idC; asignarcolor(7);
        Cancion* c = lib->buscarCancionId(idC);
        if (c) {
            usuario->agregarFavorito(c);
            cout << "  Agregado a favoritos." << endl;
        }
        else cout << "  Cancion no encontrada." << endl;
        pausar();
    }

    // Solo impresion: el ordenamiento ya viene resuelto desde GestorUsuarios.
    void verFavoritosOrdenados() {
        vector<Cancion*> favsOrdenados = GestorUsuarios::obtenerFavoritosOrdenados(usuario);
        cout << "\n  Favoritos ordenados (A-Z):" << endl;
        imprimirCanciones(favsOrdenados, "  (Sin canciones favoritas)");
        pausar();
    }

public:
    MenuFavoritos(GestorBiblioteca* lib, Usuario* usuario) : lib(lib), usuario(usuario) {}

    void ejecutar() {
        int op = 0;
        while (op != 4) {
            cabecera("MIS FAVORITOS");
            vector<string> opciones = {
                "Ver canciones favoritas",
                "Agregar cancion a favoritos",
                "Ver favoritos ordenados A-Z (Insercion)",
                "Volver"
            };
            op = menuInteractivo(opciones, 5, 4) + 1;

            if (op == 1) { vector<Cancion*> favs = usuario->getFavoritos().toVector(); verFavoritos(favs); }
            else if (op == 2) agregarFavorito();
            else if (op == 3) verFavoritosOrdenados();
        }
    }
};


// ============================================================
//  MENU: MI SUSCRIPCION
// ============================================================
class MenuSuscripcion {
private:
    GestorUsuarios* userG;

public:
    MenuSuscripcion(GestorUsuarios* userG) : userG(userG) {}

    void ejecutar() {
        cabecera("MI SUSCRIPCION");
        Usuario* u = userG->getUsuarioLogueado();

        if (u->esPremium()) {
            ubicar(5, 5); cout << "Plan actual: Premium | Vence: " << u->getSuscripcion().fechaVencimiento;
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
            userG->activarPlanPremium(u);
            cout << "\n  Plan Premium activado exitosamente." << endl;
        }
        pausar();
    }
};


// ============================================================
//  MENU: STREAMING (pantalla principal tras iniciar sesion)
// ============================================================
class MenuStreaming {
private:
    GestorBiblioteca* lib;
    GestorUsuarios* userG;
    GestorReproduccion* repro;
    Usuario* actual;

    void mostrarCabecera() {
        system("cls");

        // Ancho total del marco (58 caracteres)
        string borde = "==========================================================";

        asignarcolor(14);
        Console::SetCursorPosition(35, 3);  cout << borde;

        // Fila del Título
        Console::SetCursorPosition(35, 4);
        asignarcolor(14); cout << "| ";
        asignarcolor(7);  cout << "               ECHOSTREAM PLAYER                ";
        asignarcolor(14); cout << " |";

        // Fila de Usuario y Estado
        string nombre = actual->getNombre();
        string tipo = actual->esPremium() ? "Premium" : "Gratuito";

        // Formateamos el texto central: " Usuario: NOMBRE | Estado: TIPO"
        // Usamos un buffer para calcular la longitud exacta del texto interior
        string contenido = " Usuario: " + nombre + " | Estado: " + tipo;

        // Calculamos cuánto espacio sobra para rellenar
        // El marco interno mide 54 caracteres (58 total - 2 de los bordes laterales "| " - 2 de los bordes laterales " |")
        int anchoInterior = 54;
        int espaciosNecesarios = anchoInterior - contenido.length();

        Console::SetCursorPosition(35, 5);
        asignarcolor(14); cout << "| ";
        asignarcolor(7);  cout << contenido;

        // Imprimimos los espacios para que el borde derecho quede alineado
        for (int i = 0; i < espaciosNecesarios; i++) cout << " ";

        asignarcolor(14); cout << "|";

        // Fila inferior
        Console::SetCursorPosition(35, 6);
        cout << borde;

        // Reset de color
        asignarcolor(7);
    }

    void reproducirSiguiente() {
        cabecera("REPRODUCIENDO");
        Cancion* c = repro->reproducirSiguiente();
        if (!c) cout << "  La cola esta vacia." << endl;
        pausar();
    }

    void gestionarCola() {
        cabecera("GESTIONAR COLA");
        mostrarCola(repro);

        // Calculamos la posicion Y dinamicamente debajo de la cola actual impresa
        cout << "\n";
        int currentY = Console::CursorTop;

        vector<string> opcionesCola = {
            "Mezclar aleatoriamente (Shuffle recursivo)",
            "Volver"
        };
        int sub = menuInteractivo(opcionesCola, 5, currentY + 1) + 1;

        if (sub == 1) {
            if (repro->modoAleatorio()) {
                cout << '\n' << '\n' << "--- Cola mezclada aleatoriamente ---" << endl;
                mostrarCola(repro);
            }
        }
        pausar();
    }

    void mostrarHistorialYSuscripcion() {
        cabecera("MI HISTORIAL");
        actual->mostrarHistorial();
        pausar();
        MenuSuscripcion(userG).ejecutar();
    }

    void mostrarRecomendaciones() {
        int idUsu = actual->getId();
        vector<double> perfilUsuario = lib->calcularPreferencias(idUsu);
        vector<GestorBiblioteca::Recomendacion> recomendaciones = lib->obtenerRecomendaciones(perfilUsuario, idUsu);

        cout << "\n--- TUS RECOMENDACIONES ---" << endl;
        if (recomendaciones.empty()) {
            cout << "  (Sin recomendaciones disponibles por ahora)" << endl;
        }
        else {
            for (GestorBiblioteca::Recomendacion& r : recomendaciones) {
                cout << " [Distancia: " << r.distancia << "] ";
                r.cancion->mostrarDetalles();
            }
        }
        pausar();
    }

public:
    MenuStreaming(GestorBiblioteca* lib, GestorUsuarios* userG, GestorReproduccion* repro)
        : lib(lib), userG(userG), repro(repro), actual(userG->getUsuarioLogueado()) {
    }

    void dibujarNotaEnCuadrado(int x, int y, int color) {
        // Definimos el marco exterior con un poco de margen interno
        asignarcolor(color);Console::SetCursorPosition(x, y);      cout << "+-----------------------------+";
        asignarcolor(color);Console::SetCursorPosition(x, y + 1);  cout << "|                             |"; // Espacio superior
        asignarcolor(color);Console::SetCursorPosition(x, y + 2);  cout << "|      ;;;;;;;;;;;;;;;;;;;    |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 3);  cout << "|      ;;;;;;;;;;;;;;;;;;;    |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 4);  cout << "|      ;                 ;    |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 5);  cout << "|      ;                 ;    |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 6);  cout << "|      ;                 ;    |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 7);  cout << "|      ;                 ;    |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 8);  cout << "|      ;                 ;    |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 9);  cout << "|      ;                 ;    |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 10); cout << "|      ;                 ;    |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 11); cout << "|  ,;;;;;             ,;;;;;  |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 12); cout << "|  ;;;;;;             ;;;;;;  |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 13); cout << "|  `;;;;'             `;;;;'  |";
        asignarcolor(color);Console::SetCursorPosition(x, y + 14); cout << "|                             |"; // Espacio inferior
        asignarcolor(color);Console::SetCursorPosition(x, y + 15); cout << "+-----------------------------+";
    }

    void ejecutar() {
        bool salir = false;
        while (!salir) {
            mostrarCabecera();
            Console::CursorVisible = false;
            
            dibujarNotaEnCuadrado(49, 10, 6);
            
            vector<vector<string>> opcionesMenuPrincipal = {
                { "AJUSTES" , ""   , ""  , ""      , "PLAYLIST 1"  },
                { ""        , ""   , ""  , ""      , "PLAYLIST 2"  },
                { ""        , ""   , ""  , ""      , "PLAYLIST 3"  },
                { ""        , ""   , ""  , ""      , "PLAYLIST 4"  },
                { ""        , ""   , ""  , ""      , "PLAYLISTS +"  },
                { "NOMBRECANCION"  , "|<" , "o" , ">|"    , "COLA"        },
            };

            pair<int, int> sel = menuInteractivoGrid(opcionesMenuPrincipal, 14, 9, 23, 4);
            int fila = sel.first, columna = sel.second;


            // Acciones segun la fila seleccionada
            if (fila == 0)      MenuExplorar(lib).ejecutar();
            else if (fila == 1) MenuAgregarACola(lib, repro, actual).ejecutar();
            else if (fila == 2) {
                mostrarRecomendaciones();

            }
            else if (fila == 3) gestionarCola();
            else if (fila == 4) MenuPlaylists(lib, actual).ejecutar();
            else if (fila == 5) MenuFavoritos(lib, actual).ejecutar();
            else if (fila == 6) mostrarHistorialYSuscripcion();
            else if (fila == 7) salir = true; // Cerrar Sesion
            else if (fila == 8) {
                if (columna == 0) {}   //reproducirAnterior();
                else if (columna == 1) reproducirSiguiente(); // o tu logica de "reproducir/pausar"
                else if (columna == 2)  reproducirSiguiente();
                else if (columna == 3) gestionarCola(); //TODA LA LOGICA DE LA COLA AQUI
            }


        }
        userG->cerrarSesion();
        cout << "  Sesion cerrada." << endl;
    }
};

//MENU PRINCIPAL: opciones en filas y columnas 
            /*vector<vector<string>> opcionesMenuPrincipal = {
                { "Explorar Biblioteca y Catalogo" },
                {  "h"},
                { "TOP MAS RECOMENDADAS PARA TI" },
                { "Gestionar Cola (Ver / Mezclar)" },
                { "Mis Playlists" },
                { "Mis Favoritos" },
                { "Mi Historial e Info Suscripcion" },
                { "Cerrar Sesion" },
                { "|<", "Reproducir", ">|", "Cola"}
            };

            pair<int, int> sel = menuInteractivoGrid(opcionesMenuPrincipal, 40, 9, 30,2);
            int fila = sel.first, columna = sel.second;


            // Acciones segun la fila seleccionada
            if (fila == 0)      MenuExplorar(lib).ejecutar();
            else if (fila == 1) MenuAgregarACola(lib, repro, actual).ejecutar();
            else if (fila == 2) {
                mostrarRecomendaciones();

            }
            else if (fila == 3) gestionarCola();
            else if (fila == 4) MenuPlaylists(lib, actual).ejecutar();
            else if (fila == 5) MenuFavoritos(lib, actual).ejecutar();
            else if (fila == 6) mostrarHistorialYSuscripcion();
            else if (fila == 7) salir = true; // Cerrar Sesion
            else if (fila == 8) {
                if (columna == 0) {}   //reproducirAnterior();
                else if (columna == 1) reproducirSiguiente(); // o tu logica de "reproducir/pausar"
                else if (columna == 2)  reproducirSiguiente();
                else if (columna == 3) gestionarCola(); //TODA LA LOGICA DE LA COLA AQUI
            }
            */

// ============================================================
//  MENU: ACCESO (pantalla de entrada: login / registro)
// ============================================================
class MenuAcceso {
private:
    GestorBiblioteca* biblioteca;
    GestorUsuarios* usuarios;

    void mostrarLogo(int x, int y) {
        int ancho = 105; // Ancho total del recuadro

        asignarcolor(14);

        // Borde superior
        Console::SetCursorPosition(x, y - 1);
        for (int i = 0; i <= ancho; i++) { cout << "="; }

        // Arte ASCII y columnas laterales
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

        // Borde inferior
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
        if (usuarios->iniciarSesion(email, pass)) {
            cout << "\n  Bienvenido/a, " << usuarios->getUsuarioLogueado()->getNombre() << "!" << endl;

            GestorReproduccion* reproduccion = new GestorReproduccion(biblioteca, usuarios->getUsuarioLogueado());
            pausar();
            MenuStreaming(biblioteca, usuarios, reproduccion).ejecutar();
            delete reproduccion;
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
        usuarios->registrarNuevoUsuario(id, nom, email, pass, plan);

        cout << "  Usuario registrado." << endl;
        if (plan == 2) cout << "  Plan Premium activado." << endl;

        Console::SetCursorPosition(55, 25);
        pausar();
    }

public:
    MenuAcceso(GestorBiblioteca* biblioteca, GestorUsuarios* usuarios)
        : biblioteca(biblioteca), usuarios(usuarios) {
    }

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
        cout << "\n  Hasta pronto! ECHOSTREAM v4.0 (Refactored)" << endl;
    }
};