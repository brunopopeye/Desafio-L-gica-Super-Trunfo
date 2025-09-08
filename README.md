# Desafio Lógica Super Trunfo (C)

Projeto em C focado na **lógica de comparação** entre **duas cartas**, com:
- **Regra de Super Trunfo** (vence automaticamente se apenas uma carta tiver);
- Comparação por atributo escolhido;
- **Desempates em cascata** (PIB per capita → Densidade → Código).

## Campos das cartas
- **Estado**: `A..H`
- **Código**: ex. `A01`
- **Nome da cidade**
- **População** (hab)
- **Área** (km²)
- **PIB** (em **bilhões**)
- **Super Trunfo**: `S` ou `N`

## Regras
1. Se **apenas uma** carta tiver **Super Trunfo**, **ela vence** diretamente.
2. Caso ambas tenham (ou nenhuma), avalia-se o **atributo escolhido**:
   - População, Área, PIB, PIB per capita → **MAIOR vence**
   - Densidade → **MENOR vence**
3. **Empate?** Desempates:
   1) **PIB per capita** (maior vence)  
   2) **Densidade** (menor vence)  
   3) **Código** (ordem lexicográfica crescente)

## Compilação

### Linux/macOS
```bash
make
./bin/logica_super_trunfo
