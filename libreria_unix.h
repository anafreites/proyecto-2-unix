#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Node{
    string nombre;
    bool esArchivo;
    string contenido;
    Node *hijoIzq;
    Node *hermanoDer;
    Node *padre; //para hacer mas facil la navegacion
};

//funciones para manejo de nodos
//comprobar si ya existe un nodo con ese nombre en el directorio actual
bool existingNode(Node *p, string name) {
    if (!p) return false;
    Node *aux = p->hijoIzq;
    while(aux){
        if (aux->nombre == name) return true;
        aux = aux->hermanoDer;
    }
    return false;
}

//buscar un nodo por su nombre
Node* searchNode(Node *p, string nombre) {
    if (!p) return NULL;
    if (p->nombre == nombre) return p;
    
    Node *encontrado = searchNode(p->hijoIzq, nombre);
    if (encontrado) return encontrado;
    return searchNode(p->hermanoDer, nombre);
}

//verificar si es un archivo (tiene extension .txt)
bool verifArchivo(string nombre){
    string extension = ".txt";
    if (extension.length() > nombre.length()) return false;
    
    for (int i = 0; i < extension.length(); i++) {
        if (nombre[nombre.length() - extension.length() + i] != extension[i]) return false;
    }
    return true;
}

//crear un nuevo nodo (archivo o carpeta)
bool createNode(Node **p, string name, bool esArchivo, Node *padre = NULL){
    Node* nuevo = new Node;
    if (!nuevo) return false;
    nuevo->nombre = name;
    nuevo->esArchivo = esArchivo;
    nuevo->contenido = "";
    nuevo->hijoIzq = NULL;
    nuevo->hermanoDer = NULL;
    nuevo->padre = padre;

    if (*p == NULL) *p = nuevo;
    else {
        if (existingNode(*p, name) || verifArchivo((*p)->nombre)) return false;
        else {
            Node *aux = *p;
            if (aux->hijoIzq == NULL) aux->hijoIzq = nuevo;
            else {
                Node *ultHermano = aux->hijoIzq;
                while(ultHermano->hermanoDer != NULL){
                    ultHermano = ultHermano->hermanoDer;
                }
                ultHermano->hermanoDer = nuevo;
            }
        }
    }
    return true;
}

//cmanejo del archivo .txt
void archivo(Node **p) {
    ifstream arch("file_explorer.txt");
    string linea;

    if (!arch) {
        cout << "ERROR: El archivo no existe o no se pudo abrir correctamente." << endl;
        return;
    }

    while (getline(arch, linea)) {
        Node *aux = *p;
        int inicio = 0;
        int fin;

        int separadorContenido = linea.find("//");
        bool tieneContenido = false;
        string lineaSinContenido;
        
        if (separadorContenido != string::npos) {
            tieneContenido = true;
            lineaSinContenido = linea.substr(0, separadorContenido);
        } else {
            lineaSinContenido = linea;
        }

        while ((fin = lineaSinContenido.find('/', inicio)) != string::npos) {
            string parte = lineaSinContenido.substr(inicio, fin - inicio);
            Node *nodoExistente = NULL;
            if (aux) {
                nodoExistente = searchNode(aux, parte);
            }
            if (!nodoExistente) {
                if (aux) {
                    createNode(&aux, parte, false, aux);
                    nodoExistente = searchNode(aux, parte);
                } else {
                    createNode(p, parte, false);
                    nodoExistente = searchNode(*p, parte);
                }
            }
            aux = nodoExistente;
            inicio = fin + 1;
        }

        string ultparte = lineaSinContenido.substr(inicio);
        if (!ultparte.empty()) { 
            bool esArchivo = verifArchivo(ultparte);
            if (!aux) {
                createNode(p, ultparte, esArchivo);
                if (esArchivo && tieneContenido) {
                    Node* nuevoArchivo = searchNode(*p, ultparte);
                    if (nuevoArchivo) nuevoArchivo->contenido = linea.substr(separadorContenido + 2);
                }
            } else if (aux->esArchivo) {
                cout << "\tERROR: No se pueden crear archivos dentro de otros archivos." << endl;
            } else {
                createNode(&aux, ultparte, esArchivo, aux);
                if (esArchivo && tieneContenido) {
                    Node* nuevoArchivo = searchNode(aux, ultparte);
                    if (nuevoArchivo) nuevoArchivo->contenido = linea.substr(separadorContenido + 2);
                }
            }
        }
    }
    arch.close();
}

//funciones de la terminal
string obtenerRuta(Node* actual) {
    string rutaObtenida;
    if (!actual) return "/";

    string ruta;
    Node* nodo = actual;

    string partes[100];
    int count = 0;

    while (nodo != NULL && count < 100) {
        partes[count++] = nodo->nombre;
        nodo = nodo->padre;
    }

    for (int i = count - 1; i >= 0; --i) {
        if (partes[i] != "/") {
            ruta += "/" + partes[i];
        }
    }

    if (ruta.empty()) {
        rutaObtenida = "/"; //ruta raiz
    } else {
        rutaObtenida = ruta;
    }
    return rutaObtenida;
}

