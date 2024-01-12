
/*
* Este arquivo tem como intuito
* realizar a ordenação externa
*/
#ifndef MERGEEXTERNO_H   
#define MERGEEXTERNO_H

#include <cstring>
#include <fstream>
#include <iostream>
#include "ManipulacaoArquivosBin.h"

using namespace std;

// pacotes armazenados no arquivo
struct pacote {
    unsigned indice;
    float tempo;
    char origem[40];
    char destino[40];
    char protocolo[18];
    unsigned tamanho;
    char informacao[100]; 
};

class MergeExterno{
    private:
        ifstream arquivoInput;
        ofstream arquivoOutput;

        unsigned qtdRegistro;
        unsigned metade;
        unsigned tamanhoInformacao;

        unsigned calculaQtdRegistros();
        unsigned getTamanhoArquivo();
        unsigned calculaMetade();
        unsigned calculaTamanhoInformacao();

        void iteracaoArquivos();
        //Cria os primeiros arquivos auxiliares e copia os dados para eles
        void preProcessamento();
        // Metodo responsavel por chamar os outros metodos que irão executar o algoritmo como um todo
        void init();
        bool mergeExterno(ifstream auxEntrada[2], ofstream auxSaida[2], int passo, int saida);

        struct aux{
            int a;
            int b;
        };
    public:
        MergeExterno(string arquivoEntrada, string arquivoSaida);

};

MergeExterno::MergeExterno(string nomeArquivoEntrada, string nomeArquivoSaida){
    arquivoInput.open(nomeArquivoEntrada,ios::binary);
    arquivoOutput.open(nomeArquivoSaida,ios::binary);

    tamanhoInformacao = calculaTamanhoInformacao();
    qtdRegistro = calculaQtdRegistros();
    metade = calculaMetade();

    init();
}

unsigned MergeExterno::getTamanhoArquivo(){
    unsigned tamanho = 0;

    arquivoInput.seekg(0,ios::end);
    tamanho = arquivoInput.tellg();
    arquivoInput.seekg(0, ios::beg);

    return tamanho;
}

unsigned MergeExterno::calculaQtdRegistros(){
    return getTamanhoArquivo()/tamanhoInformacao;
}

unsigned MergeExterno::calculaMetade(){
    return (qtdRegistro/2.0) + 0.5;
}

unsigned MergeExterno::calculaTamanhoInformacao(){
    return sizeof(pacote);
}

void MergeExterno::init(){
    preProcessamento();
    iteracaoArquivos();
}

void MergeExterno::preProcessamento(){
    ofstream arqB1;
    ofstream arqB2;

    try{
        arqB1.open("arqB1.bin", ios::binary);
        arqB2.open("arqB2.bin", ios::binary);
    }catch(exception const& erro){
        cout << "Ocorreu um erro: " << erro.what() << endl;
        cerr << "Os arqivos auxiliares não puderem aberto" << endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < metade; i++)
        copiaArq<pacote>(&arquivoInput, &arqB1,tamanhoInformacao);

    for (int i = metade; i < qtdRegistro; i++)
        copiaArq<pacote>(&arquivoInput, &arqB2,tamanhoInformacao);

    arqB1.close();
    arqB2.close();
    arquivoInput.close();
}

