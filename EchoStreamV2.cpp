#include "pch.h"       
#include "Gestores.h"
#include <vector>

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

void menuAgregarACola(GestorBiblioteca* lib, GestorReproduccion* repro, Usuario* usuario) {
    int op = 0;
    while (op != 3) {
        cabecera("AGREGAR A COLA");
        cout << "Que quieres agregar?" << endl;
        cout << "1. Cancion individual" << endl;
        cout << "2. Playlist completa" << endl;
        cout << "3. Volver" << endl;
        asignarcolor(14); cout << "Seleccione: "; cin >> op; asignarcolor(7);

        if (op == 1) {
            lib->mostrarSoloCanciones();
            int idC;
            cout << "\nID de la cancion: "; asignarcolor(14); cin >> idC; asignarcolor(7);
            repro->agregarCancionAFila(idC);
            pausar();
        }
        else if (op == 2) {
            cout << "\n--- Mis Playlists ---" << endl;
            vector<Playlist*> misPLs = usuario->getPlaylists().toVector();
            if (misPLs.empty()) {
                cout << "  Crea una playlist primero desde el menu de Playlists." << endl;
                pausar(); continue;
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
    }
}

void menuPlaylists(GestorBiblioteca* lib, Usuario* usuario) {
    int op = 0;
    while (op != 5) {
        cabecera("MIS PLAYLISTS");
        cout << "1. Ver mis playlists" << endl;
        cout << "2. Crear nueva playlist" << endl;
        cout << "3. Agregar cancion a una playlist" << endl;
        cout << "4. Ver canciones de una playlist" << endl;
        cout << "5. Volver" << endl;
        asignarcolor(14); cout << "Seleccione: "; cin >> op; asignarcolor(7);

        if (op == 1) {
            cout << "\n--- Mis Playlists ---" << endl;
            vector<Playlist*> misPLs = usuario->getPlaylists().toVector();
            if (misPLs.empty()) cout << "  (Sin playlists creadas)" << endl;
            else for (Playlist* p : misPLs) p->mostrarDetalles();
            pausar();
        }
        else if (op == 2) {
            string nom, desc;
            cout << "Nombre de la playlist: "; cin.ignore(); asignarcolor(14); getline(cin, nom); asignarcolor(7);
            cout << "Descripcion: "; asignarcolor(14); getline(cin, desc); asignarcolor(7);
            Playlist* pl = usuario->crearPlaylist(nom, desc);
            GestorPersistencia::guardarLinea("usuarios.txt", pl->toString());
            cout << "  Playlist \"" << nom << "\" creada (ID:" << pl->getId() << ")." << endl;
            pausar();
        }
        else if (op == 3) {
            cout << "\n--- Mis Playlists ---" << endl;
            vector<Playlist*> misPLs = usuario->getPlaylists().toVector();
            if (misPLs.empty()) { cout << "  No tienes playlists."; pausar(); continue; }
            for (Playlist* p : misPLs) p->mostrarDetalles();

            int idPL; cout << "ID de la playlist: "; asignarcolor(14); cin >> idPL; asignarcolor(7);
            Playlist* pl = usuario->buscarPlaylistPorId(idPL);
            if (!pl) { cout << "  Playlist no encontrada." << endl; pausar(); continue; }

            lib->mostrarSoloCanciones();
            int idC; cout << "ID de la cancion a agregar: "; asignarcolor(14); cin >> idC; asignarcolor(7);
            Cancion* c = lib->buscarCancionId(idC);
            if (c) {
                pl->agregarCancion(c);
                GestorPersistencia::guardarLinea("usuarios.txt", "PLAYLIST_CANCION," + to_string(pl->getId()) + "," + to_string(idC));
                cout << "  \"" << c->getNombre() << "\" agregada a \"" << pl->getNombre() << "\"." << endl;
            }
            else { cout << "  Cancion no encontrada." << endl; }
            pausar();
        }
        else if (op == 4) {
            cout << "\n--- Mis Playlists ---" << endl;
            vector<Playlist*> misPLs = usuario->getPlaylists().toVector();
            if (misPLs.empty()) { cout << "  No tienes playlists."; pausar(); continue; }
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
    }
}

void menuExplorar(GestorBiblioteca* lib) {
    int op = 0;
    while (op != 4) {
        cabecera("EXPLORAR BIBLIOTECA");
        cout << "1. Ver todas las canciones (Sin ordenar)" << endl;
        cout << "2. Ordenar canciones A-Z (Insercion)" << endl;
        cout << "3. Ver Top Reproducciones (Shell Sort)" << endl;
        cout << "4. Volver" << endl;
        asignarcolor(14); cout << "Seleccione: "; cin >> op; asignarcolor(7);

        if (op == 1) { lib->mostrarSoloCanciones(); pausar(); }
        else if (op == 2) { lib->mostrarCancionesOrdenadasA_Z(); pausar(); }
        else if (op == 3) { lib->mostrarTopReproducciones(); pausar(); }
    }
}

void menuFavoritos(GestorBiblioteca* lib, Usuario* usuario) {
    int op = 0;
    while (op != 4) {
        cabecera("MIS FAVORITOS");
        cout << "1. Ver canciones favoritas" << endl;
        cout << "2. Agregar cancion a favoritos" << endl;
        cout << "3. Ver favoritos ordenados A-Z (Insercion)" << endl;
        cout << "4. Volver" << endl;
        asignarcolor(14); cout << "Seleccione: "; cin >> op; asignarcolor(7);

        vector<Cancion*> favs = usuario->getFavoritos().toVector();

        if (op == 1) {
            if (favs.empty()) cout << "  (Sin canciones favoritas)" << endl;
            else for (Cancion* c : favs) c->mostrarDetalles();
            pausar();
        }
        else if (op == 2) {
            lib->mostrarSoloCanciones();
            int idC; cout << "ID de la cancion: "; asignarcolor(14); cin >> idC; asignarcolor(7);
            Cancion* c = lib->buscarCancionId(idC);
            if (c) {
                usuario->agregarFavorito(c);
                cout << "  Agregado a favoritos." << endl;
            }
            else cout << "  Cancion no encontrada." << endl;
            pausar();
        }
        else if (op == 3) {
            if (favs.empty()) { cout << "  (Sin canciones favoritas)" << endl; }
            else {
                Algoritmos::insertionSortPorNombre(favs); // Usa la vista, no daña la lista
                cout << "\n  Favoritos ordenados (A-Z):" << endl;
                for (Cancion* c : favs) c->mostrarDetalles();
            }
            pausar();
        }
    }
}

void menuSuscripcion(GestorUsuarios* userG) {
    cabecera("MI SUSCRIPCION");
    Usuario* u = userG->getUsuarioLogueado();

    if (u->esPremium()) {
        cout << "  Plan actual: Premium | Vence: " << u->getSuscripcion().fechaVencimiento << endl;
        cout << "\n  Ya disfrutas de todos los beneficios." << endl;
        pausar();
        return;
    }

    cout << "  Plan actual: Gratuito" << endl;
    cout << "\n1. Actualizar a Premium (S/29.90/mes)" << endl;
    cout << "2. Volver" << endl;
    asignarcolor(14); cout << "Seleccione: ";
    int op; cin >> op; asignarcolor(7);

    if (op == 1) {
        u->activarPremium({ "Premium", 29.90, obtenerFechaHoy(), "2026-12-31", true });
        GestorPersistencia::guardarLinea("usuarios.txt", "SUSCRIPCION,999," + to_string(u->getId()) + ",PlanPremium,Premium,29.90,Hoy,2026-12-31,1");
        cout << "  Plan Premium activado exitosamente." << endl;
    }
    pausar();
}

void menuStreaming(GestorBiblioteca* lib, GestorUsuarios* userG, GestorReproduccion* repro) {
    int op = 0;
    Usuario* actual = userG->getUsuarioLogueado();

    while (op != 8) {
        system("cls");

        asignarcolor(14); Console::SetCursorPosition(35, 3);  cout << "==========================================================";

        asignarcolor(14); Console::SetCursorPosition(35, 4);  cout << "| ";
        asignarcolor(7);  cout << "               ECHOSTREAM PLAYER                ";
        asignarcolor(14); cout << " |";

        asignarcolor(14); Console::SetCursorPosition(35, 5);  cout << "| ";
        asignarcolor(7);  cout << " Usuario: ";
        asignarcolor(14); cout << actual->getNombre();

        if (actual->esPremium()) {
            string estado = " | Estado: Premium";
            asignarcolor(7); cout << estado;
            int espacios = 52 - (10 + actual->getNombre().length() + estado.length());
            for (int i = 0; i < espacios; i++) cout << " ";
        }
        else {
            string estado = " | Estado: Gratuito";
            asignarcolor(7); cout << estado;
            int espacios = 52 - (10 + actual->getNombre().length() + estado.length());
            for (int i = 0; i < espacios; i++) cout << " ";
        }
        asignarcolor(14); cout << " |";

        asignarcolor(14); Console::SetCursorPosition(35, 6);  cout << "==========================================================";

        // --- OPCIONES DEL MENÚ (Ajustadas) ---
        asignarcolor(7);  Console::SetCursorPosition(40, 9);  cout << "1. Explorar Biblioteca y Catalogo";
        asignarcolor(7);  Console::SetCursorPosition(40, 11); cout << "2. Agregar a la Cola";
        asignarcolor(7);  Console::SetCursorPosition(40, 13); cout << "3. Reproducir Siguiente";
        asignarcolor(7);  Console::SetCursorPosition(40, 15); cout << "4. Gestionar Cola (Ver / Mezclar)";
        asignarcolor(7);  Console::SetCursorPosition(40, 17); cout << "5. Mis Playlists";
        asignarcolor(7);  Console::SetCursorPosition(40, 19); cout << "6. Mis Favoritos";
        asignarcolor(7);  Console::SetCursorPosition(40, 21); cout << "7. Mi Historial e Info Suscripcion";
        asignarcolor(8);  Console::SetCursorPosition(40, 23); cout << "8. Cerrar Sesion";

        asignarcolor(14); Console::SetCursorPosition(40, 26); cout << ">> Seleccione una opcion: ";
        cin >> op; asignarcolor(7);

        switch (op) {
        case 1: menuExplorar(lib); break;
        case 2: menuAgregarACola(lib, repro, actual); break;
        case 3:
            cabecera("REPRODUCIENDO");
            repro->reproducirSiguiente();
            pausar(); break;
        case 4: {
            cabecera("GESTIONAR COLA");
            repro->mostrarCola();
            cout << "\n1. Mezclar aleatoriamente (Shuffle recursivo)" << endl;
            cout << "2. Volver" << endl;
            asignarcolor(14); cout << "Seleccione: ";
            int sub; cin >> sub; asignarcolor(7);
            if (sub == 1) repro->modoAleatorio();
            pausar(); break;
        }
        case 5: menuPlaylists(lib, actual); break;
        case 6: menuFavoritos(lib, actual); break;
        case 7:
            cabecera("MI HISTORIAL");
            actual->mostrarHistorial();
            pausar();
            menuSuscripcion(userG);
            break;
        }
    }
    userG->cerrarSesion();
}

int main() {
    srand((unsigned)time(nullptr));

    GestorBiblioteca* biblioteca = new GestorBiblioteca();
    GestorUsuarios* usuarios = new GestorUsuarios();
    GestorReproduccion* reproduccion = nullptr;

    int opAcceso = 0;
    while (opAcceso != 3) {
        system("cls");
        asignarcolor(14); Console::SetCursorPosition(25, 6); for (int i = 0; i < 113; i++) { cout << "="; }
        for (int i = 0; i < 10; i++) { Console::SetCursorPosition(25, 7 + i); cout << "|"; Console::SetCursorPosition(137, 7 + i); cout << "|"; }

        asignarcolor(14);
        Console::SetCursorPosition(30, 8);  cout << " $$$$$$$$\\  $$$$$$\\  $$\\   $$\\  $$$$$$\\   $$$$$$\\ $$$$$$$$\\ $$$$$$$\\  $$$$$$$$\\  $$$$$$\\  $$\\      $$\\ ";
        Console::SetCursorPosition(30, 9);  cout << " $$  _____|$$  __$$\\ $$ |  $$ |$$  __$$\\ $$  __$$\\\\\\__$$  __|$$  __$$\\ $$  _____|$$  __$$\\ $$$\\    $$$ |";
        Console::SetCursorPosition(30, 10); cout << " $$ |      $$ /  \\__|$$ |  $$ |$$ /  $$ |$$ /  \\__|  $$ |   $$ |  $$ |$$ |      $$ /  $$ |$$$$\\  $$$$ |";
        Console::SetCursorPosition(30, 11); cout << " $$$$$\\    $$ |      $$$$$$$$ |$$ |  $$ |\\$$$$$$\\    $$ |   $$$$$$$  |$$$$$\\    $$$$$$$$ |$$\\$$\\$$ $$ |";
        Console::SetCursorPosition(30, 12); cout << " $$  __|   $$ |      $$  __$$ |$$ |  $$ | \\____$$\\   $$ |   $$  __$$< $$  __|   $$  __$$ |$$ \\$$$  $$ |";
        Console::SetCursorPosition(30, 13); cout << " $$ |      $$ |  $$\\ $$ |  $$ |$$ |  $$ |$$\\   $$ |  $$ |   $$ |  $$ |$$ |      $$ |  $$ |$$ |\\$  /$$ |";
        Console::SetCursorPosition(30, 14); cout << " $$$$$$$$\\ \\$$$$$$  |$$ |  $$ | $$$$$$  |\\$$$$$$  |  $$ |   $$ |  $$ |$$$$$$$$\\ $$ |  $$ |$$ | \\_/ $$ |";
        Console::SetCursorPosition(30, 15); cout << " \\________| \\______/ \\__|  \\__| \\______/  \\______/   \\__|   \\__|  \\__|\\________|\\__|  \\__|\\__|     \\__|";

        asignarcolor(14); Console::SetCursorPosition(25, 17); for (int i = 0; i < 113; i++) { cout << "="; }

        // --- OPCIONES DEL MENÚ ---
        asignarcolor(7);  Console::SetCursorPosition(70, 21); cout << "[ MENU DE ACCESO ]";
        asignarcolor(7);  Console::SetCursorPosition(70, 23); cout << "1. Iniciar Sesion";
        asignarcolor(7);  Console::SetCursorPosition(70, 25); cout << "2. Registrar Nuevo Usuario";
        asignarcolor(8);  Console::SetCursorPosition(70, 27); cout << "3. Salir";

        asignarcolor(14); Console::SetCursorPosition(70, 31); cout << ">> Seleccione una opcion: ";
        asignarcolor(15);
        cin >> opAcceso;

        if (opAcceso == 1) {
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
                reproduccion = new GestorReproduccion(biblioteca, usuarios->getUsuarioLogueado());
                pausar();
                menuStreaming(biblioteca, usuarios, reproduccion);
                delete reproduccion;
                reproduccion = nullptr;
            }
            else { pausar(); }
        }
        else if (opAcceso == 2) {
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

            Console::SetCursorPosition(55, 25);
            pausar();
        }
    }

    delete biblioteca;
    delete usuarios;
    asignarcolor(7);
    cout << "\n  Hasta pronto! ECHOSTREAM v4.0 (Refactored)" << endl;
    return 0;
}