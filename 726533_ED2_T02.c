/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 726533
 * Aluno: Gustavo Buoro Branco de Souza
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
#define TAM_STRING_INDICE (TAM_MARCA + TAM_NOME)


#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!\n"
#define INICIO_BUSCA 				"********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM				"********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 			"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO				"********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO      "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO    "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 			"Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 			"Busca por %s.\nNos percorridos:\n"


/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um nó do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000*sizeof(Chave_ip)];
char ARQUIVO_IS[2000*sizeof(Chave_is)];
/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;

/* ==========================================================================
* ========================= PROTÓTIPOS DAS FUNÇÕES =========================
* ========================================================================== */

void gerarChave(Produto *p);

/********************FUNÇÕES DO MENU*********************/
void cadastrar(Indice* iprimary, Indice* ibrand);

int alterar(Indice iprimary);

void buscar(Indice iprimary,Indice ibrand);

void insere (Indice* idx, void *chave, char marca_nome[TAM_STRING_INDICE], char ip);

void insere_aux (int rrn, char* pk, void *chave_promovida, int *filho_direito, char ip);
void insere_aux_ip (int rrn, Chave_ip* chave_ip, Chave_ip *chave_promovida_ip, int *filho_direito);
void insere_aux_is (int rrn, char* pk, Chave_is *chave_promovida_is, int *filho_direito);

void divide_no_ip (node_Btree_ip *no_x, Chave_ip* chave_ip, Chave_ip *chave_promovida_ip, int *filho_direito);
void divide_no_is (node_Btree_is *no_x, char *pk, Chave_is *chave_promovida_is, int *filho_direito);

void listar(Indice iprimary,Indice ibrand);
void listar_ip (int rrn, int nivel);
void listar_is (int rrn, int nivel);

/*******************************************************/

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
* de registros. */
int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de produtos  */
void criar_ibrand(Indice *ibrand) ;

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);

void busca_pk (Indice iprimary, char *pk);
void busca_is (Indice ibrand, Indice iprimary, char *string);
int existe_ip (int rrn_no, char *pk);
int existe_is (int rrn_no, char *string);
int buscar_indices (int rrn_no, char *pk, char ip);
/*
*   Caro aluno,
*   caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*   void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*   void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/
 
/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto p, int seleciona);
 
/* Exibe o produto */
int exibir_registro(int rrn);

int main()
{
	char *p; /* # */
  /* Arquivo */
	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip * 3 + 4 + (-1 + ordem_ip) * 14;
	tamanho_registro_is = ordem_is * 3 + 4 + (-1 + ordem_is) * (TAM_STRING_INDICE + 9);

	/* Índice primário */
	Indice iprimary ;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Produtos */
	Indice ibrand;
	criar_ibrand(&ibrand);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&iprimary, &ibrand);
			break;
		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(iprimary, ibrand);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, ibrand);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");
				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}

/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if(rrn < 0)
		return 0;

	float preco;
	int desconto;
	Produto p;
    char *cat, categorias[TAM_CATEGORIA];
	
	printf("%s\n", p.pk);
	printf("%s\n", p.nome);
	printf("%s\n", p.marca);
	printf("%s\n", p.data);
	printf("%s\n", p.ano);
	sscanf(p.desconto,"%d",&desconto);
	sscanf(p.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strcpy(categorias, p.categoria);
	
	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");

	return 1;
}

void cadastrar(Indice* iprimary, Indice* ibrand)
{
	int tam_atual = 0, iterador = 0, achou;
	char *arq = ARQUIVO + 192 * nregistros;

	Produto p;
	scanf("%[^\n]", p.nome); getchar();
	scanf("%[^\n]", p.marca); getchar();
	scanf("%[^\n]", p.data); getchar();
	scanf("%[^\n]", p.ano); getchar();
	scanf("%[^\n]", p.preco); getchar();
	scanf("%[^\n]", p.desconto); getchar();
	scanf("%[^\n]", p.categoria);
	gerarChave(&p);


	if (achou = existe_ip (iprimary-> raiz, p.pk) != -1)
		printf (ERRO_PK_REPETIDA, p.pk);
	else
	{
		iterador = sprintf(arq, "%s@", p.pk);
		arq += iterador;
		tam_atual += iterador;
		iterador = sprintf(arq, "%s@", p.nome);
		arq += iterador;
		tam_atual += iterador;
		iterador = sprintf(arq, "%s@", p.marca);
		arq += iterador;
		tam_atual += iterador;
		iterador = sprintf(arq, "%s@", p.data);
		arq += iterador;
		tam_atual += iterador;
		iterador = sprintf(arq, "%s@", p.ano);
		arq += iterador;
		tam_atual += iterador;
		iterador = sprintf(arq, "%s@", p.preco);
		arq += iterador;
		tam_atual += iterador;
		iterador = sprintf(arq, "%s@", p.desconto);
		arq += iterador;
		tam_atual += iterador;
		iterador = sprintf(arq, "%s@", p.categoria);
		arq += iterador;
		tam_atual += iterador;

		for (int i = 0; i < (TAM_REGISTRO - tam_atual); i++, arq += 1)
			sprintf(arq, "#");
		inserir_registro_indices(iprimary, ibrand, p, 0);
		nregistros ++;
	}
	return;
}

