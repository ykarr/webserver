/*fork ����*/
#include <stdio.h> //ǥ������¶��̺귯��
#include <stdlib.h> //ǥ������¶��̺귯��
#include <unistd.h> //fork�����̺귯��
#include <errno.h> //�����ڵ� ��ũ�� ����
#include <string.h> //���ڿ�ó�� ���̺귯��
#include <fcntl.h> //���ϰ��� ���̺귯��
#include <signal.h> //�ñ׳�ó�� ���̺귯��
#include <sys/types.h> //�ý��۰��� ���̺귯��
#include <sys/socket.h> //��Ʈ��ũ��� ���̺귯��
#include <netinet/in.h> //���ͳ��ּ�ü�� ��� ���̺귯��
#include <arpa/inet.h> //��Ŭ�����ϻ�� ���̺귯��
#include <sys/stat.h> // �������� ���̺귯��
#define BUFSIZE 1012 // ���������� ����
#define LOG   44 //�α� ����
#define HOME /index.html //home ����
struct stat s; //�Ʒ����� ���� ũ�⸦ ���ϱ� ���ؼ� ����� 
struct {//����ü 
	char *ext; //char ���� ���� ����
	char *filetype; // char ���� ���� ����
} extensions [] = {
	{"gif", "image/gif" },  //gif 
	{"jpg", "image/jpg"},    //jpg
	{"jpeg","image/jpeg"},   //jpeg
	{"png", "image/png" },  //png
	{"htm", "text/html" },  //htm
	{"html","text/html" },  //html
	{0,0} };//NULL

int cgi(char *buf){ //cgi 
	char *result; //char������ ���� ���� 
	char token[]="=&";// char ������  ���� ���� 
	int i; //int�� ���� ���� 
	int n1,n2;//int������ n1,n2���� 
	int sum=0;//int�� ���� sum���� 
	result=strtok(buf,token); // =������ ���ڿ� �ڸ���
	result=strtok(NULL,token); // =,&������ NNN �߶� ����
	n1=atoi(result); //result�� ���������� n1�� ���� 
	strtok(NULL,token); // &,=������ to �ڸ���
	result=strtok(NULL,token);// &,=������ MMM �߶� ����
	n2=atoi(result);//result�� ���������� n2�� ���� 

	i=-(n1-n2)+1;// ���ϴ� ���� ���� �Ǻ�
	int half=i/2;//i�� ������ 
	if(i&1){//���� �� %2��� &1����ؼ� Ȧ�� ���� ¦������ 
		//if(i%2==1){//������ 
		sum=(n1+n2)*half;//cgi��� 
		sum+=n1+half;//cgi ��� 
	}
	else
		sum=(n1+n2)*half;//cgi ��� 
	return sum;//��� ����� ���� 
}

void log(char *s1, char *s2, int size)//�α� �Ű����� 
{
	int fpp;//�α׿� ���� ���ؼ� ���� 
	char logbuffer[200];//�α� �� �� 
	sprintf(logbuffer,"%s %s %d\n",s1, s2,size); //s0=send/req, s1= ip ,s2= path/filename , size=ũ��,num=����    
	if((fpp= open("./server.log",O_WRONLY | O_APPEND,0644)) >= 0) {// ������ ����. 
		write(fpp,logbuffer,strlen(logbuffer)); //������ ������ �α׿� �ۼ��Ѵ�. 
		close(fpp);//type�� close���ش�. 
	}
}

