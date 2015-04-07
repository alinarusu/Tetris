/**
 * Programarea Calculatoarelor 2011-2012
 * Tema #3 - Joc de Tetris
 *
 * 
 * Autor: Rusu Alina-Nicoleta
 * Grupa: 311CC
 */

#include<ncurses.h>
#include<sys/time.h>
#include<stdlib.h>
#include<string.h>

#define dep
#define pozx 17
#define pozy 6

#define height  20
#define width	14

// figura de joc
typedef struct fig {
	char type;
	int orientare;
	int col_pair;
	int x,y;
	int elem[4][4];
} fig;

// ecranul
typedef struct ecran {
	WINDOW *w;
	int elem[height+6][width+1];
	int stanga;
	int jos;
} ecran;

int global_time;

//genreeaza o figura
void genelem(fig *f);

//pastreaza figura in block
void combined(ecran *ec, fig *f, int *sc, int *level);

//interschimba 2 valori
void swap(int *a, int *b);

//testeaza dak poate fi pusa o figura
int isalive(ecran *ec, fig *f);

//afiseaza ecranul
void putecran(ecran *ec);

// roteste figura
void rotate(ecran *ec, fig *f);

// deplaseaza o figura
int depl(ecran *ec, int incx, int incy, fig *f);

// combina figura cu ecranul
void combined(ecran *ec, fig *f, int *sc, int *level);

void genelem(fig *f) {
	int i;
	int j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			f->elem[i][j] = 0;

	f->elem[2][2] = 1;

	switch(f->type) {
		case 'B' : 	
			f->elem[1][1] = 1;
			f->elem[2][1] = 1;
			f->elem[1][2] = 1;
			return;

		case 'I' : 	
			if(f->orientare == 0) {
				f->elem[2][0] = 1;
				f->elem[2][1] = 1;
				f->elem[2][3] = 1;
				return;
			}

			f->elem[0][2] = 1;
			f->elem[1][2] = 1;
			f->elem[3][2] = 1;
			return;
	
		case 'S' :	
			f->elem[2][3] = 1;
			f->elem[1][1] = 1;
			f->elem[1][2] = 1;
			break;
			
		case 'Z' :	
			f->elem[2][1] = 1;
			f->elem[1][3] = 1;
			f->elem[1][2] = 1;
			break;
			
		case 'T' :	
			f->elem[2][1] = 1;
			f->elem[2][3] = 1;
			f->elem[1][2] = 1;
			break;
			
		case 'L' :	
			f->elem[2][1] = 1;
			f->elem[2][3] = 1;
			f->elem[1][1] = 1;
			break;
			
		case 'J' : 	
			f->elem[2][1] = 1;
			f->elem[2][3] = 1;
			f->elem[1][3] = 1;
			break;
	}

	int k;
	int aux[4][4];
	k = f->orientare;
	while(k--) {
		for(i=1;i<4;i++)
			for(j=1;j<4;j++)
				aux[i][j] = f->elem[j][4-i];
		for(i=1;i<4;i++)
			for(j=1;j<4;j++)
				f->elem[i][j] = aux[i][j];
	}
}

void swap(int *a, int *b) {
	int c = *a;
	*a = *b;
	*b = c;
}

int isalive(ecran *ec, fig *f) {
	int i,j;
	genelem(f);

	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			if(f->elem[i][j]) {
				if(f->x + i <0)
					return 0;
				if(f->x + i > height)
					return 0;
				if(f->y + j < 0)
					return 0;
				if(f->y + j > width)
					return 0;
				if(ec->elem[f->x + i][f->y + j])
					return 0;
			}
	return 1;
}

void putbloc(ecran *ec, int x, int y, int col) {
	attron(COLOR_PAIR(col));
	move(ec->jos - x,ec->stanga + 2*y);
	addch(' ');
	addch(' ');
	attroff(COLOR_PAIR(col));
}

