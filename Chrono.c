#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define NBMAX 30

// These are functionalities for our future chronometre


typedef struct 
{
    int etat_chrono;
    int duree_totale;
    int duree_avert;
    int duree_pause;
    int indice_de_tour;
    int nb_tour;
    int tab_stat[NBMAX];
}Chronometre;

int intervalle_ms(struct timeval debut, struct timeval fin){
    int ms;
    ms = (fin.tv_usec - debut.tv_usec)/1000;
    ms += (fin.tv_sec - debut.tv_sec)*1000;
    return ms;
}

int nb_ms_vers_centiemes(int nb_ms){
    return (nb_ms / 10) % 100;
}

int nb_ms_vers_secondes(int nb_ms){
    return (nb_ms / 1000) % 60;

}

int nb_ms_vers_minutes(int nb_ms){
    return (nb_ms / (1000 * 60)) % 60;
    
}

int nb_ms_vers_heures(int nb_ms){
    return (nb_ms / (1000 * 60 * 60)) % 99;
}

Chronometre initialiser_chronometre(){
    Chronometre chrono;
    chrono.etat_chrono = 1;
    chrono.duree_totale = 0;
    chrono.duree_avert = 25000;
    chrono.duree_pause = 0;
    chrono.indice_de_tour = 0;
    chrono.nb_tour = 0;

    return chrono;    
}

void afficher_interface(Chronometre chrono){
    mvprintw(0, (COLS/2) - 9, "== Chronometre ==");

    for (int i = LINES-11, j = 0; j < chrono.nb_tour && i >= 1; i--, j++){
        mvprintw(i, (COLS/2) - 16 - snprintf(NULL, 0, "%i", chrono.indice_de_tour-j), "Tour %d : "
        "%2d : %2d : %2d : %2d",      chrono.indice_de_tour - j,
                                        nb_ms_vers_heures(chrono.tab_stat[j]),
                                        nb_ms_vers_minutes(chrono.tab_stat[j]),
                                        nb_ms_vers_secondes(chrono.tab_stat[j]),
                                        nb_ms_vers_centiemes(chrono.tab_stat[j]));
    }
    mvprintw(LINES-9, (COLS/2)-24, "Avertissement : "
    "%2d : %2d : %2d : %2d", nb_ms_vers_heures(chrono.duree_avert),
                                        nb_ms_vers_minutes(chrono.duree_avert),
                                        nb_ms_vers_secondes(chrono.duree_avert),
                                        nb_ms_vers_centiemes(chrono.duree_avert));

    mvprintw(LINES-10, (COLS/2)-8, "%2d : %2d : %2d : %2d", nb_ms_vers_heures(chrono.duree_totale),
                                        nb_ms_vers_minutes(chrono.duree_totale),
                                        nb_ms_vers_secondes(chrono.duree_totale),
                                        nb_ms_vers_centiemes(chrono.duree_totale));
    mvhline(LINES-8, 0, '-', COLS);
    mvprintw(LINES-7, 0, "Espace : Lancer / Mettre en pause");
    mvprintw(LINES-6, 0, "r      : Reinitialiser");
    mvprintw(LINES-5, 0, "t      : Marquer tour");
    mvprintw(LINES-4, 0, "F1/F2  : Incrementer / Decrementer heure avertissement");
    mvprintw(LINES-3, 0, "F3/F4  : Incrementer / Decrementer minute avertissement");
    mvprintw(LINES-2, 0, "F5/F6  : Incrementer / Decrementer seconde avertissement");
    mvprintw(LINES-1, 0, "q      : Quitter");
}

void init_couleurs(){
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_YELLOW);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
}

void afficher_flash(int n){
    
    
    attrset(COLOR_PAIR(n));
    for (int j = 0; j<=LINES; j++){
        move(j, 0);
        for (int i = 0; i<=COLS; i++){
            addch(' ');
        }
    }
}

void ajouter_tour(Chronometre *chrono){
    for (int i = chrono->nb_tour; i > 0; i--){
        chrono->tab_stat[i] = chrono->tab_stat[i-1];
    }
    chrono->tab_stat[0] = chrono->duree_totale;
    chrono->indice_de_tour++;
    if (chrono->nb_tour < NBMAX)
        chrono->nb_tour++;
    
}

void incrementationflash(Chronometre *chrono, int n){
    chrono->duree_avert += n;
    if (chrono->duree_avert < 0)
        chrono->duree_avert -= n;
}

void boucle_chrono(){
    struct timeval debut, fin, debpause;
    Chronometre chrono = initialiser_chronometre();
    int c;
    int memLines = LINES;
    int memCols = COLS;
    int flash = 2;
    int n=0;
    afficher_interface(chrono);
    nodelay(stdscr, TRUE);
    gettimeofday(&debut, NULL);
    gettimeofday(&debpause, NULL);
    init_couleurs();

    while((c = getch())){
        if (LINES <= 14 || COLS <= 58)
            c = 'q';
        if (COLS != memCols || LINES != memLines){
            clear();
            memCols = COLS;
            memLines = LINES;
        }

        if (chrono.duree_totale >= chrono.duree_avert){
            if (n % 70000 == 0){
                afficher_flash(flash);
                flash = (flash%3)+1;
            }
            if (flash != 4)
                n++;
        }
        
        else if (chrono.duree_totale < chrono.duree_avert && flash != 4){
            flash = 4;
            n = 0;
            afficher_flash(flash);
            refresh();
        }
        
        gettimeofday(&fin, NULL);
        if (c == ' '){
            chrono.etat_chrono = (chrono.etat_chrono+1)%2;
            if (chrono.etat_chrono){
                gettimeofday(&debpause, NULL);
            }
            else {
                chrono.duree_pause += intervalle_ms(debpause, fin);
            }
        }

        else if (c == 'q' || c == 'Q'){
            endwin();
            return;
        }

        else if (c == 'r' || c == 'R'){
            int etat = chrono.etat_chrono;
            chrono = initialiser_chronometre();
            chrono.etat_chrono = etat;
            gettimeofday(&debut, NULL);
            gettimeofday(&debpause, NULL);
            clear();
            afficher_interface(chrono);
            flash = 4;
            n = 0;
            afficher_flash(flash);
        }

        else if (c == 't' || c == 'T')
            ajouter_tour(&chrono);

        else if (c == KEY_F(1))
            incrementationflash(&chrono, 3600000);
        
        else if (c == KEY_F(2))
            incrementationflash(&chrono, -3600000);

        else if (c == KEY_F(3))
            incrementationflash(&chrono, 60000);
        
        else if (c == KEY_F(4))
            incrementationflash(&chrono, -60000);
        
        else if (c == KEY_F(5))
            incrementationflash(&chrono, 1000);
        
        else if (c == KEY_F(6))
            incrementationflash(&chrono, -1000);

        if (!chrono.etat_chrono){
            chrono.duree_totale = intervalle_ms(debut, fin) - chrono.duree_pause;
        }
        afficher_interface(chrono);
    }
}

void afficher_duree(int y, int x, int nb_ms){
    initscr();

    move(y,x);
    printw("%d\n",nb_ms);

    refresh();
    getch();
    endwin();
}

int main(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    boucle_chrono();
    return 0;
}