void gerarChave(Produto *p)// Adiciona o campo pk[] a struct produto
{
	int i = 0;
	strncpy (p-> pk, p-> nome, 2);
	strncpy (p-> pk + 2, p-> marca, 2);
	strncpy (p-> pk + 4, p-> data, 2);
	p-> pk[6] = p-> data[3];
	p-> pk[7] = p-> data[4];
	strcpy (p-> pk + 8, p-> ano);
	p-> pk[10] = '\0';
	while (p-> pk[i]) //Passa tudo para maiúsculo, até o '\0'
	{
     	toupper(p-> pk[i]);
    	i++;
    }
}

void *criar_no(char ip)
{
	if (ip == 'p') // iprimary
	{
		node_Btree_ip *noip;
		noip = (node_Btree_ip *) malloc (sizeof (node_Btree_ip));
		noip-> chave = (Chave_ip *) malloc ((ordem_ip - 1) * sizeof (Chave_ip)); // possíveis nós filhos
		noip-> desc = (int *) malloc (ordem_ip * sizeof (int));
		noip-> num_chaves = 0;
		noip-> folha = 'F';
		for (int i = 0; i < ordem_ip; i++)
		{
			if (i != ordem_ip - 1)
				strcpy (noip-> chave[i].pk, "\0");
			noip-> desc[i] = -1;
		}
		return noip;
	}
	else if (ip == 's') // isec
	{
		node_Btree_is *nois;
		nois = (node_Btree_is *) malloc (sizeof (node_Btree_is));
		nois-> chave = (Chave_is *) malloc ((ordem_is - 1) * sizeof (Chave_is)); // possíveis nós filhos
		nois-> desc = (int *) malloc (ordem_is * sizeof (int));
		nois-> num_chaves = 0;
		nois-> folha = 'F';
		for (int i = 0; i < ordem_is; i++)
		{
			if (i != ordem_is - 1)
				strcpy (nois-> chave[i].pk, "\0");
			nois-> desc[i] = -1;
		}
		return nois;	
	}
}

void buscar(Indice iprimary, Indice ibrand) // Buscar do case: 3
{
	char pk[TAM_PRIMARY_KEY], marca[TAM_MARCA], nome[TAM_NOME];
	char string[TAM_STRING_INDICE];
	int op;

	scanf ("\n%d", &op);

	if (op == 1)
	{
		scanf ("\n%10s", pk);
		busca_pk (iprimary, pk);
	}
	else if (op == 2)
	{
		scanf ("\n%50[^\n]s", marca);
		scanf ("\n%50[^\n]s", nome);
		strcpy (string, marca);
		strcat (string, "$");
		strcat (string, nome);
		busca_is (ibrand, iprimary, string);
	}
	else
		printf(OPCAO_INVALIDA);
}

void busca_pk (Indice iprimary, char *pk)
{
	int comparacao, virgula = 0;
	Indice prox_iprimary;
	node_Btree_ip *registro_ip = read_btree(iprimary.raiz, 'p');
	
	if (existe_ip (iprimary.raiz, pk) != -1)
	{
		printf(NOS_PERCORRIDOS_IP, pk);
		for (int i = 0; i < registro_ip-> num_chaves; i++)
		{
			comparacao = strcmp(pk, registro_ip-> chave[i].pk);
			if (comparacao == 0) // achou
			{	
				printf("\n");
				exibir_registro(registro_ip-> chave[i].rrn);
				return;
			}	
			else
			{
				if (virgula == 0)
				{
					printf("%s", registro_ip-> chave[i].pk); // Imprime a chave que passou por
					virgula = 1;
				}
				else
					printf(", %s", registro_ip-> chave[i].pk);

				if (comparacao < 0) //Nó a esquerda
				{
					printf("\n");
					prox_iprimary.raiz = registro_ip-> desc[i];
					busca_pk (prox_iprimary, pk);
				}
				else if (comparacao > 0) //Nó a direita
				{
					printf("\n");
					prox_iprimary.raiz = registro_ip-> desc[i + 1];
					busca_pk (prox_iprimary, pk);
				}
			}
		}
	}
	else
		printf(REGISTRO_N_ENCONTRADO);
	return;
}

