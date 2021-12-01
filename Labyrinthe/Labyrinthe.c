#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <MLV/MLV_all.h>


#define PERE 0
#define RANG 1
#define N 20


typedef struct{
	/*Structure représentant une case par ses coordonnées*/
	int x;
	int y;
}Case;

typedef struct{
	/*Structure représentant les 4 murs d'une case*/
	Case cases;
	int tab[4];
}Mur;

typedef struct{
	/*Structure servant à la gestion des paramètres choisi par l'utilisateur*/
	int ligne;
	int colonne;
	int graine;
	int attente;
	int unique;
	int victor;
	int acces;
	int affichage;
}Parametres;


Parametres init_parametre(){
	/*Fonction initialisant et renvoyant les paramètres du Labyrinthe.
	Par défaut, ce dernier est de taille 6x8, s'affiche instantanément et est aléatoire*/
	Parametres para;
	para.ligne = 6;
	para.colonne = 8;
	para.graine = -1;
	para.unique = 0;
	para.attente = 0;
	para.victor = 0;
	para.acces = 0;
	para.affichage = 0;
	return para;
}

Case nombre_vers_case(Parametres para, Mur tab_mur[][para.colonne], int entier){
	/*Fonction convertissant et renvoyant le numéro d'une case par rapport à ses coordonnées*/
	int i, j;
	i = entier / para.colonne;
	j = entier % para.ligne ;
	return tab_mur[i][j].cases;
}

void init_mur(Parametres para, Mur tab_mur[][para.colonne]){
	/*Fonction initialisant le tableau représentant tout les murs du labyrinthe.
	Par défaut, tout les murs sont présents*/
	int i,j;
	int tab_base[4] = {1,1,1,1};
	for(i = 0; i < para.ligne; i++){
		for(j = 0; j < para.colonne; j++){
			tab_mur[i][j].cases.x = i;
			tab_mur[i][j].cases.y = j;
			memcpy(tab_mur[i][j].tab, tab_base, sizeof(tab_mur[i][j].tab));
		}
	}
	tab_mur[0][0].tab[0] = 0;
	tab_mur[para.ligne-1][para.colonne-1].tab[2] = 0;
}

void Initialisation(int peres[][2], Parametres para){
	/*Fonction initialisant le tableau peres représentant l'ensemble auquel appartient une case*/
	int i;
	for(i = 0; i < para.ligne * para.colonne; i++){
		peres[i][PERE] = i;
		peres[i][RANG] = 1;
	}
}

int TrouveCompresse(int peres[][2], int x){
	/*Fonction renvoyant le représentant de l'ensemble auquel appartient une case*/
	if(peres[x][PERE] != x){
		peres[x][PERE] = TrouveCompresse(peres, peres[x][PERE]);
	}
	return peres[x][PERE];
}

void FusionRang(int peres[][2], int x, int y){
	/*Fonction effectuant la fusion de l'ensemble contenant l'élément x et l'ensemble content l'élément y*/
	int racine_x = TrouveCompresse(peres, x);
	int racine_y = TrouveCompresse(peres, y);
	if(racine_x != racine_y){
		if(peres[racine_x][RANG] >= peres[racine_y][RANG]){
			peres[racine_y][PERE] = racine_x;
			peres[racine_x][RANG] += peres[racine_y][RANG];
		}
		else{
			peres[racine_x][PERE] = racine_y;
			peres[racine_y][RANG] += peres[racine_x][RANG];
		}
	}
}

void supprime_mur(Parametres para, Mur tab_mur[][para.colonne], int peres[][2]){
	/*Fonction abattant un mur aléatoirement*/
	int x,y;
	int cote;
	x = rand() % para.ligne;
	y = rand() % para.colonne;
	do{
		cote = rand() % 4;
	}
	while((x == 0 && cote == 1) || (x == (para.ligne - 1) && cote == 3) || (y == 0 && cote == 0) || (y == (para.colonne-1) && cote == 2));
	tab_mur[x][y].tab[cote] = 0;
	if(cote == 0){
		tab_mur[x][y-1].tab[2] = 0;
		FusionRang(peres, para.colonne*x + y, (para.colonne*x + y) - 1);
	}
	if(cote == 1){
		tab_mur[x-1][y].tab[3] = 0;
		FusionRang(peres, para.colonne*x + y, para.colonne*(x-1) + y);
	}
	if(cote == 2){
		tab_mur[x][y+1].tab[0] = 0;
		FusionRang(peres, para.colonne*x + y, (para.colonne*x + y) + 1);
	}
	if(cote == 3){
		tab_mur[x+1][y].tab[1] = 0;
		FusionRang(peres, para.colonne*x + y, para.colonne*(x+1) + y);
	}
}

