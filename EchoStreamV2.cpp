#include "pch.h"
#include "UI.h"

using namespace System;
using namespace std;


int main() {
    srand((unsigned)time(nullptr));

    GestorBiblioteca biblioteca;
    GestorUsuarios usuarios(&biblioteca);

    MenuAcceso app(&biblioteca, &usuarios);
    app.ejecutar();

    return 0;
}