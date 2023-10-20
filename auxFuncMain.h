#pragma once
#include <fstream>
#include <iostream>
#include <functional>

#include "ArchivosRectangulo.h"
#include "Rectangulo.h"
#include "AuxFunctions.h"


#include <chrono>

const unsigned int M = 102;  // Capacidad m�xima de HIJOS de un nodo.


//funcion auxiliar en construirRtrree
// recibe un vector rectangulos y los pone en grupos de tama�o M
std::vector<std::vector<Rectangulo>> agruparRectangulos(const std::vector<Rectangulo>& rectangulos) {
    std::vector<std::vector<Rectangulo>> grupos;

    // Iteramos sobre el vector principal y vamos agregando rect�ngulos a cada grupo
    for (size_t i = 0; i < rectangulos.size(); i += M) {
        // Uso de `begin() + i` y `begin() + i + M` para delimitar el rango de rect�ngulos a agregar.
        // La funci�n `min` se utiliza para manejar el caso en que el �ltimo grupo tenga menos de M rect�ngulos.
        grupos.emplace_back(rectangulos.begin() + i, rectangulos.begin() + std::min(i + M, rectangulos.size()));
    }

    return grupos;
}



// Construye el Rtree a partir de un vector con los rectangulos ordenados
Nodo* construirRtree(const std::vector<Rectangulo>& rectangulosOrdenados) {
    if (rectangulosOrdenados.empty()) return nullptr;

    std::vector<std::vector<Rectangulo>> gruposs = agruparRectangulos(rectangulosOrdenados);//se agrupan los rectangulos en grupos de tama�o M excepto x el ultimo quizas
    std::vector<Nodo*> nodos = crearNodosDesdeGrupos(gruposs); //CREA LOS n/M nodos hoja 
    std::cout << nodos.size() << std::endl;
    // la variable nodos contiene mis nodos hoja.

    // Si el n�mero de nodos (o rect�ngulos) es menor a M, entonces simplemente deber�amos tener un nodo ra�z que apunte a todos ellos.
    if (nodos.size() <= M) {
        Nodo* raiz = new Nodo();
        for (Nodo* nodo : nodos) {
            raiz->entradas.push_back(Entrada(nodo->calcularMBR(), nodo));

        }

        return raiz;
    }

    // Agrupamos los nodos en grupos de tama�o M y creamos nuevos nodos hasta llegar a la ra�z.
    while (nodos.size() > M) {
        std::vector<Nodo*> nodosNuevos;

        for (size_t i = 0; i < nodos.size(); i += M) {
            Nodo* padre = new Nodo();

            // Por cada nodo en este grupo, a�adimos una entrada al nodo padre.
            for (size_t j = i; j < i + M && j < nodos.size(); ++j) {
                Nodo* hijo = nodos[j];
                Rectangulo mbr = hijo->calcularMBR();
                padre->entradas.push_back(Entrada(mbr, hijo));
            }

            nodosNuevos.push_back(padre);
        }

        nodos = nodosNuevos;
    }

    // Finalmente, el �nico nodo que queda es la ra�z.
    return nodos[0];
}



//recibe un puntero a la raiz y imprime en Cmd el arbol
void imprimirRtree(Nodo* nodo, int nivel = 0) {
    if (!nodo) {
        std::cout << "hla\n";
        return;

    }
    // Sangr�a para visualizaci�n jer�rquica.
    for (int i = 0; i < nivel; ++i) {
        std::cout << "       "; // Usamos dos espacios por nivel.
    }

    // Imprime informaci�n del nodo actual.
    Rectangulo mbrNodo = nodo->calcularMBR();
    std::cout << "Nodo (MBR: [" << mbrNodo.x1 << ", " << mbrNodo.y1 << ", " << mbrNodo.x2 << ", " << mbrNodo.y2 << "])" << std::endl;

    // Si es un nodo hoja, imprime directamente las entradas.
    if (nodo->entradas[0].hijo == nullptr) {
        for (const auto& entrada : nodo->entradas) {
            for (int i = 0; i < nivel + 1; ++i) {
                std::cout << "           ";
            }
            std::cout << "Entrada (MBR: [" << entrada.mbr.x1 << ", " << entrada.mbr.y1 << ", " << entrada.mbr.x2 << ", " << entrada.mbr.y2 << "])" << std::endl;
        }
    }
    else {
        // Si no es una hoja, imprimimos recursivamente los hijos.
        for (const auto& entrada : nodo->entradas) {
            imprimirRtree(entrada.hijo, nivel + 1);
        }
    }
}


