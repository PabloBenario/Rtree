
#include <fstream>
#include <iostream>
#include <functional>

#include "ArchivosRectangulo.h"
#include "Rectangulo.h"
#include "AuxFunctions.h"


#include <chrono>

const unsigned int M = 102;  // Capacidad máxima de HIJOS de un nodo.

std::vector<std::vector<Rectangulo>> agruparRectangulos(const std::vector<Rectangulo>& rectangulos) {
    std::vector<std::vector<Rectangulo>> grupos;

    // Iteramos sobre el vector principal y vamos agregando rectángulos a cada grupo
    for (size_t i = 0; i < rectangulos.size(); i += M) {
        // Uso de `begin() + i` y `begin() + i + M` para delimitar el rango de rectángulos a agregar.
        // La función `min` se utiliza para manejar el caso en que el último grupo tenga menos de M rectángulos.
        grupos.emplace_back(rectangulos.begin() + i, rectangulos.begin() + std::min(i + M, rectangulos.size()));
    }

    return grupos;
}

Nodo* construirRtree(const std::vector<Rectangulo>& rectangulosOrdenados) {
    if (rectangulosOrdenados.empty()) return nullptr;

    std::vector<std::vector<Rectangulo>> gruposs = agruparRectangulos(rectangulosOrdenados);//se agrupan los rectangulos en grupos de tamaño M excepto x el ultimo quizas
    std::vector<Nodo*> nodos = crearNodosDesdeGrupos(gruposs); //CREA LOS n/M nodos hoja 
    std::cout << nodos.size() << std::endl;
    // la variable nodos contiene mis nodos hoja.

    // Si el número de nodos (o rectángulos) es menor a M, entonces simplemente deberíamos tener un nodo raíz que apunte a todos ellos.
    if (nodos.size() <= M) {
        Nodo* raiz = new Nodo();
        for (Nodo* nodo : nodos) {
            raiz->entradas.push_back(Entrada(nodo->calcularMBR(), nodo));

        }

        return raiz;
    }

    // Agrupamos los nodos en grupos de tamaño M y creamos nuevos nodos hasta llegar a la raíz.
    while (nodos.size() > M) {
        std::vector<Nodo*> nodosNuevos;

        for (size_t i = 0; i < nodos.size(); i += M) {
            Nodo* padre = new Nodo();

            // Por cada nodo en este grupo, añadimos una entrada al nodo padre.
            for (size_t j = i; j < i + M && j < nodos.size(); ++j) {
                Nodo* hijo = nodos[j];
                Rectangulo mbr = hijo->calcularMBR();
                padre->entradas.push_back(Entrada(mbr, hijo));
            }

            nodosNuevos.push_back(padre);
        }

        nodos = nodosNuevos;
    }

    // Finalmente, el único nodo que queda es la raíz.
    return nodos[0];
}

void imprimirRtree(Nodo* nodo, int nivel = 0) {
    if (!nodo) {
        std::cout << "hla\n";
        return;

    }
    // Sangría para visualización jerárquica.
    for (int i = 0; i < nivel; ++i) {
        std::cout << "       "; // Usamos dos espacios por nivel.
    }

    // Imprime información del nodo actual.
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



void guardarNodo(const Nodo* nodo, std::ofstream& outFile) {
    if (!nodo) {
        // Puedes decidir qué hacer aquí: ya sea devolver un error, escribir algún valor por defecto o simplemente retornar.
        return;
    }

    if (nodo->entradas[0].hijo) {
        outFile << "N ";  // Nodo interno
    }
    else {
        outFile << "L ";  // Nodo hoja
    }

    outFile << nodo->entradas.size() << std::endl;  // Guardar cuántas entradas tiene el nodo.

    for (const Entrada& entrada : nodo->entradas) {
        // Guardar el MBR.
        outFile << entrada.mbr.x1 << " " << entrada.mbr.y1 << " " << entrada.mbr.x2 << " " << entrada.mbr.y2 << std::endl;

        // Si tiene un hijo, recursivamente guardar ese nodo. Si no, simplemente continúa.
        if (entrada.hijo) {
            guardarNodo(entrada.hijo, outFile);
        }
    }
}


void guardarRTree(const Nodo* raiz, const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        guardarNodo(raiz, outFile);
        outFile.close();
    }
}

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




void imprimirNodo2(const Nodo* nodo, int nivel = 0) {
    // Imprimir indentación basada en el nivel del nodo
    for (int i = 0; i < nivel; ++i) {
        std::cout << "    ";
    }

    // Imprimir el MBR del nodo actual
    Rectangulo mbr = nodo->calcularMBR();
    std::cout << "Nodo (MBR: [" << mbr.x1 << ", " << mbr.y1 << ", " << mbr.x2 << ", " << mbr.y2 << "])" << std::endl;

    // Recorrer las entradas del nodo
    for (const Entrada& entrada : nodo->entradas) {
        for (int i = 0; i < nivel + 1; ++i) {
            std::cout << "    ";
        }

        // Si la entrada tiene un nodo hijo, imprimirlo recursivamente
        if (entrada.hijo) {
            std::cout << "Entrada (MBR: [" << entrada.mbr.x1 << ", " << entrada.mbr.y1 << ", " << entrada.mbr.x2 << ", " << entrada.mbr.y2 << "])" << std::endl;
            imprimirNodo2(entrada.hijo, nivel + 2);
        }
        else {
            std::cout << "Rectángulo: [" << entrada.mbr.x1 << ", " << entrada.mbr.y1 << ", " << entrada.mbr.x2 << ", " << entrada.mbr.y2 << "]" << std::endl;
        }
    }
}

