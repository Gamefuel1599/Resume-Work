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
#include "packet.cpp"

using namespace std;

int main(int argc, char *argv[]){
  
  struct sockaddr_in server;
  struct sockaddr_in client;
  int mysocket = 0;
  int i = 0;
  int packetAmt;
  socklen_t clen = sizeof(client);
  char payload[512];
  char RecPayload[512];
  int portNo = 0;
  int randPort = 0;
  int RecSeqNum = 0;
  int RecType;
  packet packPayload = packet(0,0,0,NULL);
  fstream MyFile;
  ofstream ArrivalLog("arrival.log");
  char* fileContents;

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

//ACK Creatiom

  MyFile.open(argv[2], ios::out);
  if(MyFile.is_open()){
    cout << "File opened" << endl;
  }
  else{
    cout << "not opened" << endl;
  }
  for(int i = 0; i < packetAmt+2; i++){
    if(recvfrom(mysocket, RecPayload, 512,0, (struct sockaddr *)&client, &clen)== -1){
      cout << "Failed to receive"<< endl;
    }
    cout << "Recived Packet: "<< i<< endl;
    //Create Packet to deserialize into
    packPayload = packet(0,0,strlen(RecPayload),RecPayload);
    //deserialize
    packPayload.deserialize(RecPayload);
    //get SeqNum
    RecSeqNum = packPayload.getSeqNum();
    char tmp[512];
    sprintf(tmp, "%d", RecSeqNum);
    //log the SeqNum
    ArrivalLog.write(tmp,strlen(tmp));
    ArrivalLog.write("\n", 1);
    //Check type
    RecType = packPayload.getType();
    if(RecType == 3){
      //EOT recived from client
      packPayload = packet(2, RecSeqNum+1, 0, NULL);
      if(sendto(mysocket, payload, 512, 0, (struct sockaddr *)&client, clen)== -1){
        cout << "Error in sendto function\n";
      }
      cout << "EOT Recieved Closing Connection" << endl;
      MyFile.close();
      ArrivalLog.close();
      close(mysocket);
    }
    else{
      //get and write data
      fileContents = packPayload.getData();
      //cout << fileContents << endl;
      packPayload.printContents();
      MyFile.write(fileContents, strlen(fileContents));
      //Generate ACK
      packPayload = packet(0, RecSeqNum, 0, NULL);
      memset(payload, 0, sizeof(payload));
      packPayload.serialize(payload);
      if(sendto(mysocket, payload, 512, 0, (struct sockaddr *)&client, clen)== -1){
        cout << "Error in sendto function\n";
      }
    }


    //get and write data
    /*fileContents = packPayload.getData();
    packPayload.printContents();
    MyFile.write(fileContents, strlen(fileContents));
    */
    
    
    //EOT Check
    /*RecType = packPayload.getType();
    printf("%d\n",RecType);
    cout << "packet number: " << i << endl;
    if(RecType == 3){
      //Create EOT Packet
      packPayload = packet(2, RecSeqNum, 0, NULL);
    }
    else if(RecType == 1){
      //Create ACK
      packPayload = packet(0, RecSeqNum, 0, NULL);
    }*/
    /*memset(payload,0, sizeof(payload));
    packPayload.serialize(payload);

    if(sendto(mysocket, payload, 512, 0, (struct sockaddr *)&client, clen)== -1){
      cout << "Error in sendto function\n";
    }*/
  }
  //MyFile.close();
 // ArrivalLog.close();
  //close(mysocket);
  
  return 0;
}
