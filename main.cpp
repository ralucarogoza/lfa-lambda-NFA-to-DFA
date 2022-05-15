#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <cstring>
#include <iomanip>
using namespace std;
ifstream fin("in.txt");
int stari_lambda_nfa, tranzitii_lambda_nfa, stare_initiala_lambda_nfa, stari_finale[100], stare1, stare2;
int numar_stari_finale_lambda_nfa, stare_finala, vizitat[100], numar_simboluri, tranzitii2;
int index_stare_dfa, parcurgeri, stare_finala_dfa, index_stare_finala_dfa;
char matrice_adiacenta[100][100][100], caracter, simboluri[100], simbol;
std::queue <int> coada;
std::vector <int> inchidere[100], stari_dfa[1000], stari_finale_dfa[1000], next_states, tranzitii_dfa[1000][1000];
std::set <int> temp_states;

void citire() {
    fin >> stari_lambda_nfa >> tranzitii_lambda_nfa;
    fin >> numar_simboluri;
    for(int i = 0; i < numar_simboluri; i++) {
        fin >> simbol;
        simboluri[i] = simbol;
    }
    for(int i = 0; i < tranzitii_lambda_nfa; i++) {
        fin >> stare1 >> stare2 >> tranzitii2;
        for(int j = 0; j < tranzitii2; j++){
            fin >> caracter;
            //crearea matricei de adiacenta
            matrice_adiacenta[stare1][stare2][j] = caracter;
        }
    }
    fin >> stare_initiala_lambda_nfa;
    fin >> numar_stari_finale_lambda_nfa;
    for(int i = 0; i < numar_stari_finale_lambda_nfa; i++) {
        fin >> stare_finala;
        stari_finale[i] = stare_finala;
    }
}

void dfs(int stare_curenta) {
    vizitat[stare_curenta] = 1;
    for(int i = 0; i < stari_lambda_nfa; i++) {
        for(int j = 0; j < strlen(matrice_adiacenta[stare_curenta][i]); j++) {
            if(matrice_adiacenta[stare_curenta][i][j]=='#' && vizitat[i] ==  0) {
                coada.push(i);
                dfs(i);
            }
        }
    }
}

void lambda_inchideri() {
    for(int i = 0; i < stari_lambda_nfa; i++) {
        for(int j = 0; j < stari_lambda_nfa; j++) {
            vizitat[j]=0;
        }
        coada.push(i);
        dfs(i);
        while(!coada.empty()) {
            inchidere[i].push_back(coada.front());
            coada.pop();
        }
    }
}

void verificare_stare_finala_dfa() {
    //verific daca starea curenta este si stare finala in DFA
    stare_finala_dfa = 0;
    for(auto it = stari_dfa[parcurgeri].begin(); it != stari_dfa[parcurgeri].end(); it++) {
        for(int i = 0; i < numar_stari_finale_lambda_nfa; i++) {
            if(*it == stari_finale[i])
                stare_finala_dfa = 1;
        }
    }
    //daca este stare finala, o adaug in vectorul cu stari finale
    if(stare_finala_dfa == 1) {
        for(auto it = stari_dfa[parcurgeri].begin(); it != stari_dfa[parcurgeri].end(); it++) {
            stari_finale_dfa[index_stare_finala_dfa].push_back(*it);
        }
        index_stare_finala_dfa++;
    }
}

void verificare_stare_dfa() {
    //verific daca starea gasita nu e deja in starile existente in DFA
    //daca nu e, o adaug
    int ok = 0;
    if(temp_states.empty() != 1) {
        for(int index = 0; index < index_stare_dfa && ok != 1; index++) {
            int stari_gasite = 0, stari2 = 0;
            for(auto it = stari_dfa[index].begin(); it != stari_dfa[index].end(); it++) {
                stari2++;
                if(temp_states.find(*it) != temp_states.end())
                    stari_gasite++;
            }
            if(stari_gasite == temp_states.size() && stari2 == stari_gasite) {
                //am gasit starea in cele existente
                ok = 1;
            }
        }
        if(ok == 0) {
            //adaug starea care nu a fost gasita
            index_stare_dfa++;
            for(auto it = temp_states.begin() ; it != temp_states.end() ; it++){
                stari_dfa[index_stare_dfa].push_back(*it);
            }
        }
        temp_states.clear();
    }
}

void transformare() {
    //adaug starea initiala din lambda-NFA ca stare initiala a DFA-ului
    for(auto it = inchidere[stare_initiala_lambda_nfa].begin(); it != inchidere[stare_initiala_lambda_nfa].end(); it++) {
        stari_dfa[index_stare_dfa].push_back(*it);
    }
    while(parcurgeri <= index_stare_dfa) {
        verificare_stare_finala_dfa();
        for(auto it = stari_dfa[parcurgeri].begin(); it != stari_dfa[parcurgeri].end(); it++) {
            for(auto it2 = inchidere[*it].begin(); it2 != inchidere[*it].end(); it2++) {
                next_states.push_back(*it2);
            }
        }
        for(int i = 0; i < numar_simboluri - 1; i++) {
            for(int j = 0; j != next_states.size(); j++) {
                for(int k = 0; k < stari_lambda_nfa; k++) {
                    for(int s = 0; s < strlen(matrice_adiacenta[next_states[j]][k]); s++) {
                        if(matrice_adiacenta[next_states[j]][k][s]==simboluri[i]) {
                            //adaug starile unde ajunge starea curenta prin tranzitiile cu simbolul respectiv
                            temp_states.insert(k);
                            for(auto it = inchidere[k].begin(); it != inchidere[k].end(); it++) {
                                temp_states.insert(*it);
                            }
                        }
                    }
                }
            }
            for(auto it = temp_states.begin(); it != temp_states.end(); it++) {
                tranzitii_dfa[parcurgeri][i].push_back(*it);
            }
            verificare_stare_dfa();
        }
        parcurgeri++;
        next_states.clear();
    }
}


void afisare_dfa() {
    std::cout << "STARE INITIALA: ";
    for(auto it = stari_dfa[stare_initiala_lambda_nfa].begin(); it != stari_dfa[stare_initiala_lambda_nfa].end(); it++)
        std::cout << *it << " ";
    std::cout<<"\nSTARI FINALE: ";
    for(int i = 0; i <= index_stare_finala_dfa; i++) {
        for(auto it = stari_finale_dfa[i].begin(); it != stari_finale_dfa[i].end(); it++)
            std::cout << *it << " ";
        std::cout << '\n' << setw(15);
    }
    std::cout<<"\nSTARE DFA                 ";
    for(int i = 0; i < strlen(simboluri) - 1; i++)
        std::cout << simboluri[i] << "                  ";
    std::cout<<'\n';
    for(int i = 0; i <= index_stare_dfa; i++) {
        for(auto it = stari_dfa[i].begin(); it != stari_dfa[i].end(); it++)
            std::cout << *it << " ";
        std::cout << "        ";
        for(int s = 0; s < numar_simboluri - 1; s++) {
            if(tranzitii_dfa[i][s].empty() != 1) {
                for(auto it2 = tranzitii_dfa[i][s].begin(); it2 != tranzitii_dfa[i][s].end(); it2++) {
                    std::cout << *it2 << " ";
                }
            }
            else
                std::cout << '-';
            std::cout << "        ";
        }
        std::cout << '\n';
    }
}

int main() {
    //citirea datelor
    citire();
    //calculez lambda inchiderile
    lambda_inchideri();
    //transformarea lambda-NFA -> DFA
    transformare();
    //afisarea DFA-ului rezultat
    afisare_dfa();
    return 0;
}
