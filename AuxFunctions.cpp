#include "AuxFunctions.h"

//funciones auxiliares, aveces sirvieron pero muy poco
// las fuimos dejando aca mientras desarollabamos

Rectangulo Nodo::calcularMBR() const {
    if (entradas.empty()) {
        return Rectangulo();  // Retorna un rectángulo vacío si no hay entradas.
    }

    double minX = entradas[0].mbr.x1;
    double minY = entradas[0].mbr.y1;
    double maxX = entradas[0].mbr.x2;
    double maxY = entradas[0].mbr.y2;

    for (const auto& entrada : entradas) {
        minX = std::min(minX, entrada.mbr.x1);
        minY = std::min(minY, entrada.mbr.y1);
        maxX = std::max(maxX, entrada.mbr.x2);
        maxY = std::max(maxY, entrada.mbr.y2);
    }

    return Rectangulo(minX, minY, maxX - minX, maxY - minY);
}

std::vector<Nodo*> crearNodosDesdeGrupos(const std::vector<std::vector<Rectangulo>>& grupos) {
    std::vector<Nodo*> nodos;

    // Por cada grupo de rectángulos, crea un nodo
    for (const auto& grupo : grupos) {
        Nodo* nodoActual = new Nodo(); // Crea un Nodo en el heap

        // Por cada rectángulo en el grupo, crea una entrada y agrégala al nodo
        for (const Rectangulo& rect : grupo) {
            Entrada entrada(rect);
            nodoActual->entradas.push_back(entrada);
        }

        nodos.push_back(nodoActual);
    }

    return nodos;
}


void imprimirMBRsDeNodos(const std::vector<Nodo>& nodos) {
    int nodoNumero = 1;  // Contador para indicar el número de nodo.

    for (const Nodo& nodo : nodos) {
        std::cout << "Nodo " << nodoNumero << " MBR:\n";
        nodo.calcularMBR().imprimir();
        std::cout << "--------------------------\n";  // Separador
        nodoNumero++;
    }
}

std::vector<Rectangulo> calcularMBRsDeNodos(const std::vector<Nodo>& nodos) {
    std::vector<Rectangulo> mbrs;

    // Para cada nodo en el vector, calcular su MBR y agregarlo al vector resultante.
    for (const Nodo& nodo : nodos) {
        mbrs.push_back(nodo.calcularMBR());
    }

    return mbrs;
}

