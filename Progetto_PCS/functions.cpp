#include <iostream>
#include <fstream> //file I/O
#include <string>
#include <map>
#include <set>
#include <Eigen/Dense>
#include <Eigen/SVD>

#include "progetto.h"


void read_file(const std::string& filename, unidirected_graph<int>& G){
    std::ifstream ifs(filename);

    //Controllo se il file è stato aperto correttamente
    if(!ifs.is_open()){
        std::cerr << "Il file dei dati non è stato aperto correttamente! \n";
        return;
    }

    //Se siamo qui, il file è stato aperto correttamente
    //La prima variabile che troviamo sarà il tipo (R oppure V) seguita dal numero
    std::string name;
    //Subito dopo avremo il valore della componente (resistenza o tensione)
    double valore;
    //Infine avremo gli estremi dell'arco 
    int nodo1, nodo2;

    //Estraiamo ora i dati dal file, andando avanti fino a che riesco a leggere il risultato
    while(ifs >> name >> valore >> nodo1 >> nodo2){
        //per i generatori il nodo_pos è nodo1, per i resistori è 0 
        int nodo_pos = (name[0] == 'V') ? nodo1 : 0;
        G.add_edge(nodo1, nodo2, name, valore, nodo_pos);
    }

    ifs.close();
}

//Algoritmo DFS
//Funzione aiuto
void recursive_dfs_aiuto(const unidirected_graph<int>& G, int v, unidirected_graph<int>& tree, std::set<int>& visited){
    visited.insert(v);
    for(auto [vicino, peso] : G.neighbours(v)){
        if(!visited.count(vicino)){
            Componente Gcomp = G.get_componente(v, vicino);
            tree.add_edge(v, vicino, Gcomp.nome, Gcomp.valore, Gcomp.nodo_pos);
            recursive_dfs_aiuto(G, vicino, tree, visited);
        }
    }
}
//Funzione ricorsiva
unidirected_graph<int> recursive_dfs(const unidirected_graph<int>& G, int v){
    unidirected_graph<int> tree;
    std::set<int> visited;

    recursive_dfs_aiuto(G, v, tree, visited);

    return tree;
}

//Cerchiamo un cammino
//Funzione aiuto
bool find_path_aiuto(const unidirected_graph<int>& T, int u, int v, std::vector<int>& path, std::set<int>& visited){
    //Qui segno u come visitato
    visited.insert(u);
    //Qui segno u come inizio del mio ipotetico cammino
    path.push_back(u);

    //Se sono arrivata a v: finito, esiste cammino tra u e v e restituisco vero
    if(u == v){
        return true;
    }

    //Se no sono arrivata a v: esploro i nodi che non ho ancora visitato
    for(auto [vicino, w] : T.neighbours(u)){
        if(!visited.count(vicino)){
            if(find_path_aiuto(T, vicino, v, path, visited)){
                return true;
            }
        }
    }

    //Se invece arrivo qui, ho esplorato tutti i nodi e v non c'è.
    path.pop_back();
    return false;
}
//Funzione principale
std::vector<int> find_path(const unidirected_graph<int>& T, int u, int v){
    std::vector<int> path;
    std::set<int> visited;

    find_path_aiuto(T, u, v, path, visited);
    //Qui restituisco path: l'eventuale cammino tra u e v, se eiste.
    return path;
}

std::vector<Ciclo> cicli_fondamentali_dfs(const unidirected_graph<int>& G){
    //Cominciamo con la creazione dell'abero di supporto (da definire come trovare v)
    unidirected_graph<int> T = recursive_dfs(G, v);

    //Coalbero
    unidirected_graph<int> C = G - T; 

    //Struttura che contiene i cicli
    std::vector<Ciclo> cicli;

    /*Per ogni arco del coalbero C trovo il perorso in T tale che 
    quel percorso + l'arco = ciclo (= maglia del circuito)*/
    for(const auto& edge : C.all_edges()){
        std::vector<int> path = find_path(T, edge.from(), edge.to());
        Ciclo c;
        c.nodi = path;
        cicli.push_back(c);
    }

    return cicli;
}

//Matrice delle resistenze R (mxm, diagonale)
Eigen::MatrixXd Rmatrix(const unidirected_graph<int>& G){
    std::vector<double> resistori = G.valori_res();
    //Eigen non ha std::vector, quindi devo usare Eigen::Map per poter convertirlo in Eigen::VectorXd
    Eigen::VectorXd resist = Eigen::Map<Eigen::VectorXd>(resistori.data(), resistori.size());
    return resist.asDiagonal();
}

//Matrice di incidenza B (mxn)
Eigen::MatrixXd Bmatrix(const unidirected_graph<int>& G, const std::vector<Ciclo>& cicli){
    std::vector<unidirected_edge<int>> res_archi = G.archi_resistori();
    //Cominziamo con una matrice di zeri
    Eigen::MatrixXd B = Eigen::MatrixXd::Zero(res_archi.size(), cicli.size());
    //Per poi modificare solo gli elementi che mi interessano
    for(int j = 0; j < cicli.size(); j++){
        int len = cicli[j].nodi.size();
        for(int step = 0; step < len; step++){
            int node_u = cicli[j].nodi[step];
            int node_v = cicli[j].nodi[(step + 1) % len];

            //Se nel grafo non è presente l'aro fatto da u e v, prossima iterazione
            if(!G.has_edge(node_u, node_v)){
                continue;
            }
            Componente comp = G.get_componente(node_u, node_v);
            //Qui controllo se l'arco preso è resistore o no
            if(!comp.is_resistore()){
                continue;
            }

            //Adesso determino il segno
            unidirected_edge<int> arco(node_u, node_v);
            for(int i = 0; i < res_archi.size(); i++){
                if(res_archi[i] == arco){
                    if(node_u == arco.from()){
                        B(i, j) = +1.0;
                    } else{
                        B(i, j) = -1.0;
                    }
                    break;
                }
            }

        }
    }
    return B;
}

Eigen::VectorXd termini_noti(const unidirected_graph<int>& G, const std::vector<Ciclo>& cicli){
    Eigen::VectorXd v = Eigen::VectorXd::Zero(cicli.size());

    //Stesso ragionamento per il for della matrice b
    for(int j = 0; j < cicli.size(); j++){
        int len = cicli[j].nodi.size();
        for(int step = 0; step < len; step++){
            int node_u = cicli[j].nodi[step];
            int node_v = cicli[j].nodi[(step + 1) % len];

            if(!G.has_edge(node_u, node_v)){
                continue;
            }
            Componente comp = G.get_componente(node_u, node_v);
            if(!comp.is_generatore()){
                continue;
            }

            //Adesso assegno il segno: se arrivo al +, vado da - a + -> +v, altrimenti -v
            if(node_v == comp.nodo_pos){
                v(j) += comp.valore;
            }else{
                v(j) -= comp.valore;
            }
        }
    }
    return v;
}