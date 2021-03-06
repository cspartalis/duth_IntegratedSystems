#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define N 288 /* frame dimension for QCIF format */
#define M 352 /* frame dimension for QCIF format */
#define filename "akiyoy.yuv"
#define finalfilename "comb_opt_y.yuv"


/* code for armulator*/

int current_y[N][M];
int temparray[N+2][M+2];
int newtemparray[N+2][M+2];

#pragma arm section zidata="sram1"
int block1[3];
int block2[3];
int block3[3];
#pragma arm section

#pragma arm section zidata="sram2"
int buffer1[M+2];
int buffer2[M+2];
int buffer3[M+2];
#pragma arm section

int i,j,k,s;

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
	for(i=0;i<N+2;i++){
		for(j=0;j<M;j+=4){
			temparray[i][j]=0;
			temparray[i][j+1]=0;
			temparray[i][j+2]=0;
			temparray[i][j+3]=0;
    }
			temparray[i][352]=0;
			temparray[i][353]=0;
  }

/*Etsi exoume enan pinaka san ton arxiko mono pou ta oria toy exoyn epektathei kata 1 pros kathe kateuthunsh*/
/*Oi grammes/ sthles pou prostethkan sta akra einai gemismenes me 0*/
	for(i=1;i<N+1;i++)
		for(j=1;j<M+1;j+=4){
			temparray[i][j]=current_y[i-1][j-1];
			temparray[i][j+1]=current_y[i-1][j];
			temparray[i][j+2]=current_y[i-1][j+1];
			temparray[i][j+3]=current_y[i-1][j+2];
    }

/*Edw dinetai to filtrarisma me ton 2d pinaka highPassFilter*/
	for (s=0; s<M+2; s++){
		for(i=1;i<N+1;i++){

			/*Edw xwrizoume ton pinaka temparray se tmhmata*/
			/*H epeksergasia (to filtarisma) ginetai xrhsimopoiwntas ayta ta tmhmata*/
			if(i==1){
				for(k=0;k<M+2;k++){
					buffer1[k]=temparray[0][k];
					buffer2[k]=temparray[1][k];
					buffer3[k]=temparray[2][k];
				}
			}else{
				for(k=0;k<M+2;k++){
					buffer1[k]=buffer2[k];
					buffer2[k]=buffer3[k];
					buffer3[k]=temparray[i+1][k];
				}	
			}

			for (k=s; k<s+3;k++){
				block1[k]=buffer1[k];
				block2[k]=buffer2[k];
				block3[k]=buffer3[k];
			}
				
			newtemparray[i][s+1]=(block2[s+1] * 8 + // pollaplasiazei to idio stoixeio me 8
						  block1[s] *(-1) +   // stoixeio panw aristera
						  block2[s] *(-1) +   // stoixeio aristera
						  block3[s] *(-1) +   // stoixeio katw aristera
						  block1[s+1] *(-1) +   // stoixeio apo panw
						  block3[s+1] *(-1) +   // stoixeio apo katw
						  block1[s+2] *(-1) +   // stoixeio panw deksia
						  block2[s+2] *(-1) +   // stoixeio deksia
						  block3[s+2] *(-1)  // stoixeio katw deksia
						  );

			newtemparray[i][s+2]=(block2[s+2] * 8 + // pollaplasiazei to idio stoixeio me 8
						  block1[s+1] *(-1) +   // stoixeio panw aristera
						  block2[s+1] *(-1) +   // stoixeio aristera
						  block3[s+1] *(-1) +   // stoixeio katw aristera
						  block1[s+2]   *(-1) +   // stoixeio apo panw
						  block3[s+2]   *(-1) +   // stoixeio apo katw
						  block1[s+3] *(-1) +   // stoixeio panw deksia
						  block2[s+3] *(-1) +   // stoixeio deksia
						  block3[s+3] *(-1)  // stoixeio katw deksia
						  );

			newtemparray[i][s+3]=(block2[s+3] * 8 + // pollaplasiazei to idio stoixeio me 8
						  block1[s+2] *(-1) +   // stoixeio panw aristera
						  block2[s+2] *(-1) +   // stoixeio aristera
						  block3[s+2] *(-1) +   // stoixeio katw aristera
						  block1[s+3]   *(-1) +   // stoixeio apo panw
						  block3[s+3]   *(-1) +   // stoixeio apo katw
						  block1[s+4] *(-1) +   // stoixeio panw deksia
						  block2[s+4] *(-1) +   // stoixeio deksia
						  block3[s+4] *(-1)  // stoixeio katw deksia
						  );

			  /*Vgazoume ta peritta pleon oria pou prosthesame sto parapanw vhma*/
			  /*Edw ginetai h klimakwsh. To 0 tha paei sth mesh dhl. 127*/
			  /*[0-126] --> arnhtika kai [128-255] --> thetika*/
			  current_y[i-1][s]=newtemparray[i][s+1] + 127;
			  if (current_y[i-1][s] > 255) current_y[i-1][s] = 255;
			  if (current_y[i-1][s] < 0) current_y[i-1][s] = 0;

			  current_y[i-1][s+1]=newtemparray[i][s+2] + 127;
			  if (current_y[i-1][s+1] > 255) current_y[i-1][s+1] = 255;
			  if (current_y[i-1][s+1] < 0) current_y[i-1][s+1] = 0;

			  current_y[i-1][s+2]=newtemparray[i][s+3] + 127;
			  if (current_y[i-1][s+2] > 255) current_y[i-1][s+2] = 255;
			  if (current_y[i-1][s+2] < 0) current_y[i-1][s+2] = 0;
			
		}
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