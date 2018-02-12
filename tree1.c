/*Jiaqing Mo
 * CPE225-09*/
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "tree.h"
typedef struct dirent dirent;
int main(int argc, char* argv[]){
   int i = 1;
   int hidden = 0;
   int longf = 0;

   struct stat buf;
   while (i<argc && argv[i][0]=='-'){/*switches*/
      if (strcmp(argv[i],"-a")==0){ /* hidden switch on*/
         hidden++;
         i++;
      }else if (strcmp(argv[i],"-l")==0){/*long switch on*/
         longf++;
         i++;
      }else if (strcmp(argv[i],"-la")==0 || strcmp(argv[i],"-al")==0){
         hidden++;
         longf++;
         i++;
      }
      else{/*it is a filename*/
         perror("switch not valid");
         exit(-1);
      }
   }
   
   if (hidden && longf){/*mode = 3 */
      if (i==argc){
         print_dir(".",0,3);
         return 0;
      }
      while (i<argc){
         if (stat(argv[i], &buf)){
            perror (argv[i]);
            exit(-1);
         }
         print_dir(argv[i],0,3);
         i++;
      }
   }else if(hidden){/*mode = 2*/
      if (i==argc){
         print_dir(".",0,2);
         return 0;
      }
      while (i<argc){
         if (stat(argv[i], &buf)){
            perror (argv[i]);
            exit(-1);
         }
         print_dir(argv[i],0,2);
         i++;
      }
   }else if(longf){/*mode = 1*/
      if (i==argc){
         print_dir(".",0,1);
         return 0;
      }
      while (i<argc){
         if (stat(argv[i], &buf)){
            perror (argv[i]);
            exit(-1);
         }
         print_dir(argv[i],0,1);
         i++;
      }
   }
   else{ /*both switches off*/
      if (i==argc){
         print_dir(".",0,0);
         return 0;
      }
      while (i<argc){
         if (stat(argv[i], &buf)){
            perror (argv[i]);
            exit(-1);
         }
         print_dir(argv[i],0,0);
         i++;
      }
   }
   return 0;
}

void print_dir(char* filename, int depth, int mode){
   dirent* dp = NULL;
   DIR* dirp = NULL;
   int size;
   int i = depth;
   char** strArr;
   //printf("%s %d\n", filename, depth);
   if (filename==NULL || strlen(filename)==0) return;
   while (i-1>0){
      printf("|   ");
      i--;
   }
   if (i>0) printf("|-- ");
   if (mode ==1 || mode ==3){
      printAccess(filename);
   }
   printf("%s\n",filename);
   dirp = opendir(filename);
   if (dirp==NULL){/*not an directory*/
      return;
   }
   if (chdir(filename)!=0){ /*can't change into dir*/
      perror("Can't change into directory");
      return;
   }
   strArr = checked_malloc(sizeof(char*)*10);
   size =10;
   i = 0;
   while ((dp = readdir(dirp))!=NULL){
      if ((strcmp(dp->d_name,".")==0) ||strcmp(dp->d_name,"..")==0 ) continue;/*hidden files*/
      if (mode<2 && (dp->d_name)[0]=='.') continue;
      if (i>=(size-2)){/*enlarge*/
         strArr = checked_realloc(strArr,(size+10)*sizeof(char*));
         size+=10;
      }
      strArr[i] = checked_malloc((strlen(dp->d_name)+1)*sizeof(char));
      strcpy(strArr[i],dp->d_name);
      i++;
   }
   strArr[i]=NULL;
   closedir(dirp);
   sort(strArr,i);
   i = 0;
   while(strArr[i]){
      print_dir(strArr[i],depth+1,mode);
      i++;
   }
   freeArr(strArr);
   chdir("..");
   //free(strArr);
   return;
}
/*void print_dir_l(char* filename, int depth){
   return;
}
void print_dir_h(char* filename, int depth){
   return;
}
void print_dir_hl(char* filename, int depth){
   return;
}*/
void printList(char*strArr[]){
   int i=0;
   if (strArr==NULL){
      perror("array is NULL");
      exit(-1);
   }
   while(strArr[i]!=NULL){
      printf("%s\n", strArr[i]);
      i++;
   }
   return;
}

void* checked_malloc(size_t size){
   void* p;
   p = malloc(size);
   if (p==NULL){
      perror("");
      exit(-1);
   }
   return p;
}
void* checked_realloc(void* p, size_t size){
   void* temp = realloc(p, size);
   if (temp==NULL){
      perror("");
      exit(-1);
   }
   return temp;
}
void sort(char* strArr[], int size){
   qsort(strArr,size,sizeof(char*),cstring_cmp);
   return;
}
int cstring_cmp(const void*a, const void*b){
   const char **ia = (const char**)a;
   const char **ib = (const char**)b;
   return strcmp(*ia, *ib);
}

void freeArr(char* strArr[]){
   int i = 0;
   if(strArr==NULL) return;
   while (strArr[i]!=NULL){
      free(strArr[i]);
      i++;
   }
   free(strArr);
   return;
}

void printAccess(char* filename){

   struct stat buf;
   char m;
   mode_t mode;
      if (stat(filename, &buf)){
         perror (filename);
         exit(-1);
      }
      mode = buf.st_mode;
      if (S_ISREG(mode)){
         m = '-';
      }
      else if (S_ISDIR(mode)){
         m = 'd';
      }
      else if (S_ISCHR(mode)){
         m = 'c';
      }
      else if (S_ISBLK(mode)){
         m = 'b';
      }
      else if (S_ISFIFO(mode)){
         m = 'p';
      }
      else if (S_ISLNK(mode)){
         m = 'l';
      }
      else if (S_ISSOCK(mode)){
         m = 's';
      }
      else{
         perror("File type is not found");
         exit(-1);
      }
      printf("[%c",m);
      if (mode&S_IRUSR) putchar('r');
      else putchar('-');
      if (mode & S_IWUSR) putchar('w');
      else putchar('-');
      if ((mode&S_ISUID)!=0 && (mode&S_IXUSR)==0) putchar('S');
      else if ((mode&S_ISUID)!=0 && (mode&S_IXUSR)!=0) putchar('s');
      else if (mode&S_IXUSR) putchar('x');
      else putchar('-');
      
      if (mode&S_IRGRP) putchar('r');
      else putchar('-');
      if (mode & S_IWGRP) putchar('w');
      else putchar('-');
      if ((mode&S_ISGID)!=0 && (mode&S_IXGRP)==0) putchar('S');
      else if ((mode&S_ISGID)!=0 && (mode&S_IXGRP)!=0) putchar('s');
      else if (mode&S_IXGRP) putchar('x');
      else putchar('-');
      
      if (mode&S_IROTH) putchar('r');
      else putchar('-');
      if (mode & S_IWOTH) putchar('w');
      else putchar('-');
      if (mode&S_IXOTH) putchar('x');
      else putchar('-');
      putchar(']');
      putchar(' ');
}









