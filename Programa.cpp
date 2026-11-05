#include <iostream>  // Permite mostrar y leer datos en la consola.
#include <string>    // Permite almacenar nombres de procesos como texto.
#include <vector>    // Se usa para representar la cola y la pila de memoria.
#include <algorithm> // Aporta find y sort para buscar y ordenar elementos.
#include <cstdlib>   // Aporta rand para elegir el ID inicial de la demostracion.
#include <cstring>   // Aporta strcmp para comparar nombres de procesos.

using namespace std;

// Limite total de memoria simulada; no representa la RAM real de la computadora.
const int MEMORIA_MAXIMA = 1024; // KB (1 MB).

// Cada objeto Proceso guarda los datos de un proceso y un enlace al siguiente.
// El campo siguiente permite conectar varios objetos y formar una lista enlazada.
struct Proceso {
    int id;                 // Numero unico que identifica al proceso.
    string nombre;          // Nombre descriptivo del proceso.
    int prioridad;          // Un numero mayor representa mayor prioridad.
    int estado;             // 0: listo, 1: en espera, 2: ejecutando, 3: finalizado.
    int memoria;            // Cantidad total de memoria asignada, en KB.
    Proceso* siguiente;     // Direccion del siguiente nodo de la lista.
};

// Representa un bloque de memoria que pertenece a un proceso.
// El vector de bloques se usa como pila: el ultimo elemento es el tope.
struct Bloque {
    int idProceso;  // ID del proceso propietario del bloque.
    int cantidad;   // Tamano del bloque de memoria, en KB.
};

// Puntero al primer nodo de la lista enlazada de procesos.
Proceso* listaProcesos = NULL;
// Guarda los ID de los procesos en espera, ordenados por prioridad.
vector<int> colaPrioridad;
// Guarda los bloques de memoria; el ultimo agregado queda en el tope.
vector<Bloque> pilaMemoria;
// Apunta al proceso que esta usando la CPU; NULL significa CPU libre.
Proceso* procesoActual = NULL;

// Convierte el codigo numerico del estado en una palabra entendible.
string obtenerEstado(int estado) {
    if (estado == 0) return "Listo";
    if (estado == 1) return "En espera";
    if (estado == 2) return "En ejecucion";
    return "Finalizado"; // Si no es 0, 1 ni 2, se considera finalizado.
}

// Lee un numero entero y repite la lectura mientras el usuario escriba algo invalido.
int leerNumero(string mensaje) {
    int numero;
    cout << mensaje;
    while (!(cin >> numero)) { // La extraccion falla cuando la entrada no es un entero.
        cout << "Ingrese un numero valido: ";
        cin.clear();             // Limpia el estado de error de cin.
        cin.ignore(10000, '\n'); // Descarta los caracteres incorrectos de la linea.
    }
    cin.ignore(10000, '\n'); // Descarta el Enter para la siguiente lectura.
    return numero;
}

// Recorre la lista enlazada y devuelve el proceso con el ID indicado.
// Si no existe, devuelve NULL.
Proceso* buscarProceso(int id) {
    Proceso* actual = listaProcesos;
    while (actual != NULL) {
        if (actual->id == id) return actual; // Se encontro el ID buscado.
        actual = actual->siguiente;          // Avanza al siguiente nodo.
    }
    return NULL;
}

// Crea un nodo nuevo y lo agrega al final de la lista de procesos.
void agregarProceso(int id, string nombre, int prioridad) {
    // Reserva memoria para un nuevo nodo.
    Proceso* nuevo = new Proceso;
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->prioridad = prioridad;
    nuevo->estado = 0;       // Todo proceso nuevo comienza listo.
    nuevo->memoria = 0;      // Al crearlo aun no tiene memoria asignada.
    nuevo->siguiente = NULL; // Por ahora no hay otro nodo despues de este.

    if (listaProcesos == NULL) {
        listaProcesos = nuevo; // Si la lista esta vacia, el nuevo nodo es el primero.
    } else {
        Proceso* actual = listaProcesos;
        while (actual->siguiente != NULL) actual = actual->siguiente; // Busca el final.
        actual->siguiente = nuevo; // Enlaza el ultimo nodo con el nuevo.
    }
}