void elimina(ecran *ec, fig *f) {
	genelem(f);
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			if(f->elem[i][j]) {
				putbloc(ec,f->x+i,f->y+j,0);
			}
	move(0,0);
	refresh();
}

void pune(ecran *ec, fig *f) {
	genelem(f);
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			if(f->elem[i][j]) {
				putbloc(ec,f->x+i,f->y+j,f->col_pair);
			}
	move(0,0);
	refresh();
}

void rotate(ecran *ec, fig *f) {
	int orient = f->orientare;
	switch(f->type) {
		case 'B' : 	
			break;

		case 'I' : 	
		case 'S' :
		case 'Z' :	
			f->orientare = (f->orientare+1)%2;
			break;
				
		case 'T' :
		case 'L' :
		case 'J' : 	
			f->orientare = (f->orientare+1)%4;
			break;
	}

	if(isalive(ec,f)) {
		swap(&orient,&f->orientare);
		elimina(ec,f);
		swap(&orient,&f->orientare);
		pune(ec,f);		
	}
	else {
		f->orientare = orient;
		return;
	}
}

int depl(ecran *ec, int incx, int incy, fig *f) {
	f->x += incx;
	f->y += incy;
	if(isalive(ec,f)) {
		f->x -= incx;
		f->y -= incy;
		elimina(ec,f);

		f->x += incx;
		f->y += incy;
		pune(ec,f);

		return 1;
	}
	f->x -= incx;
	f->y -= incy;
	return 0;
}

void combined(ecran *ec, fig *f, int *sc, int *level) {
	genelem(f);
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			if(f->elem[i][j]) {
				ec->elem[f->x+i][f->y+j] = f->col_pair;
			}
	int ok;
	for(i=0;i<height+6;i++) {
		ok =0;
		for(j=0;j<width;j++)
			if(ec->elem[i][j])
				ok++;
		if(ok == width)	{
			int k;
			for(k=i--;k<height+5;k++)
				for(j=0;j<width;j++)
					ec->elem[k][j] = ec->elem[k+1][j];
			*sc += *level*10;
			int p;
			p = 5;
			for(k=0;k<*level;k++)
				p*= 10*10;
			if(*sc >= p)
				*level = *level + 1;				
		}
	}
}
		
int diftime(struct timeval *a, struct timeval *b) {
	int x,y;
	x = a->tv_sec %3600 *100 + a->tv_usec / 10000;
	y = b->tv_sec %3600 *100 + b->tv_usec / 10000;
	if(x > y)
		return (x - y);
	return (y - x);
}

int placetime(struct timeval *ok, int level) {
	move(1,60);
	init_pair(2,COLOR_BLUE,COLOR_RED);
	int a=2;
	attron(COLOR_PAIR(a));
	struct timeval tv;
	gettimeofday(&tv,NULL);
	printw("%02d:%02d:%02d.%03d",	tv.tv_sec%86400/3600,\
					tv.tv_sec%3600/60,\
					tv.tv_sec%60,\
					tv.tv_usec/1000);

	int x,y;
	x = tv.tv_sec %3600 *1000 + tv.tv_usec / 1000;
	y = ok->tv_sec %3600 *1000 + ok->tv_usec / 1000;
	if(x - y > 700 / level ) {
		ok->tv_sec = tv.tv_sec;
		ok->tv_usec= tv.tv_usec;
		attroff(COLOR_PAIR(a));
		move(0,0);
		refresh();
		return 1;
	}
	move(5,60);
	if ( global_time == 0 ) 
		global_time = tv.tv_sec;
	tv.tv_sec -= global_time;
	printw("%02d:%02d:%02d",	tv.tv_sec%86400/3600,\
					tv.tv_sec%3600/60,\
					tv.tv_sec%60);

	move(7,60);

	printw("%7d", level);

	attroff(COLOR_PAIR(a));
	move(0,0);
	refresh();
	return 0;
}

void placescore(int sc) {
	move(3,60);

	attron(COLOR_PAIR(2));
	printw("%09d",sc);
	move(0,0);
	refresh();
	attroff(COLOR_PAIR(2));
}