Node* buscarDirectorio(Node* actual, const string& nombre) {
    if (!actual) return NULL;
    
    Node* hijo = actual->hijoIzq;
    while (hijo != NULL) {
        if (hijo->nombre == nombre && !hijo->esArchivo) {
            return hijo;
        }
        hijo = hijo->hermanoDer;
    }
    return NULL;
}

//comandos unix
//ver directoios y archivos en el directorio actual
void ls(Node* directorio) {
    if (!directorio || directorio->esArchivo) {
        cerr << "--- ERROR: no se puede acceder al directorio ----" << endl;
        return;
    }

    Node* hijo = directorio->hijoIzq;
    if (!hijo) return;

    while (hijo) {
        if (!hijo->nombre.empty()) {
            if (hijo->esArchivo) {
                cout << hijo->nombre << "   ";
            } else {
                cout << hijo->nombre << "   ";
            }
        }
        hijo = hijo->hermanoDer;
    }
    cout << endl;
}

//cambiar el directorio
Node* cd(Node* actual, Node* raiz, const string& ruta) {
    if (ruta.empty()) return actual; //si la ruta esta vacia
    
    //ir al directorio padre del actual
    if (ruta == "..") {
        if (actual->padre != nullptr) {
            return actual->padre;
        }
        return actual;
    }
    
    //ir a la raiz
    if (ruta == "/") {
        return raiz;
    }
    Node* destino = nullptr;
    
    //ruta absoluta
    if (ruta[0] == '/') {
        destino = raiz;
        string rutaAbsoluta = ruta.substr(1);
        
        if (rutaAbsoluta.empty()) {
            return raiz;
        }
        size_t inicio = 0;
        size_t fin = rutaAbsoluta.find('/');
        while (fin != string::npos) {
            string dir = rutaAbsoluta.substr(inicio, fin - inicio);
            if (!dir.empty()) {
                if (dir == "..") {
                    if (destino->padre != nullptr) {
                        destino = destino->padre;
                    }
                } else {
                    destino = buscarDirectorio(destino, dir);
                    if (!destino) {
                        cerr << "--- ERROR: Directorio '" << dir << "' no encontrado. ---" << endl;
                        return actual;
                    }
                }
            }
            inicio = fin + 1;
            fin = rutaAbsoluta.find('/', inicio);
        }
        string ultimoDir = rutaAbsoluta.substr(inicio);
        if (!ultimoDir.empty()) {
            if (ultimoDir == "..") {
                if (destino->padre != nullptr) {
                    destino = destino->padre;
                }
            } else {
                destino = buscarDirectorio(destino, ultimoDir);
                if (!destino) {
                    cerr << "--- ERROR: Directorio '" << ultimoDir << "' no encontrado. ---" << endl;
                    return actual;
                }
            }
        }
        return destino;
    }
    
    //ruta relativa
    size_t separador = ruta.find('/');
    if (separador == string::npos) {
        Node* hijo = buscarDirectorio(actual, ruta);
        if (hijo) {
            return hijo;
        }
        cerr << "--- ERROR: Directorio '" << ruta << "' no encontrado. ---" << endl;
        return actual;
    }
    
    //si la ruta es relativa y tiene varios niveles
    destino = actual;
    size_t inicio = 0;
    size_t fin = ruta.find('/');
    
    while (fin != string::npos) {
        string dir = ruta.substr(inicio, fin - inicio);
        if (!dir.empty()) {
            if (dir == "..") {
                if (destino->padre != nullptr) {
                    destino = destino->padre;
                }
            } else {
                destino = buscarDirectorio(destino, dir);
                if (!destino) {
                    cerr << "--- ERROR: Directorio '" << dir << "' no encontrado. ---" << endl;
                    return actual;
                }
            }
        }
        inicio = fin + 1;
        fin = ruta.find('/', inicio);
    }
    
    string ultimoDir = ruta.substr(inicio);
    if (!ultimoDir.empty()) {
        if (ultimoDir == "..") {
            if (destino->padre != nullptr) {
                destino = destino->padre;
            }
        } else {
            destino = buscarDirectorio(destino, ultimoDir);
            if (!destino) {
                cerr << "--- ERROR: Directorio '" << ultimoDir << "' no encontrado. ---" << endl;
                return actual;
            }
        }
    }
    return destino;
}
//crear un nuevo directorio
void mkdir(Node* actual, const string& nombre) {
    Node* temp = actual->hijoIzq;
    while (temp) {
        if (temp->nombre == nombre && !temp->esArchivo) {
            cout << "--- ERROR: La carpeta ya existe. ---" << endl;
            return;
        }
        temp = temp->hermanoDer;
    }

    Node* nueva = new Node{nombre, false, "", NULL, NULL, actual};
    nueva->hermanoDer = actual->hijoIzq;
    actual->hijoIzq = nueva;

    cout << "--- Carpeta '" << nombre << "' creada. ---" << endl;
}