// Funcion auxiliar para ordenar los ID de la cola de mayor a menor prioridad.
bool compararPrioridad(int idA, int idB) {
    Proceso* procesoA = buscarProceso(idA);
    Proceso* procesoB = buscarProceso(idB);
    // Este caso evita acceder a un puntero nulo si algun ID no corresponde a un proceso.
    if (procesoA == NULL || procesoB == NULL) return idA < idB;
    return procesoA->prioridad > procesoB->prioridad;
}

// Agrega el ID a la cola solo si aun no estaba y vuelve a ordenar la cola.
void agregarACola(int id) {
    // find busca el ID desde el inicio hasta el final del vector.
    if (find(colaPrioridad.begin(), colaPrioridad.end(), id) == colaPrioridad.end()) {
        colaPrioridad.push_back(id); // Agrega el proceso al final temporalmente.
        sort(colaPrioridad.begin(), colaPrioridad.end(), compararPrioridad); // Ordena la cola.
    }
}

// Busca el ID en la cola y lo elimina sin modificar la lista de procesos.
void quitarDeCola(int id) {
    for (int i = 0; i < (int)colaPrioridad.size(); i++) {
        if (colaPrioridad[i] == id) {
            colaPrioridad.erase(colaPrioridad.begin() + i); // Elimina la posicion encontrada.
            return;
        }
    }
}

// Calcula cuantos KB estan ocupados sumando todos los bloques de la pila.
int memoriaOcupada() {
    int total = 0;
    for (int i = 0; i < (int)pilaMemoria.size(); i++) {
        total += pilaMemoria[i].cantidad;
    }
    return total;
}

// Crea un bloque solo si la cantidad solicitada cabe dentro del limite simulado.
bool asignarMemoria(int id, int cantidad) {
    int disponible = MEMORIA_MAXIMA - memoriaOcupada();
    if (cantidad <= 0 || cantidad > disponible) {
        cout << "Error: memoria insuficiente. Disponible: " << disponible
             << " KB de " << MEMORIA_MAXIMA << " KB.\n";
        return false;
    }

    Bloque bloque;
    bloque.idProceso = id;
    bloque.cantidad = cantidad;
    pilaMemoria.push_back(bloque); // push_back agrega el bloque en el tope de la pila.
    Proceso* proceso = buscarProceso(id);
    if (proceso != NULL) proceso->memoria += cantidad; // Actualiza su memoria acumulada.
    return true;
}

// Quita todos los bloques asociados al proceso y pone su memoria asignada en cero.
void liberarMemoria(int id) {
    Proceso* proceso = buscarProceso(id);
    if (proceso == NULL) {
        cout << "No se encontro el proceso.\n";
        return;
    }

    int total = 0; // Acumula cuanta memoria se libera.
    // Se copian aqui los bloques de otros procesos para conservarlos.
    vector<Bloque> bloquesRestantes;
    for (int i = 0; i < (int)pilaMemoria.size(); i++) {
        if (pilaMemoria[i].idProceso == id) {
            total += pilaMemoria[i].cantidad; // Este bloque pertenece al proceso solicitado.
        } else {
            bloquesRestantes.push_back(pilaMemoria[i]); // Conserva bloques ajenos.
        }
    }
    pilaMemoria = bloquesRestantes; // Reemplaza la pila por los bloques que siguen ocupados.
    proceso->memoria = 0;           // Ya no quedan bloques para este proceso.

    if (total == 0) cout << "El proceso no tiene memoria asignada.\n";
    else cout << "Se liberaron " << total << " KB.\n";
}