//guarda un nodo del arbol en un archivo, sirve para implementar la funcion que guarda el arbol entero en el archivo
void guardarNodo(const Nodo* nodo, std::ofstream& outFile) {
    if (!nodo) {
        // Puedes decidir qu� hacer aqu�: ya sea devolver un error, escribir alg�n valor por defecto o simplemente retornar.
        return;
    }

    if (nodo->entradas[0].hijo) {
        outFile << "N ";  // Nodo interno
    }
    else {
        outFile << "L ";  // Nodo hoja
    }

    outFile << nodo->entradas.size() << std::endl;  // Guardar cu�ntas entradas tiene el nodo.

    for (const Entrada& entrada : nodo->entradas) {
        // Guardar el MBR.
        outFile << entrada.mbr.x1 << " " << entrada.mbr.y1 << " " << entrada.mbr.x2 << " " << entrada.mbr.y2 << std::endl;

        // Si tiene un hijo, recursivamente guardar ese nodo. Si no, simplemente contin�a.
        if (entrada.hijo) {
            guardarNodo(entrada.hijo, outFile);
        }
    }
}


//funcion que recibe un puntero a la root y guarda el rtree en el archivo
void guardarRTree(const Nodo* raiz, const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        guardarNodo(raiz, outFile);
        outFile.close();
    }
}


//esta funcion es importante porque un nodo del archivo, es decir extrae un bloque
Nodo* leerNodo(std::ifstream& inFile) {
    char tipoNodo;
    inFile >> tipoNodo;

    Nodo* nodo = new Nodo();
    size_t numEntradas;
    inFile >> numEntradas;

    for (size_t i = 0; i < numEntradas; ++i) {
        Rectangulo mbr;
        inFile >> mbr.x1 >> mbr.y1 >> mbr.x2 >> mbr.y2;

        Entrada entrada(mbr);
        if (tipoNodo == 'N') {
            entrada.hijo = leerNodo(inFile);
        }

        nodo->entradas.push_back(entrada);
    }

    return nodo;
}


//funcion que lee el arbol entero y retorna un puntero a su raiz
//va haciendo lecturas de bloques sucesivas gracias a la implementacion de 
// leerNodo()
Nodo* leerRTree(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << "\n";
        return nullptr;
    }

    Nodo* raiz = leerNodo(inFile);
    inFile.close();
    return raiz;
}


//ESTA FUNCION ES IMPORTANTE Y SIRVE COMO AYUDA PARA implementar la interseccion de rectangulos
// ya que esta funcion ejemplifica lo que es un acceso a disco cuando extrae el tipo de nodo 
//y sus entradas para luego buscar recursivamente
void buscarNodo(std::ifstream& inFile, const Rectangulo& C, std::vector<Rectangulo>& resultados, int& accesos) {
    accesos++;

    char tipoNodo;
    inFile >> tipoNodo;

    size_t numEntradas;
    inFile >> numEntradas;

    for (size_t i = 0; i < numEntradas; ++i) {
        Rectangulo mbr;
        inFile >> mbr.x1 >> mbr.y1 >> mbr.x2 >> mbr.y2;

        if (C.intersecta(mbr)) {
            if (tipoNodo == 'N') { // Nodo interno
                buscarNodo(inFile, C, resultados, accesos);
            }
            else { // Nodo hoja
                resultados.push_back(mbr);
            }
        }
    }
}

//esta funcion usaa la anterior pero retornar el array de rectangulos intersecatados y contar los accesos
std::vector<Rectangulo> buscarRectangulosIntersectados(const std::string& filename, const Rectangulo& C, int& accesos) {
    std::vector<Rectangulo> resultados;
    accesos = 0;

    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << "\n";
        return resultados;
    }

    buscarNodo(inFile, C, resultados, accesos);

    inFile.close();
    return resultados;
}


