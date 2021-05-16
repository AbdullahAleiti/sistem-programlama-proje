#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jrb.h"
#include "fields.h"

// opsiyon 0'da encode modu, 0'dan farkli her sayi icin decode 
int kilit_dosyasindan_jrb_doldur(char *dosya,JRB t, int opsiyon){
  enum Durum {d1,d2,d3,d4,d5,d6,d7,d8};
  FILE *file;
  file = fopen(dosya,"r");
  if(file){
    enum Durum d = 0;
    char buffer[500];
    char *key;
    char *val;
    buffer[0]='\0';
    char c;
    char son_karakter;
    unsigned int buffer_indeks;
    unsigned int karakter_sayisi = 0;
    unsigned int virgul_sayisi = 0;
    unsigned int token_sayisi = 0;
    while((c = fgetc(file))!= EOF){
      if(c=='\n' || (c==' '&& d!=d3 && d!=d6) )
	continue;
      else {
	switch (d){
	case d1:
	  if (c == '{') d++;
	  else return -1;
	  break;
	case d2:
	  if (c == '\"') {
	    d++;
	    buffer_indeks = 0;
	  }
	  else if (c == '}') d = d8;
	  else return -1;
	  break;
	case d3:
	  if(son_karakter != '\\' && c=='\"') {
	    d++;
	    buffer[buffer_indeks] = '\0';
	    if      (opsiyon == 0)  key = strdup(buffer);
	    else    val = strdup(buffer);
	  }
	  else {
	    buffer[buffer_indeks]=c;
	    buffer_indeks++;
	  }
	  break;
	case d4:
	  if(c!=':') return -1;
	  else d++;
	  break;
	case d5:
	  if(c!='\"') return -1;
	  else {
	    d++;
	    buffer_indeks = 0;
	  }
	  break;
	case d6:
	  if(son_karakter != '\\' && c=='\"') {
	    d++;
	    buffer[buffer_indeks] = '\0';
	    if      (opsiyon == 0)  val = strdup(buffer);
	    else    key = strdup(buffer);
	    jrb_insert_str(t,key,new_jval_s(val));
	  }
	  else {
	    buffer[buffer_indeks]=c;
	    buffer_indeks++;
	  }
	  break;
	case d7:
	  if (c==',') d = d2;
	  else if (c== '}') d = d8;
	  else return -1;
	  break;
	case d8:
	  return -1;
	  break;
	}
	son_karakter = c;
      }
    }
    if(d!=d8) return -1;
    fclose(file);
  }else{
    printf("dosya acarken hata olustu.");
    return -1;
  }
  return 0;
}

void hatali_kullanim(){
  fprintf(stderr,"Kullanim: kripto [-e giris_metin] [-d giris_metin] cikis_metin.\n");
  exit(EXIT_FAILURE);
}

int main(int argc,char **argv){
  JRB t,tmp;
  int opt;
  int opsiyonFlag = -1;
  char dosyaIsmi[255];
  while((opt = getopt(argc,argv,"e:d:"))!=-1){
    switch(opt){
    case 'e':
      opsiyonFlag = 0;
      break;
    case 'd':
      opsiyonFlag = 1;
      break;
    default:
      hatali_kullanim();
    }
  }
  
  if(opsiyonFlag == -1) hatali_kullanim();
  
  t = make_jrb();
  if(kilit_dosyasindan_jrb_doldur(".kilit",t,opsiyonFlag) != -1){
    jrb_traverse(tmp, t) {
      printf("%s %s\n", tmp->key, tmp->val);
    }
  }
  return 0;
}
