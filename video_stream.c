#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>         
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


static void send_dgram(int comm_socket, const char *buffer, int len)
{
        struct sockaddr_in snd_addr;
	memset(&snd_addr, 0, sizeof(snd_addr));
        snd_addr.sin_family = AF_INET;
        snd_addr.sin_port = htons(11111);
        inet_aton("127.0.0.1", &snd_addr.sin_addr);
	sendto(comm_socket, buffer, len, 0, (struct sockaddr *) &snd_addr, sizeof(snd_addr));
}


int main(void)
{
   char buffer[2048];
   FILE *fp = fopen("dump.data", "rb");
   if(!fp) {
      perror("fopen()");
      exit(-1);
   }

   int comm_socket = socket(AF_INET, SOCK_DGRAM, 0);
   if(comm_socket<0) {
	 perror("socket()");
         fclose(fp);
	 exit(-1);
    }

   for(;;) {
   rewind(fp);
   ssize_t len;
   while(!feof(fp)) {
     int lenbytes = fread(&len, sizeof(ssize_t), 1, fp);
     if(lenbytes<0) {
        perror("fread()");
        exit(-1);
     }
     int nbytes = fread(buffer, 1, len, fp);
     if(nbytes<0) {
         perror("fread()");
         exit(-1);
     }
     printf("nbytes=%d\n", nbytes);
     send_dgram(comm_socket, buffer, nbytes);
     usleep(10000);
   }
   }
   close(comm_socket);
   fclose(fp);
   return 0;
}
