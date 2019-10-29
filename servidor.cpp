#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CONNECTION 5

void* client(void* socket);

void get_path(char* path);

struct sockaddr_in server_addr;
int server_addr_size = sizeof(server_addr);
int server_fd = 0;
int client_fd = 0;
char initial_path[8193] = {0};

pthread_mutex_t buffer_mutex;

//CLIENT
FILE* file_p = NULL;
char buffer[8193] = {0};
char command[8193] = {0};

using namespace std;

int main() {
	
	
	server_addr.sin_family      = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port        = htons(3300);
	
	cout << "INFO: Trying to open server." << endl;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (server_fd == 0) {
		cout << "EXIT FAILURE: Unable to open server." << endl;
		return EXIT_FAILURE;
	} else {
		cout << "SUCCESS: Server successfully opened." << endl;
	}
	
	cout << "INFO: Trying to bind server file descriptor." << endl;	
	int binder = bind(server_fd, (struct sockaddr*)&server_addr, server_addr_size);
	
	if (binder < 0) {
		cout << "EXIT FAILURE: Unable to bind server." << endl;
		close(server_fd);
		return EXIT_FAILURE;
	} else {
		cout << "SUCCESS: Server bind successfully done." << endl;
	}
	
	cout << "INFO: Trying to listen server connections." << endl;
	int listener = listen(server_fd, MAX_CONNECTION);
	
	if (listener < 0) {
		cout << "EXIT FAILURE: Unable to listen connections." << endl;
		close(server_fd);
		return EXIT_FAILURE;
	} else {
		cout << "SUCCESS: Server is listening for connections." << endl << endl;
	}
	
	pthread_t tid[500];
	pthread_mutex_init(&buffer_mutex, NULL);
	
	get_path(initial_path);
	
	while (1) {
		cout << "INFO: Trying to establish connection." << endl;
		client_fd = accept(server_fd, (struct sockaddr*)&server_addr, (socklen_t*)&server_addr_size);
		if (client_fd < 0) {
			cout << "DENIED: Connection not accepted." << endl;
		} else {
			cout << "ALLOWED: Connection " << client_fd << " accepted." << endl;
			pthread_create(&tid[client_fd], NULL, client, (void*)&client_fd);
            pthread_detach(tid[client_fd]);
		}
	}
	
	cout << "INFO: Closing server." << endl;
	pthread_mutex_destroy(&buffer_mutex);
	
	close(server_fd);
	cout << "SUCCESS: Server closed." << endl;
	
	
	return EXIT_SUCCESS;	
}

void* client(void* socket) {
	
	int sock = *(int*)socket;
	int leave = 0;
	int comparator = 0;
	char* new_path = NULL;
	char path[8193] = {0};
	
	//COPY PATH
	strcpy(path, initial_path);
	
	//LOCK MUTEX
	pthread_mutex_lock(&buffer_mutex);
	
	snprintf(buffer, sizeof(buffer), "CONNECTED AT SERVER PORT 3300. \n\n Try one:\n'mkdir': create directory. \n'rmdir': Delete directory.\n'cd': enter directory.\n'ls': list directory.\n'touch': create file.\n'rm': remove file.\n'cat': show content of file.\n'echo msg': send message.\n'exit': exit server.\n\n");
    send(sock, buffer, strlen(buffer), 0);
	
	pthread_mutex_unlock(&buffer_mutex);
	//UNLOCK MUTEX
	
	while (leave == 0) {
		//LOCK MUTEX
		pthread_mutex_lock(&buffer_mutex);
		
		memset(command, 0, sizeof(command));
        memset(buffer, 0, sizeof(buffer));
        read(sock, command, sizeof(command));
        
        cout << "UPDATE: New command from " << sock << " received." << endl;
        
        comparator = strcmp(command, "exit");
        
        if (comparator == 0) {
			
			close(sock);
			leave = 1;
			
		} else if ((command[0] == 'c') && (command[1] == 'd')) {
			
			new_path = &command[3];
            chdir(new_path);
            get_path(path);
            snprintf(buffer, sizeof(buffer), "SUCCESS: New Path: %s\n", path);
            send(sock, buffer, strlen(buffer), 0);
            
		} else {
			file_p = popen(command, "r");
            fread(buffer, sizeof(char), 8193, file_p);
            pclose(file_p);
            if (strcmp(buffer,"") == 0) snprintf(buffer, sizeof(buffer), "\n");
            send(sock, buffer, strlen(buffer), 0);
		}
		
		cout << "UPDATE: Command from " << sock << " answered." << endl;
		
		pthread_mutex_unlock(&buffer_mutex);
		//UNLOCK MUTEX
	}
	
	cout << "INFO: Thread fechada." << endl;
	pthread_exit(NULL);
}

void get_path(char* path) {

    char aux[8193] ={0};
    FILE* file_p = popen("pwd", "r");
    fread(aux, sizeof(char), 8193, file_p);
    pclose(file_p);
    int j = 0;
    for (int i = 0; i <= 8193; i++) {
        if (aux[i] == ' ') {
            path[j++] = '\\';
            path[j++] = ' ';
        } else path[j++] = aux[i];
        if (j > 8193) break;
    }
    path[strlen(path)-1] = '/';

    return;
}
