#include<stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){
    
    if (argc != 4){
        printf("usage: samples file numberfrags maxfragsize\n");
        return -1;
    }
  
    FILE * fp;

    fp = fopen(argv[1], "r");
    

    if (fp==NULL){
        printf("The file could not be open\n");
        return -1;
    }

    int num, c, m,n,i; 
    char s;

    fseek(fp,0,SEEK_END);
    int len=ftell(fp);
    int l=len;
    for(i=0;i<l;i++){
        fseek(fp,i,SEEK_SET);
        s=getc(fp);
        if(s<32 || s==127){
            len--;
        }
    }

    srandom(0);

    n=atoi(argv[2]);
    m=atoi(argv[3]);

    if(len<m){
        printf("Maxfragsize is greater than document's size\n");
        return -1;
    }

    for(i=0;i<n;i++) {
        char str[m+1];
        num = (random()%(len-m));
        int x=0;
        for(c=0;x<m;c++){
            fseek(fp,num+c,SEEK_SET);
            s=getc(fp);
            if(s>=32 && s!=127){
                str[x]=s;
                x++;
            }
        }
        str[m]=0;
        printf(">%s<\n",str);
    }

    fclose(fp);

    return 0;

}

