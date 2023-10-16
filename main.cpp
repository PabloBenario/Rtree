
#include <fstream>
#include <iostream>
#include <functional>

#include "ArchivosRectangulo.h"
#include "Rectangulo.h"
#include "AuxFunctions.h"

const unsigned int M = 40;  // Capacidad máxima de HIJOS de un nodo.
const unsigned int N = 600; 



Nodo* construirRtree(const std::vector<Rectangulo>& rectangulosOrdenados) {
    if (rectangulosOrdenados.empty()) return nullptr;

    // Convertir los rectángulos en nodos hoja.
    std::vector<Nodo*> nodos;
    for (const auto& rect : rectangulosOrdenados) {
        Nodo* nodo = new Nodo();
        nodo->entradas.push_back(Entrada(rect));
        nodos.push_back(nodo);
    }
    for (const auto& nodo_ptr : nodos) {
        nodo_ptr->entradas[0].mbr.imprimir(); //ESTO SOLO PARA COMPROBAR QUE CREA LOS NODOS HOJA!!!
    }

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
    if (!nodo) return;

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

        // Si tiene un hijo, recursivamente guardar ese nodo.
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






int main() {

    //ver si pudimos escribir los rectangulos
    escribirRectangulos(N, "rectangles.bin"); 
    std::cout << "Se escribieron " << N << " rectangulos en rectangles.bin\n";               //1ra linea


    //este bloque de codigo es solo para chequear si ordenamos bien los rectangulos
    std::string filename = "ordered-rectangles.bin";
    ordenarRectangulosPorCentro("rectangles.bin", filename);
    if (estanOrdenados("ordered-rectangles.bin")) {  
        std::cout << "Los rectangulos estan correctamente ordenados.\n";                    //2da linea
        std::cout << "Se pusieron ordenados en ordered-rectangles.bin\n";                   //3ra linea 
    }
    else {
        std::cerr << "Los rectángulos no están ordenados.\n";
    }


    //estas dos lineas solo revisa que se lean los rectangulos ORDENADOS desde el binario
    //que los TENIA ordenados.
    std::vector < Rectangulo > rectangles = leerRectangulos(filename); 
    std::cout << "Se leyeron los rectangulos de ordered-bin y se guardaron en un array\n"; //4ta linea

    
    // PARA MAS COMPROBACION DE LO ANTERIOR.
    // en esta las lineas.
    // imprimimos todos los rectangulos ordenados.
    std::cout << "\nAhora los imprimimos ordenados (estos rectangulos serian las hojas)\n";                                    //5ta linea
    for (const Rectangulo& rect : rectangles) {
        rect.imprimir();
    }


    // Aqui construimos el R-tree EN RAM y lo imprimimos
    std::cout << "\nComenzamos la construccion del R-tree\n";
    Nodo* raiz = construirRtree(rectangles);
    std::cout << "Construido!\n";
    std::cout << "\nAHORA LO IMPRIMIMOS\n";
    std::cout << "-----------------" << std::endl;
    imprimirRtree(raiz, 0);


    // este bloque es importante porque aqui guardamos el arbol anterior en.
    // disco y lo volvimos a imprimir para comprobar que sea igual al anterior.
    std::cout << "\n\n";
    guardarRTree(raiz, "Rtree.bin");
    Nodo* raizLeida = leerRTree("Rtree.bin");
    std::cout << "Arbol R leido del archivo, pero leido desde el archivo:\n";
    std::cout << "--------------------------\n";
    imprimirRtree(raizLeida, 0);
    std::cout << "--------------------------\n";

    int accesos = 0;
    Rectangulo C(0.1,0.1, 499999.0, 499999.0 );  // Define tu rectángulo de consulta aquí.
    std::vector<Rectangulo> rectangulosIntersectados = buscarRectangulosIntersectados("Rtree.bin", C, accesos);
    std::cout << "Accesos a disco: " << accesos << std::endl;
    for (const Rectangulo& rect : rectangulosIntersectados) {
        std::cout << "\nAAASHDJAakakaHAHD\n";
        rect.imprimir();
    }




           


    // Crear y mostrar grupos de rectángulos
    std::cout << "\nAhora los agrupamos en grupos de tamano" << M << " y los imprimimos : \n";
    std::vector < std::vector < Rectangulo >> grupos = agruparRectangulos(rectangles, M);
    int numeroGrupo = 1; // Iniciar contador de grupos
    for (const auto& grupo : grupos) {
        std::cout << "\n---- Grupo " << numeroGrupo << " ----\n";
        for (const Rectangulo& rect : grupo) {
            rect.imprimir();
        }
        std::cout << "--------------\n";
        numeroGrupo++;
    }
    std::cout << "\n--------------------------" << "\n--------------------------\n";



    std::cout << "\n111111111111111111\n";
    // Después de haber creado los grupos:
    std::vector<Nodo> nodos = crearNodosDesdeGrupos(grupos);
    std::cout << "Aqui los grupos que creamos antes los transformamos en nodos!\n";
    std::cout << "Se crearon " << nodos.size() << " nodos." << std::endl;
    std::cout << "\n--------------------------\n";
    
    std::cout << "\nprinteamos los hijos de cada unos de estos nodos\n";
    for (size_t i = 0; i < nodos.size(); ++i) {
        std::cout << "Nodo " << (i + 1) << " tiene los siguientes hijos:\n";
        for (const Entrada& entrada : nodos[i].entradas) {
            if (entrada.hijo) {
                // Si tienes alguna forma de identificar cada nodo, la puedes imprimir aquí.
                // Por simplicidad, aquí solo indicamos que hay un hijo.
                std::cout << "  - Tiene un hijo\n";
              

            }
            else {
                // No hay hijo, solo es una entrada de hoja.
                entrada.mbr.imprimir();
            }
        }
        std::cout << "------------------------\n";
    }


    //Aqui calculamos el MBR de cada uno de los nodos que creamos antes y los ordenamos, obviamente
    std::cout << "\nAqui calculamos el MBR de cada uno de los nodos que creamos antes y los ordenamos, obviamente\n";
    std::vector<Rectangulo> rectangulosMBRS = calcularMBRsDeNodos(nodos); //ESTAN DESORDENADOS.
    ordenarPorCentroX(rectangulosMBRS);
    // Imprimir rectángulos MBRS de 
    std::cout << "\nAhora los imprimimos \n" << "--------------------------\n";
    for (const Rectangulo& rect : rectangulosMBRS) {
        rect.imprimir();
    }
    std::cout << "--------------------------\n";




    std::cout << "Esta ultima parte es mas que nada para comprobar\nque se estaba haciendo bien el procedimiento\n";

    imprimirMBRsDeNodos(nodos);

    std::cout << "--------------------------\n";

    return 0;
}

