#pragma once
#include "Rectangulo.h"
#include <vector>


// Declaraci�n adelantada de Nodo.
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

    Rectangulo calcularMBR();  // Aseg�rate de implementar esta funci�n o marcarla como virtual y proveer una implementaci�n en una subclase.
};