// Solicita los datos del proceso, valida el ID y lo registra en la lista.
void crearProceso() {
    int id = leerNumero("ID del proceso: ");
    if (id <= 0 || buscarProceso(id) != NULL) { // Evita IDs no positivos o repetidos.
        cout << "El ID debe ser positivo y no debe estar registrado.\n";
        return;
    }
    // getline permite que el nombre contenga espacios.
    string nombre;
    cout << "Nombre del proceso: ";
    getline(cin, nombre);
    if (nombre.empty()) {
        cout << "El nombre no puede estar vacio.\n";
        return;
    }
    // La prioridad se usara para ordenar la cola de ejecucion.
    int prioridad = leerNumero("Prioridad (mayor numero = mayor prioridad): ");
    // Cada bloque creado se registra en la pila de memoria del sistema.
    int cantidadMemoria = leerNumero("Memoria inicial en KB (0 si no requiere): ");
    if (cantidadMemoria < 0) {
        cout << "La memoria no puede ser negativa.\n";
        return;
    }
    int disponible = MEMORIA_MAXIMA - memoriaOcupada();
    if (cantidadMemoria > disponible) {
        cout << "Error: memoria insuficiente. Solicito " << cantidadMemoria
             << " KB, pero solo hay " << disponible << " KB disponibles.\n";
        return;
    }
    agregarProceso(id, nombre, prioridad);
    if (cantidadMemoria > 0) {
        asignarMemoria(id, cantidadMemoria); // Registra el bloque y actualiza el proceso.
    }
    // Un proceso nuevo queda en espera hasta que el planificador lo ejecute.
    Proceso* procesoNuevo = buscarProceso(id);
    procesoNuevo->estado = 1;
    agregarACola(id); // Lo incorpora a la cola ordenada por prioridad.
    cout << "Proceso creado y agregado a la cola de prioridad.\n";
}

// Encuentra el nodo del proceso, lo desconecta de las estructuras y libera su memoria.
void eliminarProceso() {
    int id = leerNumero("ID del proceso que desea eliminar: ");
    Proceso* actual = listaProcesos;
    Proceso* anterior = NULL; // Permite reconectar la lista al borrar el nodo actual.
    while (actual != NULL && actual->id != id) {
        anterior = actual; // Recuerda el nodo anterior antes de avanzar.
        actual = actual->siguiente;
    }
    if (actual == NULL) {
        cout << "No se encontro el proceso.\n";
        return;
    }
    if (actual == procesoActual) {
        cout << "No se puede eliminar el proceso que se esta ejecutando.\n";
        return;
    }
    quitarDeCola(id);          // Evita que quede un ID inexistente en la cola.
    // Solo llama la liberacion si el proceso tenia memoria asignada.
    if (actual->memoria > 0) liberarMemoria(id);
    if (anterior == NULL) listaProcesos = actual->siguiente; // Se elimina el primer nodo.
    else anterior->siguiente = actual->siguiente;            // Se salta el nodo eliminado.
    delete actual;             // Devuelve la memoria dinamica del nodo al sistema.
    cout << "Proceso eliminado.\n";
}

// Busca un proceso por su identificador y muestra todos sus datos.
void buscarPorID() {
    int id = leerNumero("ID que desea buscar: ");
    Proceso* proceso = buscarProceso(id);
    if (proceso == NULL) {
        cout << "No se encontro el proceso.\n";
        return;
    }
    cout << "ID: " << proceso->id << " | Nombre: " << proceso->nombre
         << " | Prioridad: " << proceso->prioridad
         << " | Estado: " << obtenerEstado(proceso->estado)
         << " | Memoria: " << proceso->memoria << " KB\n";
}

// Busca procesos por nombre; se pueden mostrar varios si comparten el mismo nombre.
void buscarPorNombre() {
    string nombre;
    cout << "Nombre que desea buscar: ";
    getline(cin, nombre);
    Proceso* actual = listaProcesos;
    bool encontrado = false;
    while (actual != NULL) {
        // c_str convierte los string a cadenas compatibles con strcmp.
        if (strcmp(actual->nombre.c_str(), nombre.c_str()) == 0) {
            cout << "ID: " << actual->id << " | Prioridad: " << actual->prioridad
                 << " | Estado: " << obtenerEstado(actual->estado)
                 << " | Memoria: " << actual->memoria << " KB\n";
            encontrado = true;
        }
        actual = actual->siguiente;
    }
    if (!encontrado) cout << "No se encontro el proceso.\n";
}

// Actualiza la prioridad. Si el proceso esperaba en la cola, se reordena.
void modificarPrioridad() {
    int id = leerNumero("ID del proceso: ");
    Proceso* proceso = buscarProceso(id);
    if (proceso == NULL) {
        cout << "No se encontro el proceso.\n";
        return;
    }
    proceso->prioridad = leerNumero("Nueva prioridad: ");
    quitarDeCola(id); // Quita la posicion antigua para que el orden pueda cambiar.
    if (proceso->estado == 1) agregarACola(id); // Lo vuelve a insertar con prioridad nueva.
    cout << "Prioridad actualizada.\n";
}