void busca_is (Indice ibrand, Indice iprimary, char *string)
{
	int achou = 0, comparacao, virgula = 0;
	Indice prox_ibranc;
	node_Btree_is *registro_is = read_btree(ibrand.raiz, 's');

	if (existe_ip (ibrand.raiz, string) != -1)
	{
		printf(NOS_PERCORRIDOS_IS, string);
		for (int i = 0; i < registro_is-> num_chaves; i++)
		{
			comparacao = strcmp(string, registro_is-> chave[i].string);
			if (comparacao == 0) // achou
			{
				achou = 1;	
				busca_pk (iprimary, registro_is-> chave[i].pk);//chamar busca_ip
				return;
			}	
			else
			{
				if (virgula == 0)
				{
					printf("%s", registro_is-> chave[i].string); // Imprime a chave que passou por
					virgula = 1;
				}
				else
					printf(", %s", registro_is-> chave[i].string);

				if (comparacao < 0) //Nó a esquerda
				{
					printf("\n");
					prox_ibranc.raiz = registro_is-> desc[i];
					busca_is (prox_ibranc, iprimary, string);
				}
				else if (comparacao > 0) //Nó a direita
				{
					printf("\n");
					prox_ibranc.raiz = registro_is-> desc[i + 1];
					busca_is (prox_ibranc, iprimary, string);
				}
			}
		}
	}
	else
		printf(REGISTRO_N_ENCONTRADO);
	return;
}

void *read_btree(int rrn, char ip)
{
	char *registro_ip, *registro_is, folha;
	node_Btree_ip *nodeip;
	node_Btree_is *nodeis;

	if (ip == 'p')
	{
		nodeip = criar_no('p');
		registro_ip = ARQUIVO_IP + tamanho_registro_ip * rrn;
		sscanf (registro_ip, "%03d", &(nodeip-> num_chaves));
		for (int i = 0; i < nodeip-> num_chaves; i++)
		{
			sscanf (registro_ip + 3 + 14 * i, "%10s", nodeip-> chave[i].pk);
			sscanf (registro_ip + 13 + 14 * i, "%04d", &(nodeip-> chave[i].rrn));
		}
		sscanf (registro_ip + 3 + 14 * (ordem_ip - 1), "%c", &(nodeip-> folha));
		if (folha == 'N')
		{
			for (int i = 0; i <= nodeip-> num_chaves; i++)
			{
				sscanf (registro_ip + 3 + 14 * (ordem_ip -1) + 1 + 3 * i, "%03d", &(nodeip-> desc[i]));
			}
		}
		return nodeip;
	}
	else if (ip == 's')
	{
		nodeis = criar_no('s');
		registro_is = ARQUIVO_IS + tamanho_registro_is * rrn;
		sscanf (registro_is, "%03d", &(nodeis-> num_chaves));
		for (int i = 0; i < nodeis-> num_chaves; i++)
		{
			sscanf (registro_is + 3 + i * 111, "%10s", nodeis-> chave[i].pk);
			sscanf (registro_is + 3 + i * 111 + 10, "%101s", nodeis-> chave[i].string); //"%101[^#]s"
		}
		sscanf (registro_is + 3 + 111 * (ordem_is - 1), "%c", &(nodeis-> folha));
		if (folha == 'N')
		{
			for (int i = 0; i <= nodeis-> num_chaves; i++)
				sscanf (registro_is + 3 + 14 * (ordem_is -1) + 1 + 3 * i, "%03d", &(nodeis-> desc[i]));
		}
		return nodeis;
	}
}

