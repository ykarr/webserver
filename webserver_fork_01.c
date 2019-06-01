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
#define BUFSIZE 1024
#define LOG   44
#define HOME /index.html
struct stat s;
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

   void log(int type,char *s0, char *s1, char *s2, int size,int num)
   {
      
      char logbuffer[200];

      sprintf(logbuffer," INFO: %s %s %s %d %d\n",s0,s1, s2,size,num); //s0=send/req, s1= ip ,s2= path/filename , num=숫 자    
      if((type = open("./server.log", O_CREAT| O_WRONLY | O_APPEND,0644)) >= 0) {
         write(type,logbuffer,strlen(logbuffer)); 
         close(type);
      }
   }

//   void web(int fd, char *buff,int hit)
//   {
//      char file_name[15];//파일 이름
//      int size;//파일 크기를 구하기 위한 변수 
//      int j, file_fd, buflen, len;
//      int i, ret;
//      char * fstr;
//      static char buffer[BUFSIZE+1];   
//
//      ret =read(fd,buffer,BUFSIZE); //fd에서 계속 읽어옴  
//
//      if(ret == 0 || ret == -1) {
//         exit(1);
//      }
//      if(ret > 0 && ret < BUFSIZE)   
//         buffer[ret]=0;   
//      else buffer[0]=0;
//      //printf("%s\n",buffer);
//
//      for(i=4;i<BUFSIZE;i++) { //GET /images/05_08-over.gif 이런식으로 만들어줌 
//         if(buffer[i] == ' ') { 
//            buffer[i] = 0;
//            break;
//         }
//      }
//      //  printf("%s\n",buffer);
//
//      if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6)) 
//         strcpy(buffer,"GET /index.html");   
//      //옮기기 전자리 
//      //  printf("%s\n\n",buffer);
//      buflen=strlen(buffer);
//      //printf("%d\n\n",buflen);
//      fstr = (char *)0;
//      //printf("%s   ",fstr);
//      for(i=0;extensions[i].ext != 0;i++) {
//         len = strlen(extensions[i].ext);
//         if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
//            fstr =extensions[i].filetype; //gif형식이면 image/gif로 
//            // printf("%s   \n\n",fstr);
//            break;
//         }
//      }
//      //if(fstr == 0) log(SORRY,"file extension type not supported",buff,buffer,fd);
//
//
//      strcpy(file_name,&buffer[5]);
//      file_fd = open(file_name,O_RDONLY); //get을 떼어네고 파일을 열어봄 없는 파일일때 
//      fstat(file_fd,&s);
//      if(file_fd==-1){
//         if(strstr(buffer,"&to")){ 
//            int n=cgi(buffer);
//            sprintf(buffer,"HTTP/1.1 200 OK\nContent-Type: %s\n\n", fstr); /* Header + a blank line */
//
//            write(fd,buffer,strlen(buffer));
//            sprintf(buffer, "<HTML><BODY><H1>%d</H1></BODY></HTML>\r\n",n);
//            write(fd,buffer,strlen(buffer));
//            log(LOG,"SEND",buff,file_name,strlen(buffer)-1,hit); //ip,파일, 몇번 째
//            exit(1);
//         }
//         else
//            write(fd,"HTTP/1.1 404 Found",18); 
//         sprintf(buffer, "<HTML><BODY><H1>Web Server Sorry: 없음</H1></BODY></HTML>\r\n");   
//         write(fd,buffer,strlen(buffer));
//         exit(1);    
//      }
//      else if(S_ISDIR(s.st_mode)){
//         write(fd,"HTTP/1.1 404 Found",18); 
//         sprintf(buffer, "<HTML><BODY><H1>Web Server Sorry: 없음</H1></BODY></HTML>\r\n");   
//         write(fd,buffer,strlen(buffer));
//         exit(1);    
//      }
//      sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
//      size=s.st_size;
//      log(LOG,"SEND",buff,file_name,size,hit); //ip,파일, 몇번 째
//      //  printf("%s\nend\n\n",buffer);
//      write(fd,buffer,strlen(buffer));
//      // printf("im buff1: %s\n",buffer);
//      /*html을가져오고 다시 표시하는 부분같음*/
//      while ((ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
//         //   printf("im buff2: %s\n",buffer);
//         write(fd,buffer,ret);
//      }
//      exit(1);
//   }


   int main(int argc, char **argv)
   {
      int i, port,listenfd, socketfd, hit;
      pid_t pid;
      size_t length;
      static struct sockaddr_in cli_addr,serv_addr; 
      //pid_t pid;

      if(chdir(argv[1]) == -1){ 
         printf("ERROR: Can't Change to directory %s\n",argv[1]);
         exit(4);
      }

      //      if(fork() != 0)
      //         return 0; 
      (void)signal(SIGCLD, SIG_IGN);  //데몬서버 
      (void)signal(SIGHUP, SIG_IGN);  //https://riucc.tistory.com/203
      //   for(i=0;i<32;i++)
      //      (void)close(i);   
      //   (void)setpgrp();   


      if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0){ //sd
         perror("error");
         exit(1);
      }
      //log(ERROR, "system call",0,"socket",0);

      port = atoi(argv[2]); //입력받은 포트값을 port에 저장 

      memset((char*)&serv_addr,'\0',sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      serv_addr.sin_port = htons(port);

      if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0){
         perror("error");
         exit(1); 
      }
      if( listen(listenfd,100) <0){
         perror("error");
         exit(1);
      }
      char *buff;
      for(hit=1;;hit++){

         length = sizeof(cli_addr);
         if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0){
            perror("error");
            exit(1);
         }
         buff=inet_ntoa(cli_addr.sin_addr);
         if((pid = fork()) < 0) {
            exit(1);
         }
         else{
            if(pid == 0) {////////////////////fork시작 
               close(listenfd);
               //   web(socketfd,buff,hit);
               char file_name[15];//파일 이름
               int size;//파일 크기를 구하기 위한 변수 
               int j, file_fd, buflen, len;
               int i, ret;
               char * fstr;
               static char buffer[BUFSIZE+1];   

               ret =read(socketfd,buffer,BUFSIZE); //fd에서 계속 읽어옴  

               if(ret == 0 || ret == -1) {
                  exit(1);
               }
               if(ret > 0 && ret < BUFSIZE)   
                  buffer[ret]=0;   
               else buffer[0]=0;
               //printf("%s\n",buffer);

               for(i=4;i<BUFSIZE;i++) { //GET /images/05_08-over.gif 이런식으로 만들어줌 
                  if(buffer[i] == ' ') { 
                     buffer[i] = 0;
                     break;
                  }
               }
               //  printf("%s\n",buffer);

               if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6)) 
                  strcpy(buffer,"GET /index.html");   
               //옮기기 전자리 
               //  printf("%s\n\n",buffer);
               buflen=strlen(buffer);
               //printf("%d\n\n",buflen);
               fstr = (char *)0;
               //printf("%s   ",fstr);
               for(i=0;extensions[i].ext != 0;i++) {
                  len = strlen(extensions[i].ext);
                  if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
                     fstr =extensions[i].filetype; //gif형식이면 image/gif로 
                     // printf("%s   \n\n",fstr);
                     break;
                  }
               }
               //if(fstr == 0) log(SORRY,"file extension type not supported",buff,buffer,fd);


               strcpy(file_name,&buffer[5]);
               file_fd = open(file_name,O_RDONLY); //get을 떼어네고 파일을 열어봄 없는 파일일때 
               fstat(file_fd,&s);
               if(file_fd==-1){
                  if(strstr(buffer,"&to")){ 
                     int n=cgi(buffer);
                     sprintf(buffer,"HTTP/1.1 200 OK\nContent-Type: %s\n\n", fstr); /* Header + a blank line */
                     write(socketfd,buffer,strlen(buffer));
                     sprintf(buffer, "<HTML><BODY><H1>%d</H1></BODY></HTML>\r\n",n);
                     write(socketfd,buffer,strlen(buffer));
                     log(LOG,"SEND",buff,file_name,strlen(buffer)-1,hit); //ip,파일, 몇번 째
                     exit(1);
                  }
                  else{
                     write(socketfd,"HTTP/1.1 404 Found",18); 
                     sprintf(buffer, "<HTML><BODY><H1>Web Server Sorry: 없음</H1></BODY></HTML>\r\n");   
                     write(socketfd,buffer,strlen(buffer));
                     exit(1);
                  }
               }
               else if(S_ISDIR(s.st_mode)){
                  write(socketfd,"HTTP/1.1 404 Found",18); 
                  sprintf(buffer, "<HTML><BODY><H1>Web Server Sorry: 없음</H1></BODY></HTML>\r\n");   
                  write(socketfd,buffer,strlen(buffer));
                  //로그는? 
                  exit(1);    
               }
               sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
               size=s.st_size;
               log(LOG,"SEND",buff,file_name,size,hit); //ip,파일, 몇번 째
               //  printf("%s\nend\n\n",buffer);
               write(socketfd,buffer,strlen(buffer));
               // printf("im buff1: %s\n",buffer);
               
               while ((ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
                  //printf("im buff2: %d\n",BUFSIZE);
                  write(socketfd,buffer,ret);
               }

               exit(1);
            } 
            else {
               close(socketfd);
            }
         }

      }
      return 0;
   }
