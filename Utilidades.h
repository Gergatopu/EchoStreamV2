#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <conio.h>

using namespace std;
using namespace System;

#define TECLA_ARRIBA 72
#define TECLA_ABAJO 80
#define TECLA_ENTER 13
#define TECLA_IZQUIERDA 75
#define TECLA_DERECHA   77

void ubicar(int x, int y) {
    Console::SetCursorPosition(x, y);
}

void asignarcolor(int color) {
    switch (color)
    {
    case 0: Console::ForegroundColor = ConsoleColor::Black; break;
    case 1: Console::ForegroundColor = ConsoleColor::Blue; break;
    case 2: Console::ForegroundColor = ConsoleColor::Green; break;
    case 3: Console::ForegroundColor = ConsoleColor::Cyan; break;
    case 4: Console::ForegroundColor = ConsoleColor::Red; break;
    case 5: Console::ForegroundColor = ConsoleColor::Magenta; break;
    case 6: Console::ForegroundColor = ConsoleColor::Yellow; break;
    case 7: Console::ForegroundColor = ConsoleColor::White; break;
    case 8: Console::ForegroundColor = ConsoleColor::Gray; break;
    case 9: Console::ForegroundColor = ConsoleColor::DarkBlue; break;
    case 10: Console::ForegroundColor = ConsoleColor::DarkGreen; break;
    case 11: Console::ForegroundColor = ConsoleColor::DarkCyan; break;
    case 12: Console::ForegroundColor = ConsoleColor::DarkRed; break;
    case 13: Console::ForegroundColor = ConsoleColor::DarkMagenta; break;
    case 14: Console::ForegroundColor = ConsoleColor::DarkYellow; break;
    case 15: Console::ForegroundColor = ConsoleColor::DarkGray; break;

    default: Console::ForegroundColor = ConsoleColor::Blue; break;
    }
}

string obtenerFechaHoy() {
    time_t t = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
    return string(buf);
}

string obtenerHoraActual() {
    time_t t = time(nullptr);
    char buf[30];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}



// Version "grid": permite filas con varias opciones laterales.
// Las celdas vacias ("") se saltan hasta encontrar una valida.
pair<int, int> menuInteractivoGrid(const vector<vector<string>>& opciones, int posX, int posY, int anchoColumna, int altoFila) {
    int fila = 0, columna = 0;

    // Ubicar la primera celda no vacia como punto de partida
    for (int f = 0; f < (int)opciones.size(); f++) {
        for (int c = 0; c < (int)opciones[f].size(); c++) {
            if (!opciones[f][c].empty()) { fila = f; columna = c; goto encontrado; }
        }
    }
encontrado:;

    bool eligiendo = true;
    while (eligiendo) {
        for (int f = 0; f < (int)opciones.size(); f++) {
            for (int c = 0; c < (int)opciones[f].size(); c++) {
                const string& texto = opciones[f][c];
                if (texto.empty()) continue; // no se imprime ni ocupa espacio visual

                Console::SetCursorPosition(posX + (c * anchoColumna), posY + (f * altoFila));

                if (f == fila && c == columna) {
                    asignarcolor(4); 
                    cout << ">> " << texto << "   ";
                }
                else {
                    asignarcolor(7);
                    cout << "   " << texto << "   ";
                }
            }
        }
        asignarcolor(7);

        int tecla = _getch();
        if (tecla == 224) {
            tecla = _getch();

            // Definir la dirección del movimiento (Delta Fila, Delta Columna)
            int df = 0, dc = 0;
            if (tecla == TECLA_ARRIBA)       df = -1;
            else if (tecla == TECLA_ABAJO)   df = 1;
            else if (tecla == TECLA_IZQUIERDA) dc = -1;
            else if (tecla == TECLA_DERECHA)   dc = 1;

            if (df != 0 || dc != 0) {
                int nf = fila + df;
                int nc = columna + dc;

                // Avanzar en la dirección indicada hasta encontrar un string lleno o salir del límite
                while (nf >= 0 && nf < (int)opciones.size() &&
                    nc >= 0 && nc < (int)opciones[nf].size()) {

                    if (!opciones[nf][nc].empty()) {
                        // Se encontró una celda con texto, actualizamos la posición y rompemos el bucle
                        fila = nf;
                        columna = nc;
                        break;
                    }
                    // Si está vacío, continuamos buscando en la misma dirección
                    nf += df;
                    nc += dc;
                }
                // Si el bucle termina y nunca hizo 'break', significa que llegó al borde de la matriz.
                // Como consecuencia, 'fila' y 'columna' conservan su valor original, sin causar colapsos.
            }
        }
        else if (tecla == TECLA_ENTER) {
            eligiendo = false;
        }
    }
    return { fila, columna };
}