// creo que no la usamos alfinal
int contarNodos(const Nodo* nodo) {
    if (!nodo) return 0;

    int cuenta = 1; // Contar el nodo actual.

    for (const auto& entrada : nodo->entradas) {
        if (entrada.hijo) { // Si tiene hijo, recorrer recursivamente.
            cuenta += contarNodos(entrada.hijo);
        }
    }

    return cuenta;
}





// funcion de utilidad importante que devuelve vectores de rectangulos de tama�o n leyendolos de los binarios que
// contienen los binarios con rectangulos YA ORDENADOS !!
std::vector<std::vector<Rectangulo>*> cargarVectoresDesdeArchivos(const std::vector<unsigned long long>& numeros) {
    std::vector<std::vector<Rectangulo>*> vectores;

    for (unsigned long long num : numeros) {
        // Construye el nombre del archivo en base al n�mero
        std::string filename = "ordered-rectangles" + std::to_string(num) + ".bin";

        // Lee los rect�ngulos desde el archivo
        std::vector<Rectangulo> rectangulos = leerRectangulos(filename);

        // Crea un nuevo vector en el heap y le copia los rect�ngulos
        std::vector<Rectangulo>* vectorEnHeap = new std::vector<Rectangulo>(rectangulos);

        // Agrega el puntero al vector en vectores
        vectores.push_back(vectorEnHeap);
    }

    return vectores;
}


//el nombre lo dice
std::vector<Nodo*> construirRTreesDesdeVectores(const std::vector<std::vector<Rectangulo>*>& vectoresDeRectangulos) {
    std::vector<Nodo*> rtrees;

    for (const auto& rectangulos : vectoresDeRectangulos) {
        Nodo* rtree = construirRtree(*rectangulos);
        rtrees.push_back(rtree);
    }

    return rtrees;
}


//guarda todo los Rtree en memoria
void guardarTodosLosRTrees(const std::vector<Nodo*>& rtrees) {
    for (size_t i = 0; i < rtrees.size(); ++i) {
        std::string filename = "RTree_" + std::to_string(i) + ".txt";
        guardarRTree(rtrees[i], filename);
        std::cout << "Se guard� el R-tree " << i << " en el archivo " << filename << std::endl;
    }
}





// crea un vector de consulta()
std::vector<Rectangulo> crearConsulta() {
    std::vector<Rectangulo> rectangulos;

    for (int i = 0; i < 100; ++i) {
        double x1 = rand() % 500001;               // generar un n�mero entre 0 y 500000
        double y1 = rand() % 500001;
        double ancho = rand() % 100001;            // generar un n�mero entre 0 y 100000 para el ancho
        double alto = rand() % 100001;             // generar un n�mero entre 0 y 100000 para el alto

        rectangulos.push_back(Rectangulo(x1, y1, ancho, alto));
    }

    return rectangulos;
}


//funcion que toma el tiempo de la consultas, recibe un vector con el nombre de los archivos donde estan guardados
// los tree y retorna las consultas y el tiempo en un csv
void realizarConsultasEnRTrees(const std::vector<std::string>& nombresArchivos) {
    // Generar 100 consultas
    std::vector<Rectangulo> consultas = crearConsulta();

    // Abre el archivo para escritura
    std::ofstream outFile("resultados_metodo1.csv");
    if (!outFile.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo de resultados." << std::endl;
        return;
    }

    // Escribir encabezados en el archivo CSV
    outFile << "Nombre del R-tree,Promedio de accesos,Tiempo promedio de consulta (segundos)\n";

    for (const std::string& filename : nombresArchivos) {  // Para cada R-tree
        int totalAccesos = 0;
        auto inicio = std::chrono::high_resolution_clock::now();  // Inicio del cron�metro

        for (const Rectangulo& C : consultas) {
            int accesos = 0;
            buscarRectangulosIntersectados(filename, C, accesos);
            totalAccesos += accesos;
        }

        auto fin = std::chrono::high_resolution_clock::now();  // Fin del cron�metro
        std::chrono::duration<double> tiempo = fin - inicio;  // Duraci�n total de las 100 consultas

        double promedioAccesos = static_cast<double>(totalAccesos) / 100.0;
        double promedioTiempo = tiempo.count() / 100.0;  // Tiempo promedio por consulta

        // Escribir resultados en el archivo CSV
        outFile << filename << "," << promedioAccesos << "," << promedioTiempo << "\n";
    }

    outFile.close();  // Cierra el archivo de resultados
}
