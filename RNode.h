#pragma once
#include "Rectangulo.h"
#include <vector>


// Declaración adelantada de Nodo.
struct Nodo;

struct Entrada {
public:
    Rectangulo mbr;
    Nodo* hijo;

    Entrada(const Rectangulo& r);
    Entrada(const Rectangulo& r, Nodo* h);
};

struct Nodo {
public:
    std::vector<Entrada> entradas;

    Nodo();
    Nodo(const std::vector<Rectangulo>& rects);
    Nodo(const std::vector<Nodo*>& childNodes);

    Rectangulo calcularMBR();  // Asegúrate de implementar esta función o marcarla como virtual y proveer una implementación en una subclase.
};