void write_btree(void *salvar, int rrn, char ip)
{
	char *registro_ip, *registro_is, ultimo;
	node_Btree_ip *ptr_ip;
	node_Btree_is *ptr_is;
	int i, j, vazio;

	if (ip == 'p')
	{
		ptr_ip = (node_Btree_ip *) salvar;
		registro_ip =  ARQUIVO_IP + tamanho_registro_ip * rrn;
		sprintf (registro_ip, "%03d", ptr_ip-> num_chaves);
		for (i = 0; i < ptr_ip-> num_chaves; i++)
		{
			sprintf (registro_ip + 3 + 14 * i, "%10s", ptr_ip-> chave[i].pk);
			sprintf (registro_ip + 13 + 14 * i, "%04d", ptr_ip-> chave[i].rrn);
		}
		vazio = ordem_ip - 1 - ptr_ip-> num_chaves;
		for (j = 0; j < 14 * vazio; j++)
			sprintf (registro_ip + 3 + 14 * (ptr_ip-> num_chaves) + j, "#");
		sprintf (registro_ip + 3 + 14 * (ordem_ip -1), "%c", ptr_ip-> folha);
		if (ptr_ip-> folha == 'N')
		{
			ultimo = *(registro_ip + 3 + 14 * (ordem_ip - 1) + 1 + 3 * ordem_ip);
			for (i = 0; i <= ptr_ip-> num_chaves; i++)
				sprintf (registro_ip + 3 + 14 * (ordem_ip -1) + 1 + 3 * i, "%03d", ptr_ip-> desc[i]);
			for (i = 3 * i; i < 3 * ordem_ip; i++)
				sprintf (registro_ip + 3 + 14 * (ordem_ip -1) + 1 + i, "*");
			*(registro_ip + 3 + 14 * (ordem_ip -1) + 1 + i) = ultimo; //tira o \0 que o sprintf colocou
		}
		else if (ptr_ip-> folha == 'F')
		{
			for (i = 0; i < 3 * ordem_ip; i++)
				sprintf (registro_ip + 3 + 14 * (ordem_ip -1) + 1 + i, "*");
		}
	}
	else if (ip == 's')
	{
		ptr_is = (node_Btree_is *) salvar;
		registro_is =  ARQUIVO_IS + tamanho_registro_is * rrn;
		sprintf (registro_is, "%03d", ptr_is-> num_chaves);
		for (i = 0; i < ptr_is-> num_chaves; i++)
		{
			sprintf (registro_is + 3 + i * 111, "%s", ptr_is-> chave[i].pk);
			sprintf (registro_is + 3 + i * 111 + 10, "%s", ptr_is-> chave[i].string); //"%101[^#]s"
		}
		vazio = ordem_is - 1 - ptr_is-> num_chaves;
		for (j = 0; j < 111 * vazio; j++)
			sprintf (registro_is + 3 + 111 * (ptr_is-> num_chaves) + j, "#");
		sprintf (registro_is + 3 + 111 * (ordem_is - 1), "%c", ptr_is-> folha);
		if (ptr_is-> folha == 'N')
		{
			ultimo = *(registro_is + 3 + 14 * (ordem_is - 1) + 1 + 3 * ordem_is);
			for (i = 0; i <= ptr_is-> num_chaves; i++)
				sprintf (registro_is + 3 + 111 * (ordem_is -1) + 1 + 3 * i, "%03d", ptr_is-> desc[i]);
			for (i = 3 * i; i < 3 * (ordem_is); i++)
				sprintf (registro_is + 3 + 111 * (ordem_is -1) + 1 + i, "*");
			*(registro_is + 3 + 14 * (ordem_is -1) + 1 + i) = ultimo;
		}
		else
		{
			for (i = 0; i < 3 * ordem_is; i++)
				sprintf (registro_is + 3 + 111 * (ordem_is -1) + 1 + i, "*");
		}
	}
}

void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Produto p, int seleciona) // Inserir is com base na string e n na pk!!!
{
	char marca_nome[TAM_STRING_INDICE];
	Indice *achou_ip = (Indice *) malloc (sizeof(Indice));
	Indice *achou_is = (Indice *) malloc (sizeof(Indice));
	Chave_ip *chave_ip = (Chave_ip *) malloc (sizeof(Chave_ip));
	Chave_is *chave_is = (Chave_is *) malloc (sizeof(Chave_is));
	strcpy (marca_nome, p.marca);
	strcat (marca_nome, "$");
	strcat (marca_nome, p.nome);

	strcpy (chave_ip-> pk, p.pk);
	chave_ip-> rrn = nregistros;

	if (seleciona == 0 || seleciona == 1)
	{
		if (achou_ip-> raiz = buscar_indices (iprimary-> raiz, p.pk, 'p') == iprimary-> raiz)
			insere (iprimary, chave_ip, ">.<", 'p');
		else
			insere (achou_ip, chave_ip, ">.<", 'p');
	}
	if (seleciona == 0 || seleciona == 2)
	{
		/*if (achou_is-> raiz = buscar_indices (ibrand-> raiz, p.pk, 's') == ibrand-> raiz)
			insere (ibrand, p.pk, marca_nome, 's');
		else
			insere (achou_is, p.pk, marca_nome, 's');*/
	}
}

void insere (Indice *idx, void *chave, char marca_nome[TAM_STRING_INDICE], char ip)
{
	int filho_direito = -1;

	if (ip == 'p')
	{
		Chave_ip *chave_promovida_ip = (Chave_ip *) malloc (sizeof(Chave_ip));
		strcpy (chave_promovida_ip-> pk, "\0");
		chave_promovida_ip-> rrn = -1;
		Chave_ip *chave_ip = (Chave_ip *) chave;
		node_Btree_ip *no_x;

		if (idx-> raiz == -1)
		{
			no_x = criar_no('p');
			no_x-> folha = 'F';
			no_x-> num_chaves = 1;
			strcpy (no_x-> chave[0].pk, chave_ip-> pk);
			no_x-> chave[0].rrn = chave_ip-> rrn;
			idx-> raiz = 0;
			write_btree (no_x, nregistrosip, 'p');
			libera_no (no_x, 'p');
			nregistrosip ++;
		}
		else
		{
			insere_aux_ip (idx-> raiz, chave_ip, chave_promovida_ip, &filho_direito);

			if (chave_promovida_ip-> rrn != -1)
			{
				no_x = criar_no('p');
				no_x-> folha = 'N';
				no_x-> num_chaves = 1;
				strcpy (no_x-> chave[0].pk, chave_promovida_ip-> pk);
				no_x-> chave[0].rrn = chave_promovida_ip-> rrn;
				no_x-> desc[0] = idx-> raiz;
				no_x-> desc[1] = filho_direito;
				idx-> raiz = filho_direito + 1;
				write_btree (no_x, chave_promovida_ip-> rrn, 'p');
				libera_no (no_x, 'p');
				nregistrosip ++;
			}
		}
	}
	/*else if (ip == 's')
	{
		node_Btree_is *no_x;
		Chave_is *chave_promovida_is = (Chave_is *) malloc (sizeof(Chave_is));
		chave_promovida_is = NULL;

		if (idx-> raiz == -1)
		{
			no_x = criar_no('s');
			no_x-> folha = 'F';
			no_x-> num_chaves = 1;
			strcpy (no_x-> chave[0].pk, pk);
			strcpy (no_x-> chave[0].string, marca_nome);
			idx-> raiz = 0;
			write_btree (no_x, idx-> raiz, 's');
		}
		else
		{
			insere_aux_is (idx-> raiz, pk, chave_promovida_is, &filho_direito);
			if (chave_promovida_is != NULL)
			{
				no_x = criar_no('s');
				no_x-> folha = 'N';
				no_x-> num_chaves = 1;
				strcpy (no_x-> chave[0].pk, chave_promovida_is-> pk);
				strcpy (no_x-> chave[0].string, chave_promovida_is-> string);
				no_x-> desc[0] = idx-> raiz;
				no_x-> desc[1] = filho_direito;
				idx-> raiz = filho_direito + 1;
				write_btree (no_x, idx-> raiz, 's');
			}
		}
	}*/
}

