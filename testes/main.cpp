#include <iostream>
#include <cstring>
#include <fstream>
#include "../classes/cMergeExterno.h"
#include "../classes/ManipulacaoArquivosBin.h"

using namespace std;

int main(){
    MergeExterno ordExt("captura_pacotes.bin","output.bin");
    
    /*
        Mostrando os vinte primeiros dados para que possamos
        checar se a ordenação foi concluida com sucesso 
    */
    le_arquivo_bin<pacote>("captura_pacotes.bin", 20);
    le_arquivo_bin<pacote>("output.bin", 20);
  
    return 0;
}