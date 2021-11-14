#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <signal.h>
#include <stdbool.h>
#include <openssl/md5.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FLAGLEN 0x20

char flag[FLAGLEN];

void read_input(char* buf, size_t size) {
    int ret;
    ret = read(0, buf, size);
    if (ret <= 0) {
        puts("read error");
        _exit(1);
    }
    if(buf[ret-1] == '\n')
        buf[ret-1] = '\x00';


}


size_t read_long(){
    char buf[0x100];
    read_input(buf,0x100);
    return atoll(buf);
}

void handler(){
	puts("Timeout");
	exit(0);
}



void login_menu(){
	puts("++++++++++++++++++++++++++");
	puts("      Message Center      ");
	puts("++++++++++++++++++++++++++");
	puts("  1. Login                ");
	puts("  2. Sign up              ");
	puts("  3. Exit                 ");
	puts("++++++++++++++++++++++++++");
	printf("Your choice: "); 

}

struct user {
	size_t id;
	char *username;
	char password[0x10];
	struct user *next ;
};

struct MsgFile {
	size_t FromID;
	uint8_t Read: 1;
	uint8_t Reserved: 7;
	uint8_t subjectlen;
	uint16_t msglen;
};

struct Msg {
	size_t Offset;
	struct MsgFile MsgInfo;
	char subject[0x20] ;
	char *msg ;
};

struct user *cur_user = NULL;
struct user *userhead =  NULL;
size_t user_cnt = 0 ;
bool is_login = false;
struct Msg *msglist[16] ;

struct user *search_user(char *username){
	struct user *iter_user = NULL;
	if(userhead){
		iter_user = userhead;
		while(iter_user){
			if(!strcmp(iter_user->username,username))
				return iter_user;
			iter_user = iter_user->next;

		}
	}
	return NULL;
}


struct user *search_user_by_id(size_t id){
	struct user *iter_user = NULL;
	if(userhead){
		iter_user = userhead;
		while(iter_user){
			if(iter_user->id == id)
				return iter_user;
			iter_user = iter_user->next;

		}
	}
	return NULL;
}

void reg_user(char *username,char *password){
	if(search_user(username)){
		puts("User exist!");
	}
	struct user *tmp_user = (struct user*)calloc(1,sizeof(struct user));
	struct user *iter_user = NULL;
	tmp_user->id = user_cnt++;
	tmp_user->username = strdup(username);
	strncpy(tmp_user->password,password,0x10);
	if(!userhead){
		userhead = tmp_user;
	}else{
		iter_user = userhead;
		while(iter_user->next){
			iter_user = iter_user->next;
		}
		iter_user->next = tmp_user;
	}
	

}

void login(){
	char name[0x100];
	char pass[0x100];
	struct user *tmp_user = NULL;
	memset(name,0,0x100);
	memset(pass,0,0x100);
	printf("Username:");
	read_input(name,0xff);
	printf("Password:");
	read_input(pass,0xff);
	tmp_user = search_user(name);
	if(tmp_user){
		if(!strcmp(tmp_user->password,pass)){
			puts("Login success");
			cur_user = tmp_user;
			is_login = true;
			return;
		}		
	}
	puts("Invalid username or password !");
	exit(-1);
}	


void menu(){
	puts("++++++++++++++++++++++++++");
	puts("      Message Center      ");
	puts("++++++++++++++++++++++++++");
	puts("  1. Logout               ");
	puts("  2. User Info            ");
	puts("  3. Change Password      ");
	puts("  4. Read Message         ");
	puts("  5. Send Message         ");
	puts("  6. Exit                 ");
	puts("++++++++++++++++++++++++++");
	printf("Your choice: "); 
}


void read_menu(){
	puts("++++++++++++++++++++++++++");
	puts("      Message Center      ");
	puts("++++++++++++++++++++++++++");
	puts("  1. List All             ");
	puts("  2. List Unread          ");
	puts("  3. Display Message      ");
	puts("  4. Delete Message       ");
	puts("  5. Delete All Message   ");
	puts("  6. Return               ");
	puts("++++++++++++++++++++++++++");
	printf("Your choice: "); 
}