int main(int argc, char *argv[])//�����Լ� 
{
	int ff;//�α� ������ �缳���ϱ� ���ؼ�  
	ff=open("./server.log", O_CREAT|O_TRUNC,0644);//�α������� �����ش�. 
	//printf("start\n");
	close(ff);//�α������� �ݾ��ش�. 
	int i, port,listenfd, socketfd, hit;//int������ ���� 
	pid_t pid;//��ũ�� ����ϱ� ���� ���� 
	size_t length;//
	static struct sockaddr_in cli_addr,serv_addr; //���� ����� ���� ����ü 
	char *path=argv[1]; // path�� ��� ����
	signal(SIGCLD, SIG_IGN);  // �ڽ����μ����� �ϳ��� ����Ǹ� �θ𿡰� ��ȣ���� 
	signal(SIGHUP, SIG_IGN);  // ����� �͹̳��� ���� ����
	if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0){ //���� ���ϱ���� �� 
		perror("error");//���� 
		exit(1);//������. 
	}
	port = atoi(argv[2]); //�Է¹��� ��Ʈ���� port�� ���� 
	memset((char*)&serv_addr,'\0',sizeof(serv_addr));//�ʱ�ȭ 
	serv_addr.sin_family = AF_INET;//���� �ּ� ����ü1 
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//all ip?���� �ּ� ����ü2 
	serv_addr.sin_port = htons(port);//�־��� ��Ʈ�� ���� ���� �ּ� ����ü3
	if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0){//���Ͽ� �̸� �����ϱ� 
		perror("error");//bind���н� ��� 
		exit(1); //������. 
	}
	if( listen(listenfd,100) <0){//Ŭ���̾�Ʈ ���� ��ٸ��� 
		perror("error");//listen���н� ��� 
		exit(1);//������. 
	}
	char *buff;//������ �������� 
	for(hit=1;;hit++){//while���� ���� 
		length = sizeof(cli_addr);//cli_addr����� length�� �����Ѵ�. 
		if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0){//�����û ���� 
			perror("error");//accept�� �߾ȵǸ� ���� 
			exit(1);//������. 
		}
		buff=inet_ntoa(cli_addr.sin_addr);//�����Ǹ� buff�� �����Ѵ�. 
		if((pid = fork()) < 0) {//�ȿ����� 
			exit(1);//������. 
		}
		else{//�ƴϸ� ���� 
			if(pid == 0) {////////////////////fork���� 
				close(listenfd);//listenfd�� �ݾ��ش�. 
				char file_name[50];//���� �̸�
				// int size;//���� ũ�⸦ ���ϱ� ���� ���� ���� 
				int j, file_fd, buflen, len;//int������ ���� 
				int i, ret;//int�� ���� ���� 
				char * fstr;//content type�� ������ ���ڿ� ���� 
				static char buffer[BUFSIZE+1];//���� ����   
				ret =read(socketfd,buffer,BUFSIZE); //fd���� ��� �о��  
				if(ret == 0 || ret == -1) {//�б� �����ϸ� 
					exit(1);//������. 
				}
				if(ret > 0 && ret < BUFSIZE)  //ret�� 0���� ũ�� BUFSIZE���� ������ 
					buffer[ret]=0;   //buffer[ret]�� 0�� �ȴ�. 
				else buffer[0]=0;//���� �������� �ʴ´ٸ� buffer[0]=0�̵ȴ�. 
					for(i=4;i<BUFSIZE;i++) { //GET /images/05_08-over.gif �̷������� ������� 
					if(buffer[i] == ' ') { //������ Ȯ�� 
						buffer[i] = 0;//�����϶� 0 
						break;//for�� Ż�� 
					}
				}
				if( !strncmp(&buffer[0],"GET /\0",6))//GET /\0�϶�  
					strcpy(buffer,"GET /index.html");   //index.html����ϵ��� request���� 
				buflen=strlen(buffer); // buflen�� buffer���� ����
				fstr = NULL;//null�� �ʱ�ȭ 
				for(i=0;extensions[i].ext != 0;i++) { // ����ü �� Ž��
					len = strlen(extensions[i].ext); // ��������
					if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) { // ������ ���� ���������� ��
						fstr =extensions[i].filetype; //gif�����̸� image/gif�� 
						break;//for���� ������. 
					}
				}
				strcpy(file_name,&buffer[5]);//buffer[5] �� ���� �̸��� filename�� �������ش�. 
			//	printf("%s\n",file_name); 
				sprintf(path,"%s/%s",path,file_name);//path�� path/filename�� ���ش�. 
				file_fd = open(path,O_RDONLY); //get�� ����װ� ������ ��� ���� �����϶� 
				fstat(file_fd,&s);//�������� �ƴ��� Ȯ���ϱ� ���ؼ� ��� 
				if(file_fd==-1){//������ �ƴ϶��? 
					if(strstr(file_name,"&to")){ //���� ���� �̸��� &to�� �ִٸ�? 
						int n=cgi(&buffer[5]);//���ϳ����� cgi�� �ְ� cgi�� ���ϰ��� n���� �޴´�. 
						sprintf(buffer,"HTTP/1.1 200 OK\nContent-Type: text/html\r\n\r\n<HTML><BODY><H1>%d</H1></BODY></HTML>\r\n",n); /* Header + a blank line */
						// write(socketfd,buffer,strlen(buffer));//socketfd�� ���۳����� ����. 
						// sprintf(buffer, "",n);//cgi���� ��� 
						write(socketfd,buffer,strlen(buffer));//socketfd�� ���۸� ���ش�.
					//	printf("%s\n",file_name); 
						log(buff,file_name,strlen(buffer)-80); //ip,����, ��� °                     
					}
					else{
						sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<HTML><BODY><H1>NOT FOUND</H1></BODY></HTML>\r\n");//200���� ����� ���� 
						//write(socketfd,buffer,strlen(buffer));       //fd�� ������ ������ ���ش�.  
						//sprintf(buffer, "");   //���ۿ� ȭ�鿡 �Է��� ������ ���� 
						write(socketfd,buffer,strlen(buffer)); //������ ������ fd�� ���ش�. 
						log(buff,file_name,9);//�α��ۼ�    
					}
					exit(1);//������. 
				}
				else if(S_ISDIR(s.st_mode)){//�����϶� 
					sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<HTML><BODY><H1>NOT FOUND</H1></BODY></HTML>\r\n");//200���� ����� ���� 
					//write(socketfd,buffer,strlen(buffer));       //fd�� ������ ������ ���ش�.  
					//sprintf(buffer, "");   //���ۿ� ȭ�鿡 �Է��� ������ ���� 
					write(socketfd,buffer,strlen(buffer)); //������ ������ fd�� ���ش�. 
					log(buff,file_name,9);//�α��ۼ� 
					exit(1);    //���� 
				}
				sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);//200���� ����� ���� 
				log(buff,file_name,s.st_size); //�α��ۼ� 
				write(socketfd,buffer,strlen(buffer));//socekfd�� ���۸� ���ش�. 
				while ((ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {//������ �д´�. 
					write(socketfd,buffer,ret);//���� ������ ���ش�. 
				}
				exit(1);//������. 
			} 
			else {
				close(socketfd);//socketfd�� �ݴ´�. 
			}
		}
	}
	return 0; // 0��ȯ
}

