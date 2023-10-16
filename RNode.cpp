#include "RNode.h"



// Definici�n del constructor por defecto
Nodo::Nodo() {
    // Implementaci�n del constructor (si es que necesitas hacer algo aqu�)
}

// Definici�n del constructor con rect�ngulos
Nodo::Nodo(const std::vector<Rectangulo>& rects) {
    for (const Rectangulo& r : rects) {
        entradas.push_back(Entrada(r));
    }
}

// Definici�n del constructor con nodos
Nodo::Nodo(const std::vector<Nodo*>& childNodes) {
    for (Nodo* child : childNodes) {
        entradas.push_back(Entrada(child->calcularMBR(), child));
    }
}

Rectangulo Nodo::calcularMBR() {
    if (entradas.empty()) {
        return Rectangulo();  // Un rect�ngulo por defecto si no hay entradas.
    }

    Rectangulo mbr = entradas[0].mbr;
    for (size_t i = 1; i < entradas.size(); i++) {
        const Rectangulo& rect = entradas[i].mbr;

        mbr.x1 = std::min(mbr.x1, rect.x1);
        mbr.y1 = std::min(mbr.y1, rect.y1);
        mbr.x2 = std::max(mbr.x2, rect.x2);
        mbr.y2 = std::max(mbr.y2, rect.y2);
    }

    return mbr;
}

