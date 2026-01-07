#include <iostream>
#include "libreria_unix.h"
#include <fstream>
#include <string>
using namespace std;

int main(){
    cout << "\n\n\t\t--- PROYECTO 2: SIMULADOR DE TERMINAL UNIX ---" << endl;
    cout << "\tElaborado por Anabella Freites :-)" << endl;
    cout << "\tPresione ENTER para ingresar a la terminal..." << endl;
    cin.get();

    Node *terminal = NULL;
    createNode(&terminal, "/", false);
    cargarDesdeArchivo(&terminal); //cargar la estructura de directorios desde el .txt

    if (!terminal) {
        cout << "ERROR. no se pudo cargar la estructura :-(.\n";
        return 1; //termina el programa si no se pudo abrir el archivo
    }

    Node* actual = terminal;
    Node* raiz = terminal;

    string comando;
    while (true) {
        cout << obtenerRuta(actual) << "$ ";
        getline(cin, comando);

        if (comando.empty()) continue;
        else if (comando == "help")
        help();
        else if (comando == "exit") {
            guardarEstructura(raiz);    
            cout << "--- Gracias por usar el programa :-) ---" << endl;
            break;                                      
        }
        //cd
        else if (comando.size() > 3 && comando.substr(0, 3) == "cd ") { 
            string dir = comando.substr(3); 
            Node* nuevoDir = cd(actual, raiz, dir);
            if (nuevoDir) {
                actual = nuevoDir;
            } else {
                cout << "--- ERROR: Directorio " << dir << " no encontrado. ---" << endl;
            }
        }
        //ls
        else if (comando.substr(0, 2) == "ls") {
            if (comando.size() == 2) {
                ls(actual);
            }
            else if (comando[2] == ' ') {
                string dir = comando.substr(3);
                Node* target = cd(actual, raiz, dir);
                if (target) {
                    ls(target);
                } else {
                    cout << "--- ERROR: Directorio " << dir << " no encontrado. ---" << endl;
                }
            }
            else {
                cout << "--- ERROR: Comando no reconocido. Use: ls <directorio> ---" << endl;
            }
        }
        //mkdir
        else if (comando.substr(0, 6) == "mkdir ") {
            string nombre = comando.substr(6);
            mkdir(actual, nombre);
        }
        //touch
        else if (comando.substr(0, 6) == "touch ") {
            string nombre = comando.substr(6); 
            touch(actual, nombre);
        }
        //mv
        else if (comando.substr(0, 3) == "mv ") {
            string argumentos = comando.substr(3);
            size_t espacio = argumentos.find(' ');
            if (espacio == string::npos) {
                cout << "--- ERROR: Uso: mv <origen> <destino> ---" << endl;
                continue;
            }
            
            string origen = argumentos.substr(0, espacio);
            string destino = argumentos.substr(espacio + 1);
           
            while (!destino.empty() && destino[0] == ' ') {
                destino = destino.substr(1);
            }
            if (origen.empty() || destino.empty()) {
                cout << "--- ERROR: Uso: mv <origen> <destino> ---" << endl;
                continue;
            }
            mv(actual, origen, destino);
        }
        //cat
        else if (comando.substr(0, 4) == "cat ") {
            string nombreArchivo = comando.substr(4);
            Node* archivo = searchNode(actual, nombreArchivo);
            cat(archivo);
        }
        //edit
        else if (comando.substr(0, 5) == "edit ") {
            string nombreArchivo = comando.substr(5);
            Node* archivo = searchNode(actual, nombreArchivo);
            edit(archivo);
        }
        else {
            cout << "---ERROR: Comando no reconocido. Ingrese el comando 'help' para obtener la lista de comandos. --- \n";
        }
    }

    return 0;
}