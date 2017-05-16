#include "pacote.h"

// Variavel utilizada para debug
#define DEBUG 0

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

  //TODO: funcao de impressao

  #if EXEMPLO == 0
  /* Polinomio gerador do CRC32:
   *
   * POL: x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1
   * HEX: 0x04c11db7
   */
  static const unsigned char gerador[4] = {
    0x04, 0xc1, 0x1d, 0xb7
  };
  // Impressao do polinomio gerador
  printf("Generator ponynomial: g(x) = x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x + 1\n");
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
  while (i < 520) {
      printf("0x%x ", pkt6904[i]);
      i++;
  }
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
  unsigned char encoding[TAM_PACOTE+3];
  // Nibbles (4 bits cada nibble)
  unsigned short int lsnibble;
  unsigned short int msnibble;
  unsigned short int mmsnibble_proximo;

  // Impressao do encoding
  printf("Encoding [Part I]: i(x) << 2 (byte to byte) = ");
  i = 0;
  // Preenche o encoding deslocando o i(x) por 2 bits 0
  while (i < TAM_PACOTE) {

    #if DEBUG == 1
    printf("Endereco\n");
    printf("aux = %p\n", aux);
    printf("pkt = %p\n", (void*)&pkt6904[i]);

    printf("Conteudo\n");
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
    deslocamento = (unsigned int)deslocamento << 2;
    printf("0x%x ", deslocamento);

    // Separacao em nibbles
    // Nibble menos significativo = 0x0n
    lsnibble = deslocamento & 0x0F;
    #if DEBUG == 3
    printf("(LSN) %x\t", lsnibble);
    #endif
    // Nibble mais significativo = 0xn0
    msnibble = deslocamento & 0xF0;
    msnibble >>= 4;
    #if DEBUG == 3
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
    deslocamento_proximo = (unsigned int)deslocamento_proximo << 2;
    #if DEBUG == 2
    printf("(<<2) %x\t", deslocamento_proximo);
    #endif

    // Separacao em nibbles
    // Nibble mais mais significativo = 0xn00
    mmsnibble_proximo = deslocamento_proximo & 0xF00;
    mmsnibble_proximo >>= 8;
    #if DEBUG == 3
    printf("(MMSN) %x\n", mmsnibble_proximo);
    #endif

    // Construcao do novo vetor
    #if DEBUG == 3
    printf("Enconding\n");
    printf("MS %x\t", msnibble << 4);
    printf("LS %x\t", lsnibble);
    printf("MMS_P %x\t", mmsnibble_proximo);
    printf("LS||MMS_P %x\n", (lsnibble) | (mmsnibble_proximo));
    #endif

    encoding[i] = (msnibble << 4) | (lsnibble) | (mmsnibble_proximo);
    #if DEBUG == 3
    printf("%x\n", encoding[i]);
    #endif

    // Loop
    i++;
  }
  // Preenche os 3 ultimos bytes com 0
  while (i < TAM_PACOTE+3) {
    // Loop
    encoding[i] = 0;
    i++;
  }

  // Impressao do encoding
  printf("\n\nEncoding [Part II]: i(x) << 24 (3 bytes) = ");
  i = 0;
  while (i < TAM_PACOTE+3) {
    printf("0x%x ", encoding[i]);
    // Loop
    i++;
  }
  printf("\n\n");

  /*
   * Divisao
   */


  //TODO: xor byte a byte do pacote com o gerador
  //int contador = 0;
  //while(contador <= SHIFTS){
    // XOR bit a bit
    //resultado = resultado ^ gerador_shift;

    // Impressao
    //printf("\n\t%X\n", gerador_shift);
    //printf("\t________");
    //printf("\n\t%X\n", resultado);

    // Se eh multiplo, para
    //if(resultado == 0){
      //break;
    //}

    // Desloca o gerador para a direita
    //gerador_shift = gerador_shift >> 1;
    //contador++;
  //}

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
  // x3 i(x) = x6 + x5
  // Acrescenta 3 bits 0 no fim do i(x)
  unsigned char encoding = *info;
  encoding <<= 3;
  printf("\nEncoding: x^3*i(x) = 0x%x\n", encoding);

  /*
   * Divisao
   */


  #endif

  return 0;
}
