#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "mpi.h"
#define REZERWA_WEGLA 100
#define ILOSC_MIEJSC 100
#define ZATRZYMAJ_POCIAG 1
#define NIE_ZATRZYMAJ_POCIAGU 0
#define STACJA 1
#define START 2
#define PRZEJAZD 3
#define KONIEC_PRZEJAZDU 4
#define KOLIZJA 5
#define PRZEKROCZENIE_MIEJSC 6
#define DODAJ_WEGIEL 5000
int wegiel = 5000;
int LADUJ=1, NIE_LADUJ=0;
int liczba_procesow;
int nr_procesu;
int ilosc_pociagow;
int ilosc_torow=4;
int ilosc_pasazerow;
int ilosc_zajetych_torow=0;
int tag=1;
int wyslij[2];
int odbierz[2];
MPI_Status mpi_status;
void Wyslij(int nr_pociagu, int stan) 
{
wyslij[0]=nr_pociagu;
wyslij[1]=stan;
MPI_Send(&wyslij, 2, MPI_INT, 0, tag, MPI_COMM_WORLD);
sleep(1);
}
void Stacja(int liczba_procesow){
int nr_pociagu,status;
ilosc_pociagow= liczba_procesow - 1;
printf("Halo, Witam serdecznie, tu wieża kontrolna stacji \n");
if(rand()%2==1){
printf("Mamy piekna pogode. Tory są suche i idealne to bezpiecznej jazdy. Życzymy przyczepnosci\e\n");
}
else{
printf("Niestety pogoda jest fatalna. Przyczepnosc torów jest fatalna do jazdy\n");
}
printf("Zyczymy Panstwu, przyjemnej podrozy \n \n \n");
printf("Dysponujemy %d wolnymi torami\n", ilosc_torow);
sleep(2);
while(ilosc_torow<=ilosc_pociagow){
MPI_Recv(&odbierz,2,MPI_INT,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD, &mpi_status); //odbieram od kogokolwiek
nr_pociagu=odbierz[0];
status=odbierz[1];
if(status==1){
printf("Pociąg %d stoi na wolnym torze\n", nr_pociagu);
}
if(status==2){
printf("Pociag %d prosi o pozwolenie na start z toru nr %d\n", nr_pociagu, ilosc_zajetych_torow);
ilosc_zajetych_torow--;
}
if(status==3){
printf("Pociag %d jedzie\n", nr_pociagu);
}
if(status==4){
if(ilosc_zajetych_torow<ilosc_torow){
ilosc_zajetych_torow++;
MPI_Send(&LADUJ, 1, MPI_INT, nr_pociagu, tag, MPI_COMM_WORLD);
}
else{
MPI_Send(&NIE_LADUJ, 1, MPI_INT, nr_pociagu, tag, MPI_COMM_WORLD);
}
}
if(status==5){
ilosc_pociagow--;
printf("Ilosc pociagow %d\n", ilosc_pociagow);
}
} 
printf("KONIEC DZIAŁANIA PROGRAMU:)\n");
}
void Pociag(){
int stan,suma,i;
stan=PRZEJAZD; 
while(1){
if(stan==1){
if(rand()%2==1){
stan=START;
wegiel=DODAJ_WEGIEL;
ilosc_pasazerow = rand()%125;
if(ilosc_pasazerow > 100)

{
 printf("Przekroczona liczba pasazerow. Podrozujesz na wlasne ryzyko!  \n");
	
}

printf("Prosze o pozwolenie na start, pociag %d\n",nr_procesu);
Wyslij(nr_procesu,stan);
}
else{
Wyslij(nr_procesu,stan);
}
}
else if(stan==2){
printf("Wyjechalem, pociag %d\n",nr_procesu);
stan=PRZEJAZD;
Wyslij(nr_procesu,stan);
}
else if(stan==3){
wegiel-=rand()%500; 
if(wegiel<=REZERWA_WEGLA){
stan=KONIEC_PRZEJAZDU;
printf("Prosze o pozwolenie na wjazd na stacje\n");
Wyslij(nr_procesu,stan);
}
else{
for(i=0; rand()%10000;i++);
}
}
else if(stan==4){
int temp;
MPI_Recv(&temp, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &mpi_status);
if(temp==LADUJ){
stan=STACJA;
printf("Dojechalem, pociag %d\n", nr_procesu);
}
else
{
wegiel-=rand()%500;
if(wegiel>0){
Wyslij(nr_procesu,stan);
}
else{
stan=KOLIZJA;
printf("WYKOLEJENIE!!!!\n");
Wyslij(nr_procesu,stan);
return;
}
}
}
}
}
int main(int argc, char *argv[])
{
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD,&nr_procesu);
MPI_Comm_size(MPI_COMM_WORLD,&liczba_procesow);
srand(time(NULL));
if(nr_procesu == 0) 
Stacja(liczba_procesow);
else 
Pociag();
MPI_Finalize();
return 0;
}
