#include <iostream>
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
            cout << "Thread " << this_thread::get_id() << " utilizó un recurso. Recursos disponibles: " << resourceCount << endl;

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
            cout << "Thread " << this_thread::get_id() << " devolvió un recurso. Recursos disponibles: " << resourceCount << endl;
        }
        else
        {
            // No hay recursos disponibles
            cout << "Thread " << this_thread::get_id() << " no pudo utilizar un recurso. Recursos disponibles: " << resourceCount << endl;

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

    // Crear los threads
    thread threads[threadCount];
    for (int i = 0; i < threadCount; i++)
    {
        threads[i] = thread(threadFunction);
    }

    // Esperar a que los threads terminen
    for (int i = 0; i < threadCount; i++)
    {
        threads[i].join();
    }

    return 0;
}