// Tu funcion original, ahora como wrapper de la version grid (1 sola columna)
int menuInteractivo(const vector<string>& opciones, int posX, int posY) {
    vector<vector<string>> grid;
    for (const string& s : opciones) grid.push_back({ s });
    return menuInteractivoGrid(grid, posX, posY, 0, 2).first;
}

string obtenerNombreGenero(int id) {
    switch (id) {
    case 10: return "Metal";
    case 20: return "Rock";
	case 40: return "Pop";
    case 60: return "Electronica";
    case 80: return "Reggaeton";
    case 90: return "Cumbia";
    default: return "Desconocido";
    }
}

string obtenerNombreArtista(int id) {
    switch (id) {
        // --- 10: METAL (Thrash/Extremo -> Alternativo -> Clásico/Proto-Rock) ---
    case 11: return "Slayer";
    case 12: return "Pantera";
    case 13: return "Megadeth";
    case 14: return "Metallica";
    case 15: return "Slipknot";
    case 16: return "Korn";
    case 17: return "System of a Down";
    case 18: return "Judas Priest";
    case 19: return "Iron Maiden";
    case 20: return "Black Sabbath"; // Puente hacia Hard Rock

        // --- 20: ROCK (Hard Rock -> Grunge -> Indie -> Clásico/Pop-Rock) ---
    case 21: return "AC/DC";
    case 22: return "Led Zeppelin";
    case 23: return "Nirvana";
    case 24: return "Arctic Monkeys";
    case 25: return "The Strokes";
    case 26: return "Radiohead";
    case 27: return "Soda Stereo";
    case 28: return "Libido";
    case 29: return "Queen";
    case 30: return "The Beatles"; // Puente hacia Pop

        // --- 30: POP (Pop-Rock -> Mainstream -> Dance-Pop) ---
    case 31: return "Coldplay";
    case 32: return "Harry Styles";
    case 33: return "Ed Sheeran";
    case 34: return "Taylor Swift";
    case 35: return "Billie Eilish";
    case 36: return "Ariana Grande";
    case 37: return "Katy Perry";
    case 38: return "Lady Gaga";
    case 39: return "Dua Lipa";
    case 40: return "Michael Jackson"; // Puente hacia Electrónica

        // --- 40: ELECTRONICA (Synth-Pop -> House/EDM -> Experimental/Trap beats) ---
    case 41: return "The Weeknd";
    case 42: return "Daft Punk";
    case 43: return "Calvin Harris";
    case 44: return "David Guetta";
    case 45: return "Tiesto";
    case 46: return "Avicii";
    case 47: return "Zedd";
    case 48: return "Martin Garrix";
    case 49: return "Skrillex";
    case 50: return "Grimes"; // Alternativo/Urbano, puente

        // --- 50: REGGAETON (EDM-Urbano -> Pop-Urbano -> Clásico) ---
    case 51: return "J Balvin";
    case 52: return "Rauw Alejandro";
    case 53: return "Bad Bunny";
    case 54: return "Feid";
    case 55: return "Maluma";
    case 56: return "Karol G";
    case 57: return "Ozuna";
    case 58: return "Daddy Yankee";
    case 59: return "Don Omar";
    case 60: return "Wisin & Yandel"; // Puente hacia ritmos caribeños/cumbia

        // --- 60: CUMBIA (Cumbia Pop/Urbana -> Villera -> Clásica) ---
    case 61: return "Rombai"; // Puente perfecto desde Reggaeton
    case 62: return "Marama";
    case 63: return "Ke Personajes";
    case 64: return "Rafaga";
    case 65: return "Amar Azul";
    case 66: return "Los Angeles Azules";
    case 67: return "Grupo 5";
    case 68: return "Agua Marina";
    case 69: return "Armonia 10";
    case 70: return "Corazon Serrano";

    default: return "Desconocido";
    }
}

