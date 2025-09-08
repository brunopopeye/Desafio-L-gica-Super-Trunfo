// Desafio Lógica Super Trunfo (C)
// Autor: você 😉
// Compilação (Linux/macOS):  gcc -O2 -std=c11 src/logica_super_trunfo.c -o logica_super_trunfo
// Compilação (Windows MinGW): gcc -O2 -std=c11 src\logica_super_trunfo.c -o logica_super_trunfo.exe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char estado;            // 'A'..'H'
    char codigo[8];         // ex.: "A01"
    char nome[64];          // nome da cidade
    int  populacao;         // hab
    float area_km2;         // km²
    float pib_bilhoes;      // em bilhões
    int super_trunfo;       // 1 = tem Super Trunfo; 0 = não
} Carta;

/* ---------- Utilidades de entrada ---------- */

static void limpar_entrada(void){
    int ch; while((ch=getchar())!='\n' && ch!=EOF){}
}

static void ler_linha(char *buf, size_t n){
    if(!fgets(buf,(int)n,stdin)){ buf[0]='\0'; return; }
    size_t L=strlen(buf); if(L && buf[L-1]=='\n') buf[L-1]='\0';
}

static int ler_int_lim(const char* prompt, int minv, int maxv, int* out){
    while(1){
        printf("%s", prompt);
        char t[64]; ler_linha(t,sizeof t);
        if(!t[0]) continue;
        char *e; long v=strtol(t,&e,10);
        if(*e=='\0' && v>=minv && v<=maxv){ *out=(int)v; return 1; }
        printf("  Valor inválido (%d..%d).\n",minv,maxv);
    }
}

static int ler_float_lim(const char* prompt, float minv, float maxv, float* out){
    while(1){
        printf("%s", prompt);
        char t[64]; ler_linha(t,sizeof t);
        if(!t[0]) continue;
        char *e; float v=strtof(t,&e);
        if(*e=='\0' && v>=minv && v<=maxv){ *out=v; return 1; }
        printf("  Valor inválido (%.2f..%.2f).\n",minv,maxv);
    }
}

/* ---------- Entrada de uma carta ---------- */

static void ler_estado(Carta* c){
    while(1){
        printf("Estado (A..H): ");
        char t[16]; ler_linha(t,sizeof t);
        if(strlen(t)==1){
            char ch=(char)toupper((unsigned char)t[0]);
            if(ch>='A' && ch<='H'){ c->estado=ch; return; }
        }
        printf("  Inválido. Digite A..H.\n");
    }
}

static void ler_codigo(Carta* c){
    while(1){
        printf("Código da carta (ex.: A01): ");
        char t[16]; ler_linha(t,sizeof t);
        for(size_t i=0;t[i];++i) t[i]=(char)toupper((unsigned char)t[i]);
        if(strlen(t)==3 && t[0]>='A' && t[0]<='H' &&
           isdigit((unsigned char)t[1]) && isdigit((unsigned char)t[2])){
            strncpy(c->codigo,t,sizeof c->codigo);
            c->codigo[sizeof c->codigo - 1] = '\0';
            return;
        }
        printf("  Formato inválido.\n");
    }
}

static void ler_nome(Carta* c){
    while(1){
        printf("Nome da cidade: ");
        ler_linha(c->nome,sizeof c->nome);
        if(strlen(c->nome)>=2) return;
        printf("  Digite ao menos 2 caracteres.\n");
    }
}

static void ler_super_trunfo(Carta* c){
    while(1){
        printf("Esta carta tem SUPER TRUNFO? (S/N): ");
        char t[8]; ler_linha(t,sizeof t);
        if(!t[0]) continue;
        char ch=(char)toupper((unsigned char)t[0]);
        if(ch=='S'){ c->super_trunfo=1; return; }
        if(ch=='N'){ c->super_trunfo=0; return; }
        printf("  Responda S ou N.\n");
    }
}

static void ler_carta(Carta* c, int idx){
    printf("\n===== Cadastro da Carta %d =====\n", idx);
    ler_estado(c);
    ler_codigo(c);
    ler_nome(c);
    ler_int_lim("População: ", 1, 2000000000, &c->populacao);
    ler_float_lim("Área (km²): ", 0.01f, 30000000.f, &c->area_km2);
    ler_float_lim("PIB (em bilhões): ", 0.001f, 1000000.f, &c->pib_bilhoes);
    ler_super_trunfo(c);
}

/* ---------- Métricas derivadas ---------- */

static float densidade(const Carta* c){ // hab/km²
    return c->area_km2>0.f ? (c->populacao / c->area_km2) : 0.f;
}
static float pib_per_capita(const Carta* c){ // R$
    if(c->populacao<=0) return 0.f;
    double pib_reais=(double)c->pib_bilhoes*1e9;
    return (float)(pib_reais / (double)c->populacao);
}

/* ---------- Impressão ---------- */

