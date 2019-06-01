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
pthread_mutex_t m_lock;
typedef struct num{
	int *nn;
	char *buff;
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
   char *result; //char������ ���� ���� 
   char token[]="=&";// char ������  ���� ���� 
   int i; //int�� ���� ���� 
   int n1,n2;//int������ n1,n2���� 
   int sum=0;//int�� ���� sum���� 

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
void log(int type,char *s0, char *s1, char *s2,int size) //������,���ϸ�, ���� ũ�� 
{
   int fd ;
   char logbuffer[BUFSIZE*2];

   switch (type) {
   case LOG: (void)sprintf(logbuffer," INFO: %s %s %s %d",s0,s1, s2,size); break; //s0=send/req, s1= ip ,s2= path/filename , num=�� �� 
   }   

   if((fd = open("server.log", O_CREAT| O_WRONLY | O_APPEND,0644)) >= 0) {
      (void)write(fd,logbuffer,strlen(logbuffer)); 
      (void)write(fd,"\n",1);      
      (void)close(fd);
   }
}
void *web(void *n1)
{
	
	num2 *n2=(num2 *)n1;
	printf("%d\n",n2->num);
	int *fd = n2->nn;
	char *buff=n2->buff;
//	printf("%s\n",buff);
	char file_name[15];//���� �̸�
	int size;//���� ũ�⸦ ���ϱ� ���� ���� 
	int j, file_fd, buflen, len;
	int i, ret;
	char * fstr;
	char buffer[BUFSIZE+1];	

	ret=read(fd,buffer,BUFSIZE); //fd���� ��� �о��  
//	printf("%s\n",buffer);
	
	if(ret == 0 || ret == -1) {
		return 0;
	}
	if(ret > 0 && ret < BUFSIZE)   
		buffer[ret]=0;   
	else buffer[0]=0;
//	printf("%s\n",buffer);

      for(i=4;i<BUFSIZE;i++) { //GET /images/05_08-over.gif �̷������� ������� 
         if(buffer[i] == ' ') { 
            buffer[i] = 0;
            break;
         }
      }
      //  printf("%s\n",buffer);

      if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6)) 
         strcpy(buffer,"GET /index.html");   
      //�ű�� ���ڸ� 
      //  printf("%s\n\n",buffer);
      buflen=strlen(buffer);
      //printf("%d\n\n",buflen);
      fstr = (char *)0;
      //printf("%s   ",fstr);
      for(i=0;extensions[i].ext != 0;i++) {
         len = strlen(extensions[i].ext);
         if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
            fstr =extensions[i].filetype; //gif�����̸� image/gif�� 
            // printf("%s   \n\n",fstr);
            break;
         }
      }
      //if(fstr == 0) log(SORRY,"file extension type not supported",buff,buffer,fd);
      strcpy(file_name,&buffer[5]);
      file_fd = open(file_name,O_RDONLY); //get�� ����װ� ������ ��� ���� �����϶� 
      fstat(file_fd,&s);
      if(file_fd==-1){
         if(strstr(buffer,"&to")){ 
            int n=cgi(buffer);
            sprintf(buffer,"HTTP/1.1 200 OK\nContent-Type: %s\n\n", fstr); /* Header + a blank line */

            write(fd,buffer,strlen(buffer));
            sprintf(buffer, "<HTML><BODY><H1>%d</H1></BODY></HTML>\r\n",n);
            write(fd,buffer,strlen(buffer));
            
            
            
            
            
            
            
            
            
          log(LOG,"SEND",buff,file_name,strlen(buffer)-1); //ip,����, ��� °
          
          
          
          
          
          
          
          
          
          
          
        	return 0;
         }
         else{
		 
            write(fd,"HTTP/1.1 404 Found",18); 
         sprintf(buffer, "<HTML><BODY><H1>Web Server Sorry: ����</H1></BODY></HTML>\r\n");   
         write(fd,buffer,strlen(buffer));
         return 0;}    
      }
      else if(S_ISDIR(s.st_mode)){
         write(fd,"HTTP/1.1 404 Found",18); 
         sprintf(buffer, "<HTML><BODY><H1>Web Server Sorry: ����</H1></BODY></HTML>\r\n");   
         write(fd,buffer,strlen(buffer));
         return 0;    
      }
      sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
      size=s.st_size;
      // log(LOG,"SEND",buff,&buffer[5],hit); //ip,����, ��� °
      
      
      
      
      
      
     log(LOG,"SEND",buff,file_name,size); //ip,����, ��� °
     
     
     
     
     
     
     
     
     
     
      //  printf("%s\nend\n\n",buffer);
      write(fd,buffer,strlen(buffer));
       printf("im buff1: %s\n",buffer);
      /*html���������� �ٽ� ǥ���ϴ� �κа���*/
      while ((ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
         //   printf("im buff2: %s\n",buffer);
         write(fd,buffer,ret);
      }
	close(fd);
	free(n1);
	return 0;
}


int main(int argc, char *argv[])
{
	pthread_t tid;
	num2 *n1;
	int port;
	int i=0;
	int j=0;
	struct sockaddr_in sin,cli;
	int sd,clientlen=sizeof(cli);
	int *ns;
	if(chdir(argv[1]) == -1){ 
		printf("ERROR: Can't Change to directory %s\n",argv[1]);
		exit(4);
	}
	if((sd=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket");
		exit(1);
	}
	port = atoi(argv[2]); //�Է¹��� ��Ʈ���� port�� ���� 
	memset((char*)&sin,'\0',sizeof(sin));
	sin.sin_family=AF_INET;
	sin.sin_port=htons(port);
	sin.sin_addr.s_addr=inet_addr("0.0.0.0");

	if(bind(sd,(struct sockaddr *)&sin,sizeof(sin))){
		perror("bind");
		exit(1);
	}
	if(listen(sd,100)){
		perror("listen");
		exit(1);
		}
	while(1)
	{
pthread_mutex_init(&m_lock, NULL);
		n1=(num2*)malloc(sizeof(num2));
		//ns=(int *)malloc(sizeof(int));
		if((n1->nn=accept(sd,(struct sockaddr *)&cli,&clientlen))==-1){
			perror("accept");
			//exit(1);
			return 0;
		}
		
			n1->buff=inet_ntoa(cli.sin_addr);
			pthread_create(&tid,NULL,web,n1);

	pthread_mutex_destroy(&m_lock);

	}
	return 0;

}

