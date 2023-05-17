#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>

using namespace std;

int resourceCount = 0; // Contador del recurso
mutex resourceMutex; // Mutex para proteger el acceso al recurso
int iterationCount = 0; // Cantidad de iteraciones que los threads deben hacer
int waitTimeMin = 1; // Tiempo mínimo de espera
int waitTimeMax = 5; // Tiempo máximo de espera

ofstream logfile; // Archivo de bitácora

void logMessage(const string& message)
{
    resourceMutex.lock();
    logfile << message << endl;
    resourceMutex.unlock();
}

void threadFunction()
{
    for (int i = 0; i < iterationCount; i++)
    {
        // Bloquear el mutex para acceder al recurso compartido
        resourceMutex.lock();

        // Verificar si hay recursos disponibles
        if (resourceCount > 0)
        {
            // Utilizar un recurso
            resourceCount--;
            logMessage("Thread " + to_string(hash<thread::id>{}(this_thread::get_id())) + " no pudo utilizar un recurso. Recursos disponibles: " + to_string(resourceCount));


            // Liberar el mutex para permitir que otros threads utilicen el recurso
            resourceMutex.unlock();

            // Esperar por una cantidad de tiempo aleatoria
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(waitTimeMin, waitTimeMax);
            int waitTime = dis(gen);
            this_thread::sleep_for(chrono::seconds(waitTime));

            // Bloquear el mutex para acceder al recurso compartido nuevamente
            resourceMutex.lock();

            // Devolver el recurso utilizado
            resourceCount++;
            logMessage("Thread " + to_string(hash<thread::id>{}(this_thread::get_id())) + " devolvió un recurso. Recursos disponibles: " + to_string(resourceCount));
        }
        else
        {
            // No hay recursos disponibles

            logMessage("Thread " + to_string(hash<thread::id>{}(this_thread::get_id())) + " no pudo utilizar un recurso. Recursos disponibles: " + to_string(resourceCount));

            // Liberar el mutex para permitir que otros threads intenten utilizar el recurso
            resourceMutex.unlock();
        }
    }
}

int main()
{
    int threadCount = 5; // Cantidad de threads a crear

    // Pedir al usuario la cantidad de recursos disponibles
    cout << "Ingrese la cantidad de recursos disponibles: ";
    cin >> resourceCount;

    // Pedir al usuario la cantidad de iteraciones que los threads deben hacer
    cout << "Ingrese la cantidad de iteraciones que los threads deben hacer: ";
    cin >> iterationCount;

    // Pedir al usuario el tiempo mínimo de espera
    cout << "Ingrese el tiempo mínimo de espera en segundos: ";
    cin >> waitTimeMin;

    // Pedir al usuario el tiempo máximo de espera
    cout << "Ingrese el tiempo máximo de espera en segundos: ";
    cin >> waitTimeMax;

    // Abrir el archivo de bitácora
    logfile.open("bitacora.txt");

    // Escribir el encabezado en la bitácora
    logMessage("Iniciando programa");
    logMessage("Cantidad de recursos disponibles: " + to_string(resourceCount));
    logMessage("Cantidad de iteraciones por thread: " + to_string(iterationCount));
    logMessage("Tiempo mínimo de espera: " + to_string(waitTimeMin) + " segundos");
    logMessage("Tiempo máximo de espera: " + to_string(waitTimeMax) + " segundos");

    // Crear los threads
    vector<thread> threads;
    for (int i = 0; i < threadCount; i++)
    {
        threads.emplace_back(threadFunction);
    }

    // Esperar a que los threads terminen
    for (auto& thread : threads)
    {
        thread.join();
    }

    // Escribir mensaje de finalización en la bitácora
    logMessage("Programa finalizado");

    // Cerrar el archivo de bitácora
    logfile.close();

    return 0;
}