void insere_aux_ip (int rrn, Chave_ip* chave_ip, Chave_ip *chave_promovida_ip, int *filho_direito)
{
	int i, m;

	node_Btree_ip *no_x = read_btree (rrn, 'p');

	if (no_x-> folha == 'F')
	{
		if (no_x-> num_chaves < ordem_ip - 1)
		{
			for (i = no_x-> num_chaves - 1; i >= 0 && strcmp (chave_ip-> pk, no_x-> chave[i].pk) < 0; i--)
			{
				strcpy (no_x-> chave[i + 1].pk, no_x-> chave[i].pk);
				no_x-> chave[i + 1].rrn = no_x-> chave[i].rrn;
			}
			strcpy (no_x-> chave[i + 1].pk, chave_ip-> pk);
			no_x-> chave[i + 1].rrn = chave_ip-> rrn;
			no_x-> num_chaves ++;
			write_btree (no_x, rrn, 'p');// n mudou a raiz!
			libera_no (no_x, 'p');
			strcpy (chave_promovida_ip-> pk, "\0");
			chave_promovida_ip-> rrn = -1;
			*filho_direito = -1; // nulo
		}
		else
		{	
			*filho_direito = -1; // nulo		
			divide_no_ip (no_x ,chave_ip, chave_promovida_ip, filho_direito); // -1 equivale a um nó nulo
		}
	}
	else
	{
		for (i = no_x-> num_chaves - 1; i >= 0 && strcmp (chave_ip-> pk, no_x-> chave[i].pk) < 0; i--){}
		i ++;

		insere_aux_ip (no_x-> desc[i], chave_ip, chave_promovida_ip, filho_direito);
	
		if (chave_promovida_ip-> rrn != -1)
		{
			strcpy (chave_ip-> pk, chave_promovida_ip-> pk);
			chave_ip-> rrn = chave_promovida_ip-> rrn;
			if (no_x-> num_chaves < ordem_ip - 1)
			{
				
				for (i = no_x-> num_chaves - 1; i >= 0 && strcmp (chave_ip-> pk, no_x-> chave[i].pk) < 0; i--)
				{
					strcpy (no_x-> chave[i + 1].pk, no_x-> chave[i].pk);
					no_x-> chave[i + 1].rrn = no_x-> chave[i].rrn;
					no_x-> desc[i + 2] = no_x-> desc[i + 1];
				}
				strcpy (no_x-> chave[i + 1].pk, chave_ip-> pk);
				no_x-> chave[i + 1].rrn = chave_ip-> rrn;
				no_x-> desc[i + 2] = *filho_direito;
				no_x-> num_chaves++;

				write_btree (no_x, rrn, 'p');// n mudou a raiz!
				libera_no (no_x, 'p');
				strcpy (chave_promovida_ip-> pk, "\0");
				chave_promovida_ip-> rrn = -1;
				*filho_direito = -1; // nulo
			}
			else
				divide_no_ip (no_x ,chave_ip , chave_promovida_ip, filho_direito);
		}
		else
		{
			strcpy (chave_promovida_ip-> pk, "\0");
			chave_promovida_ip-> rrn = -1;
			*filho_direito = -1; // nulo
		}
		return;
	}
}
/*D E S A T U A L I Z A D O*/
/*void insere_aux_is (int rrn, char* pk, Chave_is *chave_promovida_is, int *filho_direito)
{
	int i, m;
	node_Btree_is *no_x = read_btree (rrn, 's');

	if (no_x-> folha == 'F')
	{
		if (no_x-> num_chaves < ordem_is - 1)
		{
			i = no_x-> num_chaves;
			while (i >= 0 && strcmp (pk, no_x-> chave[i].pk) < 0)
			{
				no_x-> chave[i + 1] = no_x-> chave[i];
				i --;
			}
			strcpy (no_x-> chave[i + 1].pk, pk);
			no_x-> num_chaves ++;

			write_btree (no_x, rrn, 's');// n mudou a raiz!

			chave_promovida_is = NULL;
			*filho_direito = -1; // nulo

			return;
		}
		else
		{
			divide_no_is (no_x ,pk, chave_promovida_is, filho_direito); // -1 equivale a um nó nulo
			return;
		}
	}
	else
	{
		i = no_x-> num_chaves;
		while (i >= 0 && strcmp (pk, no_x-> chave[i].pk) < 0)
			i--;
		insere_aux_is (no_x-> desc[i], pk, chave_promovida_is, filho_direito);
	
		if (chave_promovida_is != NULL)
		{
			strcpy (pk, chave_promovida_is-> pk);
			if (no_x-> num_chaves < ordem_is - 1)
			{
				i = no_x-> num_chaves;
				while (i >= 0 && strcmp (pk, no_x-> chave[i].pk) < 0)
				{
					no_x-> chave[i + 1] = no_x-> chave[i];
					no_x-> desc[i + 2] = no_x-> desc[i + 1];
					i--;
				}
				strcpy (no_x-> chave[i + 1].pk, pk);
				no_x-> desc[i + 2] = *filho_direito;
				no_x-> num_chaves++;

				write_btree (no_x, rrn, 's');// n mudou a raiz!

				chave_promovida_is = NULL;
				*filho_direito = -1; // nulo
				return;
			}
			else
			{
				divide_no_is (no_x ,pk , chave_promovida_is, filho_direito);
				return;
			}
		}
		else
		{
			chave_promovida_is = NULL;
			*filho_direito = -1; // nulo
			return;
		}
	}
}*/

