#include "auxFuncMain.h"

/*
 * Experimento para medir el rendimiento del m�todo 1 en b�squedas en R-trees.
 *
 * Descripci�n:
 * - Este c�digo genera una serie de consultas y las ejecuta en R-trees preconstruidos.
 * - Se mide el n�mero promedio de accesos a disco y el tiempo promedio de consulta para cada R-tree.
 * - Los resultados son guardados en un archivo CSV para an�lisis posterior.
 *
 * Precondiciones:
 * - Los binarios que contienen a los rect�ngulos ORDENADOS deben estar en el mismo directorio que este c�digo.
 * - Los archivos con los R-trees preconstruidos tambi�n deben estar en el mismo directorio.
 * - Dado que el conjunto completo de datos es grande, se proporciona por separado.
 * - Para un funcionamiento �ptimo, recomendamos ejecutar este c�digo en la IDE Visual Studio.
 *   Proporcionamos una captura de pantalla que muestra la estructura de directorio recomendada.
 * 
 *  Observacion: el codigo ira printeando cosas mientras se ejecuta. Al final se pueden recuperar los 
 * resultados desde el CSV llamado "resultados_metodo1.csv" .
 *
 * @return 0 si el experimento se complet� con �xito.
 */


int main() {

    std::vector<unsigned long long> numeros;

    for (int i = 10; i <= 25; ++i) {
        numeros.push_back(pow(2, i));
    }



    for (unsigned long long num : numeros) {
        std::cout << "\n";
        std::cout << num << std::endl;

        // Crear el nombre del archivo basado en el n�mero actual
        std::string filename = "rectangles" + std::to_string(num) + ".bin";

        // Escribir los rect�ngulos para cada n�mero
        escribirRectangulos(num, filename);
        std::cout << "Se escribieron " << num << " rect�ngulos en " << filename << "\n"; // 1ra l�nea

        // Ordenar los rect�ngulos y comprobar si est�n ordenados
        std::string orderedFilename = "ordered-rectangles" + std::to_string(num) + ".bin";
        ordenarRectangulosPorCentro(filename, orderedFilename);
        if (estanOrdenados(orderedFilename)) {
            std::cout << "Los rect�ngulos est�n correctamente ordenados.\n"; // 2da l�nea
            std::cout << "Se pusieron ordenados en " << orderedFilename << "\n"; // 3ra l�nea
        }
        else {
            std::cerr << "Los rect�ngulos no est�n ordenados.\n";
        }
    }

    std::cout << "Ahora se muestran el numero de nodos hoja de cada arbol\n";



    std::vector<std::vector<Rectangulo>*> vectoresDeRectangulos = cargarVectoresDesdeArchivos(numeros);


    std::vector<Nodo*> rtrees = construirRTreesDesdeVectores(vectoresDeRectangulos);

    guardarTodosLosRTrees(rtrees);

    // Lista de los nombres de los archivos R-tree
    std::vector<std::string> nombresArchivos;
    for (int i = 0; i <= 15; ++i) {
        nombresArchivos.push_back("RTree_" + std::to_string(i) + ".txt");
    }

    realizarConsultasEnRTrees(nombresArchivos);


}

