#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

// Programme qui utilise 

/*int main(){
    struct timeval temps_debut, temps_fin;
    int sec, usec;
    gettimeofday(&temps_debut, NULL);
    gettimeofday(&temps_fin, NULL);
    sec = temps_fin.tv_sec - temps_debut.tv_sec;
    usec = temps_fin.tv_usec - temps_debut.tv_usec;
}*/

// 1



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

void chronosimple(){
    struct timeval debut, fin;
    int nbms;
    gettimeofday(&debut, NULL);
    while(1){
        gettimeofday(&fin, NULL);
        nbms = intervalle_ms(debut, fin);
        printf("%d : %d : %d : %d\n", nb_ms_vers_heures(nbms),
                                      nb_ms_vers_minutes(nbms),
                                      nb_ms_vers_secondes(nbms),
                                      nb_ms_vers_centiemes(nbms));
        usleep(500000);
    }
}

void chronomoyen(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    struct timeval debut, fin, debpause;
    int nbms = 0;
    int tempspause = 0;
    int pause = 1;
    int c;
    nodelay(stdscr, TRUE);
    gettimeofday(&debut, NULL);
    gettimeofday(&debpause, NULL);

    mvprintw(0, 0, "%d : %d : %d : %d\n", nb_ms_vers_heures(nbms),
                                        nb_ms_vers_minutes(nbms),
                                        nb_ms_vers_secondes(nbms),
                                        nb_ms_vers_centiemes(nbms));

    while((c = getch())){
        gettimeofday(&fin, NULL);
        if (c == ' '){
            pause = (pause+1)%2;
            if (pause){
                gettimeofday(&debpause, NULL);
            }
            else {
                tempspause += intervalle_ms(debpause, fin);
            }
        }

        if (!pause){
            nbms = intervalle_ms(debut, fin) - tempspause;
            mvprintw(0, 0, "%d : %d : %d : %d\n", nb_ms_vers_heures(nbms),
                                        nb_ms_vers_minutes(nbms),
                                        nb_ms_vers_secondes(nbms),
                                        nb_ms_vers_centiemes(nbms));
        }
    }
}

int main(int argc, char *argv[]){
    if (argc > 1){
        switch (argv[1][0])
        {
        case '1':
            chronosimple();
            break;
        
        case '2':
            chronomoyen();
        
        default:
            break;
        }
    }
    //chronosimple();
    chronomoyen();
    return 0;
}