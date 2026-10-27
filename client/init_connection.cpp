#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <vector>

using namespace std;

int main(void){

    int clientId;
    clientId=socket(PF_INET,SOCK_STREAM,0); 
    if (clientId == -1) {
        perror("Socket creation failed");
        return 1;
    }

    sockaddr_in client_addr{};
    client_addr.sin_family=AF_INET;
    client_addr.sin_port=htons(3333);
    client_addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);

    if (connect(clientId, (sockaddr*)&client_addr, sizeof(client_addr)) == -1)
    {
        perror("Failed to connect");
        close(clientId);
        return 1;
    }

    char message[11] = "Helloworld";
    char response[10];
    for(int i=0;i<10;i++){
        if (write(clientId, message, 10) == -1) {
            perror("Write failed");
            break;
        }
        //read(clientId,response,10);
    }
    write(clientId,"close",5);
    write(clientId,"sth_good",8);
    shutdown(clientId,SHUT_RDWR);
    close(clientId);
    return 0;
}