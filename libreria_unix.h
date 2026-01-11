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
Node* searchNode(Node* directorio, const string& nombre) {
    if (!directorio || directorio->esArchivo) return NULL;
    
    Node* hijo = directorio->hijoIzq;
    while (hijo) {
        if (hijo->nombre == nombre) {
            return hijo;
        }
        hijo = hijo->hermanoDer;
    }
    return NULL;
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
    if (!p) return false;
    
    //si estamos creando la raiz
    if (*p == NULL) {
        Node* nuevo = new Node;
        nuevo->nombre = name;
        nuevo->esArchivo = esArchivo;
        nuevo->contenido = "";
        nuevo->hijoIzq = NULL;
        nuevo->hermanoDer = NULL;
        nuevo->padre = NULL;
        *p = nuevo;
        return true;
    }
    //si estamos agregando a un directorio existente
    else {
        //verificar si ya existe un hijo con ese nombre
        if (existingNode(*p, name)) return false;
        
        //verificar que el padre no sea un archivo
        if ((*p)->esArchivo) return false;
        
        Node* nuevo = new Node;
        nuevo->nombre = name;
        nuevo->esArchivo = esArchivo;
        nuevo->contenido = "";
        nuevo->hijoIzq = NULL;
        nuevo->hermanoDer = NULL;
        nuevo->padre = *p;  //el directorio actual es el padre
        
        //agregar al inicio de la lista de hijos
        nuevo->hermanoDer = (*p)->hijoIzq;
        (*p)->hijoIzq = nuevo;
        
        return true;
    }
}

