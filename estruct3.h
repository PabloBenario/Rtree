#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "Rectangulo.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

struct Punto {
    double x;
    double y;
};

void ordenarPuntosPorCoordenadaX(std::vector<Punto>& puntos) {
    std::sort(puntos.begin(), puntos.end(), [](const Punto& a, const Punto& b) {
        return a.x < b.x;
        });
}

void ordenarPuntosPorCoordenadaY(std::vector<Punto>& puntos) {
    std::sort(puntos.begin(), puntos.end(), [](const Punto& a, const Punto& b) {
        return a.y < b.y;
        });
}

void ordenarPuntosSTR(const std::string& archivoEntrada, const std::string& archivoSalida, int M) {
    std::ifstream inFile(archivoEntrada, std::ios::binary);
    if (!inFile) {
        std::cerr << "Error al abrir el archivo de entrada." << std::endl;
        return;
    }

    std::vector<Rectangulo> rectangulos;
    Rectangulo rect;
    while (inFile.read(reinterpret_cast<char*>(&rect), sizeof(Rectangulo))) {
        rectangulos.push_back(rect);
    }
    inFile.close();

    std::vector<Punto> puntos;
    for (const Rectangulo& rect : rectangulos) {
        Punto punto;
        punto.x = rect.getCenterX();
        punto.y = rect.getCenterY();
        puntos.push_back(punto);
    }

    int n = puntos.size();
    int S = static_cast<int>(std::sqrt(n / M));
    if (S < 1) {
        return;  // No se puede dividir más
    }

    // Ordenar inicialmente por coordenada X
    ordenarPuntosPorCoordenadaX(puntos);

    // Dividir en S grupos
    for (int i = 0; i < S; ++i) {
        int grupoInicio = i * M;
        int grupoFin = (i == S - 1) ? n : grupoInicio + M;
        // Ordenar cada grupo por coordenada Y
        ordenarPuntosPorCoordenadaY(puntos);
    }

    // Dividir recursivamente
    for (int i = 0; i < S; ++i) {
        int grupoInicio = i * M;
        int grupoFin = (i == S - 1) ? n : grupoInicio + M;
        ordenarPuntosSTR(archivoEntrada, archivoSalida, M);
    }

    // Escribir los puntos ordenados en el archivo de salida
    std::ofstream outFile(archivoSalida, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error al abrir el archivo de salida." << std::endl;
        return;
    }

    for (const Punto& p : puntos) {
        outFile.write(reinterpret_cast<const char*>(&p), sizeof(Punto));
    }
    outFile.close();
}