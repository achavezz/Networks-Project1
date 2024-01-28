#include "NetworkHeader.h"

/* function declarations */

int main (int argc, char *argv[]) {

  /* There are certainly slicker ways of doing this,
   * but I figured there's no need to make the code
   * any more confusing than it has to be at this point.
   */

  // Argument parsing variables
  char *firstName;
  char *lastName;
  char *serverHost = SERVER_HOST;
  unsigned short serverPort = atoi(SERVER_PORT);
  char *servPortString;
  char c;
  int i;


  if ((argc < 5) || (argc > 7)) {
    printf("Error: Usage Project0Client [-s <server IP>[:<port>]] -f <firstName> -l <lastName>\n");
    exit(1);
  }

  for (i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      c = argv[i][1];

      /* found an option, so look at next
       * argument to get the value of
       * the option */
      switch (c) {
      case 'f':
	firstName = argv[i+1];
	break;
      case 'l':
	lastName = argv[i+1];
	break;
      case 's':
	serverHost = strtok(argv[i+1],":");
	if ((servPortString = strtok(NULL, ":")) != NULL) {
	  serverPort = atoi(servPortString);
	}
	break;
      default:
	break;
      }
    }
  }

    /* Your networking code starts here */
  char result[BUFFSIZE];
  char *base = "CS332";
  char *hello = "HELLO";
  char *bye = "BYE";
  sprintf(result, "%s %s %s %s\n", base, hello, firstName, lastName);
  //Error handiling function
  void DieWithError(char *errorMessage){
    printf("%s", errorMessage);
  }

  //creating socket
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sock < 0)
    DieWithError("socket() failed");

  //Construct the server address structure
  struct sockaddr_in servAddr; //Server address
  memset(&servAddr, 0, sizeof(servAddr)); //Zero out structure
  servAddr.sin_family = AF_INET; //IPv4

  //Convert Address
  int rtnVal = inet_pton(AF_INET, serverHost, &servAddr.sin_addr.s_addr);
  if (rtnVal == 0)
    DieWithError("inet_pton() failed, invalid string address");
  else if (rtnVal < 0)
    DieWithError("inet_pton() failed");
  servAddr.sin_port = htons(serverPort); //Server Port htons() was here

  //Establish connection to echo server
  if(connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    DieWithError("connect() failed");

  size_t echoStringLen = strlen(result); // Determine input length

  // Send the string to the server
  ssize_t numBytes = send(sock, result, echoStringLen, 0);
  if (numBytes < 0)
    DieWithError("send() failed");
  else if (numBytes != echoStringLen)
    DieWithError("send(), sent unexpected number of bytes");

  // Receive the same string back from the server
  fputs("Received: ", stdout); // Setup to print the echoed string
  char byeMessage[BUFFSIZE];

  //Receive the same string back from the server
  char *ackMessage; //ackMessage set up
  ackMessage = malloc(BUFFSIZE);
  int bool = 0;
  while(bool == 0){
    char buffer[BUFFSIZE]; // I/O buffer for ACK message
    numBytes = recv(sock, buffer, BUFFSIZE, 0);
    if(numBytes < 0)
      DieWithError("recv() failed");
    else if(numBytes == 0)
      DieWithError("recv(), connection closed prematurely");

    strcat(ackMessage,buffer); //Append buffer to 
    int x;
    for(x=0;x<sizeof(buffer);x++){
	if (buffer[x] == '\n') bool = 1;
    }
    fputs(buffer, stdout); // Print string
    
  }
   ackMessage += strlen(base) + 5;

  /*Send BYE message to the server*/
  sprintf(byeMessage, "%s %s ", base, bye); //setup BYE message as "CS332 BYE "
  strcat(byeMessage, ackMessage); //Append characters from copyACK+10 to end of byeMessage
  
  numBytes = send(sock, byeMessage, strlen(byeMessage), 0); //send BYE message to server
  if (numBytes < 0)
    DieWithError("send() failed");
  else if (numBytes != strlen(byeMessage))
    DieWithError("send(), sent unexpected number of bytes");

  fputc('\n', stdout); // Print a final linefeed

  close(sock);

  exit(0);
}
