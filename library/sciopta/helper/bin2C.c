#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static void
data (FILE *in, FILE *out, const char *arrayName,int len)
{
  int c;
  int n = 0;
  int i = 1;

  if ( len > 0 ){
    fprintf (out, "const char %s[%d] = {\n", arrayName,len);
    while( len ){
      ++n;
      c = getc(in);
      if (i == 8) {
	fprintf (out, "0x%02X,\n", c);
	i = 0;
      } 
      else if (i == 1) {
	fprintf (out, "\t0x%02X,", c);      
      } 
      else {
	fprintf(out, "0x%02X,",c);
      } 
      ++i;
      --len;
    }
    fprintf(out, "\n");
    fprintf(out, "};\n");
    fprintf(out, "unsigned int sizeof_%s = %d;\n",arrayName,n);
  } else {    
    fprintf (out, "const char %s[] = {\n", arrayName);
    while((c = getc (in)) != EOF) {
      ++n;
      if (i == 8) {
	fprintf (out, "0x%02X,\n", c);
	i = 0;
      } 
      else if (i == 1) {
	fprintf (out, "\t0x%02X,", c);      
      } 
      else {
	fprintf(out, "0x%02X,",c);
      } 
      ++i;
    }

    fprintf(out, "\n");
    fprintf(out, "};\n");
    fprintf(out, "unsigned int sizeof_%s = %d;\n",arrayName,n);
  }
}

int 
main (int argc, char *argv[])
{
  char c;
  int len = 0;
  FILE *in = stdin;
  FILE *out = stdout;
  char *name = "dummy";
  
  while ((c = getopt (argc, argv, "i:o:n:")) != -1) {
    switch (c) {
    case 'i':
      if (!(in = fopen (optarg, "rb"))) {
	perror ("Input file open");
      }
      fseek(in,0L,SEEK_END);
      len = ftell(in);
      fseek(in,0L,SEEK_SET);
      break;
    case 'o':
      if (!(out = fopen (optarg, "wb"))) {
	perror ("Input file open");
      }
      break;
    case 'n':
      name = optarg;
      break;
    default:
      break;
    }
  }
  
  data (in, out, name,len);
  return 0;
}