void divide_no_ip (node_Btree_ip *no_x, Chave_ip *chave_ip, Chave_ip *chave_promovida_ip, int *filho_direito)
{
	int i, chave_alocada;

	node_Btree_ip *novo_no;
	i = no_x-> num_chaves - 1;
	chave_alocada = 0;
	novo_no = criar_no('p');
	novo_no-> folha = no_x-> folha;
	novo_no-> num_chaves = (int)((ordem_ip - 1) / 2);

	for (int j = novo_no-> num_chaves - 1; j >= 0; j--)
	{
		if (!chave_alocada && strcmp (chave_ip-> pk, no_x-> chave[i].pk) > 0)
		{
			strcpy (novo_no-> chave[j].pk, chave_ip-> pk);
			novo_no-> chave[j].rrn = chave_ip-> rrn;
			novo_no-> desc[j + 1] = *filho_direito;
			chave_alocada = 1;
		}
		else
		{
			strcpy (novo_no-> chave[j].pk, no_x-> chave[i].pk);
			novo_no-> chave[j].rrn = no_x-> chave[i].rrn;
			novo_no-> desc[j + 1] = no_x-> desc[i + 1];	
			i --;
		}

	}
	if (!chave_alocada)
	{
		while (i >= 0 && strcmp(chave_ip-> pk, no_x-> chave[i].pk) < 0)
		{
			strcpy (no_x-> chave[i + 1].pk, no_x-> chave[i].pk);
			no_x-> desc[i + 2] = no_x-> desc[i + 1]; 
			i --;
		}
		strcpy (no_x-> chave[i + 1].pk, chave_ip-> pk);
		no_x-> chave[i + 1].rrn = chave_ip-> rrn;
		no_x-> desc[i + 2] = *filho_direito;
	}
	
	strcpy (chave_promovida_ip-> pk, no_x-> chave[ordem_ip / 2].pk);
	chave_promovida_ip-> rrn = no_x-> chave[(ordem_ip / 2) + 1].rrn;
	novo_no-> desc[0] = no_x-> desc[(int)(ordem_ip / 2) + 2];
	no_x-> num_chaves = (int)(ordem_ip / 2);
	write_btree (novo_no, nregistrosip, 'p');
	libera_no (novo_no, 'p');
	*filho_direito = nregistrosip;
	nregistrosip ++;
	write_btree (no_x, nregistrosip, 'p');
	return;
}

/*D E S A T U A L I S A D O*/
/*void divide_no_is (node_Btree_is *no_x, char *pk, Chave_is *chave_promovida_is, int *filho_direito)
{

}*/


