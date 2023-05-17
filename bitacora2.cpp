#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
#include <semaphore.h>
#include <fstream>

using namespace std;

int availableResources = 0; // Recursos disponibles
mutex resourceMutex; // Mutex para proteger el acceso a los recursos
int iterationCount = 0; // Cantidad de iteraciones que los threads deben hacer
int waitTimeMin = 1; // Tiempo mínimo de espera
int waitTimeMax = 5; // Tiempo máximo de espera
sem_t semaphore; // Semáforo para controlar el acceso a los recursos
ofstream logFile; // Archivo de bitácora

int decrease_count(int count) {
    if (availableResources < count)
        return -1;
    else {
        availableResources -= count;
        return 0;
    }
}

int increase_count(int count) {
    availableResources += count;
    return 0;
}

void threadFunction()
{
    logFile << "Iniciando thread " << this_thread::get_id() << endl;

    for (int i = 0; i < iterationCount; i++)
    {
        // Esperar a que el semáforo esté disponible
        sem_wait(&semaphore);

        // Bloquear el mutex para acceder a los recursos compartidos
        resourceMutex.lock();

        // Verificar si hay suficientes recursos disponibles
        int requestedResources = 3; // Cantidad de recursos que se desea consumir
        if (decrease_count(requestedResources) == 0)
        {
            logFile << this_thread::get_id() << " - (!) Recurso tomado" << endl;
            logFile << this_thread::get_id() << " - Buenos dias! Recurso usado" << endl;

            // Liberar el mutex para permitir que otros threads utilicen los recursos
            resourceMutex.unlock();

            // Esperar por una cantidad de tiempo aleatoria
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(waitTimeMin, waitTimeMax);
            int waitTime = dis(gen);
            this_thread::sleep_for(chrono::seconds(waitTime));

            // Bloquear el mutex para acceder a los recursos compartidos nuevamente
            resourceMutex.lock();

            // Incrementar la cantidad de recursos utilizados
            increase_count(requestedResources);

            // Liberar el semáforo para permitir que otros threads consuman recursos
            sem_post(&semaphore);

            logFile << this_thread::get_id() << " - Recurso devuelto :)" << endl;
        }
        else
        {
            logFile << this_thread::get_id() << " - (!) Recurso tomado" << endl;
            logFile << this_thread::get_id() << " - Recurso no disponible" << endl;

            // Liberar el semáforo para permitir que otros threads intenten consumir recursos
            sem_post(&semaphore);

            // Liberar el mutex
            resourceMutex.unlock();
        }
    }

    logFile << this_thread::get_id() << " - Semaforo muerto" << endl;
    logFile << "Terminando thread " << this_thread::get_id() << endl;
}

int main()
{
    int threadCount = 5; // Cantidad de threads a crear

    // Pedir al usuario la cantidad de recursos disponibles
    cout << "Ingrese la cantidad de recursos disponibles: ";
    cin >> availableResources;

    // Pedir al usuario la cantidad de iteraciones que los threads deben hacer
    cout << "Ingrese la cantidad de iteraciones: ";
    cin >> iterationCount;

    // Inicializar el semáforo
    sem_init(&semaphore, 0, availableResources);

    // Abrir el archivo de bitácora
    logFile.open("bitacora2.txt");

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

    // Cerrar el archivo de bitácora
    logFile.close();

    return 0;
}
