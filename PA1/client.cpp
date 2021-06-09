// Andrew Lisotta (ajl482)

#include <iostream>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>   // if you want to use inet_addr() function
#include <string.h>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[]){
  
  struct hostent *s; 
  s = gethostbyname(argv[1]);
  
  struct sockaddr_in server;
  int mysocket = 0;
  socklen_t slen = sizeof(server);
  int portNo = 0;
  int buffSize;
  char payload[512];
  char fileContents[512];
  char numPacket[512];
  fstream MyReadFile;
  string fileLength;
  string fileBuff;
  MyReadFile.open(argv[3]);
  while(getline(MyReadFile, fileLength)){
    fileBuff.append(fileLength);
  }

  cout << fileBuff.length() << "Chars" << endl;
  buffSize = fileBuff.length();
  buffSize = (buffSize+2)/4;
  cout << buffSize << " Num Packets" << endl;
  MyReadFile.close();
  
  portNo = stoi(argv[2]);

  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    cout << "Error in creating socket.\n";
    
  memset((char *) &server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(portNo);
  bcopy((char *)s->h_addr, 
	(char *)&server.sin_addr.s_addr,
	s->h_length);
      
  for (int i=0; i<1; i++) {
    sprintf(numPacket, "%d", buffSize);
    //strcpy()
    cout << "Sending test packet " << endl;
    cout << numPacket << endl;
    if (sendto(mysocket, numPacket, 512, 0, (struct sockaddr *)&server, slen)==-1)
      cout << "Error in sendto function.\n";
  }
  
  recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&server, &slen); 
  cout << payload << endl;
  
  close(mysocket);

  portNo = stoi(payload);

//2nd Socket Data Transfer
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
      cout << "Error in creating socket.\n";
      
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(portNo);
    bcopy((char *)s->h_addr, 
    (char *)&server.sin_addr.s_addr,
    s->h_length);

  
  //get the text from file into str to check the number of packets req to send in 4 characters chunks
  MyReadFile.open(argv[3]);
  for (int i=0; i < buffSize + 1 ; i++) {
    MyReadFile.read(fileContents, 4);
    strcpy(payload, fileContents);
    cout << "Sending packet " << i << endl;
    cout << payload << endl;
    if (sendto(mysocket, payload, 512, 0, (struct sockaddr *)&server, slen)==-1){
      cout << "Error in sendto function.\n";
    }
    if (recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&server, &slen)==-1){
      cout << "Failed to receive.\n";
    }
    cout << "Received data: " << payload << endl; 
  }

  return 0;
}