// Andrew Lisotta (ajl482)

#include<iostream>
#include <fstream>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <time.h>        // used for random number generation
#include <string.h> // using this to convert random port integer to string
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]){
  
  struct sockaddr_in server;
  struct sockaddr_in client;
  int mysocket = 0;
  int i = 0;
  int packetAmt;
  socklen_t clen = sizeof(client);
  char payload[512];
  int portNo = 0;
  int randPort = 0;
  fstream MyFile("zorkzorkzork.txt", fstream::out);
  string fileContents;

  //Random Port Selection
  portNo = stoi(argv[1]);
  cout << portNo << endl;

  srand(time(NULL));
  randPort = rand() % 65535 + 1025;
  cout << "The random port chosen is: " << randPort << endl;

  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in socket creation.\n";
  
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(portNo);
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(mysocket, (struct sockaddr *)&server, sizeof(server)) == -1)
    cout << "Error in binding.\n";
  

  for (i=0; i<1; i++) {
    if (recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&client, &clen)==-1)
      cout << "Failed to receive.\n";

    cout << "Received data: " << payload << endl;
    packetAmt = atoi(payload); 
  }

  char tmp[512];
  sprintf(tmp, "%d ", randPort);
  
  if (sendto(mysocket, tmp, 512, 0, (struct sockaddr *)&client, clen)==-1){
    cout << "Error in sendto function.\n";
  }
  
  close(mysocket);

//2nd Data Transfer socket
    if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
      cout << "Error in socket creation.\n";
    
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(randPort);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(mysocket, (struct sockaddr *)&server, sizeof(server)) == -1)
      cout << "Error in binding.\n";

//ACK Creation
  char ack[64] = "ACK";
  //MyFile.open("zorkzorkzork.txt", ios::out);
  if(MyFile.is_open()){
    cout << "File opened" << endl;
  }
  else{
    cout << "not opened" << endl;
  }
  for(int i = 0; i < packetAmt+1; i++){
    if(recvfrom(mysocket, payload, 512,0, (struct sockaddr *)&client, &clen)== -1){
      cout << "Failed to receive"<< endl;
    }
    fileContents = payload;
    cout << fileContents << endl;
    MyFile.write(payload, 4);
    //cout << payload << endl;

    if(sendto(mysocket, ack, 64, 0, (struct sockaddr *)&client, clen)== -1){
      cout << "Error in sendto function\n";
    }
  }
  MyFile.close();
  close(mysocket);
  
  return 0;
}
