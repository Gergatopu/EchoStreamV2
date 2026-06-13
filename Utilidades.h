#pragma once
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace System;

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