void insere_ip (Indice *idx, void *chave)
{
	int filho_direito = -1;
	Chave_ip *chave_promovida;
	chave_promovida = NULL;

	if (idx-> raiz == NULL)
	{
		no_x = criar_no('p');
		no_x-> folha = 'F';
		no_x-> num_chaves = 1;
		strcpy (no_x-> chave[0].pk, chave_ip-> pk);
		no_x-> chave[0].rrn = chave_ip-> rrn;
		idx-> raiz = 0;
		write_btree (no_x, nregistrosip, 'p');
		libera_no (no_x, 'p');
	}
	else
	{
		insere_aux_ip (Indice *idx, void *chave, &filho_direito, chave_promovida);

		if (chave_promovida != NULL)
		{
			no_x = criar_no('p');
			no_x-> folha = 'F';
			no_x-> num_chaves = 1;
			strcpy (no_x-> chave[0].pk, chave_promovida-> pk);
			no_x-> chave[0].rrn = chave_promovida-> rrn;
			no_x-> desc[0] = idx-> raiz;
			no_x-> desc[0] = filho_direito;
			nregistrosip ++
			idx-> raiz = nregistrosip;
			write_btree (no_x, nregistrosip, 'p');
			libera_no (no_x, 'p');
		}
	}
}

















int existe_ip (int rrn_no, char *pk)
{
	node_Btree_ip* nodeip = read_btree(rrn_no, 'p');

	if (rrn_no == - 1)
		return -1;
	for (int i = 0; i < nodeip-> num_chaves; i++)
	{
		if (strcmp (nodeip-> chave[i].pk, pk) == 0)
			return rrn_no;
		else if (nodeip-> folha == 'F' && i == nodeip-> num_chaves - 1)// Ultima chave e no folha =  n achou
			return -1;
		if (i < nodeip-> num_chaves - 1)
		{
			if (i == 0 && strcmp (nodeip-> chave[i].pk, pk) > 0)
				return existe_ip (nodeip-> desc[i], pk);
			else if (strcmp (nodeip-> chave[i].pk, pk) < 0 && strcmp (nodeip-> chave[i + 1].pk, pk) > 0)
				return existe_ip (nodeip-> desc[i + 1], pk);
		}
		else if (i == nodeip-> num_chaves - 1)
			return existe_ip (nodeip-> desc[i + 1], pk);
	}
}

int existe_is (int rrn_no, char *string)
{
	node_Btree_is* nodeis = read_btree(rrn_no, 's');

	if (rrn_no == - 1)
		return -1;
	for (int i = 0; i < nodeis-> num_chaves; i++)
	{
		if (strcmp (nodeis-> chave[i].string, string) == 0)
			return rrn_no;
		else if (nodeis-> folha == 'F' && i == nodeis-> num_chaves - 1)// Ultima chave e no folha =  n achou
			return -1;
		if (i < nodeis-> num_chaves - 1)
		{
			if (i == 0 && strcmp (nodeis-> chave[i].string, string) > 0)
				return existe_ip (nodeis-> desc[i], string);
			else if (strcmp (nodeis-> chave[i].string, string) < 0 && strcmp (nodeis-> chave[i + 1].string, string) > 0)
				return existe_ip (nodeis-> desc[i + 1], string);
		}
		else if (i == nodeis-> num_chaves - 1)
			return existe_ip (nodeis-> desc[i + 1], string);
	}
}

int buscar_indices (int rrn_no, char *pk, char ip)
{
	int i = 0;
	if (rrn_no == -1)
			return -1; // Não achou
	if (ip == 'p')
	{
		node_Btree_ip* nodeip = read_btree(rrn_no, 'p');

		if (nodeip-> folha == 'F')
			return rrn_no;
		for (i = 0; i < nodeip-> num_chaves; i++)
		{
			if (i < nodeip-> num_chaves - 1)
			{
				if (i == 0 && strcmp (nodeip-> chave[i].pk, pk) > 0)
					return buscar_indices (nodeip-> desc[i], pk, 'p');
				else if (strcmp (nodeip-> chave[i].pk, pk) < 0 && strcmp (nodeip-> chave[i + 1].pk, pk) > 0)
					return buscar_indices (nodeip-> desc[i + 1], pk, 'p');
			}
			else if (i == nodeip-> num_chaves - 1)
				return buscar_indices (nodeip-> desc[i + 1], pk, 'p');
		}
	}
	else if (ip == 's')
	{
		node_Btree_is* nodeis = read_btree(rrn_no, 's');

		if (nodeis-> folha == 'F')
			return rrn_no;
		for (i = 0; i < nodeis-> num_chaves; i++)
		{
			if (i < nodeis-> num_chaves - 1)
			{
				if (i == 0 && strcmp (nodeis-> chave[i].pk, pk) > 0)
					return buscar_indices (nodeis-> desc[i], pk, 's');
				else if (strcmp (nodeis-> chave[i].pk, pk) < 0 && strcmp (nodeis-> chave[i + 1].pk, pk) > 0)
					return buscar_indices (nodeis-> desc[i + 1], pk, 's');
			}
			else if (i == nodeis-> num_chaves - 1)
				return buscar_indices (nodeis-> desc[i + 1], pk, 's');
		}
	}
}

