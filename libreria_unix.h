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