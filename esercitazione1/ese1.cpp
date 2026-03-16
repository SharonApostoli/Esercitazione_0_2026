#include <iostream>
#include <fstream>
#include "ese1.h"

int main(int argc, const char *argv[])
{
    //controllo che quando lancio il programma ci sia specificato anche il nome del file (argc = 2)
    if(argc < 2){
        std::cerr << "Non è stato specificato il nome del file! \n";
        return -1;
    }
    //il nome del file che voglio aprire è in argv[1]
    std::string filename = argv[1];
    std::ifstream ifs(filename);

    if(!ifs.is_open()){ //se il file non si apre -> errore
        std::cerr << "Il file non è stato aperto correttamente! \n";
        return -2;
    }
    //il file si apre!
    std::cout << "File aperto correttamente! \n";
    std::string location;
    double zero, sei, dod, dic;
    //estraiamo i dati dal file
    //continuo ad andare avanti fino a che riesco a leggere il file e nel frattempo stampo il risultato
    while(ifs >> location >> zero >> sei >> dod >> dic){
        std::cout << location << " " << media(zero, sei, dod, dic) << "\n";
    }
    return 0;
}