void placepreview(ecran *ec,fig *f) {
	f->x = 10;
	f->y = 20;
	pune(ec,f);
}

void createfig(fig *f) {	
	switch(rand()%6) {
		case 0: 
			f->type = 'B'; 	
			break;
		case 1: 
			f->type = 'I'; 	
			break;
		case 2: 
			f->type = 'S';	
			break;
		case 3: 
			f->type = 'Z';  
			break;
		case 4: 
			f->type = 'J';  
			break;
		case 5: 
			f->type = 'L';  
			break;
		case 6: 
			f->type = 'T';  
			break;
		default : 
			f->type = 'B';
	}	

	f->x = pozx;
	f->y = pozy;
	f->col_pair = rand()%6+1;
	f->orientare = 0;
}

void createplayfig(ecran *ec,fig *f, fig *g) {
	f->type = g->type;
	f->x = pozx;
	f->y = pozy;
	f->col_pair = g->col_pair;
	f->orientare = 0;
	elimina(ec,g);
	createfig(g);
}

void createec(ecran *ec) {
	ec -> w = initscr();
	ec -> stanga 	= 20;
	int i,j;
	for(i=0;i<height+6;i++)
		for(j=0;j<width;j++)
			ec -> elem[i][j] = 0;
	for(i=0;i<height+6;i++)
		ec-> elem[i][width] = 1;

	ec -> jos = 22;
}

void initializare(ecran *ec) {
int i,j;
for(i=-1;i<=height+1;i++)
	for(j=-1;j<=width;j++)
		if(i==-1 || j==-1 || i==height+1 || j==width) {
			move(ec->jos-height+i, ec->stanga+2*j);
			if(j==-1)
				addch(' ');
			else
				addch('#');
			if(j != width)
				addch('#');
		}
refresh();
}

void putecran(ecran *ec) {
	int i,j;
	for(i=0;i<height;i++)
		for(j=0;j<width;j++) {
				putbloc(ec,i,j,ec->elem[i][j]);
			}

	move(0,0);	
	refresh();
}


void scoreupdate(int sc) {
	initscr();
	char nume[256];
	nodelay(stdscr,FALSE);
	clear();
	echo();
	printw("scorul tau este %d\n",sc);
	printw("nume : ");
	scanw("%s",&nume);
	
	FILE *f;
	f = fopen("score.txt","rb");
	int exsc;
	if( f != NULL ) {
		fread(&exsc,1,sizeof(exsc),f);
		fclose(f);
	}
	else {
		exsc = -1;
	}

	if(exsc < sc) {
		f = fopen("score.txt","wb");
		fwrite(&sc,1,sizeof(sc),f);
		fwrite(nume,1,strlen(nume),f);
		fclose(f);	
	}
	
	endwin();	
}

void swapc(char *a, char *b) {
	char c;
	c = *a;
	*a = *b;
	*b = c;
}

void swapf(ecran *ec,fig *f,fig *g) {
	g->x = f->x;
	g->y = f->y;
	if(isalive(ec,g)) {
		int cp = f->col_pair;
		f->col_pair = 0;
		depl(ec,0,0,f);
		f->col_pair = g->col_pair;

		g ->col_pair = 0;
		placepreview(ec,g);
		g->col_pair = cp;
		
		swap(&g->orientare,&f->orientare);
		swapc(&g->type,&f->type);
		depl(ec,0,0,f);
	}
}

