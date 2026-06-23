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
        // --- 10: METAL ---
    case 11: return "Metallica";
    case 12: return "Megadeth";
    case 13: return "Iron Maiden";
    case 14: return "Black Sabbath";
    case 15: return "Judas Priest";
    case 16: return "Slayer";
    case 17: return "Pantera";
    case 18: return "Slipknot";
    case 19: return "System of a Down";
    case 20: return "Korn";

        // --- 20: ROCK ---
    case 21: return "AC/DC";
    case 22: return "Led Zeppelin";
    case 23: return "Nirvana";
    case 24: return "Queen";
    case 25: return "The Strokes";
    case 26: return "Arctic Monkeys";
    case 27: return "Radiohead";
    case 28: return "Libido";
    case 29: return "Soda Stereo";
    case 30: return "The Beatles";

        // --- 40: POP ---
    case 41: return "Coldplay";
    case 42: return "Ed Sheeran";
    case 43: return "Taylor Swift";
    case 44: return "Dua Lipa";
    case 45: return "Billie Eilish";
    case 46: return "Ariana Grande";
    case 47: return "Katy Perry";
    case 48: return "Lady Gaga";
    case 49: return "Michael Jackson";
    case 50: return "Harry Styles";

        // --- 60: ELECTRONICA ---
    case 61: return "Daft Punk";
    case 62: return "Calvin Harris";
    case 63: return "The Weeknd";
    case 64: return "Avicii";
    case 65: return "David Guetta";
    case 66: return "Grimes";
    case 67: return "Tiesto";
    case 68: return "Skrillex";
    case 69: return "Martin Garrix";
    case 70: return "Zedd";

        // --- 80: REGGAETON ---
    case 81: return "Daddy Yankee";
    case 82: return "Don Omar";
    case 83: return "Wisin & Yandel";
    case 84: return "Bad Bunny";
    case 85: return "J Balvin";
    case 86: return "Ozuna";
    case 87: return "Rauw Alejandro";
    case 88: return "Maluma";
    case 89: return "Karol G";
    case 90: return "Feid";

        // --- 90: CUMBIA ---
    case 91: return "Agua Marina";
    case 92: return "Grupo 5";
    case 93: return "Armonia 10";
    case 94: return "Corazon Serrano";
    case 95: return "Los Angeles Azules";
    case 96: return "Rombai";
    case 97: return "Marama";
    case 98: return "Ke Personajes";
    case 99: return "Rafaga";
    case 100: return "Amar Azul";

    default: return "Desconocido";
    }
}