// Recorre y muestra todos los nodos de la lista enlazada.
void mostrarProcesos() {
    if (listaProcesos == NULL) {
        cout << "No hay procesos registrados.\n";
        return;
    }
    Proceso* actual = listaProcesos;
    while (actual != NULL) {
        cout << "ID: " << actual->id << " | Nombre: " << actual->nombre
             << " | Prioridad: " << actual->prioridad
             << " | Estado: " << obtenerEstado(actual->estado)
             << " | Memoria: " << actual->memoria << " KB\n";
        actual = actual->siguiente;
    }
}

// Muestra la cola desde el proceso con mayor prioridad hasta el de menor prioridad.
void mostrarCola() {
    if (colaPrioridad.empty()) {
        cout << "La cola de prioridad esta vacia.\n";
        return;
    }
    cout << "Cola de prioridad (primero se ejecuta el de mayor prioridad):\n";
    // Cada elemento del vector es un ID; los demas datos se consultan en la lista.
    for (int i = 0; i < (int)colaPrioridad.size(); i++) {
        Proceso* proceso = buscarProceso(colaPrioridad[i]);
        if (proceso != NULL)
            cout << proceso->id << " - " << proceso->nombre
                 << " - Prioridad " << proceso->prioridad << "\n";
    }
}

// Toma el primer elemento de la cola y lo marca como proceso actual de la CPU.
void ejecutarSiguiente() {
    if (procesoActual != NULL) {
        cout << "Primero termine el proceso actual.\n";
        return;
    }
    if (colaPrioridad.empty()) {
        cout << "No hay procesos en espera.\n";
        return;
    }
    int id = colaPrioridad[0]; // La posicion cero contiene la prioridad mas alta.
    colaPrioridad.erase(colaPrioridad.begin()); // Quita el proceso de la cola de espera.
    procesoActual = buscarProceso(id);
    if (procesoActual != NULL) {
        procesoActual->estado = 2;
        cout << "Ejecutando: " << procesoActual->nombre << "\n";
    }
}

// Marca el proceso actual como finalizado y deja libre la CPU.
void terminarProcesoActual() {
    if (procesoActual == NULL) {
        cout << "No hay proceso en ejecucion.\n";
        return;
    }
    cout << "Proceso terminado: " << procesoActual->nombre << "\n";
    procesoActual->estado = 3;
    procesoActual = NULL;
}

// Recorre la pila desde el tope hacia la base y muestra cada bloque.
void mostrarMemoria() {
    if (pilaMemoria.empty()) {
        cout << "La pila de memoria esta vacia.\n";
        cout << "Memoria asignada: 0 de " << MEMORIA_MAXIMA << " KB.\n";
        cout << "Memoria disponible: " << MEMORIA_MAXIMA << " KB.\n";
        return;
    }
    cout << "Bloques de memoria (el ultimo agregado esta en el tope):\n";
    int total = 0;
    // Se empieza en el ultimo indice porque representa el tope de la pila.
    for (int i = (int)pilaMemoria.size() - 1; i >= 0; i--) {
        cout << "Proceso " << pilaMemoria[i].idProceso << " - "
             << pilaMemoria[i].cantidad << " KB\n";
        total += pilaMemoria[i].cantidad;
    }
    cout << "Total asignado: " << total << " de " << MEMORIA_MAXIMA << " KB\n";
    cout << "Memoria disponible: " << MEMORIA_MAXIMA - total << " KB\n";
}

// Agrupa en una sola pantalla la lista, el estado de CPU, la cola y la memoria.
void mostrarEstadoGeneral() {
    cout << "\n--- ESTADO GENERAL DEL SISTEMA ---\n";
    mostrarProcesos();
    if (procesoActual == NULL) cout << "CPU: sin proceso en ejecucion.\n";
    else cout << "CPU: " << procesoActual->nombre << "\n";
    mostrarCola();
    mostrarMemoria();
}

