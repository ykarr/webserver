#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include<pthread.h>
#define BUFSIZE 1024
#define LOG   44
#define HOME /index.html
struct stat s;
int lol;
pthread_mutex_t m_lock;
typedef struct num{
   int nn;
   char *buff;
   int num;
}num2;

struct {
   char *ext;
   char *filetype;
} extensions [] = {
   {"gif", "image/gif" },  
   {"jpg", "image/jpg"}, 
   {"jpeg","image/jpeg"},
   {"png", "image/png" },  
   {"htm", "text/html" },  
   {"html","text/html" },  

   {0,0} };

int cgi(char *buf){ //cgi 
   char *result; //char형식의 변수 선언 
   char token[]="=&";// char 형식의  변수 선언 
   int i; //int형 변수 선언 
   int n1,n2;//int형변수 n1,n2선언 
   int sum=0;//int형 변수 sum선언 

   result=strtok(buf,token);
   result=strtok(NULL,token);
   n1=atoi(result);
   strtok(NULL,token);
   result=strtok(NULL,token);
   n2=atoi(result);

   i=-(n1-n2)+1;
   int half=i/2;
   if(i&1){
      //if(i%2==1){
      sum=(n1+n2)*half;
      sum+=n1+half;
   }
   else
      sum=(n1+n2)*half;

   return sum;

   }
   
//  void log(int type,char *s0, char *s1, char *s2,int size) //아이피,파일명, 파일 크기 
// {
//    //sprintf(logbuffer," INFO: %s %s %s %d",s0,s1, s2,size); break; //s0=send/req, s1= ip ,s2= path/filename , num=숫 자
//    {
//      write(fd, logbuffer, strlen(logbuffer));
//      write(fd, "\n", 1);
//      close(fd);
//    }
// }  
   
   
void *web(void *n1)
{
   
   struct num *n2 = (struct num *)n1;
   char logbuffer[BUFSIZE];
   //printf("%d\n",n2->nn);
   //int hit;
   int *fd = n2->nn;
   char *buff=n2->buff;
   char file_name[BUFSIZ];//파일 이름
   int size;//파일 크기를 구하기 위한 변수 
   int j, file_fd, buflen, len;
   int i, ret;
   char * fstr;
   char buffer[BUFSIZE+1];   

   ret=read(fd,buffer,BUFSIZE); //fd에서 계속 읽어옴  
  
   
   if(ret == 0 || ret == -1) {
      close(fd);
      return 0;   }
   if(ret > 0 && ret < BUFSIZE)   
      buffer[ret]=0;   
   else buffer[0]=0;
//   printf("%s\n",buffer);
   printf("im ret %d\n",ret);
      for(i=4;i<BUFSIZE;i++) { //GET /images/05_08-over.gif 이런식으로 만들어줌 
         if(buffer[i] == ' ') { 
            buffer[i] = 0;
            break;
         }
      }
        printf("%s\n",buffer);

      if( !strncmp(&buffer[0],"GET /\0",6)) 
         strcpy(buffer,"GET /index.html");   
 
      
      buflen=strlen(buffer);
      //printf("%d\n\n",buflen);
      fstr = (char *)0;
      //printf("%s   ",fstr);
      for(i=0;extensions[i].ext != 0;i++) {
         len = strlen(extensions[i].ext);
         if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
            fstr =extensions[i].filetype; //gif형식이면 image/gif로 
             printf("%s   \n\n",fstr);
            break;
         }
      }
      //if(fstr == 0) log(SORRY,"file extension type not supported",buff,buffer,fd);
      strcpy(file_name,&buffer[5]);
      file_fd = open(file_name,O_RDONLY); //get을 떼어네고 파일을 열어봄 없는 파일일때 
      fstat(file_fd,&s);
      if(file_fd==-1){
         if(strstr(file_name,"&to")){ 
            int n=cgi(file_name);
            sprintf(buffer,"HTTP/1.1 200 OK\nContent-Type: %s\n\n", fstr); /* Header + a blank line */

            write(fd,buffer,strlen(buffer));
            sprintf(buffer, "<HTML><BODY><H1>%d</H1></BODY></HTML>\r\n",n);
            write(fd,buffer,strlen(buffer));
            //log(LOG,"SEND",buff,file_name,strlen(buffer)-1); //ip,파일, 몇번 째
             close(fd);
         free(n1);
         close(file_fd);
           return 0;         }
         else{
            write(fd,"HTTP/1.1 404 Found",18); 
         sprintf(buffer, "<HTML><BODY><H1>Web Server Sorry: 없음</H1></BODY></HTML>\r\n");   
         write(fd,buffer,strlen(buffer));
         
       	
        close(fd);
         free(n1);
         close(file_fd);
         return 0;
      }
  
      }
      else if(S_ISDIR(s.st_mode)){
         write(fd,"HTTP/1.1 404 Found",18); 
         sprintf(buffer, "<HTML><BODY><H1>Web Server Sorry: 없음</H1></BODY></HTML>\r\n");   
         write(fd,buffer,strlen(buffer));
         close(fd);
         free(n1);
         close(file_fd);
         return 0;   
      }
      sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
      size=s.st_size;
      //log(LOG,"SEND",buff,file_name,size); //ip,파일, 몇번 째
      sprintf(logbuffer,"%s %s %d\n",buff,file_name, size);
	  //  printf("%s\nend\n\n",buffer);
      write(fd,buffer,strlen(buffer));
     //  printf("im buff1: %s\n",buffer);
     write(lol,logbuffer,strlen(logbuffer));
      /*html을가져오고 다시 표시하는 부분같음*/
      while ((ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
         //   printf("im buff2: %s\n",buffer);
         write(fd,buffer,ret);
      }
      
   close(fd);
   free(n1);
   close(file_fd);
   //close(file_fd);
   return 0;
}


int main(int argc, char *argv[])
{
   pthread_t tid;
   num2 *n1;
   int port;
   struct sockaddr_in sin,cli;
   int sd,clientlen=sizeof(cli);
   lol = open("./server.log", O_CREAT| O_WRONLY | O_TRUNC,0644);
   if(chdir(argv[1]) == -1){ 
      printf("ERROR: Can't Change to directory %s\n",argv[1]);
      exit(4);
   }
   (void)signal(SIGCLD, SIG_IGN);
   (void)signal(SIGHUP, SIG_IGN);
   if((sd=socket(AF_INET,SOCK_STREAM,0))<0){
      perror("socket");
   }
   port = atoi(argv[2]); //입력받은 포트값을 port에 저장 
   memset((char*)&sin,'\0',sizeof(sin));
   sin.sin_family=AF_INET;
   sin.sin_port=htons(port);
   sin.sin_addr.s_addr=htons(INADDR_ANY);
	
   if(bind(sd,(struct sockaddr *)&sin,sizeof(sin))<0){
      perror("bind");
      return 0;
   }
   if(listen(sd,100)<0){
      perror("listen");
      return 0;
   }
    
   while(1)
   {
      pthread_mutex_init(&m_lock, NULL);
      n1=(num2 *)malloc(sizeof(num2));
      //ns=(int *)malloc(sizeof(int));
      if((n1->nn=accept(sd,(struct sockaddr *)&cli,&clientlen))<0){
         perror("accept");
         exit(1);
      }
      
      n1->buff=inet_ntoa(cli.sin_addr);
      pthread_create(&tid,NULL,web,n1);
      pthread_detach(tid);
      pthread_mutex_destroy(&m_lock);
   }
	close(lol);
   return 0;

}