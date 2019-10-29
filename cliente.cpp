#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

struct sockaddr_in server_addr;
int    server_addr_len = sizeof(server_addr);
int    server_fd       = 0;
char   command[8193]   = {0};
char   buffer[8193]    = {0};

using namespace std;

int main() {

    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(3300);
    
    
    cout << "INFO: Trying to open server." << endl;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (server_fd == 0) {
		cout << "EXIT FAILURE: Socket error. Unable to reach server." << endl;
		return EXIT_FAILURE;
	} else {
		cout << "SUCCESS: Server found." << endl;
	}
  
    cout << "INFO: Trying to connect with server." << endl;	
	
    int connector = connect(server_fd, (struct sockaddr*)&server_addr, (socklen_t)server_addr_len);
    if (connector < 0) {
        cout << "EXIT FAILURE: Unable to connect with server." << endl;
        return EXIT_FAILURE;
    }

    read(server_fd, buffer, sizeof(buffer));
    cout<< "LOG -> " << buffer;

    int leave = 0;
    while (leave == 0) {
        cout<< "COMMAND: ";
        memset(command, 0, sizeof(command));
        memset(buffer, 0, sizeof(buffer));
        
        fgets(command, sizeof(command), stdin);
        
        command[strlen(command)-1] = '\0';
        
        if (strcmp(command, "exit") == 0) {
            
            cout << "INFO: Leaving connection." << endl;
            send(server_fd, command, strlen(command), 0);
            leave = 1;
            
        } else {
			
            send(server_fd, command, strlen(command), 0);
            read(server_fd, buffer, sizeof(buffer));
            cout<< "LOG -> " << buffer;
        }
    }
    
    cout << "INFO: Ending the connection." << endl;
    close(server_fd);
        
    cout << "SUCCESS: Conection terminated." << endl;
    return EXIT_SUCCESS;
}
