#ifndef TERMFUNCS_H
#define TERMFUNCS_H

#define startbuff() printf("\033[?1049h\033[H")
#define clear() printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))
#define godownx(x) printf("\033[%dB", (x))
#define goupx(x) printf("\033[%dA", (x))
#define goleftx(x) printf("\033[%dD", (x))
#define gorightx(x) printf("\033[%dC", (x))
#define delete(x) printf("\033[%dD \033[%dD", 1, 1)
#define display() printf("\n")
#define endbuff() printf("\033[?1049l")



/* Initialize new terminal i/o settings */
void initTermios(int echo);

void resetTermios(void);

int getch_(int echo);

int getch(void);

int getche(void);

int kbhit();
#endif