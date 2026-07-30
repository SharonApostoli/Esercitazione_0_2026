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
    //Cominciamo con la creazione dell'abero di supporto
    unidirected_graph<int> T = recursive_dfs(G, *G.all_nodes().begin());

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

//Algoritmo De Pina
//Funzioni Aiuto:
//Prodotto Scalare (mod 2)
int dot_mod2(const std::vector<int>& a, const std::vector<int>& b){
    int s = 0;
    for(int i = 0; i < a.size(); i++){
        s ^= (a[i] & b[i]);
    }
    return s;
}

//Differenza simmetrica
std::vector<int> diff_simm(const std::vector<int>& a, const std::vector<int>& b){
    //Creo un vettore dove metto i risultati
    std::vector<int> r(a.size());
    for(int i = 0; i < a.size(); i++){
        r[i] = a[i] ^ b[i];
    }
    return r;
}

//Lifting del grafo
lifting(const unidirected_graph<int>& G, const std::vector<unidirected_edge<int>>& edge_list ){
    //1. Prendi tutti i vertici v nel grafo G e duplicali (v+ e v-)
    std::vector<int> nodes(G.all_nodes().begin(), G.all_nodes.().end());
    int n = nodes.size();
    int m = edge_list.size();
    std::map<int, int> nodo_indice;
    for(int i = 0; i < n; i++){
        nodo_indice[nodes[i]] = i;
    }
    //2. Per ogni edge(u, v) in G
        //Se edge è attivo in S[i], aggiungi a G' (u+, v-) e (u-, v+)
        //Caso contrario aggiungi a G' (u+, v+) e (u-, v-)
    
    //3. Per ogni vertive v in G
        //Calcola il cammino minimi (Dijkstra) tra v- e v+ in G'

    //4. I cammini trovati includono nodi "positivi" e "negativi".
    //   Per ogni cammino
            //Costruisci un vettore incidenza C_mu di lunghezza |E|, in cui
            //Per ogni occorrenza di (u, v)[pos o neg] si incrementa modulo 2 l'elemento relativo all'arco (u, v)

    //5. Di tutti i C_mu trovati, conserviamo quello con il piu piccolo
    //  numero di elementi 1. Questo sarà il vettore C_i che cerchiamo
}

std::vector<Ciclo> cicli_de_pina(const unidirected_graph<int>& G){
    //Albero di DFS e coalbero
    unidirected_graph<int> T = recursive_dfs(G, *G.all_nodes().begin());
    unidirected_graph<int> C = G - T;

    //Archi in ordine lessicografico (uso vector perche mi serve indicizzare s)
    std::vector<unidirected_edge<int>> edge_list(G.all_nodes().begin(), G.all_nodes().end());

    //k vettori booleani di lunghezza m
    int k = C.all_edges().size();
    int m = edge_list.size();
    
    //Inizializzo i vettori S
    std::vector<std::vector<int>> S(k, std::vector<int>(m, 0));
    int indice = 0;
    for(const auto& arco : C.all_edges()){
        S[indice][G.edge_number(arco)] = 1;
        indice++;
    }

    std::vector<Ciclo> base;
    //Ciclo for: trovo camm. min., calcolo vettore incidenza, aggiungo C alla base, aggiorno S
    for(int i = 0; i < k; i++){
        //trovo ciclo minimo
        //DA FINIRE A SCRIVERE!!!
        Ciclo ci = 

        //calcolo vettore incidenza
        std::vector<int> inci(m, 0);
        int len = ci.nodi.size();
        for(int j = 0; j < len; j++){
            unidirected_edge<int> e(ci.nodi[j], ci.nodi[(j + 1)  len]);
            int e_indice = G.edge_number(e);
            if(e_indice >= 0){
                inci[e_indice] ^= 1;
            }
        }

        //aggiungo c alla base
        base.push_back(ci);

        //aggiorno S
        for(int w = i + 1; w < k; w++){
            if(dot_mod2(inci, S[w]) == 1){
                S[w] = diff_simm(S[w], S[i]);
            }
        }
    }
    
    //Restituisco la base
    return base;
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