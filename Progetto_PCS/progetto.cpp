#include <iostream>

#include "progetto.h"

int main(int argc, char **argv){
    //Lettura file
    if(argc < 2){
        std::cerr << "Non è stato inserito il nome del file!\n";
        return -1;
    }

    //Creazione grafo
    unidirected_graph<int> G;
    read_file(argv[1], G);

    //Cicli fondamentali
    std::vector<Ciclo> cicli = cicli_fondamentali_dfs(G);

    //Costruisco le matrici
    Eigen::MatrixXd R = Rmatrix();
    Eigen::MatrixXd B = Bmatrix();
    Eigen::VectorXd v = termini_noti();

    //Risolvo il sist. lin. (B^T R B i = v)
    Eigen::MatrixXd A = B.transpose() * R * B;
    Eigen::VectorXd corr = A.lu().solve(v);

    //Calcolo tensioni
    Eigen::VectorXd tensioni = R * B * corr;
    std::vector<unidirected_edge<int>> res_archi = G.archi_resistori();
    for(int i = 0; i < res_archi.size(); i++){
        Componente comp = G.get_componente(res_archi[i]);
        std::cout << comp.nome << ": V = " << tensioni(i) << " volts, I = "  << tensioni(i) / comp.valore << " amps.\n";
    }

    return 0;
}