//manejo del archivo .txt
void cargarDesdeArchivo(Node** raiz, const string& nombreArchivo = "file_explorer.txt") {
    ifstream arch(nombreArchivo);
    if (!arch) {
        cerr << "ERROR: No se pudo abrir '" << nombreArchivo << "'" << endl;
        return;
    }
    
    string linea;
    while (getline(arch, linea)) {
        if (linea.empty()) continue;
        //manejar contenido después de "//"
        string ruta, contenido;
        size_t sep = linea.find("//");
        if (sep != string::npos) {
            ruta = linea.substr(0, sep);
            contenido = linea.substr(sep + 2);
        } else {
            ruta = linea;
            contenido = "";
        }
        
        //procesar la ruta
        Node* actual = *raiz;
        size_t inicio = 0;
        size_t fin;
        
        //procesar cada parte de la ruta
        while ((fin = ruta.find('/', inicio)) != string::npos) {
            string dir = ruta.substr(inicio, fin - inicio);
            if (!dir.empty()) {
                //buscar si ya existe este directorio
                Node* hijo = actual->hijoIzq;
                Node* dirExistente = NULL;
                while (hijo) {
                    if (hijo->nombre == dir && !hijo->esArchivo) {
                        dirExistente = hijo;
                        break;
                    }
                    hijo = hijo->hermanoDer;
                }
                
                if (!dirExistente) {
                    //crear nuevo directorio
                    Node* nuevoDir = new Node;
                    nuevoDir->nombre = dir;
                    nuevoDir->esArchivo = false;
                    nuevoDir->contenido = "";
                    nuevoDir->hijoIzq = NULL;
                    nuevoDir->hermanoDer = actual->hijoIzq;
                    nuevoDir->padre = actual;
                    actual->hijoIzq = nuevoDir;
                    dirExistente = nuevoDir;
                }
                
                actual = dirExistente;
            }
            inicio = fin + 1;
        }
        
        //ultima parte (puede ser archivo o directorio)
        string nombreFinal = ruta.substr(inicio);
        if (!nombreFinal.empty()) {
            //verificar si ya existe
            Node* hijo = actual->hijoIzq;
            bool existe = false;
            while (hijo) {
                if (hijo->nombre == nombreFinal) {
                    existe = true;
                    break;
                }
                hijo = hijo->hermanoDer;
            }
            
            if (!existe) {
                Node* nuevo = new Node;
                nuevo->nombre = nombreFinal;
                nuevo->esArchivo = verifArchivo(nombreFinal);
                nuevo->contenido = (nuevo->esArchivo) ? contenido : "";
                nuevo->hijoIzq = NULL;
                nuevo->hermanoDer = actual->hijoIzq;
                nuevo->padre = actual;
                actual->hijoIzq = nuevo;
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
void mv(Node* actual, const string& origen, const string& destino) {
    if (!actual) {
        cout << "--- ERROR: Directorio actual no válido. ---" << endl;
        return;
    }
    //buscar el origen del directorio
    Node* nodoOrigen = NULL;
    Node* prevOrigen = NULL;
    Node* curr = actual->hijoIzq;
    
    while (curr) {
        if (curr->nombre == origen) {
            nodoOrigen = curr;
            break;
        }
        prevOrigen = curr;
        curr = curr->hermanoDer;
    }
    
    if (!nodoOrigen) {
        cout << "--- ERROR: '" << origen << "' no encontrado en el directorio actual. ---" << endl;
        return;
    }
    
    //buscar el directorio destino
    Node* destinoDir = NULL;
    
    //verficar si es el directorio padre
    if (destino == "..") {
        if (actual->padre) {
            destinoDir = actual->padre;
        } else {
            cout << "--- ERROR: No hay directorio padre. ---" << endl;
            return;
        }
    }
    //si es el mismo directorio lo renombramos
    else if (destino == ".") {
        cout << "--- ERROR: Use 'mv <viejo> <nuevo>' para renombrar. ---" << endl;
        return;
    }
    //verificamos si el destino existe como directorio
    else {
        //buscamos en los hijos del directorio actual
        Node* temp = actual->hijoIzq;
        while (temp) {
            if (temp->nombre == destino && !temp->esArchivo) {
                destinoDir = temp;
                break;
            }
            temp = temp->hermanoDer;
        }
        
        //si no lo encontramos, verificamos si es una ruta absoluta o relativa
        if (!destinoDir && destino.find('/') != string::npos) {
            Node* tempRaiz = actual;
            while (tempRaiz->padre) tempRaiz = tempRaiz->padre;
            destinoDir = cd(actual, tempRaiz, destino);
            if (destinoDir && destinoDir->esArchivo) {
                cout << "--- ERROR: El destino no puede ser un archivo. ---" << endl;
                return;
            }
        }
    }
    
    //si se encontró el directorio destino, movemos el nodo
    if (destinoDir && !destinoDir->esArchivo) {
        //verificar si ya existe un nodo con el mismo nombre en el destino
        Node* temp = destinoDir->hijoIzq;
        while (temp) {
            if (temp->nombre == nodoOrigen->nombre) {
                cout << "--- ERROR: Ya existe '" << nodoOrigen->nombre << "' en el destino. ---" << endl;
                return;
            }
            temp = temp->hermanoDer;
        }
        
        //desenlazar nodoOrigen de su ubicación actual
        if (prevOrigen) {
            prevOrigen->hermanoDer = nodoOrigen->hermanoDer;
        } else {
            actual->hijoIzq = nodoOrigen->hermanoDer;
        }
        
        //enlazar nodoOrigen en el destino
        nodoOrigen->hermanoDer = destinoDir->hijoIzq;
        destinoDir->hijoIzq = nodoOrigen;
        
        //actualizar padre
        nodoOrigen->padre = destinoDir;
        
        cout << "--- Movido: '" << origen << "' a '" << destinoDir->nombre << "' ---" << endl;
    }
    //si no se encontró directorio destino, asumimos renombrar
    else {
        //verificar que el nuevo nombre no exista
        if (existingNode(actual, destino)) {
            cout << "--- ERROR: Ya existe '" << destino << "' en el directorio actual. ---" << endl;
            return;
        }
        
        //renombrar
        string nombreViejo = nodoOrigen->nombre;
        nodoOrigen->nombre = destino;
        cout << "--- Renombrado: '" << nombreViejo << "' -> '" << destino << "' ---" << endl;
    }
}

//mostrar el contenido de un archivo txt
void cat(Node* archivo) {
    if (!archivo) {
        cerr << "--- ERROR: Archivo no encontrado. ---" << endl;
        return;
    }
    if (!archivo->esArchivo) {
        cerr << "--- ERROR: '" << archivo->nombre << "' no es un archivo. ---" << endl;
        return;
    }
    cout << archivo->contenido << endl; 
}

//editar el contenido de un archivo txt
void edit(Node* archivo) {
    if (!archivo) {
        cerr << "--- ERROR: Archivo no encontrado. ---" << endl;
        return;
    }
    if (!archivo->esArchivo) {
        cerr << "--- ERROR: '" << archivo->nombre << "' no es un archivo. ---" << endl;
        return;
    }

    cout << "Editando " << archivo->nombre << "...\nEscribe ':end' en una línea nueva para guardar:" << endl;
    cout << "--- Contenido actual: ---" << endl;
    cat(archivo);
    
    cout << "--- Escribe el nuevo contenido (línea por línea): ---" << endl;

    string nuevoContenido = "";
    string linea;
    cin.ignore();
    
    while (true) {
        cout << "> ";
        getline(cin, linea);
        if (linea == ":end") break;
        if (!nuevoContenido.empty()) nuevoContenido += "\n";
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
    cout << "ls <directorio>a: Lista archivos/directorios." << endl;
    cout << "mkdir <nombre>: Crea una carpeta." << endl;
    cout << "touch <archivo>: Crea un archivo .txt." << endl;
    cout << "mv <original> <destino>: Renombra o mueve archivos." << endl;
    cout << "cat <archivo>: Permite visualizar el contenido de un archivo .txt." << endl;
    cout << "edit <archivo>: Permite editar el contenido de un archivo .txt." << endl;
    cout << "exit: Cierra el programa." << endl;
    cout << endl; 
}