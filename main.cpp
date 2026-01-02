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
    cout << "\033[2J\033[H" << flush;

    

    return 0;
}