void supprime_mur_unique(Parametres para, Mur tab_mur[][para.colonne], int peres[][2]){
	/*Fonction abattant un mur de manière aléatoire et faisant en sorte que le chemin reste unique*/
	int x,y;
	int cote;
	x = rand() % para.ligne;
	y = rand() % para.colonne;
	do{
		cote = rand() % 4;
	}
	while((x == 0 && cote == 1) || (x == (para.ligne - 1) && cote == 3) || (y == 0 && cote == 0) || (y == (para.colonne-1) && cote == 2));
	tab_mur[x][y].tab[cote] = 0;
	if(cote == 0 && TrouveCompresse(peres, para.colonne * x + y) != TrouveCompresse(peres, (para.colonne * x + y) - 1)){
		tab_mur[x][y-1].tab[2] = 0;
		if(TrouveCompresse(peres, para.colonne * x + y) != TrouveCompresse(peres, (para.colonne * x + y) - 1)){
			FusionRang(peres, para.colonne*x + y, (para.colonne*x + y) - 1);
		}
		else{
			tab_mur[x][y].tab[cote] = 1;
			tab_mur[x][y-1].tab[2] = 1;
		}
	}
	if(cote == 1 && TrouveCompresse(peres, para.colonne*x + y) != TrouveCompresse(peres, para.colonne*(x-1) + y)){
		tab_mur[x-1][y].tab[3] = 0;
		if(TrouveCompresse(peres, para.colonne * x + y) != TrouveCompresse(peres, para.colonne*(x-1) + y)){
			FusionRang(peres, para.colonne*x + y, para.colonne*(x-1) + y);
		}
		else{
			tab_mur[x][y].tab[cote] = 1;
			tab_mur[x][y-1].tab[3] = 1;
		}
	}
	if(cote == 2 && TrouveCompresse(peres, para.colonne*x + y) !=  TrouveCompresse(peres, (para.colonne*x + y) + 1)){
		tab_mur[x][y+1].tab[0] = 0;
		if(TrouveCompresse(peres, para.colonne * x + y) != TrouveCompresse(peres, (para.colonne*x + y) + 1)){
			FusionRang(peres, para.colonne*x + y, (para.colonne*x + y) + 1);
		}
		else{
			tab_mur[x][y].tab[cote] = 1;
			tab_mur[x][y-1].tab[0] = 1;
		}
	}
	if(cote == 3 && TrouveCompresse(peres, para.colonne*x + y) != TrouveCompresse(peres, para.colonne*(x+1) + y)){
		tab_mur[x+1][y].tab[1] = 0;
		if(TrouveCompresse(peres, para.colonne * x + y) != TrouveCompresse(peres,  para.colonne*(x+1) + y)){
			FusionRang(peres, para.colonne*x + y, para.colonne*(x+1) + y);
		}
		else{
			tab_mur[x][y].tab[cote] = 1;
			tab_mur[x][y-1].tab[1] = 1;
		}
	}
}

void console_affichage(Parametres para, Mur tab_mur[][para.colonne]){
	/*Fonction affichant le labyrinthe sur le terminale*/
	int i,j;
	for(j = 0; j < para.colonne; j++){
		printf("+--");
	}
	printf("+\n");
	for(i = 0; i < para.ligne; i++){
		for(j = 0 ; j < para.colonne; j++){
			if(tab_mur[i][j].tab[0] == 1)
				printf("|  ");
			else
				printf("   ");
		}
		if(tab_mur[i][para.colonne-1].tab[2] == 1)
			printf("|\n");
		else
			printf("\n");
		for(j = 0; j < para.colonne; j++){
			if(tab_mur[i][j].tab[3] == 1)
				printf("+--");
			else
				printf("+  ");
		}
		printf("+\n");
	}
}

