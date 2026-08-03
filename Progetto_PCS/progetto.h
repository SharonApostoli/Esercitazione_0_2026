#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>
#include <string>
#include <map>
#include <set>
#include <Eigen/Dense>


struct Componente{
    std::string nome;
    double valore;
    int nodo_pos;

    //costruttore di default
    Componente(){
        nome = "";
        valore = 0.0;
        nodo_pos = 0;
    }

    Componente(const std::string& n, double v, int np = 0){
        nome = n;
        valore = v;
        nodo_pos = np;
    }

    //Funzione per vedere se un componente è resistore
    bool is_resistore() const{
        return !nome.empty() && nome[0] == 'R';
    }
    //Funzione per vedere se un componente è generatore
    bool is_generatore() const{
        return !nome.empty() && nome[0] == 'V';
    }
};

struct Ciclo{
    std::vector<int> nodi;
};


template<typename T>
class unidirected_edge{
    T node_f;
    T node_t;

public:
    unidirected_edge(const T& partenza, const T& arrivo){
        if(partenza < arrivo){
            node_f = partenza;
            node_t = arrivo;
        }else{
            node_f = arrivo;
            node_t = partenza;
        }
    }

    T from() const {
        return node_f;
    }

    T to() const {
        return node_t;
    }

    bool operator<(const unidirected_edge& other) const {
        unidirected_edge ret = *this;
        if(ret.node_f != other.node_f){
            return ret.node_f < other.node_f;
        }
        return ret.node_t < other.node_t;
    }

    bool operator==(const unidirected_edge& other) const {
        unidirected_edge ret = *this;
        return (ret.node_f == other.node_f && ret.node_t == other.node_t);
    }
};

//operator<<
template<typename T>
std::ostream& operator<<(std::ostream& os, const unidirected_edge<T>& edge){
    os << "(" << edge.from() << ", " << edge.to() << ")";
    return os;
};


template<typename T>
class unidirected_graph{
    std::map<T, std::vector<std::pair<T, int>>> adiacenza;
    std::set<unidirected_edge<T>> edge;
    std::map<unidirected_edge<T>, Componente> components;
public:
    unidirected_graph(){
    };

    unidirected_graph(const unidirected_graph& other){
        adiacenza = other.adiacenza;
        edge = other.edge;
        components = other.components;
    }

    std::vector<std::pair<T,int>> neighbours(const T& nodo) const{
        auto key = adiacenza.find(nodo);
        if(key == adiacenza.end()){
            return {};
        }
        return key -> second;
    }

    void add_edge(const T& a,const T& b, const std::string& nome, double valore, int nodo_pos = 0){
        unidirected_edge<T> arco(a, b);
        if(edge.find(arco) != edge.end()){
            return;
        }
        edge.insert(arco);

        int w = edge_number(arco);
        components[arco] = Componente(nome, valore, nodo_pos);
        adiacenza[a].push_back({b, w});
        adiacenza[b].push_back({a, w});
    }

    std::set<unidirected_edge<T>> all_edges() const{
        return edge;
    }

    std::set<T> all_nodes() const{
        std::set<T> nodi;
        for(const auto& pair : adiacenza){
            nodi.insert(pair.first);
        }
        return nodi;
    }

    int edge_number(const unidirected_edge<T> & arco) const{
        int pos = 0;
        for(const unidirected_edge<T>& i : edge){
            if(i == arco){
                return pos;
            }
            pos++;
        }
        return -1;
    }

    std::optional<unidirected_edge<T>> edge_at(int num) const{
        if(num < 0 || num >= edge.size()){
            return std::nullopt;
        }
        auto it = edge.begin();
        std::advance(it, num);
        return *it;
    }

    unidirected_graph operator-(const unidirected_graph& other) const{
        unidirected_graph archi;
        for(const unidirected_edge<T>& el : edge){
            if(other.edge.find(el) == other.edge.end()){
                Componente comp = components.at(el);
                archi.add_edge(el.from(), el.to(), comp.nome, comp.valore, comp.nodo_pos);
            }
        }
        return archi;  
    }

    std::vector<double> valori_res() const{
        std::vector<double> valori;

        for(const auto& [arco, componente] : components){
            if(componente.nome[0] == 'R'){
                valori.push_back(componente.valore);
            }
        }
        return valori;
    }

    std::vector<unidirected_edge<T>> archi_resistori() const{
        std::vector<unidirected_edge<T>> res;

        for(const auto& [arco, componente] : components){
            if(componente.nome[0] == 'R'){
                res.push_back(arco);
            }
        }
        return res;
    }

    Componente get_componente(const unidirected_edge<T>& e) const{
        return components.at(e);
    }

    //Questo mi serve nel caso in cui gli passo i nodi
    Componente get_componente(const T& from, const T& to) const{
        return get_componente(unidirected_edge<T>(from, to));
    }

    bool has_edge(const T& a, const T& b) const{
        return edge.count(unidirected_edge<T>(a,b)) > 0;
    }
};

//Funzione per lettura e immagazzinazione dati dal file netlist
void read_file(const std::string& filename, unidirected_graph<int>& G);

void recursive_dfs_aiuto(const unidirected_graph<int>& G, int v, unidirected_graph<int>& tree, std::set<int>& visited);

unidirected_graph<int> recursive_dfs(const unidirected_graph<int>& G, int v);

bool find_path_aiuto(const unidirected_graph<int>& T, int u, int v, std::vector<int>& path, std::set<int>& visited);

std::vector<int> find_path(const unidirected_graph<int>& T, int u, int v);

std::vector<Ciclo> cicli_fondamentali_dfs(const unidirected_graph<int>& G);

int dot_mod2(const std::vector<int>& a, const std::vector<int>& b);

std::vector<int> diff_simm(const std::vector<int>& a, const std::vector<int>& b);

Ciclo lifting(const unidirected_graph<int>& G, const std::vector<unidirected_edge<int>>& edge_list, const std::vector<int>& S);

std::vector<Ciclo> cicli_de_pina(const unidirected_graph<int>& G);
//MANCA DE PINA

Eigen::MatrixXd Rmatrix(const unidirected_graph<int>& G);

Eigen::MatrixXd Bmatrix(const unidirected_graph<int>& G, const std::vector<Ciclo>& cicli);

Eigen::VectorXd termini_noti(const unidirected_graph<int>& G, const std::vector<Ciclo>& cicli);