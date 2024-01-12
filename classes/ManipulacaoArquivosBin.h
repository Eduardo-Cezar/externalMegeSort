/*
Esta lib foi criada para que facilitasse
a manipulação dos arquivos excluindo
a possibilidade de erro na manipulação
de arquivos, umas vez que ela já estivesse
funcionando corretamente.

Também é útil para deixar o código das
outras libs e da main mais limpo.
*/

#ifndef MANIPILACAOARQUIVOSBIN_H   
#define MANIPILACAOARQUIVOSBIN_H

#include <fstream>
#include <iostream>

using namespace std;

template <class T> void copiaArq(std::ifstream * origem, std::ofstream * destino, unsigned tamanhoInformacao){
    T intermediario;
    origem->read((char*)&intermediario, tamanhoInformacao);
    destino->write((char*)&intermediario, tamanhoInformacao);
}

template <class T> bool lerUmDado(std::ifstream * arquivo, T * info, unsigned tamanhoInformacao){
    bool concluido = arquivo->read((char*)info, tamanhoInformacao);
    return concluido;
}

template <class T> bool escreveUmDado(std::ofstream * arquivo, T info, unsigned tamanhoInformacao){
    bool concluido = arquivo->write((char*)&info, tamanhoInformacao);
    return concluido;
}

template <class T> void le_arquivo_bin(std::string nomeArquivo, int qtdInfo) {
    std::ifstream arquivo(nomeArquivo, ios::binary);
    T umaInformacao;
    int i = 0;

    cout << "\nDados das primeiras " << qtdInfo << " linhas do arquivo " << nomeArquivo << endl;
    //cout << "Indice | Tempo | Origem | Destino | Protocolo | Tamanho | Informacao " << std::endl;
    while(arquivo.read((char*) &umaInformacao, sizeof(umaInformacao)) and i < qtdInfo) {
        cout << '[' << i+1 << "] ";
        //cout << umaInformacao.indice << "; ";
        std::cout << umaInformacao.tempo << "; ";
        //cout << umaInformacao.origem << "; ";
        //cout << umaInformacao.destino << "; ";
        //cout << umaInformacao.protocolo << "; ";
        std::cout << umaInformacao.tamanho << "; ";
        //cout << umaInformacao.informacao << endl;
        std::cout << std::endl;
        i++;
    }
    cout << endl;
}

#endif