void affiche_MLV(Parametres para, Mur tab_mur[][para.colonne]){
	/*Fonction utilisant la lib MLV pour afficher le labyrinthe*/
	int i, j;
	for(i = 0; i < para.ligne; i++){
		for(j = 0; j < para.colonne; j++){
			if(tab_mur[i][j].tab[0] == 1)
				MLV_draw_line(j*400/para.colonne, i*400/para.ligne, j*400/para.colonne, (i+1)*400/para.ligne, MLV_COLOR_WHITE);
			if(tab_mur[i][j].tab[1] == 1)
				MLV_draw_line(j*400/para.colonne, i*400/para.ligne, (j+1)*400/para.colonne, i*400/para.ligne, MLV_COLOR_WHITE);
		}
	}
	MLV_draw_line(0, 400, 400, 400, MLV_COLOR_WHITE);
	MLV_draw_line(400, 0, 400, 400-400/para.ligne, MLV_COLOR_WHITE);
}

void gestion_parametres(int argc, char* argv[], Parametres *para){
	/*Fonction effectuant la gestion des paramètres possible suivant les demandes de l'utilisateur 
	et qui modifie la structure paramètre*/
	int i, j, k, n;
	char ligne[3];
	char colonne[3];
	char graine_char[10];
	char parametre[10];
	char attentes[10];
	char mode[10];
	for(i = 1; i < argc; i++){
		j = 2;
		while(argv[i][j] != '=' && strcmp(parametre, "unique") != 0 && strcmp(parametre, "acces") != 0 && strcmp(parametre, "victor") != 0){
			parametre[j-2] = argv[i][j];
			j++;
			parametre[j-2] = '\0';
		}
		parametre[j-2] = '\0';
		j++;
		if(strcmp(parametre, "taille") == 0){
			while(argv[i][j] != 'x'){
				ligne[j-9] = argv[i][j];
				j++;
			}
			ligne[j-9] = '\0';
			j++;
			while(j < strlen(argv[i])){
				colonne[j-10-strlen(ligne)] = argv[i][j];
				j++;
			}
			colonne[j-10-strlen(ligne)] = '\0';
			para->colonne = atoi(colonne);
			para->ligne = atoi(ligne);
			
		}
		else if(strcmp(parametre, "mode") == 0){
			while(argv[i][j]){
				mode[j-7] = argv[i][j];
				j++;
			}
			mode[j-7] = '\0';
			j++;
			if(strcmp(mode, "texte") == 0)
				para->affichage = 1;
		}
		else if(strcmp(parametre, "graine") == 0){
			for(k = j; k < strlen(argv[i]); k++)
				graine_char[k-j] = argv[i][k];
			para->graine = atoi(graine_char);
		}
		else if(strcmp(parametre, "attente") == 0){
			for(n = j; n < strlen(argv[i]); n++)
				attentes[n-j] = argv[i][n];
			para->attente = atoi(attentes);
		}
		else if(strcmp(parametre, "unique") == 0)
			para->unique = 1;
		else if(strcmp(parametre, "acces") == 0)
			para->acces = 1;
		else if(strcmp(parametre, "victor") == 0)
			para->victor = 1;
		else{
			printf("Probleme de parametre, parametre %s non valide, fin de programme\n", parametre);
			exit(1);
		}
	}
}

/*void affiche_MLV_victorieux(Parametres para, Mur tab_mur[][para.colonne], int tab_vic[3]){
	int k;
	int i, j;
	for(k = 0; k < 3; k++){
		i = ceil(tab_vic[k] / para.colonne);
		j = tab_vic[k] % para.colonne;
		printf("%d %d\n", i, j);
		MLV_draw_filled_rectangle((j*400/para.colonne)+1, (i*400/para.ligne)+1, 400/para.colonne, 400/para.ligne, MLV_COLOR_RED);
	}
}*/