void chroot_dir(){
	MD5_CTX ctx;
    	char result[20]="";
    	char key[100]="";
	char dir[0x100] = "/home/message_center/data/";
	size_t dirlen = strlen(dir);
	char *source = getenv("REMOTE_HOST");
	int dirfd = -1;
	MD5_Init(&ctx);
        MD5_Update(&ctx, source, strlen(source));
        MD5_Final(result, &ctx);
	for(int i = 0; i < 16; i++)
        	sprintf(&(dir[i*2 + dirlen]), "%02hhx", result[i]&0xff);
	
	dirfd = open(dir,O_RDONLY);
	if(dirfd == -1 ){
		mkdir(dir,0777);
	}else{
		close(dirfd);
	}
	if(chdir(dir) == -1){
		puts("something error !");
		exit(0);
	}
	if(chmod(dir,0777) == -1){
		puts("something error !");
		exit(0);
	}


	if(chroot(".") == -1){
		puts("chroot error !");
		exit(-1);
	}
	setresgid(1000,1000, 1000);
	setresuid(1000,1000, 1000);


}

void read_flag(){
	int fd = -1 ;
	FILE *fp = NULL;
	fp = fopen("/home/message_center/flag","r");
	if(!fp){
		puts("open flag failed");
	}
	fread(flag,FLAGLEN,1,fp);	
}

void initproc(){
    setvbuf(stdin,0,2,0);
    setvbuf(stdout,0,2,0);
    setvbuf(stderr,0,2,0);
    signal(SIGALRM,handler);
    read_flag();
    chroot_dir();
    reg_user("admin","!toor!1234");
    reg_user("ddaa","p@ssw0rd");
    reg_user("ctf","nogamenolife");
}

void userinfo(){
	if(cur_user){
		printf(" ID : %ld\n",cur_user->id);
		printf(" UserName: %s\n",cur_user->username);
		printf(" Password: %s\n",cur_user->password);

	}else{
		puts("No such user!");
	}
	
}

void changepass(){
	char pass[0x100];
	memset(pass,0,0x100);
	if(cur_user){
		printf("Current Password:");
		read_input(pass,0xff);
		if(!strcmp(cur_user->password,pass)){
			printf("New Password:");
			read_input(cur_user->password,0x10);
			puts("Done !");
		}else{
			puts("Incorrect password !");
		}

	}else{
		puts("No such user!");
	}
	
}

void displaymsg(){


	bool is_read = false;
	unsigned int idx = 0 ;
	int cnt = 0 ;
	for(int i = 0 ; i < 16 ; i++){
		if(msglist[i]){
			cnt++;
		}
	}
	if(cnt > 0){
		printf("Index:");
		idx = read_long();
	}else{
		puts("No message can be read");
		return ;

	}
	if(idx < 16 && cnt > 0 ){
		if(msglist[idx]){
			printf("Message: %s\n",msglist[idx]->msg);
		}else{
			puts("No such message");
			return ;
		}
	}else {
		puts("Out of bound");
		exit(-1);
	}
	if(!msglist[idx]->MsgInfo.Read)
		msglist[idx]->MsgInfo.Read = 1;
	else
		is_read = true;
	size_t unread = 0 ;
	int fd = -1;
	char mfile[0x10];
	memset(mfile,0,0x10);
	sprintf(mfile,"%ld",cur_user->id);
	fd = open(mfile, O_RDWR);
	if(fd < 0){
		puts("No message");
		return; 
	}
	if(read(fd,&unread,sizeof(unread)) != sizeof(unread)){
		close(fd);
		puts("No message");
		return ;
	}
	if(is_read)
		unread--;
	lseek(fd,0,SEEK_SET);
	write(fd,&unread,sizeof(unread));
	lseek(fd,msglist[idx]->Offset,SEEK_SET);
	write(fd,&msglist[idx]->MsgInfo,sizeof(msglist[idx]->MsgInfo));
}