static void imprimir_carta(const Carta* c, const char* rot){
    printf("\n--- %s ---\n", rot);
    printf("Código/Estado: %s (%c)\n", c->codigo, c->estado);
    printf("Cidade:         %s\n", c->nome);
    printf("População:      %d hab\n", c->populacao);
    printf("Área:           %.2f km²\n", c->area_km2);
    printf("PIB:            %.3f bilhões\n", c->pib_bilhoes);
    printf("Densidade:      %.2f hab/km²\n", densidade(c));
    printf("PIB per cap.:   R$ %.2f\n", pib_per_capita(c));
    printf("Super Trunfo:   %s\n", c->super_trunfo ? "SIM" : "NÃO");
}

/* ---------- Lógica de comparação ---------- */
/*
 Regras:
 1) Se apenas uma carta tiver Super Trunfo, ela vence automaticamente.
 2) Caso ambas tenham (ou nenhuma), compara-se o atributo escolhido:
    - População, Área, PIB, PIB per capita: MAIOR vence
    - Densidade: MENOR vence
 3) Empate? Desempates em cascata:
    a) PIB per capita (maior vence)
    b) Densidade (menor vence)
    c) Código (ordem lexicográfica crescente)
 Retornos: -1 = Carta 2 vence; 0 = empate pleno; 1 = Carta 1 vence.
*/

typedef enum {
    ATR_POPULACAO=1,
    ATR_AREA=2,
    ATR_PIB=3,
    ATR_DENSIDADE=4,       // menor vence
    ATR_PIB_PER_CAPITA=5   // maior vence
} Atributo;

static int sinal_float(float a, float b){
    return (a>b) - (a<b); // 1 se a>b; -1 se a<b; 0 se iguais
}

static int comparar_basico(const Carta* c1, const Carta* c2, Atributo atr){
    switch(atr){
        case ATR_POPULACAO:      return (c1->populacao>c2->populacao) - (c1->populacao<c2->populacao);
        case ATR_AREA:           return sinal_float(c1->area_km2, c2->area_km2);
        case ATR_PIB:            return sinal_float(c1->pib_bilhoes, c2->pib_bilhoes);
        case ATR_DENSIDADE: {    // menor vence
            float d1=densidade(c1), d2=densidade(c2);
            return (d2>d1) - (d2<d1);
        }
        case ATR_PIB_PER_CAPITA: {
            float p1=pib_per_capita(c1), p2=pib_per_capita(c2);
            return sinal_float(p1,p2);
        }
        default: return 0;
    }
}

static int desempates(const Carta* c1, const Carta* c2){
    // a) PIB per capita (maior vence)
    int r = sinal_float(pib_per_capita(c1), pib_per_capita(c2));
    if(r!=0) return r;
    // b) Densidade (menor vence)
    float d1=densidade(c1), d2=densidade(c2);
    if(d1!=d2) return (d2>d1) - (d2<d1);
    // c) Código (ordem lexicográfica: "A01" < "B02")
    int cmp = strcmp(c1->codigo, c2->codigo);
    if(cmp<0) return 1;
    if(cmp>0) return -1;
    return 0;
}

static int aplica_logica_super_trunfo(const Carta* c1, const Carta* c2, Atributo atr){
    // Regra 1: Super Trunfo
    if(c1->super_trunfo && !c2->super_trunfo) return 1;
    if(!c1->super_trunfo && c2->super_trunfo) return -1;

    // Regra 2: Comparação do atributo escolhido
    int base = comparar_basico(c1,c2,atr);
    if(base!=0) return base;

    // Regra 3: Desempates
    return desempates(c1,c2);
}

/* ---------- Menu ---------- */

static void mostrar_menu(void){
    printf("\n=== Desafio Lógica – Escolha o atributo ===\n");
    printf("1) População (MAIOR vence)\n");
    printf("2) Área (MAIOR vence)\n");
    printf("3) PIB em bilhões (MAIOR vence)\n");
    printf("4) Densidade (MENOR vence)\n");
    printf("5) PIB per capita (MAIOR vence)\n");
    printf("Seleção: ");
}

/* ---------- Principal ---------- */

int main(void){
    Carta c1={0}, c2={0};

    ler_carta(&c1,1);
    ler_carta(&c2,2);

    imprimir_carta(&c1, "CARTA 1");
    imprimir_carta(&c2, "CARTA 2");

    while(1){
        mostrar_menu();
        int op=0; if(!ler_int_lim("",1,5,&op)) continue;

        int resultado = aplica_logica_super_trunfo(&c1,&c2,(Atributo)op);

        printf("\n>>> Resultado no critério escolhido: ");
        if(resultado>0){
            printf("CARTA 1 (%s) VENCEU!\n", c1.codigo);
        } else if(resultado<0){
            printf("CARTA 2 (%s) VENCEU!\n", c2.codigo);
        } else {
            printf("EMPATE TOTAL.\n");
        }

        printf("\nComparar outro atributo? (S/N): ");
        char ch=0; scanf(" %c",&ch); limpar_entrada();
        if(toupper((unsigned char)ch)!='S') break;
    }

    printf("\nEncerrando. Obrigado!\n");
    return 0;
}
