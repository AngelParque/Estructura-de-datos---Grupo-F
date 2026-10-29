#include <iostream>
using namespace std;

void crearProceso(){
	
};
void eliminarProceso(){
	
};
void buscarProcesoID(){
	
};
void buscarProcesoNombre(){
	
};
void modificarPrioridad(){
	
};
void mostrarProcesos(){
	
};

void siguienteProceso(){
	
};
void terminarProcesoActual(){
	
};
void mostrarColaPrioridad(){
	
};
void mostrarEstadoMemoria(){
	
};
void liberarMemoria(){
	
};
void estadoSistema(){
	
};
void demostracionAutomatica(){
	
};


int main() {
    int opcion;
    char subopcion;

    do {
        cout << "\n===== SISTEMA DE GESTION DE PROCESOS =====\n";
        cout << "1. GESTOR DE PROCESOS\n";
        cout << "   a. Crear nuevo proceso\n";
        cout << "   b. Eliminar proceso\n";
        cout << "   c. Buscar proceso por ID\n";
        cout << "   d. Buscar proceso por nombre\n";
        cout << "   e. Modificar prioridad de proceso\n";
        cout << "   f. Mostrar todos los procesos\n";
        cout << "2. PLANIFICADOR DE CPU\n";
        cout << "   a. Ejecutar siguiente proceso\n";
        cout << "   b. Terminar proceso actual\n";
        cout << "   c. Mostrar cola de prioridad\n";
        cout << "3. GESTOR DE MEMORIA\n";
        cout << "   a. Mostrar estado de memoria\n";
        cout << "   b. Liberar memoria de proceso\n";
        cout << "4. Estado general del sistema\n";
        cout << "5. Demostracion automatica\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                cout << "Opcion del gestor de procesos (a-f): ";
                cin >> subopcion;

                switch (subopcion) {
                    case 'a': cout << "Crear nuevo proceso\n"; break;
                    case 'b': cout << "Eliminar proceso\n"; break;
                    case 'c': cout << "Buscar proceso por ID\n"; break;
                    case 'd': cout << "Buscar proceso por nombre\n"; break;
                    case 'e': cout << "Modificar prioridad de proceso\n"; break;
                    case 'f': cout << "Mostrar todos los procesos\n"; break;
                    default: cout << "Opcion no valida\n";
                }
                break;

            case 2:
                cout << "Opcion del planificador de CPU (a-c): ";
                cin >> subopcion;

                switch (subopcion) {
                    case 'a': cout << "Ejecutar siguiente proceso\n"; break;
                    case 'b': cout << "Terminar proceso actual\n"; break;
                    case 'c': cout << "Mostrar cola de prioridad\n"; break;
                    default: cout << "Opcion no valida\n";
                }
                break;

            case 3:
                cout << "Opcion del gestor de memoria (a-b): ";
                cin >> subopcion;

                switch (subopcion) {
                    case 'a': cout << "Mostrar estado de memoria\n"; break;
                    case 'b': cout << "Liberar memoria de proceso\n"; break;
                    default: cout << "Opcion no valida\n";
                }
                break;

            case 4:
                cout << "Estado general del sistema\n";
                break;

            case 5:
                cout << "Demostracion automatica\n";
                break;

            case 0:
                cout << "Saliendo del sistema...\n";
                break;

            default:
                cout << "Opcion no valida\n";
        }

    } while (opcion != 0);

    return 0;
}