void MergeExterno::iteracaoArquivos(){
    ifstream auxEntrada[2];
    ofstream auxSaida[2];
    
    int passo = 1;
    bool ida = true;
    bool ultimo[2];

    while(passo <= qtdRegistro){
        try{
            if(ida){
                auxEntrada[0].open("arqB1.bin", ios::binary);
                auxEntrada[1].open("arqB2.bin", ios::binary);
                auxSaida[0].open("arqC1.bin", ios::binary);
                auxSaida[1].open("arqC2.bin", ios::binary);
            }else{
                auxEntrada[0].open("arqC1.bin", ios::binary);
                auxEntrada[1].open("arqC2.bin", ios::binary);
                auxSaida[0].open("arqB1.bin", ios::binary);
                auxSaida[1].open("arqB2.bin", ios::binary);
            }
        }catch(exception const& erro){
            cout << "Ocorreu um erro: " << erro.what() << endl;
            cerr << "Os arquivos auxiliares não puderam ser aberto" << endl;
            exit(EXIT_FAILURE);
        }

        while ((not auxEntrada[0].eof()) and (not auxEntrada[1].eof()))
        {
            ultimo[0] = mergeExterno(auxEntrada, auxSaida, passo, 0);
            ultimo[1] = mergeExterno(auxEntrada,auxSaida, passo, 1);
        }

        auxEntrada[0].close();
        auxEntrada[1].close();
        auxSaida[0].close();
        auxSaida[1].close();

        ida = not(ida);
        passo *= 2;
    }

    ifstream auxEnt;

    try{
        // checa quais dos arquivos auxiliares possuem os dados ordenados, de acordo com a direcao da ultima iteracao
        if (ida)
            auxEnt.open((ultimo[0] ? "arqB1.bin" : "arqB2.bin"), ios::binary);
        else
            auxEnt.open((ultimo[0] ? "arqC1.bin" : "arqC2.bin"), ios::binary);
    }catch(exception const& erro){
        cout << "Ocorreu um erro: " << erro.what() << endl;
        cerr << "Um arquivo auxiliar não pude ser aberto" << endl;
        exit(EXIT_FAILURE);
    }

    while (!auxEnt.eof()){
        copiaArq<pacote>(&auxEnt,&arquivoOutput,tamanhoInformacao);
    }
    
    auxEnt.close();

    remove("arqB1.bin");
    remove("arqB2.bin");
    remove("arqC1.bin");
    remove("arqC2.bin");

}

bool MergeExterno::mergeExterno(ifstream auxEntrada[2], ofstream auxSaida[2], int passo, int saida){
    bool intercalou = false;
    pacote pacotes[2];

    int pos[2] = {0,0};

    bool valido[2] = {false,false};

    while((pos[0] + pos[1]) < (passo * 2)){
        for (int i = 0; i < 2; i++){
            if ((pos[i] < passo) && (not valido[i])){
                lerUmDado<pacote>(&auxEntrada[i],&pacotes[i],tamanhoInformacao) ? valido[i] = true : pos[i] = passo;
            }
        }

        if (valido[0] and valido[1]){
            intercalou = true;

            if(pacotes[0].tamanho > pacotes[1].tamanho){
                escreveUmDado<pacote>(&auxSaida[saida], pacotes[0], tamanhoInformacao);
                valido[0] = false;
                pos[0] ++;
            }else if((pacotes[0].tamanho == pacotes[1].tamanho) && (pacotes[0].tempo > pacotes[1].tempo)){
                escreveUmDado<pacote>(&auxSaida[saida], pacotes[0], tamanhoInformacao);
                valido[0] = false;
                pos[0] ++;
            }else if( (pacotes[0].tamanho == pacotes[1].tamanho) && (pacotes[0].tempo < pacotes[1].tempo)){
                escreveUmDado<pacote>(&auxSaida[saida], pacotes[1], tamanhoInformacao);
                valido[1] = false;
                pos[1] ++;
            }else{
                escreveUmDado<pacote>(&auxSaida[saida], pacotes[1], tamanhoInformacao);
                valido[1] = false;
                pos[1] ++;
            }

        }else if(valido[0]){
            intercalou = true;
            auxSaida[saida].write((const char*)(&pacotes[0]), tamanhoInformacao);
            valido[0] = false;
            pos[0] ++;
        }else if(valido[1]){
            intercalou = true;
            auxSaida[saida].write((const char *)(&pacotes[1]), tamanhoInformacao);
            valido[1] = false;
            pos[1]++;
        }
    }
    return intercalou;
}
#endif