void genmsglist(int fd,bool unreadonly){
	struct Msg *curmsg = NULL;
	for(int i = 0 ; i < 16 ; i++){
		if(msglist[i]){
			free(msglist[i]->msg);
			msglist[i]->msg = NULL;
			free(msglist[i]);
			msglist[i] = NULL;
		}
	}
	for(int i = 0 ; i < 16 ; i++){
		curmsg = (struct Msg*)malloc(sizeof(struct Msg));

		while(1){
			curmsg->Offset = lseek(fd,0,SEEK_CUR);
			if(read(fd,&curmsg->MsgInfo,sizeof(curmsg->MsgInfo)) != sizeof(curmsg->MsgInfo) ){
				free(curmsg);
				curmsg = NULL;
				break;
			}
			if(!unreadonly || !curmsg->MsgInfo.Read)
				break;
			lseek(fd,curmsg->MsgInfo.subjectlen + curmsg->MsgInfo.msglen,SEEK_CUR);
		}
		if(!curmsg)
			break;
		read(fd,curmsg->subject,curmsg->MsgInfo.subjectlen);
		curmsg->msg = (char*)malloc(curmsg->MsgInfo.msglen);
		read(fd,curmsg->msg,curmsg->MsgInfo.msglen);
		msglist[i] = curmsg ;
	}
}


int listunreadmsg(){
	char mfile[0x10];
	size_t unread = 0 ;
	int fd = -1 ;
	int cnt = 0;
	memset(mfile,0,0x10);
	sprintf(mfile,"%ld",cur_user->id);
	fd = open(mfile, O_RDONLY);
	if(fd < 0){
		puts("No message");
		return 0; 
	}
	if(read(fd,&unread,sizeof(unread)) != sizeof(unread)){
		close(fd);
		puts("No message");
		return 0;
	}
	genmsglist(fd,true);
	struct user *fromuser = NULL;
	
	for(int i = 0 ; i < 16 ; i++){
		if(msglist[i]){
			fromuser = search_user_by_id(msglist[i]->MsgInfo.FromID);
			if(fromuser){
				printf("Index: %d\n",i);
				printf("From: %s  Subject: %s\n",fromuser->username, msglist[i]->subject);		
				cnt++;
			}
		}
	}
	close(fd);
	return cnt;
}

int listallmsg(){
	char mfile[0x10];
	size_t unread = 0 ;
	int fd = -1 ;
	int cnt = 0 ;
	memset(mfile,0,0x10);
	sprintf(mfile,"%ld",cur_user->id);
	fd = open(mfile, O_RDONLY);
	if(fd < 0){
		puts("No message");
		return 0 ; 
	}
	if(read(fd,&unread,sizeof(unread)) != sizeof(unread)){
		close(fd);
		puts("No message");
		return 0;
	}
	genmsglist(fd,false);
	for(int i = 0 ; i < 16 ; i++){
		if(msglist[i]){
			printf("Index: %d\n",i);
			printf("From: %s  Subject: %s\n",search_user_by_id(msglist[i]->MsgInfo.FromID)->username, msglist[i]->subject);		
			cnt++;
		}
	}
	close(fd);
	return cnt;
}



void delmsg(){
	int cnt = 0 ;
	for(int i = 0 ; i < 16 ; i++){
		if(msglist[i]){
			printf("Index: %d\n",i);
			printf("From: %s  Subject: %s\n",search_user_by_id(msglist[i]->MsgInfo.FromID)->username, msglist[i]->subject);		
			cnt++;
		}
	}
	unsigned int idx = 0 ;
	int fd = -1;
	char mfile[0x10];
	memset(mfile,0,0x10);
	if(cnt > 0){
		printf("Delete Message Index:");
		idx = read_long();
		if(idx >= 16){
			puts("Out of bound");
			exit(-1);
		}
		if(!msglist[idx]){
			puts("no such message !");
			return ;
		}
	}else{
		puts("No message can be delete");
		return ;

	}
	sprintf(mfile,"%ld",cur_user->id);
	fd = open(mfile, O_RDWR);
	if(fd < 0 ){
		puts("No message");
		return;
	}
	unsigned int movcnt = msglist[idx]->MsgInfo.subjectlen + msglist[idx]->MsgInfo.msglen + sizeof(msglist[idx]->MsgInfo);
	char *buf = (char*)calloc(1,0x16000);
	size_t start = 0 ;
	size_t readsize = 0;
	lseek(fd,msglist[idx]->Offset + movcnt,SEEK_SET);
	while(1){
		start = lseek(fd,0,SEEK_CUR);
		readsize = read(fd,buf,0x16000);
		if(readsize <= 0)
			break;
		lseek(fd,start-movcnt,SEEK_SET);
		write(fd,buf,readsize);
		lseek(fd,start+readsize,SEEK_SET);
	}

	free(buf);
	start = lseek(fd,0,SEEK_END);
	start -= movcnt ;
	ftruncate(fd,start);
	close(fd);

	memset(mfile,0,0x10);
	sprintf(mfile,"%ld",cur_user->id);
	fd = open(mfile, O_RDONLY);
	if(fd < 0){
		puts("No message");
		return; 
	}
	lseek(fd,0,SEEK_SET);
	genmsglist(fd,false);
	close(fd);
}

