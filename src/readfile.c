#include <stdio.h>
#include <stdlib.h>
#include "readfile.h"




int read(char *filename, Read *info)
{
    //printf("Y");
  FILE *fp=fopen(filename,"r");

  int i=0;
  if(fp==NULL){
    printf("error by opening\n");
    exit(1);
  }


  while(fgets((info+i)->line,100, fp)!=NULL){
    i++;
	//printf("%d",i);
  }
  fclose(fp);
  return i;
}
