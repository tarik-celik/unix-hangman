#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#define DICT "words.txt"
#define EDICT "/usr1/dict/web2"
#define MAXERR 7
#define MINSCORE 0
#define MINLEN 7

char *dictfile;
int alive;
int lost;
FILE *dict;
long int dictlen;
float errors=0;
float words=0;
int main(argc,argv) char **argv;
{
	if(argc==1) dictfile=DICT;
	else if(*argv[1]=='-') dictfile=EDICT;
	else dictfile=argv[1];
	setup();
	for(;;)
	{	startnew();
		while(alive>0)
		{	stateout();
			getguess();
		}
		words=words+1;
		if(lost) wordout();
		else youwon();
	}
}
int setup()
{
	struct stat statb;
	long t;
	t = time(NULL);
	srand(t+(t>>16));
	if((dict=fopen(dictfile,"r"))==NULL) fatal("no dictionary");
	if(stat(dictfile,&statb)<0) fatal("can't stat");
	dictlen=statb.st_size;
}
char word[26],alph[26],realword[26];
int startnew()
{	int i;
	long int pos;
	char buf[128];
	for(i=0;i<26;i++) word[i]=alph[i]=realword[i]=0;
	pos=rand()*dictlen;
	fseek(dict,pos,0);
	fscanf(dict,"%s\n",buf);
	getword();
	alive=MAXERR;
	lost=0;
}
int stateout()
{	int i;
	printf("guesses: ");
	for(i=0;i<26;i++)
		if(alph[i]!=0) putchar(alph[i]);
	printf(" word: %s ",word);
	printf("errors: %d/%d\n",MAXERR-alive,MAXERR);
}
int getguess()
{	char gbuf[128],c;
	int ok=0,i;
loop:
	printf("guess: ");
	scanf("%s", gbuf);
	if (gbuf == NULL)
	{	printf("The word was %s\n", realword);
		exit(0);
	}
	if ((c=gbuf[0])<'a' || c>'z')
	{	printf("lower case\n");
		goto loop;
	}
	if (alph[c-'a']!=0)
	{	printf("you guessed that\n");
		goto loop;
	}
	else alph[c-'a']=c;
	for (i=0;realword[i]!=0;i++)
		if(realword[i]==c)
		{	word[i]=c;
			ok=1;
		}
	if (ok==0)
	{	alive--;
		errors=errors+1;
		if(alive<=0) lost=1;
		return 0;
	}
	for(i=0;word[i]!=0;i++)
		if(word[i]=='.') return 0;
	alive=0;
	lost=0;
	return 0;
}
int wordout()
{
	errors=errors+2;
	printf("the answer was %s, you blew it\n",realword);
}
int youwon()
{
	printf("you win, the word is %s\n",realword);
}
fatal(s) char *s;
{
	fprintf(stderr,"%s\n",s);
	exit(1);
}
int getword()
{	char wbuf[128],c;
	int i,j;
loop:
	if(fscanf(dict,"%s\n",wbuf)==EOF)
	{	fseek(dict,0L,0);
		goto loop;
	}
	if((c=wbuf[0])>'z' || c<'a') goto loop;
	for(i=j=0;wbuf[j]!=0;i++,j++)
	{	if(wbuf[j]=='-') j++;
		wbuf[i]=wbuf[j];
	}
	wbuf[i]=0;
	if(i<MINLEN) goto loop;
	for(j=0;j<i;j++)
		if((c=wbuf[j])<'a' || c>'z') goto loop;
	pscore();
	strcpy(realword,wbuf);
	for(j=0;j<i;word[j++]='.');
}
int pscore()
{
	if(words!=0) printf("(%4.2f/%.0f) ",errors/words,words);
}
