#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
using namespace std;
int main (void){
    int serverId;
    sockaddr_in server_addr{};
        server_addr.sin_family=AF_INET;
        server_addr.sin_port=htons(3333);
        server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serverId=socket(PF_INET, SOCK_STREAM,0);
        if (serverId == -1) {
            perror("Socket creation failed");
            return 1;
        }

        vector <string> messages; 
    if(bind(serverId,(sockaddr*)&server_addr,sizeof(server_addr))==-1){
        perror("Bind failed");
        return 1;
    }
    if (listen(serverId, 1) == -1) {
        perror("Listen failed");
        close(serverId);
        return 1;
    }

    int clientId;
    clientId = accept(serverId,nullptr,nullptr);
    if (clientId == -1) {
        perror("Accept failed");
        close(serverId);
        return 1;
    }

    string receive="";
    char buffer[1024];
    while (receive != "close") {
        ssize_t bytes_read = read(clientId, buffer, sizeof(buffer) - 1);
        if (bytes_read == -1) {
            perror("Read failed");
            break;
        }
        if (bytes_read == 0) {
            break;
        }

        buffer[bytes_read] = '\0';
        receive.assign(buffer, bytes_read);
        messages.push_back(receive);
    }
    cout<<messages.size()<<endl;
    for(size_t i=0;i<messages.size();i++){
        cout<<messages[i]<<endl;
    }
    shutdown(clientId,SHUT_RDWR);
    close(clientId);
    close(serverId);


    return 0;
}