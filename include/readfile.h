#ifndef _readfile_
#define _readfile_

typedef struct readfile{
 char line[1000];
} Read;

int read(char *filename, Read *info);

#endif