void delallmsg(){
	char mfile[0x10];
	size_t unread = 0 ;
	int fd = -1 ;
	int cnt = 0 ;
	memset(mfile,0,0x10);
	sprintf(mfile,"%ld",cur_user->id);
	unlink(mfile);
}

void readmsg(){

	while(1){
		read_menu();
		switch(read_long()){
			case 1:
				listallmsg();
				break;
			case 2:
				listunreadmsg();
				break;
			case 3:
				displaymsg();
				break;
			case 4:
				delmsg();
				break;
			case 5:
				delallmsg();
				break;
			case 6:
				return;
				
			default :
				break;

		}
	}
}

void sendmsg(){
	char username[0x100];
	char subject[0x28];
	char mfile[0x10];
	char *msg = NULL;
	struct Msg NewMsg ;
	struct user *target_user = NULL;
	size_t unread = 0 ;
	uint16_t size = 0 ;
	int fd = -1 ;
	memset(username,0,0x100);
	memset(subject,0,0x28);
	memset(mfile,0,0x28);
	printf("To: ");
	read_input(username,0xff);
	target_user = search_user(username);
	if(target_user){
		printf("Subject	:");
		read_input(subject,0x20);
		printf("Length of Message:");
		size = read_long();
		if(size > 0x1000){
			puts("Too large");
			exit(1337);
		}
		msg = (char*)malloc(size);
		if(!msg){
			puts("Error !");
			exit(-1);
		}
		printf("Message:");
		read_input(msg,size);
		if(strlen(msg) > size){
			puts("Broken ?");
			exit(-2);
		}
		NewMsg.MsgInfo.FromID = cur_user->id;
		NewMsg.MsgInfo.subjectlen = strlen(subject);
		NewMsg.MsgInfo.msglen = strlen(msg) ;
		memcpy(NewMsg.subject,subject,strlen(subject));
		NewMsg.msg = msg ;
		sprintf(mfile,"%ld",target_user->id);
		fd = open(mfile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		if(fd < 0){
			puts("Can not save the msg");
			exit(-3);
		}
		if(read(fd,&unread,sizeof(unread)) != sizeof(unread))
			unread = 0;
		lseek(fd,0,SEEK_SET);
		unread++;
		write(fd,&unread,sizeof(unread));
		lseek(fd,0,SEEK_END);
		write(fd,&NewMsg.MsgInfo,sizeof(NewMsg.MsgInfo));
		write(fd,NewMsg.subject,NewMsg.MsgInfo.subjectlen);
		write(fd,NewMsg.msg,NewMsg.MsgInfo.msglen);
		close(fd);

		puts("Done !");

	}else {
		puts("No such user");
	}
	return ;	
}

int main(){
	initproc();
			
	while(1){
		if(!is_login){
			login_menu();
			switch(read_long()){
				case 1:
					login();
					break;
				case 2:
					puts("Not implement");
					break;
				case 3:
					puts("bye");
					exit(0);
					break;
				default:
					puts("Invalid");
					break;

			}

		}else{
			menu();
		
			switch(read_long()){
				case 1:
					is_login = false ;
					cur_user = NULL;
					break;
				case 2:
					userinfo();
					break;
				case 3:
					changepass();
					break;
				case 4:
					readmsg();
					break;
				case 5:
					sendmsg();
					break;
				case 6:
					puts("Bye !");
					exit(0);
					break;
				default:
					puts("Invalid");
					break;

			}
		
		
		
		
		
		}

	}
}
