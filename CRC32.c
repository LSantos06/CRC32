#include "pacote.h"

// Variavel utilizada para debug
#define DEBUG 0
// 0 => desativado
// 100 => imprime o deslocamento do gerador
// 101 => xor bit a bit

// Variavel utilizada para rodar o exemplo do livro
#define EXEMPLO 0

/*
 *  Projeto 1
 *  Teleinformática e Redes 1
 *  1/2017
 *
 *  Eduardo Scartezini 14/01
 *  Lucas Santos 14/0151010
 *
 *  Calculo do campo FCS de um pacote.
 *
 *  Entradas: pacote inteiro = q(x)
 *  Saidas: campo CRC do q(x)
 *  Testes: A verificacao da corretude eh feita por meio da comparacao do resultado
 * obtido com o calculo feito pelo Wireshark.
 *
 */
int main(int argc, char * argv[]){

  #if EXEMPLO == 0
  /* Polinomio gerador do CRC32:
   *
   * POL: x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
   * HEX: 0x04c11db7
   */
  unsigned char gerador[4] = {
    0x04, 0xc1, 0x1d, 0xb7
  };
  // Impressao do polinomio gerador
  printf("Generator ponynomial: g(x) = x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1\n\n");
  printf("Generator ponynomial: g(x) = 0x");
  int i = 0;
  while (i < 4) {
      printf("%x", gerador[i]);
      i++;
  }
  printf("\n\n");

  /*
   * Pacote (informacao)
   */
  // Impressao da informacao
  printf("Information: i(x) = ");
  i = 0;
  while (i < TAM_PACOTE) {
      printf("0x%x ", pkt6904[i]);
      i++;
  }
  #if DEBUG == 5
  printf("\n TAM i(x) %d\n", i);
  #endif
  printf("\n\n");

  /*
   * Calculo do enconding
   */
  // Multiplica a info por x26 (maior grau do polinomio gerador)
  // Acrescenta 26 bits 0 no fim do i(x)

  // Ponteiro para o endereco de memoria inicial do i(x) (1 byte)
  unsigned char *aux = pkt6904;
  // Representacao do deslocamento (4 bytes)
  unsigned int deslocamento;
  unsigned int deslocamento_proximo;
  // Vetor encoding (1 byte cada elemento), 3 bytes a mais para o deslocamento
  unsigned short int encoding[TAM_PACOTE+4];
  // Nibbles (4 bits cada nibble)
  unsigned short int lsnibble;
  unsigned short int msnibble;
  unsigned short int mmsnibble;
  unsigned short int mmsnibble_proximo;

  // Impressao do encoding
  printf("Encoding [Part I]: i(x) << 2 (byte to byte) = ");

  // Primeiro elemento
  deslocamento = *aux;
  deslocamento <<= 2;
  #if DEBUG == 2
  printf("%x\t", deslocamento);
  #endif
  // Nibble mais mais significativo = 0xn00
  mmsnibble = deslocamento & 0xF00;
  mmsnibble >>= 8;
  #if DEBUG == 2
  printf("\nDeslocamento Inicial\n");
  printf("(MMSN) %x\n", mmsnibble);
  #endif

  // Preenche o primeiro byte
  encoding[0] = (mmsnibble);
  i = 1;

  #if DEBUG == 2
  printf("\nencoding[0] %x\n", encoding[0]);
  #endif

  // Preenche o encoding deslocando o i(x) por 2 bits 0
  while (i < TAM_PACOTE+1) {

    #if DEBUG == 1
    printf("\nEndereco\n");
    printf("aux = %p\n", aux);
    printf("pkt = %p\n", (void*)&pkt6904[i]);

    printf("\nConteudo\n");
    printf("aux = %x\n", *aux);
    printf("pkt = %x\n", pkt6904[i]);
    #endif

    #if DEBUG == 2
    // Byte atual do i(x)
    printf("\nDeslocamento\n");
    printf("%x\t", *aux);
    #endif

    // Deslocamento armazena o byte atual do i(x)
    deslocamento = *aux;
    #if DEBUG == 2
    printf("%x\t", deslocamento);
    #endif

    // Deslocamento do byte atual por 2 bits
    deslocamento <<= 2;
    printf("0x%x ", deslocamento);

    // Separacao em nibbles
    // Nibble menos significativo = 0x0n
    lsnibble = deslocamento & 0x0F;
    #if DEBUG == 2
    printf("(LSN) %x\t", lsnibble);
    #endif
    // Nibble mais significativo = 0xn0
    msnibble = deslocamento & 0xF0;
    msnibble >>= 4;
    #if DEBUG == 2
    printf("(MSN) %x\n", msnibble);
    #endif

    // Vai para o proximo elemento do i(x)
    aux++;

    #if DEBUG == 2
    // Byte atual do i(x)
    printf("Deslocamento Proximo\n");
    printf("%x\t", *aux);
    #endif

    // Deslocamento_proximo armazena o proximo byte do i(x)
    deslocamento_proximo = *aux;
    #if DEBUG == 2
    printf("%x\t", deslocamento_proximo);
    #endif

    // Deslocamento do proximo byte por 2 bits
    deslocamento_proximo <<= 2;
    #if DEBUG == 2
    printf("(<<2) %x\t", deslocamento_proximo);
    #endif

    // Separacao em nibbles
    // Nibble mais mais significativo = 0xn00
    mmsnibble_proximo = deslocamento_proximo & 0xF00;
    mmsnibble_proximo >>= 8;
    #if DEBUG == 2
    printf("(MMSN) %x\n", mmsnibble_proximo);
    #endif

    // Construcao do novo vetor
    #if DEBUG == 2
    printf("Enconding\n");
    printf("MS %x\t", msnibble << 4);
    printf("LS %x\t", lsnibble);
    printf("MMS_P %x\t", mmsnibble_proximo);
    printf("LS||MMS_P %x\n", (lsnibble) | (mmsnibble_proximo));
    #endif

    encoding[i] = (msnibble << 4) | (lsnibble) | (mmsnibble_proximo);
    #if DEBUG == 2
    printf("%x\n", encoding[i]);
    #endif

    // Loop
    i++;
  }
  // Preenche os 3 ultimos bytes com 0
  while (i < TAM_PACOTE+4) {
    // Loop
    encoding[i] = 0;
    i++;
  }

  // Impressao do encoding
  printf("\n\nEncoding [Part II]: alignment of i(x) AND i(x) << (3 bytes) = ");
  i = 0;
  while (i < TAM_PACOTE+4) {
    printf("0x%x ", encoding[i]);
    // Loop
    i++;
  }
  #if DEBUG == 5
  printf("\n TAM encoding %d\n", i);
  #endif
  printf("\n\n");

  /*
   * Divisao
   */
  // Grau inicial do encoding
  int grau_encoding = ((TAM_PACOTE)*8)+26;

  #if DEBUG == 3
  printf("grau inicial encoding = %d\n", grau_encoding);
  #endif

  // Loop para avaliacao do grau do encoding
  i = 0;
  do {
    #if DEBUG == 3
    printf("encoding[%d] = %x\n", i, encoding[i]);
    #endif

    // Pega o msnibble
    msnibble = encoding[i] & 0xF0;
    msnibble >>= 4;
    #if DEBUG == 3
    printf("MSN = %x\n", msnibble);
    #endif

    // MS nibble nao eh 0
    if(msnibble != 0){
      // MS nibble = 0001
      if(msnibble == 1){
        grau_encoding -= 3;
        break;
      }
      // MS nibble = 001x
      else if(msnibble == 2 || msnibble == 3){
        grau_encoding -= 2;
        break;
      }
      // MS nibble = 01xx
      else if(msnibble == 4 || msnibble == 5 || msnibble == 6 || msnibble == 7){
        grau_encoding -= 1;
        break;
      }
      // MS nibble = 1xxx
    }

    // Pega o lsnibble
    lsnibble = encoding[i] & 0x0F;
    #if DEBUG == 3
    printf("LSN = %x\n", lsnibble);
    #endif

    // LS nibble nao eh 0
    if(lsnibble != 0){
      // LS nibble = 0001
      if(lsnibble == 1){
        grau_encoding -= (3+4);
        break;
      }
      // LS nibble = 001x
      else if(lsnibble == 2 || lsnibble == 3){
        grau_encoding -= (2+4);
        break;
      }
      // LS nibble = 01xx
      else if(lsnibble == 4 || lsnibble == 5 || lsnibble == 6 || lsnibble == 7){
        grau_encoding -= (1+4);
        break;
      }
      // LS nibble = 1xxx
    }

    // Loop
    grau_encoding-=8;
    i++;

  } while (encoding[i] == 0);

  //printf("Degree of encoding = %d\n\n", grau_encoding);

  // Gerador possui grau x26, para ter xn, eh necessario deslocar de n-26
  unsigned int n_deslocamentos_gerador = grau_encoding - 26;
  #if DEBUG == 4
  printf("numero deslocamentos no gerador = %d\n", n_deslocamentos_gerador);
  #endif

  // Numero de bytes 0 a serem incluidos no fim
  unsigned int n_bytes_0 = n_deslocamentos_gerador / 8;
  #if DEBUG == 4
  printf("numero bytes 0 no fim = %d\n", n_bytes_0);
  #endif

  //TODO: Genralizacao
  // Numero de bits a serem deslocados dentro do g(x)
  unsigned int n_bits = 3;
  #if DEBUG == 4
  printf("numero bits um a um = %d\n\n", n_bits);
  #endif

  // Ponteiro para endereco de memoria inicial do gerador
  unsigned char *aux_gerador = gerador;
  // Representacao do deslocamento (4 bytes)
  unsigned int deslocamento_gerador;
  unsigned int deslocamento_proximo_gerador;
  // Vetor gerador (1 byte cada elemento), n_bytes_0 bytes a mais para o deslocamento
  unsigned short int gerador_deslocado[(4+1)+n_bytes_0];
  // Nibbles (4 bits cada nibble)
  unsigned short int gsnibble;
  unsigned short int gsnibble_proximo;

  // Impressao do gerador
  printf("g(x) [Part I]: i(x) << %d (byte to byte) = ", n_bits);

  // Primeiro elemento
  deslocamento_gerador = *aux_gerador;
  deslocamento_gerador <<= n_bits;
  #if DEBUG == 7
  printf("%x\t", deslocamento_gerador);
  #endif
  // Nibble mais mais significativo = 0xn00
  mmsnibble = deslocamento_gerador & 0xF00;
  mmsnibble >>= 8;
  #if DEBUG == 7
  printf("\nDeslocamento Inicial\n");
  printf("(MMSN) %x\n", mmsnibble);
  #endif
  // Nibble greatest significativo = 0xn000
  gsnibble = deslocamento_gerador & 0xF000;
  gsnibble >>= 12;
  #if DEBUG == 7
  printf("(GSN) %x\t", gsnibble);
  #endif

  // Preenche o primeiro byte
  gerador_deslocado[0] = (gsnibble << 4) | (mmsnibble);
  i = 1;

  #if DEBUG == 7
  printf("\ngerador_deslocado[0] %x\n", gerador_deslocado[0]);
  #endif

  // Preenche o gerador_deslocado deslocando o g(x) por n_bits 0
  while (i < (4+1)) {

    #if DEBUG == 6
    printf("Endereco\n");
    printf("aux = %p\n", aux_gerador);
    printf("ger = %p\n", (void*)&gerador[i]);

    printf("Conteudo\n");
    printf("aux = %x\n", *aux_gerador);
    printf("ger = %x\n", gerador[i]);
    #endif

    #if DEBUG == 7
    // Byte atual do i(x)
    printf("\nDeslocamento\n");
    printf("%x\t", *aux_gerador);
    #endif

    // Deslocamento armazena o byte atual do i(x)
    deslocamento_gerador = *aux_gerador;
    #if DEBUG == 7
    printf("%x\t", deslocamento_gerador);
    #endif

    // Deslocamento do byte atual por n_bits bits
    deslocamento_gerador <<= n_bits;
    printf("0x%x ", deslocamento_gerador);

    // Separacao em nibbles
    // Nibble menos significativo = 0x0n
    lsnibble = deslocamento_gerador & 0x0F;
    #if DEBUG == 7
    printf("(LSN) %x\t", lsnibble);
    #endif
    // Nibble mais significativo = 0xn0
    msnibble = deslocamento_gerador & 0xF0;
    msnibble >>= 4;
    #if DEBUG == 7
    printf("(MSN) %x\t", msnibble);
    #endif
    // Nibble mais mais significativo = 0xn00
    mmsnibble = deslocamento_gerador & 0xF00;
    mmsnibble >>= 8;
    #if DEBUG == 7
    printf("(MMSN) %x\n", mmsnibble);
    #endif

    // Vai para o proximo elemento do g(x)
    aux_gerador++;

    #if DEBUG == 7
    // Byte atual do i(x)
    printf("Deslocamento Proximo\n");
    printf("%x\t", *aux_gerador);
    #endif

    // Deslocamento_proximo armazena o proximo byte do g(x)
    deslocamento_proximo_gerador = *aux_gerador;
    #if DEBUG == 7
    printf("%x\t", deslocamento_proximo_gerador);
    #endif

    // Deslocamento do proximo byte por n_bits bits
    deslocamento_proximo_gerador <<= n_bits;
    #if DEBUG == 7
    printf("(<<6) %x\t", deslocamento_proximo_gerador);
    #endif

    // Separacao em nibbles
    // Nibble greatest significativo = 0xn000
    gsnibble_proximo = deslocamento_proximo_gerador & 0xF000;
    gsnibble_proximo >>= 12;
    #if DEBUG == 7
    printf("(GSN) %x\t", gsnibble_proximo);
    #endif
    // Nibble mais mais significativo = 0xn00
    mmsnibble_proximo = deslocamento_proximo_gerador & 0xF00;
    mmsnibble_proximo >>= 8;
    #if DEBUG == 7
    printf("(MMSN) %x\n", mmsnibble_proximo);
    #endif

    // Construcao do novo vetor
    #if DEBUG == 7
    printf("Alinhamento\n");
    printf("MS %x\t", msnibble << 4);
    printf("GS_P %x\t", gsnibble_proximo << 4);
    printf("LS %x\t", lsnibble);
    printf("MMS_P %x\t", mmsnibble_proximo);
    printf("MS||GS_P %x\t", (msnibble << 4) | (gsnibble_proximo << 4));
    printf("LS||MMS_P %x\n", (lsnibble) | (mmsnibble_proximo));
    #endif

    gerador_deslocado[i] = (msnibble << 4) | (gsnibble_proximo << 4) | (lsnibble) | (mmsnibble_proximo);
    #if DEBUG == 7
    printf("%x\n", gerador_deslocado[i]);
    #endif

    // Loop
    i++;
  }
  // Preenche os n_bytes_0 ultimos bytes com 0
  while (i < (4+1)+n_bytes_0) {
    // Loop
    gerador_deslocado[i] = 0;
    i++;
  }

  // Impressao do gerador deslocado
  printf("\n\ng(x) [Part II]: alignment of g(x) AND g(x) << (%d bytes) = ", n_bytes_0);
  i = 0;
  while (i < (4+1)+n_bytes_0) {
    printf("0x%x ", gerador_deslocado[i]);
    // Loop
    i++;
  }
  #if DEBUG == 5
  printf("\n TAM g(x) %d\n", i);
  #endif
  printf("\n\n");

  // Divisao em si
  //printf("Division: ");

  // Enquanto o byte 4+n_bytes_0 do resultado for diferente do ultimo byte do gerador
  // printf("%x\n", gerador_deslocado[4+n_bytes_0]);
  // printf("%x\n", gerador[3]);

  // Ponteiro para endereco de memoria inicial do gerador deslocado
  unsigned short int *aux_gerador_deslocado = gerador_deslocado;
  unsigned short int *aux_gerador_deslocado_anterior = gerador_deslocado;
  // Representacao do deslocamento (4 bytes)
  unsigned int deslocamento_gerador_anterior;
  // Bits
  unsigned char lsbit_byte_anterior; // Ex: 0x04, 0xc1, ... => 0000 010(0) => 0;
  unsigned char msnibble_lsb;        // => 110(0) 0001 => 0
                                     // => 1100   000(1) => 1 (proximo lsbit_byte_anterior)
                                     // Aplica o deslocamento
  unsigned char msnibble_ms3b;       // => (x)110 => 6; x = lsbit_byte_anterior
  unsigned char lsnibble_ms3b;       // => (x)000 => 0; x = msnibble_lsb
                                     // loop
  // Bits de comparacao
  unsigned char bit7_g;
  unsigned char bit6_g;
  unsigned char bit5_g;
  unsigned char bit4_g;
  unsigned char bit3_g;
  unsigned char bit2_g;
  unsigned char bit1_g;
  unsigned char bit0_g;

  unsigned char bit7_e;
  unsigned char bit6_e;
  unsigned char bit5_e;
  unsigned char bit4_e;
  unsigned char bit3_e;
  unsigned char bit2_e;
  unsigned char bit1_e;
  unsigned char bit0_e;

  // Vetor de resultado
  unsigned short int resultado[(4+1)+n_bytes_0];

  // Controle do xor
  int flag1 = 0;
  int do_xor = 0;

  // Clausulas do xor
  int clausula_7;
  int clausula_6;
  int clausula_5;
  int clausula_4;
  int clausula_3;
  int clausula_2;
  int clausula_1;
  int clausula_0;

  int contador = 0;
  // Enquanto o ultimo byte do gerador_deslocado for diferente de 0xb7
  int iteracoes = 0;
  while(resultado[3+n_bytes_0]!=0xb7){

    flag1 = 0;
    do_xor = 0;

    // Impressao do gerador deslocado
    #if DEBUG == 100
    printf("\n\nShifted g(x) [Part I]: i(x) >> 1 (byte to byte) = \n");
    #endif

    // Xor bit a bit
    // printf("INIT\n");
    contador = 0;
    while (contador < TAM_PACOTE+4) {
      if(gerador_deslocado[contador]!=0){
        #if DEBUG == 101
          printf("Iteracao%d: Encoding %x ", iteracoes, encoding[contador]);
        #endif
      }

      // if x000 == x000 || 0x00 == 0x00 || 00x0 == 00x0 || 000x == 000x (byte a byte)
      //   xor
      // else
      //   desloca o gerador

      // Pega o primeiro byte diferente de 0 do gerador_deslocado
      if(gerador_deslocado[contador]!=0 && flag1 == 0){
        flag1 = 1;

        bit7_g = (gerador_deslocado[contador] & 0x80) >> 7;
        bit6_g = (gerador_deslocado[contador] & 0x40) >> 6;
        bit5_g = (gerador_deslocado[contador] & 0x20) >> 5;
        bit4_g = (gerador_deslocado[contador] & 0x10) >> 4;
        bit3_g = (gerador_deslocado[contador] & 0x8) >> 3;
        bit2_g = (gerador_deslocado[contador] & 0x4) >> 2;
        bit1_g = (gerador_deslocado[contador] & 0x2) >> 1;
        bit0_g = (gerador_deslocado[contador] & 0x1);

        bit7_e = (encoding[contador] & 0x80) >> 7;
        bit6_e = (encoding[contador] & 0x40) >> 6;
        bit5_e = (encoding[contador] & 0x20) >> 5;
        bit4_e = (encoding[contador] & 0x10) >> 4;
        bit3_e = (encoding[contador] & 0x8) >> 3;
        bit2_e = (encoding[contador] & 0x4) >> 2;
        bit1_e = (encoding[contador] & 0x2) >> 1;
        bit0_e = (encoding[contador] & 0x1);

        #if DEBUG == 101
          printf("\nBITS ENCODING: ");
          printf("%x ", bit7_e);
          printf("%x ", bit6_e);
          printf("%x ", bit5_e);
          printf("%x ", bit4_e);
          printf("%x ", bit3_e);
          printf("%x ", bit2_e);
          printf("%x ", bit1_e);
          printf("%x\n", bit0_e);

          printf("BITS GERADOR:  ");
          printf("%x ", bit7_g);
          printf("%x ", bit6_g);
          printf("%x ", bit5_g);
          printf("%x ", bit4_g);
          printf("%x ", bit3_g);
          printf("%x ", bit2_g);
          printf("%x ", bit1_g);
          printf("%x\n", bit0_g);
        #endif

        // Clausulas
        clausula_7 = (bit7_e == 0 && bit7_g == 0);
        clausula_6 = (bit6_e == 0 && bit6_g == 0);
        clausula_5 = (bit5_e == 0 && bit5_g == 0);
        clausula_4 = (bit4_e == 0 && bit4_g == 0);
        clausula_3 = (bit3_e == 0 && bit3_g == 0);
        clausula_2 = (bit2_e == 0 && bit2_g == 0);
        clausula_1 = (bit1_e == 0 && bit1_g == 0);
        clausula_0 = (bit0_e == 0 && bit0_g == 0);

        #if DEBUG == 102
          printf("CLAUSULAS:  ");
          printf("%d ", clausula_7);
          printf("%d ", clausula_6);
          printf("%d ", clausula_5);
          printf("%d ", clausula_4);
          printf("%d ", clausula_3);
          printf("%d ", clausula_2);
          printf("%d ", clausula_1);
          printf("%d\n", clausula_0);
        #endif

        // Xor em si
        if(bit7_e == 1 && bit7_g == 1){
          #if DEBUG == 102
            printf("DO XOR 7\n");
          #endif
          do_xor = 1;
          encoding[contador] ^= gerador_deslocado[contador];
        }
        else if(clausula_7 && (bit6_e == 1 && bit6_g == 1)){
          #if DEBUG == 102
            printf("DO XOR 6\n");
          #endif
          do_xor = 1;
          encoding[contador] ^= gerador_deslocado[contador];
        }
        else if(clausula_7 && clausula_6 && (bit5_e == 1 && bit5_g == 1)){
          #if DEBUG == 102
            printf("DO XOR 5\n");
          #endif
          do_xor = 1;
          encoding[contador] ^= gerador_deslocado[contador];
        }
        else if(clausula_7 && clausula_6 && clausula_5 && (bit4_e == 1 && bit4_g == 1)){
          #if DEBUG == 102
            printf("DO XOR 4\n");
          #endif
          do_xor = 1;
          encoding[contador] ^= gerador_deslocado[contador];
        }
        else if(clausula_7 && clausula_6 && clausula_5 && clausula_4 && (bit3_e == 1 && bit3_g == 1)){
          #if DEBUG == 102
            printf("DO XOR 3\n");
          #endif
          do_xor = 1;
          encoding[contador] ^= gerador_deslocado[contador];
        }
        else if(clausula_7 && clausula_6 && clausula_5 && clausula_4 && clausula_3 && (bit2_e == 1 && bit2_g == 1)){
          #if DEBUG == 102
            printf("DO XOR 2\n");
          #endif
          do_xor = 1;
          encoding[contador] ^= gerador_deslocado[contador];
        }
        else if(clausula_7 && clausula_6 && clausula_5 && clausula_4 && clausula_3 && clausula_2 && (bit1_e == 1 && bit1_g == 1)){
          #if DEBUG == 102
            printf("DO XOR 1\n");
          #endif
          do_xor = 1;
          encoding[contador] ^= gerador_deslocado[contador];
        }
        else if(clausula_7 && clausula_6 && clausula_5 && clausula_4 && clausula_3 && clausula_2 && clausula_1 && (bit0_e == 1 && bit0_g == 1)){
          #if DEBUG == 102
            printf("DO XOR 0\n");
          #endif
          do_xor = 1;
          encoding[contador] ^= gerador_deslocado[contador];
        }
        else{
          #if DEBUG == 102
            printf("DO NOT XOR\n");
          #endif
        }
      }

      else if(do_xor == 1){
        encoding[contador] ^= gerador_deslocado[contador];
      }

      if(gerador_deslocado[contador]!=0){
        #if DEBUG == 101
          printf("Gerador %x ", gerador_deslocado[contador]);
          printf("Resultado Xor %x\n\n", encoding[contador]);
        #endif
      }

      contador++;
    }
    // printf("FIM\n");

    // Primeiro elemento
    deslocamento_gerador = *aux_gerador_deslocado;
    deslocamento_gerador_anterior = *aux_gerador_deslocado_anterior;

    // Bit menos significativo do nibble mais significativo
    msnibble_lsb = deslocamento_gerador & 0x10;
    msnibble_lsb >>=4;
    #if DEBUG == 8
    printf("\nDeslocamento Inicial\n");
    printf("(msnibble_lsb) %x\t", msnibble_lsb);
    #endif

    //// Desloca o gerador_deslocado >> 1
    deslocamento_gerador >>= 1;
    #if DEBUG == 8
    printf("DESLOC %x\n", deslocamento_gerador);
    #endif

    // 3 bits mais significativos do nibble mais significativo deslocado
    msnibble_ms3b = deslocamento_gerador & 0x70;
    msnibble_ms3b >>= 4;
    #if DEBUG == 8
    printf("(msnibble_ms2b) %x\t", msnibble_ms2b);
    #endif

    // 3 bits mais significativos do nibble menos significativo deslocado
    lsnibble_ms3b = deslocamento_gerador & 0x7;
    #if DEBUG == 8
    printf("(lsnibble_ms2b) %x\n", lsnibble_ms2b);
    #endif

    // Preenche o primeiro byte
    resultado[0] = 0 | (msnibble_ms3b << 4) | (msnibble_lsb << 3) | (lsnibble_ms3b);

    #if DEBUG == 8
    printf("gerador_deslocado[0] %x\n", gerador_deslocado[0]);
    #endif

    // Entrada no loop
    aux_gerador_deslocado++;

    i = 0;
    // Desloca todos os bytes do g(x) >> 1
    while (i < (4)+n_bytes_0) {

      #if DEBUG == 11
      //printf("\nEndereco\n");
      //printf("aux = %p\n", aux_gerador_deslocado);
      //printf("aux_anterior = %p\n", aux_gerador_deslocado_anterior);
      //printf("sg(x) = %p\n", (void*)&gerador_deslocado[i]);

      // printf("Conteudo\n");
      // printf("aux = %x\n", *aux_gerador_deslocado);
      // printf("aux_anterior = %x\n", *aux_gerador_deslocado_anterior);
      // printf("sg(x) = %x\n", gerador_deslocado[i]);
      #endif

      // Deslocamento armazena o byte atualdo gerador_deslocado
      deslocamento_gerador_anterior = *aux_gerador_deslocado_anterior;
      deslocamento_gerador = *aux_gerador_deslocado;

      #if DEBUG == 11
      printf("\n\nBYTE ANT %x\n", *aux_gerador_deslocado_anterior);
      printf("BYTE %x\n", *aux_gerador_deslocado);
      #endif

      // Bit menos significativo do nibble mais significativo ANTERIOR
      lsbit_byte_anterior = deslocamento_gerador_anterior & 0x1;
      #if DEBUG == 11
      printf("Deslocamento\n");
      printf("(lsbit_byte_anterior) %x\t", lsbit_byte_anterior);
      #endif
      // Bit menos significativo do nibble mais significativo ATUAL
      msnibble_lsb = deslocamento_gerador & 0x10;
      msnibble_lsb >>=4;
      #if DEBUG == 11
      printf("(msnibble_lsb) %x\t", msnibble_lsb);
      #endif

      // Desloca o gerador_deslocado >> 1
      deslocamento_gerador >>= 1;
      #if DEBUG == 11
      printf("DESLOC %x\n", deslocamento_gerador);
      #endif

      // 3 bits mais significativos do nibble mais significativo deslocado
      msnibble_ms3b = deslocamento_gerador & 0x70;
      msnibble_ms3b >>= 4;
      #if DEBUG == 11
      printf("(msnibble_ms2b) %x\t", msnibble_ms3b);
      #endif

      // 3 bits mais significativos do nibble menos significativo deslocado
      lsnibble_ms3b = deslocamento_gerador & 0x7;
      #if DEBUG == 11
      printf("(lsnibble_ms2b) %x\n", lsnibble_ms3b);
      #endif

      // Preenche o primeiro byte
      resultado[i+1] = (lsbit_byte_anterior << 7) | (msnibble_ms3b << 4) | (msnibble_lsb << 3) | (lsnibble_ms3b);

      #if DEBUG == 100
      printf("0x%x ", resultado[i]);
      #endif

      // Vai para o proximo byte do gerador_deslocado
      aux_gerador_deslocado_anterior++;
      aux_gerador_deslocado++;
      i++;
    }
    #if DEBUG == 100
      printf("\n");
    #endif

    i = 0;
    // Salva resultado no g(x)
    while (i < (4)+n_bytes_0) {
      gerador_deslocado[i] = resultado[i];
      i++;
    }

    // Voltar para o comeco do g(x)
    aux_gerador_deslocado = gerador_deslocado;
    aux_gerador_deslocado_anterior = gerador_deslocado;

    // Impressao do campo CRC
    #if DEBUG == 101
      printf("\nCRC Field: r(x) = ");
      contador = 0;
      while (contador < TAM_PACOTE+3) {
        printf("0x%x ", encoding[contador]);
        contador++;
      }
      printf("\n");
    #endif

    // Loop
    iteracoes++;
  }

  // Impressao do campo CRC
  #if DEBUG == 0
    printf("CRC Field: r(x) = ");
    contador = 0;
    while (contador < TAM_PACOTE+4) {
      if(encoding[contador] != 0){
        printf("0x%x ", encoding[contador]);
      }
      contador++;
    }
    printf("\n");
  #endif

  #else
  /* Polinomio gerador do exemplo do livro:
   *
   * POL: x3 + x + 1
   * HEX: 0xb
   */
  static const unsigned char gerador[1] = {
    0x0b
  };
  // Impressao do polinomio gerador
  printf("Generator ponynomial: g(x) = x3 + x + 1\n");
  printf("Generator ponynomial: g(x) = ");
  printf("0x%x\n", gerador[0]);

  /*
   * Pacote (informacao)
   *
   * POL: x3 + x2
   * HEX: 0xc
   */
  static const unsigned char info[1] = {
   0x0c
  };
  // Impressao da informacao
  printf("\nInformation: i(x) = ");
  printf("0x%x\n", info[0]);

  /*
   * Calculo do enconding
   */
  // Multiplica a info por x3 (maior grau do polinomio gerador)
  // x3*i(x) = x6 + x5
  // Acrescenta 3 bits 0 no fim do i(x)
  unsigned char encoding = *info;
  encoding <<= 3;
  printf("\nEncoding: x^3*i(x) = 0x%x\n", encoding);

  /*
   * Divisao
   */
  // Desloca o gerador para o grau mais alto da info (x6)
  unsigned char gerador_deslocado = *gerador;
  // Gerador possui grau x3, para ter x6, eh necessario deslocar de 6-3 = 3
  gerador_deslocado <<= 3;
  // Impressao do gerador deslocado
  printf("\nShifted g(x): 0x");
  printf("%x\n", gerador_deslocado);

  // Divisao em si
  printf("\nDivision:");
  int i = 0;
  while (gerador_deslocado != gerador[0]) {
    printf("\n0x%x\n", encoding);
    printf("0x%x\n", gerador_deslocado);
    printf("----\n");
    // Xor bit a bit entre o gerador deslocado e o encoding
    encoding ^= gerador_deslocado;
    printf("0x%x\n", encoding);
    gerador_deslocado >>= 1;
    i++;
  }

  // Impressao do campo CRC
  printf("\nCRC Field: r(x) = ");
  printf("0x%x\n", encoding);
  printf("\n");

  #endif

  return 0;
}
