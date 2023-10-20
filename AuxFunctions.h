#pragma once
#include <iostream>
#include <vector>
#include <algorithm>  // Para std::min y std::max
#include "Rectangulo.h"

//aqui definimos la estructura del nodo de nuestro rtree

struct Nodo;  // Pre-declaración.

struct Entrada {
    Rectangulo mbr;
    Nodo* hijo;

    Entrada() : hijo(nullptr) {}  // Constructor por defecto.
    Entrada(const Rectangulo& r) : mbr(r), hijo(nullptr) {}  // Para hojas.
    Entrada(const Rectangulo& r, Nodo* h) : mbr(r), hijo(h) {}  // Para nodos internos.
};
struct Nodo {
    std::vector<Entrada> entradas;  // Utilizando un vector dinámico.

    // Método para calcular el MBR del nodo, que engloba todos los MBRs de sus entradas.
    Rectangulo calcularMBR() const;
};


std::vector<Nodo*> crearNodosDesdeGrupos(const std::vector<std::vector<Rectangulo>>& grupos);

void imprimirMBRsDeNodos(const std::vector<Nodo>& nodos);

std::vector<Rectangulo> calcularMBRsDeNodos(const std::vector<Nodo>& nodos);