void imprimirRTree2(const Nodo* raiz) {
    imprimirNodo2(raiz);
}



std::vector<std::vector<Rectangulo>*> cargarVectoresDesdeArchivos(const std::vector<unsigned long long>& numeros) {
    std::vector<std::vector<Rectangulo>*> vectores;

    for (unsigned long long num : numeros) {
        // Construye el nombre del archivo en base al número
        std::string filename = "ordered-rectangles" + std::to_string(num) + ".bin";

        // Lee los rectángulos desde el archivo
        std::vector<Rectangulo> rectangulos = leerRectangulos(filename);

        // Crea un nuevo vector en el heap y le copia los rectángulos
        std::vector<Rectangulo>* vectorEnHeap = new std::vector<Rectangulo>(rectangulos);

        // Agrega el puntero al vector en vectores
        vectores.push_back(vectorEnHeap);
    }

    return vectores;
}



std::vector<Nodo*> construirRTreesDesdeVectores(const std::vector<std::vector<Rectangulo>*>& vectoresDeRectangulos) {
    std::vector<Nodo*> rtrees;

    for (const auto& rectangulos : vectoresDeRectangulos) {
        Nodo* rtree = construirRtree(*rectangulos);
        rtrees.push_back(rtree);
    }

    return rtrees;
}


void guardarTodosLosRTrees(const std::vector<Nodo*>& rtrees) {
    for (size_t i = 0; i < rtrees.size(); ++i) {
        std::string filename = "RTree_" + std::to_string(i) + ".txt";
        guardarRTree(rtrees[i], filename);
        std::cout << "Se guardó el R-tree " << i << " en el archivo " << filename << std::endl;
    }
}






std::vector<Rectangulo> crearConsulta() {
    std::vector<Rectangulo> rectangulos;

    for (int i = 0; i < 100; ++i) {
        double x1 = rand() % 500001;               // generar un número entre 0 y 500000
        double y1 = rand() % 500001;
        double ancho = rand() % 100001;            // generar un número entre 0 y 100000 para el ancho
        double alto = rand() % 100001;             // generar un número entre 0 y 100000 para el alto

        rectangulos.push_back(Rectangulo(x1, y1, ancho, alto));
    }

    return rectangulos;
}


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
        auto inicio = std::chrono::high_resolution_clock::now();  // Inicio del cronómetro

        for (const Rectangulo& C : consultas) {
            int accesos = 0;
            buscarRectangulosIntersectados(filename, C, accesos);
            totalAccesos += accesos;
        }

        auto fin = std::chrono::high_resolution_clock::now();  // Fin del cronómetro
        std::chrono::duration<double> tiempo = fin - inicio;  // Duración total de las 100 consultas

        double promedioAccesos = static_cast<double>(totalAccesos) / 100.0;
        double promedioTiempo = tiempo.count() / 100.0;  // Tiempo promedio por consulta

        // Escribir resultados en el archivo CSV
        outFile << filename << "," << promedioAccesos << "," << promedioTiempo << "\n";
    }

    outFile.close();  // Cierra el archivo de resultados
}



int main() {

    std::vector<unsigned long long> numeros;

    for (int i = 10; i <= 25; ++i) {
        numeros.push_back(pow(2, i));
    }
    auto root = leerRTree("RTree_1.txt");
    //imprimirRtree(root);

    // Lista de los nombres de los archivos R-tree
    std::vector<std::string> nombresArchivos;
    for (int i = 0; i <= 15; ++i) {
        nombresArchivos.push_back("RTree_" + std::to_string(i) + ".txt");
    }

    realizarConsultasEnRTrees(nombresArchivos);

    /*
    // Mostrar los números para verificar
    for (unsigned long long num : numeros) {
        std::cout << num << std::endl;

        // Crear el nombre del archivo basado en el número actual
        std::string filename = "rectangles" + std::to_string(num) + ".bin";

        // Escribir los rectángulos para cada número
        escribirRectangulos(num, filename);
        std::cout << "Se escribieron " << num << " rectángulos en " << filename << "\n"; // 1ra línea

        // Ordenar los rectángulos y comprobar si están ordenados
        std::string orderedFilename = "ordered-rectangles" + std::to_string(num) + ".bin";
        ordenarRectangulosPorCentro(filename, orderedFilename);
        if (estanOrdenados(orderedFilename)) {
            std::cout << "Los rectángulos están correctamente ordenados.\n"; // 2da línea
            std::cout << "Se pusieron ordenados en " << orderedFilename << "\n"; // 3ra línea
        }
        else {
            std::cerr << "Los rectángulos no están ordenados.\n";
        }
    }

    */

    //std::vector<std::vector<Rectangulo>*> vectoresDeRectangulos = cargarVectoresDesdeArchivos(numeros);


    //std::vector<Nodo*> rtrees = construirRTreesDesdeVectores(vectoresDeRectangulos);

    //guardarTodosLosRTrees(rtrees);

    /*
    for (size_t i = 0; i < vectoresDeRectangulos.size(); ++i) {
        std::cout << "Tamaño del vector correspondiente a " << numeros[i] << " rectángulos: ";
        std::cout << vectoresDeRectangulos[i]->size() << std::endl;
    }

    // No olvides liberar la memoria en el heap al final
    for (std::vector<Rectangulo>* vector : vectoresDeRectangulos) {
        delete vector;
    }


    for (Nodo* rtree : rtrees) {
        delete rtree;
    }
    */

    //ordenarTodosLosRectangulosPorHilbert(numeros);


    




    return 0;
}

