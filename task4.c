#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct{
char dos_command[50];
char linux_command[50];
}commandMapping;

void execute_command(const char* command){
char buffer[100];
FILE* pipe = popen(command,"r");
if(!pipe){
printf("Error Executing command \n");
return;
}

while(fgets(buffer,sizeof(buffer),pipe) != NULL){
printf("%s",buffer);
}

pclose(pipe);
}

int main(){
FILE* fptr = fopen("mapping.txt","r");
if(!fptr){
printf("File not found");
return 1;
}

int numOfMappings = 0;
char lines[100];
while(fgets(lines,sizeof(lines),fptr)!= NULL)
{
numOfMappings++;
}

fseek(fptr,0,SEEK_SET);

commandMapping* mapping = malloc(numOfMappings * sizeof(commandMapping));
int i = 0;
while(fgets(lines,sizeof(lines),fptr) != NULL){
sscanf(lines,"%[^=]=%s",mapping[i].dos_command,mapping[i].linux_command);
i++;
}

fclose(fptr);

char userinput[100];
while(1){
printf("\n******SHELL SIMULATOR******\n");
printf("Enter DOS command (or 'exit' to quit): ");
fgets(userinput,sizeof(userinput),stdin);
userinput[strcspn(userinput,"\n")]='\0';

if(strcmp(userinput,"exit")==0){
break;
}
int found = 0;
for(int i = 0; i < numOfMappings;i++){
if(strcmp(userinput,mapping[i].dos_command)==0){
execute_command(mapping[i].linux_command);
found = 1;
break;
}
}
if(!found){
printf("Invalid command\n");
}
}
free(mapping);
return 0 ;
}