string obtenerNombreAlbum(int id) {
    switch (id) {
        // --- ALBUMES METAL (111 - 203) ---
    case 111: return "Reign in Blood"; case 112: return "South of Heaven"; case 113: return "Seasons in the Abyss"; // Slayer
    case 121: return "Vulgar Display of Power"; case 122: return "Cowboys from Hell"; case 123: return "Far Beyond Driven"; // Pantera
    case 131: return "Countdown to Extinction"; case 132: return "Rust in Peace"; case 133: return "Peace Sells"; // Megadeth
    case 141: return "Master of Puppets"; case 142: return "Metallica (Black Album)"; case 143: return "Ride the Lightning"; // Metallica
    case 151: return "Vol. 3"; case 152: return "Iowa"; case 153: return "All Hope Is Gone"; // Slipknot
    case 161: return "Follow the Leader"; case 162: return "Issues"; case 163: return "Korn"; // Korn
    case 171: return "Toxicity"; case 172: return "Mezmerize"; case 173: return "Steal This Album"; // SOAD
    case 181: return "British Steel"; case 182: return "Painkiller"; case 183: return "Screaming for Vengeance"; // Judas
    case 191: return "The Number of the Beast"; case 192: return "Powerslave"; case 193: return "Fear of the Dark"; // Maiden
    case 201: return "Paranoid"; case 202: return "Master of Reality"; case 203: return "Vol. 4"; // Sabbath

        // --- ALBUMES ROCK (211 - 303) ---
    case 211: return "Back in Black"; case 212: return "Highway to Hell"; case 213: return "The Razors Edge"; // AC/DC
    case 221: return "Led Zeppelin IV"; case 222: return "Led Zeppelin II"; case 223: return "Physical Graffiti"; // Zep
    case 231: return "Nevermind"; case 232: return "In Utero"; case 233: return "Bleach"; // Nirvana
    case 241: return "AM"; case 242: return "Favourite Worst Nightmare"; case 243: return "Whatever People Say"; // Arctic
    case 251: return "Room on Fire"; case 252: return "Is This It"; case 253: return "First Impressions of Earth"; // Strokes
    case 261: return "OK Computer"; case 262: return "Pablo Honey"; case 263: return "The Bends"; // Radiohead
    case 271: return "Cancion Animal"; case 272: return "Signos"; case 273: return "Doble Vida"; // Soda
    case 281: return "Libido"; case 282: return "Hembra"; case 283: return "Pop*Porn"; // Libido
    case 291: return "A Night at the Opera"; case 292: return "Jazz"; case 293: return "The Game"; // Queen
    case 301: return "Let It Be"; case 302: return "Past Masters"; case 303: return "Abbey Road"; // Beatles

        // --- ALBUMES POP (311 - 403) ---
    case 311: return "Parachutes"; case 312: return "Viva la Vida"; case 313: return "A Rush of Blood"; // Coldplay
    case 321: return "Harrys House"; case 322: return "Fine Line"; case 323: return "Harry Styles"; // Harry
    case 331: return "Divide"; case 332: return "Multiply"; case 333: return "Subtract"; // Ed Sheeran
    case 341: return "Lover"; case 342: return "Midnights"; case 343: return "The Tortured Poets Department"; // Taylor
    case 351: return "Hit Me Hard and Soft"; case 352: return "Happier Than Ever"; case 353: return "When We All Fall Asleep"; // Billie
    case 361: return "thank u next"; case 362: return "Dangerous Woman"; case 363: return "Sweetener"; // Ariana
    case 371: return "Teenage Dream"; case 372: return "PRISM"; case 373: return "One of the Boys"; // Katy
    case 381: return "The Fame"; case 382: return "Born This Way"; case 383: return "A Star Is Born"; // Gaga
    case 391: return "Future Nostalgia"; case 392: return "Radical Optimism"; case 393: return "Dua Lipa"; // Dua
    case 401: return "Thriller"; case 402: return "Bad"; case 403: return "Off the Wall"; // MJ

        // --- ALBUMES ELECTRONICA (411 - 503) ---
    case 411: return "After Hours"; case 412: return "Dawn FM"; case 413: return "Starboy"; // Weeknd
    case 421: return "Discovery"; case 422: return "Homework"; case 423: return "Random Access Memories"; // Daft Punk
    case 431: return "Motion"; case 432: return "18 Months"; case 433: return "Funk Wav Bounces"; // Calvin
    case 441: return "Nothing but the Beat"; case 442: return "Listen"; case 443: return "One Love"; // Guetta
    case 451: return "Elements of Life"; case 452: return "Kaleidoscope"; case 453: return "The London Sessions"; // Tiesto
    case 461: return "True"; case 462: return "Stories"; case 463: return "Tim"; // Avicii
    case 471: return "Clarity"; case 472: return "True Colors"; case 473: return "Orbit"; // Zedd
    case 481: return "Sentio"; case 482: return "Seven"; case 483: return "The Martin Garrix Collection"; // Garrix
    case 491: return "Scary Monsters"; case 492: return "Bangarang"; case 493: return "Recess"; // Skrillex
    case 501: return "Visions"; case 502: return "Art Angels"; case 503: return "Miss Anthropocene"; // Grimes

        // --- ALBUMES REGGAETON (511 - 603) ---
    case 511: return "Jose"; case 512: return "Colores"; case 513: return "Vibras"; // J Balvin
    case 521: return "Playa del Cielo"; case 522: return "Vice Versa"; case 523: return "Saturno"; // Rauw
    case 531: return "YHLQMDLG"; case 532: return "Un Verano Sin Ti"; case 533: return "Nadie Sabe Lo Que Va a Pasar"; // Bad Bunny
    case 541: return "Feliz Cumpleanos Ferxxo"; case 542: return "MOR"; case 543: return "Inter Shibuya"; // Feid
    case 551: return "F.A.M.E."; case 552: return "Papi Juancho"; case 553: return "11:11"; // Maluma
    case 561: return "Manana Sera Bonito"; case 562: return "KG0516"; case 563: return "Unstoppable"; // Karol G
    case 571: return "Ozutochi"; case 572: return "Odisea"; case 573: return "Aura"; // Ozuna
    case 581: return "Barrio Fino"; case 582: return "El Cartel"; case 583: return "Legendaddy"; // Daddy Yankee
    case 591: return "Meet the Orphans"; case 592: return "King of Kings"; case 593: return "The Last Don"; // Don Omar
    case 601: return "Pal Mundo"; case 602: return "Los Extraterrestres"; case 603: return "La Revolucion"; // Wisin y Yandel

        // --- ALBUMES CUMBIA (611 - 703) ---
    case 611: return "De Fiesta"; case 612: return "Rombai"; case 613: return "Noche Loca"; // Rombai
    case 621: return "Todo Comenzo Bailando"; case 622: return "Enganchados"; case 623: return "Marama"; // Marama
    case 631: return "Ke Personajes"; case 632: return "En Vivo"; case 633: return "Sesiones"; // Ke Personajes
    case 641: return "Imparables"; case 642: return "Un Siglo De Amor"; case 643: return "Vuela"; // Rafaga
    case 651: return "Cumbia Nena"; case 652: return "Gracias a Vos"; case 653: return "Me Enamore"; // Amar Azul
    case 661: return "Como Te Voy A Olvidar"; case 662: return "De Plaza en Plaza"; case 663: return "Esto Si Es Cumbia"; // Los Angeles Azules
    case 671: return "Motor y Motivo"; case 672: return "El Ritmo de mi Corazon"; case 673: return "Lloraras"; // Grupo 5
    case 681: return "Tu Traicion Se Acabo"; case 682: return "Pasitos Para Bailar"; case 683: return "Volumen 22"; // Agua Marina
    case 691: return "El Cervecero"; case 692: return "Siempre Pierdo En El Amor"; case 693: return "Llorando Tu Partida"; // Armonia 10
    case 701: return "Late Mi Corazon"; case 702: return "No Deja de Latir"; case 703: return "Vuelve"; // Corazon Serrano

    default: return "Desconocido";
    }
}