// Agrega tres procesos de ejemplo para observar la cola de prioridad y la memoria.
void demostracionAutomatica() {
    // La demostracion asigna 64 + 128 + 96 = 288 KB en total.
    if (MEMORIA_MAXIMA - memoriaOcupada() < 288) {
        cout << "Error: no hay memoria suficiente para la demostracion.\n";
        cout << "Se necesitan 288 KB disponibles.\n";
        return;
    }

    // rand elige un punto de partida para reducir la posibilidad de repetir IDs.
    int base = 9000 + rand() % 1000;
    // Se verifica que el ID inicial y los dos siguientes esten disponibles.
    while (buscarProceso(base) != NULL || buscarProceso(base + 1) != NULL ||
           buscarProceso(base + 2) != NULL) base++;

    // Registra procesos de baja, alta y mediana prioridad.
    agregarProceso(base, "Demo prioridad baja", 2);
    agregarProceso(base + 1, "Demo prioridad alta", 9);
    agregarProceso(base + 2, "Demo prioridad media", 5);
    // Asigna cantidades de memoria de ejemplo a cada proceso.
    asignarMemoria(base, 64);
    asignarMemoria(base + 1, 128);
    asignarMemoria(base + 2, 96);

    // Los tres pasan a estado de espera y se agregan a la cola ordenada.
    for (int i = 0; i < 3; i++) {
        Proceso* proceso = buscarProceso(base + i);
        proceso->estado = 1;
        agregarACola(proceso->id);
    }
    cout << "Se agregaron tres procesos de demostracion. Orden de ejecucion:\n";
    mostrarCola();
}

// Imprime las opciones principales y las subopciones disponibles.
void mostrarMenu() {
    cout << "\n========================================\n";
    cout << " SISTEMA DE GESTION DE PROCESOS\n";
    cout << "========================================\n";
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
    cout << "========================================\n";
}

// Espera que el usuario presione Enter antes de volver a mostrar el menu.
void pausa() {
    cout << "\nPresione Enter para continuar...";
    cin.get();
}

// Libera todos los nodos creados con new antes de terminar el programa.
void liberarLista() {
    while (listaProcesos != NULL) {
        Proceso* borrar = listaProcesos;
        listaProcesos = listaProcesos->siguiente;
        delete borrar;
    }
}

// Punto de inicio: muestra el menu y dirige cada opcion al procedimiento adecuado.
int main() {
    int opcion;
    char subopcion;
    do { // El ciclo mantiene el programa activo hasta que se elija salir.
        mostrarMenu();
        opcion = leerNumero("Seleccione una opcion: ");
        if (opcion == 1) { // Gestor de procesos y sus subopciones.
            cout << "Opcion del gestor (a-f): ";
            cin >> subopcion;
            cin.ignore(10000, '\n');
            if (subopcion == 'a') crearProceso();
            else if (subopcion == 'b') eliminarProceso();
            else if (subopcion == 'c') buscarPorID();
            else if (subopcion == 'd') buscarPorNombre();
            else if (subopcion == 'e') modificarPrioridad();
            else if (subopcion == 'f') mostrarProcesos();
            else cout << "Opcion no valida.\n";
            pausa();
        } else if (opcion == 2) { // Planificador de CPU.
            cout << "Opcion del planificador (a-c): ";
            cin >> subopcion;
            cin.ignore(10000, '\n');
            if (subopcion == 'a') ejecutarSiguiente();
            else if (subopcion == 'b') terminarProcesoActual();
            else if (subopcion == 'c') mostrarCola();
            else cout << "Opcion no valida.\n";
            pausa();
        } else if (opcion == 3) { // Gestor de memoria.
            cout << "Opcion del gestor de memoria (a-b): ";
            cin >> subopcion;
            cin.ignore(10000, '\n');
            if (subopcion == 'a') mostrarMemoria();
            else if (subopcion == 'b') liberarMemoria(leerNumero("ID del proceso: "));
            else cout << "Opcion no valida.\n";
            pausa();
        } else if (opcion == 4) { // Resumen general del estado.
            mostrarEstadoGeneral();
            pausa();
        } else if (opcion == 5) { // Demostracion de ejemplo.
            demostracionAutomatica();
            pausa();
        } else if (opcion != 0) {
            cout << "Opcion no valida.\n";
            pausa();
        }
    } while (opcion != 0); // Se repite mientras no se elija la opcion cero.

    liberarLista(); // Limpieza final de la memoria dinamica de los procesos.
    return 0; // Indica que el programa termino correctamente.
}

