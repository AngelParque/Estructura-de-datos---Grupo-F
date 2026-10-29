#include <iostream>
using namespace std;

int main() {
    int opcion;

    do {
        cout << "\n===== SISTEMA DE GESTION DE PROCESOS =====\n";
        cout << "1. Gestionar procesos\n";
        cout << "2. Planificar procesos en CPU\n";
        cout << "3. Gestionar memoria\n";
        cout << "4. Guardar estado del sistema\n";
        cout << "5. Cargar estado del sistema\n";
        cout << "0. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                cout << "Seleccionaste: Gestionar procesos\n";
                break;
            case 2:
                cout << "Seleccionaste: Planificar procesos en CPU\n";
                break;
            case 3:
                cout << "Seleccionaste: Gestionar memoria\n";
                break;
            case 4:
                cout << "Seleccionaste: Guardar estado del sistema\n";
                break;
            case 5:
                cout << "Seleccionaste: Cargar estado del sistema\n";
                break;
            case 0:
                cout << "Cerrando el sistema...\n";
                break;
            default:
                cout << "Opcion no valida. Intente nuevamente.\n";
        }
    } while (opcion != 0);

    return 0;
}
