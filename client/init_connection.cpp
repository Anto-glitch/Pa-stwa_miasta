#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <vector.h>

using namespace std;

int main(void){

    int clientId;
    clientId=socket(PF_INET,SOCK_STREAM,0); 
    sockaddr_in client_addr{
        .sin_family=AF_INET;
        .sin_port=htons(3333);
        .sin_addr.s_addr("127.0.0.0");
    };
    if(connect(clientId,(sockaddr*)&client_addr,sizeof(sockaddr)==-1))
    {
        perror("Failed to connect");
    }

    char message = "Helloworld";
    char response[10];
    for(int i;i<10;i++){
        write(clientId, message, 10);
        //read(clientId,response,10);
    }
    shutdown(clientId,SHUT_RDWR);
    close(clientId);
    return 0;
}