int play(int level) {
	global_time = 0;
	ecran ec;
	int sc;
	sc = 0;
	createec(&ec);	
	cbreak();
	noecho();
	clear();
	nodelay(stdscr,TRUE);
	fig playfig, waitfig;
	struct timeval ok;
	
	gettimeofday(&ok,NULL);
		
	createfig(&playfig);
	createfig(&waitfig);

	int c;
	start_color();
	initializare(&ec);

	pune(&ec,&playfig);
	c = 0;
	while(c!='q')
	{
		placescore(sc);
		placepreview(&ec,&waitfig);
		switch(c)
		{
			case KEY_DOWN:
			case 's' : 
				depl(&ec,-1,0,&playfig); 
				break;
			case KEY_LEFT:		
			case 'a' : 
				depl(&ec,0,-1,&playfig); 
				break;
			case KEY_RIGHT:
			case 'd' : 
				depl(&ec,0,1,&playfig); 
				break;
			case KEY_UP:					// rotire
			case 'r' : 
				rotate(&ec,&playfig); 
				break;		// rotire
			case 'n' : 
				combined(&ec,&playfig,&sc,&level);
				createplayfig(&ec,&playfig,&waitfig);
				putecran(&ec);
				if(isalive(&ec,&playfig) == 0)
					goto exit;
				pune(&ec,&playfig);
				c = (char)ERR;
				break;	
			case ' ' :	
				swapf(&ec,&playfig,&waitfig);
				break;
			case 'p' :	
				nodelay(stdscr,FALSE);
				while(getch()!='p');
				nodelay(stdscr,TRUE);
				break;
			default : 
				if(placetime(&ok, level))
					if(depl(&ec,-1,0,&playfig) == 0) {
						c = 'n';
					}		
		}

		if(c!='n')
			c = getch();
	}

exit:	
	endwin();
	scoreupdate(sc);
	return 0;
}


void putmenu(int x) {
	clear();
	int i;
	int a;
	char str[256];
	char text[4][256];
	strcpy(text[0],"Start Game");
	strcpy(text[1],"High Score");
	strcpy(text[2],"Set Starting Level");
	strcpy(text[3],"Exit");
	str[0] = ' ';
	str[1] = ' ';
	str[2] = 0;

	for(i=0;i<4;i++) {
		if(i == x ) {
			a = 2;
			str[0] = '#';	
		}
		else {
			str[0] = ' ';
			a = 1;
		}

		attron(COLOR_PAIR(a));
		strcat(str,text[i]);
		mvaddstr(2*i+2,2,str);
		attroff(COLOR_PAIR(a));
		str[2] = 0;
	}
}

void printscore()
{
	char nume[256];
	int sc;
	FILE *f = fopen("score.txt","rb");
	if(f == NULL) {
		printw("no score");
		return;
	}

	fread(&sc,1,sizeof(sc),f);
	nume[fread(nume,1,sizeof(nume),f)] = 0;
	printw("%s %d",nume,sc);
}

int main() {
	
	WINDOW *w = initscr();
	keypad(w, TRUE);
	nodelay(stdscr,FALSE);
	noecho();
	curs_set(0);
	start_color();
	init_pair(1,COLOR_RED,COLOR_BLUE);
	init_pair(2,COLOR_BLUE,COLOR_RED);
	init_pair(3,COLOR_RED,COLOR_GREEN);
	init_pair(4,COLOR_RED,COLOR_CYAN);
	init_pair(5,COLOR_RED,COLOR_MAGENTA);
	init_pair(6,COLOR_RED,COLOR_YELLOW);
	int c='w';
	int x;
	int level;
	level = 1;
	x = 0;
	while(1) {	
		
		switch(c) {
			case KEY_UP:
			case 'w':	
				if(x > 0)
					x--;
				break;
			case KEY_DOWN:		
			case 's':	
				if(x<3)
					x++;
				break;
			case 10:
				switch(x) {	
					case 0: 
						endwin();
						play(level);
						initscr();
						break;
					case 1:
						clear();
						printscore();
						getch();
						break;
					case 2:
						clear();
						printw("Set level, Default %d\n",level);
						echo();					
						do {
							scanw("%d",&level);
						} while(0>=level);
						noecho();
						break;
					case 3:		
						
						goto exit;
					}
				break;
			case 'q' : 
				goto exit;
		}

		putmenu(x);
		c = getch();
	}				
	exit:
	endwin();
	return 0;
}
