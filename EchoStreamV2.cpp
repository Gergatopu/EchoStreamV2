#include "pch.h"
#include "UI.h"

using namespace std;
using namespace System;

int main() {
    srand((unsigned)time(nullptr));

    GestorBiblioteca biblioteca;
    GestorUsuarios usuarios;

    MenuAcceso app(&biblioteca, &usuarios);
    app.ejecutar();

    return 0;
}