//crear un archivo txt
void touch(Node* actual, const string& nombre) {
    Node* temp = actual->hijoIzq;
    while (temp) {
        if (temp->nombre == nombre && temp->esArchivo) {
            cout << "--- ERROR: El archivo ya existe. ---" << endl;
            return;
        }
        temp = temp->hermanoDer;
    }

    Node* nuevo = new Node{nombre, true, "", NULL, NULL, actual};
    nuevo->hermanoDer = actual->hijoIzq;
    actual->hijoIzq = nuevo;

    cout << "--- Archivo '" << nombre << "' creado. ---" << endl;
}

//mover un archivo o carpeta a otro directorio
void mv(Node* actual, const string& nombreActual, const string& nombreNuevo) {
    if (!actual) {
        cout << "--- ERROR: Directorio actual no válido. ---" << endl;
        return;
    }

    Node* nodoARenombrar = NULL;
    Node* prev = NULL;
    Node* curr = actual->hijoIzq;

    while (curr) {
        if (curr->nombre == nombreActual) {
            nodoARenombrar = curr;
            break;
        }
        prev = curr;
        curr = curr->hermanoDer;
    }
    if (!nodoARenombrar) {
        cout << "--- ERROR: '" << nombreActual << "' no existe en el directorio actual. ---" << endl;
        return;
    }
    if (existingNode(actual, nombreNuevo)) {
        cout << "--- ERROR: '" << nombreNuevo << "' ya existe en el directorio actual. ---" << endl;
        return;
    }

    nodoARenombrar->nombre = nombreNuevo;
    cout << "-- Renombrado: '" << nombreActual << "' -> '" << nombreNuevo << "' ---" << endl;
}

//mostrar el contenido de un archivo txt
void cat(Node* archivo) {
    if (!archivo || !archivo->esArchivo) {
        cerr << "--- ERROR: " << archivo->nombre << " No es un archivo válido. ---" << endl;
        return;
    }
    cout << archivo->contenido << endl; 
}

//editar el contenido de un archivo txt
void edit(Node* archivo) {
    if (!archivo || !archivo->esArchivo) {
        cerr << "--- ERROR: " << archivo->nombre << " No es un archivo válido. ---" << endl;
        return;
    }

    cout << "Editando " << archivo->nombre << "...\n Escribe ':end' en una línea nueva para guardar:" << endl;
    cout << "--- Contenido actual: ---" << endl;
    cat(archivo);

    string nuevoContenido;
    string linea;
    while (true) {
        cout << "> ";
        getline(cin, linea);
        if (linea == ":end") break;
        nuevoContenido += linea;
    }

    archivo->contenido = nuevoContenido;
    cout << "--- Cambios guardados en memoria. ---" << endl;
}

//guardar la estructura de directorios en el archivo .txt
void guardar(Node* nodo, ofstream& archivo, string rutaActual) {
    if (!nodo || !archivo.is_open()) return; 

    if (nodo->nombre != "/") {
        archivo << rutaActual << nodo->nombre;
        if (nodo->esArchivo && !nodo->contenido.empty()) {
            archivo << "//" << nodo->contenido;
        }
        archivo << endl;
    }
    string nuevaRuta = rutaActual;
    if (!nodo->esArchivo && nodo->nombre != "/") {
        nuevaRuta += nodo->nombre + "/";  
    }
    guardar(nodo->hijoIzq, archivo, nuevaRuta);
    guardar(nodo->hermanoDer, archivo, rutaActual);
}

//guardar la estructura completa
void guardarEstructura(Node* raiz, const string& nombreArchivo = "file_explorer.txt") {
    ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {  //
        cerr << "--- Error al abrir el archivo para guardar. ---" << endl;
        return;
    }
    guardar(raiz, archivo, "");
    archivo.close();
    cout << "--- Estructura guardada en '" << nombreArchivo << "'. ---" << endl;
}

//comand de ayuda
void help(){
    cout << "--- Comandos disponibles: ---" << endl;
    cout << "cd <directorio>: Navega a otro directorio." << endl;
    cout << "ls [directorio]: Lista archivos/directorios." << endl;
    cout << "mkdir <nombre>: Crea una carpeta." << endl;
    cout << "rm <elemento>: Elimina un archivo o carpeta .txt." << endl;
    cout << "touch <archivo>: Crea un archivo .txt." << endl;
    cout << "mv <original> <destino>: Renombra o mueve archivos." << endl;
    cout << "cat <archivo>: Permite visualizar el contenido de un archivo .txt." << endl;
    cout << "edit <archivo>: Permite editar el contenido de un archivo .txt." << endl;
    cout << "exit: Cierra el programa." << endl;
    cout << endl; 
}