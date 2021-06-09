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
#include "packet.cpp"

using namespace std;

//packet CreatePacket(int t, int s, int l, char* p);
//int GetSeqNum();

int main(int argc, char *argv[]){
  
  struct hostent *s; 
  s = gethostbyname(argv[1]);
  
  struct sockaddr_in server;
  int mysocket = 0;
  socklen_t slen = sizeof(server);
  int portNo = 0;
  int buffSize;
  char payload[512];
  char recivedACK[512];
  char fileContents[512];
  char numPacket[512];
  int SeqNum = 0;
  int LongestOutstanding = 0;
  int WindowSize = 7;
  int type;
  packet CurrentPack = packet(0,0,0,NULL);
  packet RecivePack = packet(0,0,0,NULL);

  fstream MyReadFile;
  ofstream ClientLog("clientseqnum.log");
  ofstream ClientACK("clientack.log");
  string fileLength;
  string fileBuff;
  
  MyReadFile.open(argv[3]);
  while(getline(MyReadFile, fileLength)){
    fileBuff.append(fileLength);
  }

  cout << fileBuff.length() << "Chars" << endl;
  buffSize = fileBuff.length();
  buffSize = (buffSize)/30;
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
    cout << "Sending test packet " << endl;
    cout << numPacket << endl;
    if (sendto(mysocket, numPacket, 512, 0, (struct sockaddr *)&server, slen)==-1)
      cout << "Error in sendto function.\n";
  }
  
  recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&server, &slen); 
  cout << payload << endl;
  
  close(mysocket);

  portNo = stoi(payload);



/////////////////ALL ABOVE CODE SIMPLY EXCHANGES THE CONNECTION PORT BEING USED FOR THE ACTUAL DATA TRANSFER////////////////////



//2nd Socket Data Transfer
  if ((mysocket=socket(AF_INET, SOCK_DGRAM, 0))==-1)
      cout << "Error in creating socket.\n";
      
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(portNo);
    bcopy((char *)s->h_addr, 
    (char *)&server.sin_addr.s_addr,
    s->h_length);

  //get the text from file into str to check the number of packets req to send in 30 characters chunks
  MyReadFile.open(argv[3]);
  for (int i=0; i <= buffSize+1; i++) {
    int check = MyReadFile.peek();
    if(check == EOF){
      //set packet type to 3 and len to 0 and data to NULL

      type = 3;
      //Create Packet
      CurrentPack = packet(type, (SeqNum % 8), 0, NULL);
      memset(payload,0,sizeof(payload));
      CurrentPack.serialize(payload);
      CurrentPack.printContents();
      if (sendto(mysocket, payload, 512, 0, (struct sockaddr *)&server, slen)==-1){
        cout << "Error in sendto function.\n";
      }
      cout<<"EOT Sent as packet:"<< i << endl;
      MyReadFile.close();
      ClientLog.close();
      ClientACK.close();
    }
    else{
      //Not EOF
      MyReadFile.read(fileContents, 30);
      strcpy(payload, fileContents);
      type = 1;
      //Create Packet
      cout << payload << endl;
      CurrentPack = packet(type, (SeqNum % 8), strlen(payload), payload);
      //CurrentPack.printContents();
      //Log SeqNum
      char tmp[512];
      sprintf(tmp, "%d", CurrentPack.getSeqNum() %8 );
      ClientLog.write(tmp, strlen(tmp));
      ClientLog.write("\n", 1); 

      //Seirliaze Packet
      //cout << payload << endl;
      memset(payload,0,sizeof(payload));
      CurrentPack.serialize(payload);
      //CurrentPack.printContents();

      //Send and Start Timer
      cout << "Sending packet " << i << endl;
      //CurrentPack.printContents();
      if (sendto(mysocket, payload, 512, 0, (struct sockaddr *)&server, slen)==-1){
      cout << "Error in sendto function.\n";
      }
      //Start Timer
      //timestart

      //Inc SeqNum
      SeqNum++;
    }

    if (recvfrom(mysocket, payload, 512, 0, (struct sockaddr *)&server, &slen)==-1){
      cout << "Failed to receive.\n";
    }
    //Deserialize packet
    RecivePack.deserialize(payload);
    int tmpSeqNum = RecivePack.getSeqNum();
    sprintf(payload, "%d", tmpSeqNum);
    //log ACKs
    ClientACK.write(payload, strlen(payload));
    ClientACK.write("\n", 1);
    //cout << "Received ACK: " << endl;

  }
  
  return 0;
}