void criar_iprimary(Indice *iprimary)
{
	Produto p;
	iprimary-> raiz = -1;

	if (nregistros != 0)
	{
		for (int i = 0; i < nregistros; i++)
		{
			sscanf(ARQUIVO + i*192, "%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@", p.pk, p.nome, p.marca, p.data, p.ano, p.preco, p.desconto, p.categoria);
			gerarChave(&p);
			if (existe_ip (iprimary-> raiz, p.pk) != -1)
				inserir_registro_indices(iprimary, NULL, p, 1);// chama para inserir somente iprimary (1)
			else
				printf(ERRO_PK_REPETIDA, p.pk);
		}
	}
}

void criar_ibrand(Indice *ibrand)
{
	char string[TAM_STRING_INDICE];
	Produto p;
	ibrand-> raiz = -1;
	
	if (nregistros != 0)
	{
		for (int i = 0; i < nregistros; i++)
		{
			sscanf(ARQUIVO + i*192, "%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@", p.pk, p.nome, p.marca, p.data, p.ano, p.preco, p.desconto, p.categoria);
			gerarChave(&p);
			strcpy (string, p.marca);
			strcat (string, "$");
			strcat (string, p.nome);
			if (existe_is (ibrand-> raiz, string) != -1)
				inserir_registro_indices(NULL, ibrand, p, 2);// chama para inserir somente o ibrand (2)
			else
				printf(ERRO_PK_REPETIDA, p.pk);
		}
	}
}

int alterar(Indice iprimary)
{
	node_Btree_ip* nodeip = NULL;
	int rrn, arq_rrn, desconto;
	char pk[TAM_PRIMARY_KEY], *altera, *pch;
	scanf ("\n%s", pk);
	if (iprimary.raiz == -1)
		return 0;  
	else
	{
		if ((rrn = buscar_indices (iprimary.raiz, pk, 'p')) == 0)
		{
			nodeip = read_btree (rrn, 'p');
			for (int i = 0; i < nodeip-> num_chaves; i++)
				if (strcmp (nodeip-> chave[i].pk, pk) == 0)
					arq_rrn = nodeip-> chave[i].rrn;

			altera = ARQUIVO + arq_rrn * TAM_REGISTRO;
			pch = strtok (altera, "@");
			for (int i = 0; i < 6; i++)
				pch = strtok (NULL, "@");
			scanf("\n%03d", &desconto);
			sprintf (pch, "%03d", desconto);
		}
		else
		{
			printf(REGISTRO_N_ENCONTRADO); 
			return 0;
		}
	}
}

void listar(Indice iprimary, Indice ibrand)
{
	int op;
	scanf ("\n%d", &op);
	if (op == 1)
	{
		if (iprimary.raiz == -1)
			printf(REGISTRO_N_ENCONTRADO); return;
		listar_ip (iprimary.raiz, 1);
	}
	else if (op == 2)
	{	
		if (ibrand.raiz == -1)
			printf(REGISTRO_N_ENCONTRADO); return;
		listar_is (ibrand.raiz, 1);
	}
	return;
}

void listar_ip (int rrn, int nivel)
{
	node_Btree_ip* nodeip;
	if (rrn == -1)
		return;
	nodeip = read_btree(rrn, 'p');
	for (int i = 0; i < nodeip-> num_chaves; i++) // Emprime o nó
	{
		if (i == 0)
			printf("%d - ", nivel);
		printf("%s", nodeip-> chave[i].pk);
		if (i != (nodeip-> num_chaves) - 1)
			printf(", ");
		else
			printf("\n");
	}
	nivel++;
	for (int i = 0; i <= nodeip-> num_chaves; i++)
		listar_ip (nodeip-> desc[i], nivel);
	return;
}

void listar_is (int rrn, int nivel)
{
	node_Btree_is* nodeis;
	int i;
	char marca[TAM_MARCA], nome[TAM_NOME], *pch;
	if (rrn == -1)
		return;
	nodeis = read_btree(rrn, 's');
	for (i = 0; i < nodeis-> num_chaves; i++) // Emprime o nó
	{
		nivel ++;
		listar_is (nodeis-> desc[i], nivel);
		if (i == 0)
			printf("%d - ", nivel);

		pch = strtok (nodeis-> chave[i].string, "$");
		strcpy (marca, pch);
		pch = strtok (NULL, "\0");
		strcpy (nome, pch);

		printf("%s", marca);
		for (int i = 0; i < (50 - sizeof(marca)); i++)
			printf("-");
		printf(" ");
		printf("%s", nome);
		for (int i = 0; i < (50 - sizeof(nome)); i++)
			printf("-");
		printf("\n");
	}
	listar_is (nodeis-> desc[i++], nivel);
	return;
}

void libera_no(void *node, char ip)
{
	if (ip == 'p')
	{
		free (((node_Btree_ip *)node)-> chave);
		free (((node_Btree_ip *)node)-> desc);
		free ((node_Btree_ip *)node);
	}
	else if (ip == 's')
	{
		free (((node_Btree_is *)node)-> chave);
		free (((node_Btree_is *)node)-> desc);
		free ((node_Btree_is *)node);
	}
}