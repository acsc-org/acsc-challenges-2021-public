#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node_data{
	char data[0x8];
	char name[0x4];
};
struct node{
	struct node_data *data;
	struct node *next;
	struct node *prev;
};

char c_memory[1000];
unsigned int c_size = 0;

char* freelist[10];
int free_head=0;


struct node *head;
struct node_data *temp_memory;


char* c_malloc(unsigned int size){
	int temp = 0;

	if(size < 4)
		size = 4;

	if(c_size + (size+4) > 999){
		printf("No Space :(\n");
		exit(1);
	}

	if(free_head==0){
		temp = (size+4);
		memcpy(&c_memory[c_size],&temp,4);
		temp = c_size + 4;
		c_size += (size + 4);
		return &c_memory[temp];
	}
	else{
		return freelist[--free_head];
	}
}

void c_free(char *ptr){
	if(free_head==10)
		return;
	freelist[free_head++] = ptr; 
}

void init(){
	setvbuf(stdout, 0, 2, 0);
	setvbuf(stdin, 0, 2, 0);
	setvbuf(stderr, 0, 2, 0);
}

int main(){
	char input[0x18];
	char data[0x9];
	char name[0x5];
	char* split;
	struct node *n_node;
	struct node_data *n_data;
	struct node *temp;

	init();
	while(1){
		memset(input,0,0x18);	
		memset(data,0,0x9);
		memset(name,0,0x5);

		printf(">>> ");
		read(0,input,0x10);
		if(input[strlen(input)-1] == '\n')
			input[strlen(input)-1] = '\x00';
		split = strchr(input,'=');

		if(split){

			if(strlen(input)-strlen(split+1)-1 > 4)
				strncpy(name,input,4);
			else
				strncpy(name,input,strlen(input)-strlen(split+1)-1);

			if(strlen(split+1) > 8)
				strncpy(data,split+1,8);
			else
				strncpy(data,split+1,strlen(split+1));

			if(!head){
				n_data = c_malloc(sizeof(struct node_data));
				n_node = c_malloc(sizeof(struct node));
				n_node->data = n_data;
				n_node->next = NULL;
				n_node->prev = NULL;
				strncpy(n_data->name,name,strlen(name));
            	strncpy(n_data->data,split+1,strlen(split+1));
				head = n_node;
			}

			else{
				for(temp=head; temp!=NULL; temp=temp->next){
					if(!strncmp(temp->data->name,name,4)){
						temp_memory = c_malloc(sizeof(struct node_data));
						temp->data = temp_memory;
						strncpy(temp_memory->name,name,strlen(name));
						strncpy(temp_memory->data,split+1,strlen(split+1));
						break;
					}

					else if(!temp->next){
						n_data = c_malloc(sizeof(struct node_data));
						n_node = c_malloc(sizeof(struct node));
						n_node->data = n_data;
						n_node->next = NULL;
						n_node->prev = NULL;
						strncpy(n_data->name,name,strlen(name));
            			strncpy(n_data->data,split+1,strlen(split+1));
						temp->next = n_node;
						n_node->prev = temp;
						break;
					}
				}
			}
		}

		else{
			if(!strncmp("delete",input,6)){
				for(temp=head; temp!=NULL; temp=temp->next){
					if(!strncmp(temp->data->name,&input[7],4)){
						if(temp->prev)
							temp->prev->next = temp->next;
						if(temp->next)
							temp->next->prev = temp->prev;

						c_free(temp->data);
						c_free(temp);

						printf("delete!\n");
						break;
					}
				}
			}

			else if(!strncmp("clear",input,5)){
				head = NULL;
				printf("clear!\n");
			}

			else if(!strncmp("edit",input,4)){
				for(temp=head; temp!=NULL; temp=temp->next){
					if(!strncmp(temp->data->name,&input[5],4)){
						read(0,name,0x5);
						if(name[strlen(name)-1] == '\n')
							name[strlen(name)-1] = '\x00';
						strncpy(temp->data->name,name,4);
						break;
					}
				}
			}

			else{
				for(temp=head; temp!=NULL; temp=temp->next){
					if(!strncmp(temp->data->name,input,4)){
						printf("%s\n",temp->data->data);
						break;
					}
				}
			}


		}


	}
}
