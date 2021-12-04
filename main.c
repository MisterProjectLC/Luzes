#include <stdio.h>
#include <stdlib.h>
#define ASCII_OFFSET 48

struct Map {
   char  titulo[100];
   int   linhas;
   int   colunas;
   int   limite;
   char  mapa[100][100];
   int  mapa_luz[100][100];
};

struct Map* generate_map(int stage) {
    struct Map* map = (struct Map*)malloc(sizeof(struct Map));

    // Obter nome
    char file_name[] = "fase1.txt";
    file_name[4] = stage+48;

    // Carregar arquivo
	FILE* file = fopen(file_name, "r");
    fscanf(file, "%d %d %d\n", &map->linhas, &map->colunas, &map->limite);
    fscanf(file, "%s\n", map->titulo);

    // Gerar mapa
    char c;
    for (int i = 0; i < map->linhas; i++) {
        for (int j = 0; j < map->colunas; j++) {
            c = '\n';
            while (c == '\n')
                fscanf(file, "%c", &c);
            map->mapa[i][j] = c;
            map->mapa_luz[i][j] = 0;
        }
        fscanf(file, "%c", &c);
    }

    fclose(file);
    return map;
}


void expand_light(int x, int y, int intensity, struct Map* map) {
    if (x < 0 || x >= map->linhas || y < 0 || y >= map->colunas) // Checar se dentro do mapa
        return;

    if (map->mapa[x][y] == 'W' || ('0' <= map->mapa[x][y] && map->mapa[x][y] <= '9' && intensity + ASCII_OFFSET <= map->mapa[x][y])) // Checar se não ocupado
        return;

    map->mapa_luz[x][y] = intensity;
    if (map->mapa[x][y] == ' ' || ('0' <= map->mapa[x][y] && map->mapa[x][y] <= '9'))
        map->mapa[x][y] = intensity + ASCII_OFFSET;

    if (intensity > 1) {
        expand_light(x-1, y, intensity-1, map);
        expand_light(x+1, y, intensity-1, map);
        expand_light(x, y-1, intensity-1, map);
        expand_light(x, y+1, intensity-1, map);
    }
}


void print_map(struct Map* map) {
    printf("  ");
    for (int j = 0; j < map->colunas; j++)
        printf("%d ", j);
    printf("\n");
    
    for (int i = 0; i < map->linhas; i++) {
        printf("%d ", i);

        for (int j = 0; j < map->colunas; j++) {
            if ('0' <= map->mapa[i][j] && map->mapa[i][j] <= '9' || map->mapa[i][j] == ' ') {
                map->mapa[i][j] = ' ';
                printf("%c ", map->mapa_luz[i][j] + ASCII_OFFSET);
            } else
                printf("%c ", map->mapa[i][j]);
        }
        printf("\n");
    }
}


int main () {
    int stage_count = 5;

	// Cada fase
    for (int f = 1; f < stage_count+1; f++) {
        struct Map* map = generate_map(f);
        int luz_sobrando = map->limite;

        // Mostrando o mapa inicial
        print_map(map);
        printf("Fase: %s\n", map->titulo);
        printf("Preencha todos os espaços vazios com pelo menos 2 de luz!\n");

        // Cada posicionamento
        while (1) {
            if (luz_sobrando > 0)
                printf("Luz sobrando: %d\n", luz_sobrando);
            else {
                printf("Acabou a luz restante!\n");
                break;
            }

            // Entrada
            int x, y, raio;
            while (1) {
                printf("Digite as coordenadas da luz e a quantidade de luz que quiser posicionar.\n");
                scanf("%d %d %d", &y, &x, &raio);
                if (x < 0 || y < 0 || x >= map->linhas || y >= map->colunas) {
                    printf("Coordenadas fora do mapa!\n");
                    continue;
                }

                if (map->mapa[x][y] != ' ') {
                    printf("Espaço já ocupado!\n");
                    continue;
                }

                if (raio > luz_sobrando) {
                    printf("Não tem luz suficiente para isso!\n");
                    continue;
                }

                luz_sobrando -= raio;
                break;
            }

            // Aplicar luz
            expand_light(x, y, raio, map);

            // Atualizar iluminadores
            for (int i = 0; i < map->linhas; i++) {
                for (int j = 0; j < map->colunas; j++)
                    switch(map->mapa[i][j]) {
                        case '^':
                            for (int o = i-1; o >= 0; o--)
                                if (map->mapa_luz[o][j] < map->mapa_luz[i][j])
                                    map->mapa_luz[o][j] = map->mapa_luz[i][j];
                            break;

                        case 'V':
                            for (int o = i+1; o < map->linhas; o++)
                                if (map->mapa_luz[o][j] < map->mapa_luz[i][j])
                                    map->mapa_luz[o][j] = map->mapa_luz[i][j];
                            break;

                        case '<':
                            for (int o = j-1; o >= 0; o--)
                                if (map->mapa_luz[i][o] < map->mapa_luz[i][j])
                                    map->mapa_luz[i][o] = map->mapa_luz[i][j];
                            break;

                        case '>':
                            for (int o = j+1; o < map->colunas; o++)
                                if (map->mapa_luz[i][o] < map->mapa_luz[i][j])
                                    map->mapa_luz[i][o] = map->mapa_luz[i][j];
                            break;
                    }
            }

            // Resultado
            print_map(map);
        }

        unsigned victory = 1;
        for (int i = 0; i < map->linhas; i++) {
            for (int j = 0; j < map->colunas; j++) {
                if (map->mapa[i][j] == ' ' && map->mapa_luz[i][j] < 2) {
                    victory = 0;
                    break;
                }
            }
            if (!victory)
                break;
        }

        if (victory)
            printf("A sala está bem iluminada! Próxima fase!\n");
        else {
            printf("Ainda está escuro... Recomeçando a fase!\n");
            f--;
        }

        free(map);
    }

    

    printf("FIM DO JOGO\n");
}