void supprime_mur_acces(Parametres para, int peres[][2], Mur tab_mur[][para.colonne]){
	/*Fonction abattant des murs aléatoirement et qui fait en sorte que toutes les cases du labyrinthe 
	soient accessible*/
	int k, i, j;
	for(k = 0; k < para.ligne * para.colonne; k++){
		if(TrouveCompresse(peres, k) != TrouveCompresse(peres, 0)){
			i = ceil(k / para.colonne);
			j = k % para.colonne;
			if(TrouveCompresse(peres, k - 1) == TrouveCompresse(peres, 0) && j > 0){
				tab_mur[i][j].tab[0] = 0;
				tab_mur[i][j-1].tab[2] = 0;
				FusionRang(peres, k, k-1);
			}
			if(TrouveCompresse(peres, k + 1) == TrouveCompresse(peres, 0) && j < para.colonne - 1){
				tab_mur[i][j].tab[2] = 0;
				tab_mur[i][j+1].tab[0] = 0;
				FusionRang(peres, k, k+1);
			}
			if(TrouveCompresse(peres, k + para.colonne) == TrouveCompresse(peres, 0) && i < para.ligne - 1){
				tab_mur[i][j].tab[3] = 0;
				tab_mur[i+1][j].tab[1] = 0;
				FusionRang(peres, k, k+para.colonne);
			}
			if(TrouveCompresse(peres, k - para.colonne) == TrouveCompresse(peres, 0) && i > 0){
				tab_mur[i][j].tab[1] = 0;
				tab_mur[i-1][j].tab[3] = 0;
				FusionRang(peres, k, k - para.colonne);
			}
		}
	}
}

/*void affiche_UTF8(Parametres para, Mur tab_mur[][para.colonne]){
	char* intersections[2][2][2][2] = 
   	{ {{{" ","╶"},{"-","─"}},{{"╵","└"},{"┘","┴"}}},
     {{{"╷","┌"},{"┐","┬"}},{{"│","├"},{"┤","┼"}}} };
     int i, j;
     printf("%s", intersections[0][0][0][1]);
     for(j = 0; j < para.colonne; j++){
     	if(tab_mur[0][j].tab[2])
     		printf("%s", intersections[1][0][1][1]);
     	else
     		printf("%s", intersections[0][0][1][1]);
     }
     printf("%s\n", intersections[1][0][1][0]);
     for(i = 1; i < para.ligne; i++){
     	if(i > 1)
     		printf("%s", intersections[]);
     	for(j = 0; j < para.colonne ; j++){
     	}
     }
}*/



int main(int argc, char* argv[]){
	int x, y;
	Parametres para = init_parametre();
	gestion_parametres(argc, argv, &para);
	if(para.graine == -1)
		srand(time(NULL));
	else
		srand(para.graine);
	int peres[para.ligne*para.colonne][2];
	Mur tab_mur[para.ligne][para.colonne];
	init_mur(para, tab_mur);
	Initialisation(peres, para);
	if(para.affichage == 0){
		MLV_create_window("Labyrinthe", "", 600, 600);
		while(TrouveCompresse(peres, 0) != TrouveCompresse(peres, (para.ligne * para.colonne) - 1)){
			if(para.unique = 0)
				supprime_mur(para, tab_mur, peres);
			else
				supprime_mur_unique(para, tab_mur, peres);

			affiche_MLV(para, tab_mur);
			MLV_actualise_window();
			MLV_clear_window(MLV_COLOR_BLACK);
			MLV_wait_milliseconds(para.attente);
		}
		if(para.acces == 1)
			supprime_mur_acces(para, peres, tab_mur);
		affiche_MLV(para, tab_mur);
		MLV_actualise_window();
		MLV_wait_mouse(&x, &y);
	}
	else{
		while(TrouveCompresse(peres, 0) != TrouveCompresse(peres, (para.ligne * para.colonne) - 1)){
			if(para.unique == 0)
				supprime_mur(para, tab_mur, peres);
			else
				supprime_mur_unique(para, tab_mur, peres);
		}
		if(para.acces == 1)
			supprime_mur_acces(para,peres,tab_mur);
		console_affichage(para, tab_mur);
	}
	return 1;
}   