string obtenerNombreAlbum(int id) {
    switch (id) {
        // --- ALBUMES METAL (111 - 203) ---
    case 111: return "Master of Puppets"; case 112: return "Metallica (Black Album)"; case 113: return "Ride the Lightning";
    case 121: return "Countdown to Extinction"; case 122: return "Rust in Peace"; case 123: return "Peace Sells";
    case 131: return "The Number of the Beast"; case 132: return "Powerslave"; case 133: return "Fear of the Dark";
    case 141: return "Paranoid"; case 142: return "Master of Reality"; case 143: return "Vol. 4";
    case 151: return "British Steel"; case 152: return "Painkiller"; case 153: return "Screaming for Vengeance";
    case 161: return "Reign in Blood"; case 162: return "South of Heaven"; case 163: return "Seasons in the Abyss";
    case 171: return "Vulgar Display of Power"; case 172: return "Cowboys from Hell"; case 173: return "Far Beyond Driven";
    case 181: return "Vol. 3"; case 182: return "Iowa"; case 183: return "All Hope Is Gone";
    case 191: return "Toxicity"; case 192: return "Mezmerize"; case 193: return "Steal This Album";
    case 201: return "Follow the Leader"; case 202: return "Issues"; case 203: return "Korn";

        // --- ALBUMES ROCK (211 - 303) ---
    case 211: return "Back in Black"; case 212: return "Highway to Hell"; case 213: return "The Razors Edge";
    case 221: return "Led Zeppelin IV"; case 222: return "Led Zeppelin II"; case 223: return "Physical Graffiti";
    case 231: return "Nevermind"; case 232: return "In Utero"; case 233: return "Bleach";
    case 241: return "A Night at the Opera"; case 242: return "Jazz"; case 243: return "The Game";
    case 251: return "Room on Fire"; case 252: return "Is This It"; case 253: return "First Impressions of Earth";
    case 261: return "AM"; case 262: return "Favourite Worst Nightmare"; case 263: return "Whatever People Say";
    case 271: return "OK Computer"; case 272: return "Pablo Honey"; case 273: return "The Bends";
    case 281: return "Libido"; case 282: return "Hembra"; case 283: return "Pop*Porn";
    case 291: return "Cancion Animal"; case 292: return "Signos"; case 293: return "Doble Vida";
    case 301: return "Let It Be"; case 302: return "Past Masters"; case 303: return "Abbey Road";

        // --- ALBUMES POP (411 - 503) ---
    case 411: return "Parachutes"; case 412: return "Viva la Vida"; case 413: return "A Rush of Blood";
    case 421: return "Divide"; case 422: return "Multiply"; case 423: return "Subtract";
    case 431: return "Lover"; case 432: return "Midnights"; case 433: return "The Tortured Poets Department";
    case 441: return "Future Nostalgia"; case 442: return "Radical Optimism"; case 443: return "Dua Lipa";
    case 451: return "Hit Me Hard and Soft"; case 452: return "Happier Than Ever"; case 453: return "When We All Fall Asleep";
    case 461: return "thank u next"; case 462: return "Dangerous Woman"; case 463: return "Sweetener";
    case 471: return "Teenage Dream"; case 472: return "PRISM"; case 473: return "One of the Boys";
    case 481: return "The Fame"; case 482: return "Born This Way"; case 483: return "A Star Is Born";
    case 491: return "Thriller"; case 492: return "Bad"; case 493: return "Off the Wall";
    case 501: return "Harrys House"; case 502: return "Fine Line"; case 503: return "Harry Styles";

        // --- ALBUMES ELECTRONICA (611 - 703) ---
    case 611: return "Discovery"; case 612: return "Homework"; case 613: return "Random Access Memories";
    case 621: return "Motion"; case 622: return "18 Months"; case 623: return "Funk Wav Bounces";
    case 631: return "After Hours"; case 632: return "Dawn FM"; case 633: return "Starboy";
    case 641: return "True"; case 642: return "Stories"; case 643: return "Tim";
    case 651: return "Nothing but the Beat"; case 652: return "Listen"; case 653: return "One Love";
    case 661: return "Visions"; case 662: return "Art Angels"; case 663: return "Miss Anthropocene";
    case 671: return "Elements of Life"; case 672: return "Kaleidoscope"; case 673: return "The London Sessions";
    case 681: return "Scary Monsters"; case 682: return "Bangarang"; case 683: return "Recess";
    case 691: return "Sentio"; case 692: return "Seven"; case 693: return "The Martin Garrix Collection";
    case 701: return "Clarity"; case 702: return "True Colors"; case 703: return "Orbit";

        // --- ALBUMES REGGAETON (811 - 903) ---
    case 811: return "Barrio Fino"; case 812: return "El Cartel"; case 813: return "Legendaddy";
    case 821: return "Meet the Orphans"; case 822: return "King of Kings"; case 823: return "The Last Don";
    case 831: return "Pal Mundo"; case 832: return "Los Extraterrestres"; case 833: return "La Revolucion";
    case 841: return "YHLQMDLG"; case 842: return "Un Verano Sin Ti"; case 843: return "Nadie Sabe Lo Que Va a Pasar";
    case 851: return "Jose"; case 852: return "Colores"; case 853: return "Vibras";
    case 861: return "Ozutochi"; case 862: return "Odisea"; case 863: return "Aura";
    case 871: return "Playa del Cielo"; case 872: return "Vice Versa"; case 873: return "Saturno";
    case 881: return "F.A.M.E."; case 882: return "Papi Juancho"; case 883: return "11:11";
    case 891: return "Manana Sera Bonito"; case 892: return "KG0516"; case 893: return "Unstoppable";
    case 901: return "Feliz Cumpleanos Ferxxo"; case 902: return "MOR"; case 903: return "Inter Shibuya";

        // --- ALBUMES CUMBIA (911 - 1003) ---
    case 911: return "Tu Traicion Se Acabo"; case 912: return "Pasitos Para Bailar"; case 913: return "Volumen 22";
    case 921: return "Motor y Motivo"; case 922: return "El Ritmo de mi Corazon"; case 923: return "Lloraras";
    case 931: return "El Cervecero"; case 932: return "Siempre Pierdo En El Amor"; case 933: return "Llorando Tu Partida";
    case 941: return "Late Mi Corazon"; case 942: return "No Deja de Latir"; case 943: return "Vuelve";
    case 951: return "Como Te Voy A Olvidar"; case 952: return "De Plaza en Plaza"; case 953: return "Esto Si Es Cumbia";
    case 961: return "De Fiesta"; case 962: return "Rombai"; case 963: return "Noche Loca";
    case 971: return "Todo Comenzo Bailando"; case 972: return "Enganchados"; case 973: return "Marama";
    case 981: return "Ke Personajes"; case 982: return "En Vivo"; case 983: return "Sesiones";
    case 991: return "Imparables"; case 992: return "Un Siglo De Amor"; case 993: return "Vuela";
    case 1001: return "Cumbia Nena"; case 1002: return "Gracias a Vos"; case 1003: return "Me Enamore";

    default: return "Desconocido";
    }
}