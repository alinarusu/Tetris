/**
 * Programarea Calculatoarelor 2011-2012
 * Tema #4 - Joc de Tetris
 *
 * Exemplu de utilizare a bibliotecii curses si implementarea functiei pentru
 * obtinerea tastei apasate.
 *
 * Autor: Adrian Scoica <adrian.sc@rosedu.org>
 *
 */

#include <curses.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define NO_KEY_PRESSED (-1)

/* Functia intoarce codul caracterului apasat sau NO_KEY_PRESSED
 * daca niciun caracter nu a fost apasat.
 */
int get_key_pressed() {
  int ch = getch();
  if (ch != ERR) {
    return ch;
  }
  return NO_KEY_PRESSED;   
}

int main()
{
  /* Similar cu exemplul 1, trebuie initializat ecranul. */
  WINDOW* wnd = initscr();

  /* Urmatoarele variabile detin mesaje predefinite, pentru a fi nevoie sa le
   * scrieti de mana in program. */
  char msg_quit[] = "Pentru iesire apasati q";
  char msg_key[] = "este ultima tasta apasata";

  /* Aflare numarul de linii si coloane ale terminalului.
   * Atentie! getmaxyx este un macro, nu o functie! De aceea parametrilor li se
   * modifica valoarea, daca era vorba de o functie acest lucru nu ar fi fost
   * posibil fara sa ii transmitem prin adresa! */
  int nrows, ncols;
  getmaxyx(wnd, nrows, ncols);

  /* Cateva setari generale, explicate pe larg in exemplu1.c */
  curs_set(0);
  clear();
  noecho();
  cbreak(); 
  
  /* Specificam ca citirea unui caracter sa se faca in mod neblocant. Cu alte
   * cuvinte, interfata grafica nu se va bloca in asteptarea apasarii unei
   * taste, ci animatia se va putea misca in continuare. */
  nodelay(wnd, TRUE);

  /* Afisam doua mesaje. Nu uitati sa apelati refresh() dupa ce ati facut un set
   * de modificari pe care doriti sa le afisati pe ecran, altfel schimbarile nu
   * s-ar trimite in terminal. */
  mvaddstr(10, 10, msg_quit);
  mvaddstr(20, 20, msg_key); 
  refresh();

  /* Timpul la care a avut loc ultima mutare a utilizatorului, in secunde. */
  time_t last_move = 0;
  /* Timpul dintre doua mutari ale utilizatorului, in secunde. */
  time_t interval = 1;
  /* Timpul curent. */
  time_t current_time;

  /* Coordonatele unui bloc care cade. */
  int blocky = -1;
  int blockx = 60;

  /* Ultimul caracter citit de la tastatura. */
  char p = ' ';
  while (1) {                    
    p = get_key_pressed();
    if (p != NO_KEY_PRESSED) {

      /* Daca s-a apasat o tasta intre timp, atunci verifica daca nu e Quit. */
      if (tolower(p) == 'q') {
        break;
      }  

      /* Afiseaza tasta apasata la o pereche de coordonate. */
      move(20, 18);
      addch(p);
      refresh();
    }

    /* Daca este momentul pentru un nou turm, efectueaza miscarile necesare. */
    current_time = time(NULL);
    if (current_time - last_move > interval) {
      /* Notam cand s-a petrecut ultima mutare. */
      last_move = current_time;

      /* Stergem blocul de la coordonatele initiale. */
      move(blocky, blockx);
      addch(' ');
      /* Il mutam in jos, iar daca a cazut sub limita, cade din nou de sus. */
      blocky = (blocky + 1 < nrows) ? blocky + 1 : 0;
      /* Afisam blocul in noua pozitie. */
      move(blocky, blockx);
      addch('#');
    }
  }

  /* Nu uitati sa distrugeti fereastra inainte de iesirea din aplicatie. */
  endwin();
  return 0;
}

