/* This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   author: ABDULLAH ALEİTİ
   e-mail: abdullah.aleiti@ogr.sakarya.edu.tr
*/
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
    while((c = fgetc(file))!= EOF){
      if(isspace(c) && d!=d3 && d!=d6)
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
    printf("Kilit dosyasi bulunamadi yada acarken hata olustu.\n");
    exit(EXIT_FAILURE);
  }
  return 0;
}

void kullanim_talimati_yazdir(){
  fprintf(stderr,"Kullanim: kripto -e giris_metin cikis_metin (kriptola)\n    yada: kripto -d giris_metin cikis_metin (coz)\n\t  [-k kilit_dosyasi] opsiyonel.\n");
  exit(EXIT_FAILURE);
}

int main(int argc,char **argv){
  JRB t,tmp;
  FILE *cikis;
  IS is;
  int opt;
  int opsiyonFlag = -1;
  char *giris_pathname;
  char *cikis_pathname = argv[argc-1];
  char *kilit_pathname = NULL;
  while((opt = getopt(argc,argv,"he:d:k:"))!=-1){
    switch(opt){
    case 'h':
      kullanim_talimati_yazdir();
      break;
    case 'e':
      opsiyonFlag = 0;
      giris_pathname = strdup(optarg);
      break;
    case 'd':
      opsiyonFlag = 1;
      giris_pathname = strdup(optarg);
      break;
    case 'k':
      kilit_pathname = strdup(optarg);
      break;
    default:
      kullanim_talimati_yazdir();
    }
  }
  
  if(opsiyonFlag == -1) kullanim_talimati_yazdir();
  
  is = new_inputstruct(giris_pathname);
  if(is == NULL){
    perror(giris_pathname);
    exit(1);
  }

  cikis = fopen(cikis_pathname,"w+");
  t = make_jrb();
  if(kilit_dosyasindan_jrb_doldur(kilit_pathname == NULL ? ".kilit" : kilit_pathname,t,opsiyonFlag) != -1){
    while(get_line(is) >= 0) {
      for (int i = 0; i < is->NF; i++) {
	tmp = jrb_find_str(t,is->fields[i]);
	tmp == NULL ? fputs(is->fields[i],cikis) : fputs(jval_s(tmp->val),cikis);
	fputc(' ',cikis);
      }
      fputc('\n',cikis);
    }
  }else{
    fprintf(stderr,"Hata: bozuk kilit dosyasi.\n");
    exit(1);
  }
  return 0;
}
