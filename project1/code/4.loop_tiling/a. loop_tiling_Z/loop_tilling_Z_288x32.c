#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define N 288 /* frame dimension for QCIF format */
#define M 352 /* frame dimension for QCIF format */
#define filename "akiyoy.yuv"
#define finalfilename "akiyo_tilling_y.yuv"

#define jTile 32

/* code for armulator*/
int current_y[N][M];
int temparray[N+2][M+2];
int newtemparray[N+2][M+2];
int i,j,jj;

/*O pinakas me ton opoion tha kanoume to filtrarisma*/
// int highPassFilter [3][3] = {
//                                {-1, -1, -1} ,
//                                {-1,  8, -1} ,
//                                {-1, -1, -1}
//                              };


/*O 2d pinakas curent_y periexei thn times twn pixel (apo 0 ews 255) sthn antistoixh thesh*/
void read()
{
  FILE *frame_c;
  if((frame_c=fopen(filename,"rb"))==NULL)
  {
    printf("current frame doesn't exist\n");
    exit(-1);
  }

  for(i=0;i<N;i++)
    for(j=0;j<M;j++)
      current_y[i][j]=fgetc(frame_c); //Me thn fgetc() pairnei kathe fora to epomeno pixel
  fclose(frame_c);
}

void filtering(){

/*Arxikopoiei enan pinaka me 2 parapanw sthles kai grammes*/
  for (jj=0; jj<M+2; jj+=jTile)
  	for(i=0;i<N+2;i++)
		  for(j=jj;j<jj+jTile;j++)
  			temparray[i][j]=0;

/*Etsi exoume enan pinaka san ton arxiko mono pou ta oria toy exoyn epektathei kata 1 pros kathe kateuthunsh*/
/*Oi grammes/ sthles pou prostethkan sta akra einai gemismenes me 0*/
  for (jj=1; jj<M+1; jj+=jTile)
    for(i=1;i<N+1;i++)
      for(j=jj;j<jj+jTile;j++)
  			temparray[i][j]=current_y[i-1][j-1];

/*Edw dinetai to filtrarisma me ton 2d pinaka highPassFilter*/
  for (jj=1; jj<M+1; jj+=jTile)
    for(i=1;i<N+1;i++)
      for(j=jj;j<jj+jTile;j++)
  			newtemparray[i][j]=(temparray[i][j] * 8 + // pollaplasiazei to idio stoixeio me 8
                            temparray[i-1][j-1] *(-1) +   // stoixeio panw aristera
                            temparray[i][j-1]   *(-1) +   // stoixeio aristera
                            temparray[i+1][j-1] *(-1) +   // stoixeio katw aristera
                            temparray[i-1][j]   *(-1) +   // stoixeio apo panw
                            temparray[i+1][j]   *(-1) +   // stoixeio apo katw
                            temparray[i-1][j+1] *(-1) +   // stoixeio panw deksia
                            temparray[i][j+1]   *(-1) +   // stoixeio deksia
                            temparray[i+1][j+1] *(-1)  // stoixeio katw deksia
                            );


//Vgazoume ta peritta pleon oria pou prosthesame sto parapanw vhma*/
  for (jj=1; jj<M+1; jj+=jTile)
    for(i=1;i<N+1;i++)
      for(j=jj;j<jj+jTile;j++)
  			current_y[i-1][j-1]=newtemparray[i][j];

/*Edw ginetai h klimakwsh. To 0 tha paei sth mesh dhl. 127*/
/*[0-126] --> arnhtika kai [128-255] --> thetika*/
  for (jj=0; jj<M; jj+=jTile)
    for(i=0;i<N;i++)
      for(j=jj;j<jj+jTile;j++){
  			  current_y[i][j] += 127;
          if (current_y[i][j] > 255) current_y[i][j] = 255;
          if (current_y[i][j] < 0) current_y[i][j] = 0;
        }

}


void write()
{
  FILE *frame_y;
  frame_y=fopen(finalfilename,"wb");

  for(i=0;i<N;i++)
    for(j=0;j<M;j++)
      fputc(current_y[i][j],frame_y);
  fclose(frame_y);

}

int main()
{
  read();
  filtering();
  write();
}
