#include "pch.h"
#include "UI.h"

using namespace System;
using namespace std;


int main() {
    srand((unsigned)time(nullptr));

    Gestor gestor;
    AppUI app(&gestor);
    app.ejecutar();

    return 0;
}