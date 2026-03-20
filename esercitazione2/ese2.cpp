#include <iostream>
#include <algorithm> //Mi serve per min e max
#include <cmath> //Mi serve per sqrt

int main()
{
    //Definisco l'array di double
    static const int N = 10;
    double array[N] = {1.8, 4.8, 6.4, 8.9, 5.8, 7.3, 0.2, 9.1, 5.5, 2.1};

    //Calcolo max e min:
    //inanzitutto inizializzo due variabili, che mi serviranno poi per i confronti
    double minimo = array[0];
    double massimo = array[0];

    //faccio i confronti utilizzando std::min e std::max (algorithm)
    for(int i = 1; i < N; i++){
        minimo = std::min(minimo, array[i]);
        massimo = std::max(massimo, array[i]);
    }

    //Calcolo media:
    //prima sommo tutti gli elementi dell'array
    double somma = 0.0;
    for(int i = 0; i < N; i++){
        somma += array[i];
    }
    //poi divido la somma per il numero di elementi
    double media = somma / N;

    //Calcolo deviazione standard:
    //prima calcoliamo la somma dei quadrati della differenza tra il valore dell'i-esimo elemento dell'array e la media
    double varianza = 0.0;
    for(int i = 0; i < N; i++){
        varianza += (array[i] - media) * (array[i] - media);
    }
    //divido tale risultato per N, trovo quindi la varianza
    varianza = varianza / N;
    //faccio la radice quadrata della varianza
    double deviazione = std::sqrt(varianza);

    std::cout << "Valore minimo: " << minimo << "\n" << "Valore massimo: " << massimo << "\n" << "Media: " << media << "\n" << "Deviazione standard: " << deviazione << "\n";

    //Parte facoltativa(Bubblesort):
    for(int i = 0; i < N - 1; i++){
        //Variabile scambio mi serve per controllare a fine loop for se ho effettuato scambi
        bool scambio = false;
        //Questo loop for controlla solo i primi N - i - 1 elementi, in quanto gli ultimi elementi vengono man mano ordinati
        for(int j = 0; j < N - i - 1; j++){
            if(array[j] > array[j + 1]){
                //Scambio di valori con variabile temporanea
                double temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
                //Aggiorno la flag scambio in caso ci sia stato uno scambio
                scambio = true;
            }
        }
        //Se arrivo a fine array e non ho fatto alcuno scambio, esco dal loop for, in quanto l'array è gia ordinato
        if(scambio == false){
            break;
        }
    }

    //Stampo l'Array ordinato:
    std::cout << "Array ordinato con BubbleSort: \n";
    for(int i = 0; i < N; i++){
        std::cout << array[i] << " ";
    }
    std::cout << "\n";

    return 0;
}