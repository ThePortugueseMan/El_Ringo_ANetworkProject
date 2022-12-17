#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>

#define max(A, B) ((A) >= (B) ? (A) : (B))

void new(char _prd[], char _suc_TCP[], char _next_TCP_ip[], char _next_TCP_port[], char _prd_TCP_ip[], char _prd_TCP_port[], char _ip[], char _port[], char _chave[])
{
   if (strcmp(_next_TCP_port,"") != 0)
   {
      printf("Erro: Este processo ja tem um sucessor!\n");
      return;
   }

   if (strcmp(_prd,"") != 0)
   {
      printf("Erro: Este processo ja tem um predecessor!\n");
      return;
   }

   else
   {
      strcpy(_prd_TCP_ip,_ip);
      strcpy(_prd_TCP_port,_port);
      strcpy(_next_TCP_ip,_ip);
      strcpy(_next_TCP_port,_port);
      strcpy(_suc_TCP,_chave);
      strcpy(_next_TCP_ip,_ip);
      strcpy(_next_TCP_port,_port);
      strcpy(_prd,_chave);
   }
   return;
}

void reset_values(char _prd[], char _suc_TCP[], char _next_TCP_ip[], char _next_TCP_port[], char _prd_TCP_ip[], char _prd_TCP_port[])
{
   if (_next_TCP_port != NULL)
   {
      strcpy(_next_TCP_port,"");
   }

   if (_next_TCP_ip != NULL)
   {
      strcpy(_next_TCP_ip,"");
   }

   if (_prd_TCP_ip != NULL)
   {
      strcpy(_prd_TCP_ip,"");
   }

   if (_prd_TCP_port != NULL)
   {
      strcpy(_prd_TCP_port,"");
   }

   if (_suc_TCP != NULL)
   {
      strcpy(_suc_TCP,"");
   }

   if (_prd != NULL)
   {
      strcpy(_prd,"");
   }
   return;
}

void fecha(struct addrinfo *res_UDP, struct addrinfo *res_TCP, struct addrinfo *res_TCP_client, int fd_UDP, int fd_TCP, int fd_TCP_client, int newfd)
{
   printf("Fechado\n");
   close(fd_UDP);
   close(fd_TCP);
   //close(fd_TCP_client);
   //close(newfd);
   exit(0);
}

int splitter(char teclado[], char in1[], char in2[], char in3[], char in4[], char in5[])
{

   int code;
   int counter = 0;
   char delim[128] = " \n";

   char aux[128];
   strcpy(aux, teclado);

   char *ptr = strtok(aux, delim);
   //printf("SPLINTER\n");
   while (ptr != NULL)
   {
      if (counter == 1 && in1 != NULL)
      {
         strcpy(in1, ptr);
      }

      if (counter == 2 && in2 != NULL)
      {
         strcpy(in2, ptr);
      }

      if (counter == 3 && in3 != NULL)
      {
         strcpy(in3, ptr);
      }

      if (counter == 4 && in4 != NULL)
      {
         strcpy(in4, ptr);
      }

      if (counter == 5 && in5 != NULL)
      {
         strcpy(in5, ptr);
      }
      //printf("'%s'\n", ptr);
      ptr = strtok(NULL, delim);
      counter++;
   }
}

void create_UDP_server(int *_fd, char port[], struct addrinfo *_hints, struct addrinfo **_res)
{
   int fd;
   struct addrinfo hints;
   struct addrinfo *res;
   int errcode = 0;

   if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
   {
      close(fd);
      exit(1);
   }

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_INET;      // IPv4
   hints.ai_socktype = SOCK_DGRAM; // UDP socket
   hints.ai_flags = AI_PASSIVE;

   printf("Creating UDP server on port: %s\n", port);

   if ((errcode = getaddrinfo(NULL, port, &hints, &res)) != 0)
   {
      exit(1);
   }
   if (bind(fd, res->ai_addr, res->ai_addrlen) == -1)
   {
      printf("erro bind");
      exit(1);
   }

   // strcpy(next_port, port);
   *_fd = fd;
   *_hints = hints;
   *_res = res;
   freeaddrinfo(res);
}

void create_TCP_server(int *_fd, char port[], struct addrinfo *_hints, struct addrinfo **_res)
{
   struct addrinfo hints, *res;
   int fd, newfd, errcode;
   ssize_t n, nw;
   struct sockaddr addr;
   socklen_t addrlen;
   char *ptr, buffer[128];

   if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      printf("TCP socket error\n");
      close(fd);
      exit(1); // error
   }

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_INET;       // IPv4
   hints.ai_socktype = SOCK_STREAM; // TCP socket
   hints.ai_flags = AI_PASSIVE;

   if ((errcode = getaddrinfo(NULL, port, &hints, &res)) != 0) /*error*/
      exit(1);
   if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) /*error*/
      exit(1);
   if (listen(fd, 5) == -1) /*error*/
      exit(1);

   *_fd = fd;
   *_hints = hints;
   *_res = res;
   freeaddrinfo(res);
}

void run_servers (int *_fd_UDP, int *_fd_TCP, char UDP_port[], char TCP_port[], struct addrinfo *_hints_UDP, struct addrinfo *_hints_TCP, struct addrinfo **_res_UDP, struct addrinfo **_res_TCP)
{
   create_UDP_server(_fd_UDP, UDP_port, _hints_UDP, _res_UDP);
   create_TCP_server(_fd_TCP, TCP_port, _hints_TCP, _res_TCP);
}

int send_to_shortcut_UDP(int fd, char message[], char next_UDP[], char next_ip[], char next_port[], struct addrinfo _hints, struct addrinfo *_res)
{
   int errcode; //, fd
   struct timeval tv;
   tv.tv_sec=0;
   tv.tv_usec=10000;
   struct sockaddr addr;
   socklen_t addrlen;
   ssize_t n;
   fd_set fds;
   int counter;
   FD_ZERO(&fds);
   printf("Sending...\n");
   /*if (fd == -1)
   {
      close(fd);
      exit(1);
   }*/

   errcode = getaddrinfo(next_ip, next_port, &_hints, &_res);
   if (errcode != 0) /*error*/
      exit(1);
   n = sendto(fd, message, strlen(message), 0, _res->ai_addr, _res->ai_addrlen);
   if (n == -1) /*error*/
   {
      exit(1);
   }
   printf("Sent: %s \n", message);

   FD_SET(fd,&fds);
   counter=select(fd+1,&fds,NULL,NULL,&tv);
   if(counter == -1)
   {//erro
    printf("Erro no counter do send_to_shortcut_UDP");
    exit(1);
   }
   if(counter == 0)
   {
      printf("No UDP server reply!\n");
      strcpy(next_ip,"");
      strcpy(next_port,"");
      strcpy(next_UDP,"");

      return 0;
   }
    return 1;


}

void send_TCP(int *_fd, char message[], char ip[], char port[], struct addrinfo _hints, struct addrinfo *_res)
{
   struct addrinfo hints, *res;
   int n, fd;
   ssize_t nbytes, nleft, nwritten, nread;
   char *ptr, buffer[128];

   // ptr = strcpy(buffer, message);
   if (*_fd == -1) // se não existir fd_tcp_client, ou newfd ou old_newfd, senão usa o fd_TCP_client que já está connectado
   {
      fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket

      if (fd == -1)
      {
         close(fd);
         exit(1); // error
      }

      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_INET;       // IPv4
      hints.ai_socktype = SOCK_STREAM; // TCP socket

      n = getaddrinfo(ip, port, &hints, &res);

      if (n != 0) /*error*/
      {
         close(fd);
         exit(1);
      }

      n = connect(fd, res->ai_addr, res->ai_addrlen);
      if (n == -1) /*error*/
      {
         perror("Error: ");
         close(fd);
         exit(1);
      }
   }
   else// se existir usa esse _fd
   {
      fd = *_fd;
   }

   ptr = message;
   nbytes = strlen(message);
   nleft = nbytes;
   while (nleft > 0)
   {
      nwritten = write(fd, ptr, nleft);
      //if (nwritten <= 0) /*error*/ //exit(1);
      nleft -= nwritten;
      ptr += nwritten;
   }
   nleft = nbytes;
   ptr = buffer;


   if (*_fd == -1)
   {
      freeaddrinfo(res);
   }
   *_fd = fd;

   printf("Mensagem TCP para %s: %s\n",port, message);
}

void add_one_to_char(char _caracter[])
{
   int caracter;
   char novo_caracter[5];
   caracter = atoi(_caracter);         // converte char para int
   caracter = caracter + 1;            // adiciona 1
   sprintf(_caracter, "%d", caracter); // converte de volta em char
}

int d_32(char _ini[], char _fin[])
{
   int ini, fin;
   ini = atoi(_ini);
   fin = atoi(_fin);

   if(strcmp(_ini,"") == 0)
   {
      return 33;
   }

   int ans = fin - ini;

   if (ans >= 0 && ans < 32)
   {
      return ans;
   }

   else
   {
      if (ini == 0)
      {
         return fin;
      }
      if (fin == 0)
      {
         return 32 - ini;
      }

      ans = ans + (32 * ((fin / ini) + 1));
   }

   return ans;
}

void getip(char _ip[], char hostname[])
{
   struct addrinfo hints, *res, *p;
   int errcode;
   char buffer[INET_ADDRSTRLEN];
   struct in_addr *addr;
   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_INET; // IPv4
   hints.ai_socktype = SOCK_DGRAM;
   hints.ai_flags = AI_CANONNAME;
   if ((errcode = getaddrinfo(hostname, NULL, &hints, &res)) != 0)
      fprintf(stderr, "error: getaddrinfo: %s\n", gai_strerror(errcode));
   else
   {
      printf("canonical hostname: %s\n", res->ai_canonname);
      for (p = res; p != NULL; p = p->ai_next)
      {
         addr = &((struct sockaddr_in *)p->ai_addr)->sin_addr;
         printf("internet address: %s (%08lX)\n", inet_ntop(p->ai_family, addr, buffer, sizeof buffer), (long unsigned int)ntohl(addr->s_addr));
      }
      strcpy(_ip, buffer);
      freeaddrinfo(res);
   }
}

int main(int argc, char **argv)
{
   int running = 0;

   ssize_t n, nread; // data type - It represents the size of any object in bytes and returned by sizeof operator
   char buffer[128] = "";
   char teclado[128] = "";
   char aux[128] = "";
   char host_name[128] = "";
   char chave[20] = "";
   char chave_recebida[20] = "";
   char chave_find[20] = "";
   int result;
   char result_c[128] = "";
   char comando[128] = "";
   char tag[20] = "60";
   char ip[INET_ADDRSTRLEN];
   int i = 0;

   // SIGPIPE
   struct sigaction act;

   memset(&act, 0, sizeof act);
   act.sa_handler = SIG_IGN;
   if (sigaction(SIGPIPE, &act, NULL) == -1)
      exit(1);

   // ring variables
   char prd[20] = "";
   char suc_TCP[20] = "";
   char suc_UDP[20] = "";

   // UDP server variables
   struct addrinfo hints_UDP, *res_UDP;
   int fd_UDP = -1;
   int UDP_code = -1;
   int errcode;
   struct sockaddr addr_UDP;
   socklen_t addrlen_UDP;
   char UDP_port[20] = "";
   char next_UDP_port[20] = "";
   char next_UDP_ip[20] = "";

   //UDP special
   char UDP_special[20] = "";
   char UDP_ip_special[20] = "";
   char UDP_port_special[20] = "";
   struct sockaddr addr_UDP_special;
   socklen_t addrlen_UDP_special;

   // TCP server variables
   struct addrinfo hints_TCP, *res_TCP;
   int fd_TCP = -1;
   struct sockaddr addr_TCP;
   socklen_t addrlen_TCP;
   char *ptr;
   ssize_t n_TCP, nw;
   char TCP_port[20] = "";
   char next_TCP_ip[20] = "";
   char next_TCP_port[20] = "";

   // TCP client variables
   int fd_TCP_client = -1;
   struct addrinfo *res_TCP_client;
   struct sockaddr addr_TCP_client;
   socklen_t addrlen_TCP_client;
   int keyboard_sendtcp = 0;

   // TCP special connections
   char TCP_ip_special[20] = "";
   char TCP_port_special[20] = "";

   //TCP prd
   char prd_TCP_ip[20] = "";
   char prd_TCP_port[20] = "";

   // Select
   int newfd = -1, old_newfd = -1, afd = 0;
   fd_set rfds;
   int maxfd, counter;

   //FND variables
   char ori[5] = "";
   char ori_tag[5] = "";
   char ori_ip[20] = "";
   char ori_port[20] = "";

   if (argc == 2) // ./main 58002
   {
      strcpy(UDP_port, argv[1]);
      strcpy(TCP_port, argv[1]);
   }
   if (argc == 3) // ./main 58002 10
   {
      strcpy(UDP_port, argv[1]);
      strcpy(TCP_port, argv[1]);
      strcpy(chave, argv[2]);
   }
   if (argc == 4) // ./main 10 127.0.1.1 58001
   {
      strcpy(chave, argv[1]);
      strcpy(ip, argv[2]);
      strcpy(UDP_port, argv[3]);
      strcpy(TCP_port, argv[3]);
   }
   if (argc == 1)
   {
      strcpy(UDP_port, "58025");
      strcpy(chave, "25");
      strcpy(TCP_port, UDP_port);
      // strcpy(next_UDP_port,next_TCP_port);
      // strcpy(next_UDP_ip,ip);

   }

   /* for (int i = 0; i < argc; ++i)
    {
       printf("argv[%d]: %s\n", i, argv[i]); //print out the arguments passed to your C program
    }
   printf("argc = %d\n", argc );*/

   if (gethostname(host_name, 128) == -1)
   {
      printf("Erro hostname\n");
      exit(1);
   }
   else
   {
      printf("host name: %s\n", host_name);
   }
   getip(ip, host_name);

   run_servers (&fd_UDP, &fd_TCP, UDP_port, TCP_port, &hints_UDP, &hints_TCP, &res_UDP, &res_TCP);
   running = 1;
   // strcpy(next_TCP_ip,ip);
   while (1)
   {

      FD_ZERO(&rfds); // reset do set rfds

      if (running == 1)
      {
         FD_SET(0, &rfds);      // adiciona teclado ao set rfds
         FD_SET(fd_UDP, &rfds); // sx o servidor UDP ao set rfds
         FD_SET(fd_TCP, &rfds); // adiciona o servidor TCP ao set
         if (newfd != -1)
         { // se existir newfd dá set dele  (lẽ do servidor)
            FD_SET(newfd, &rfds);
         }
         if (fd_TCP_client != -1) //(lê do cliente)
         {// se gouver fd_tcp client dá set dele. Este fd recebe as mensagens quando se usa o anel, enquanto que o newfd recebe as mensagens quando se estabelece concexões pela primeira vez
            FD_SET(fd_TCP_client, &rfds);
         }
         // maxfd = fd_TCP;
      }
      else
      {
         FD_SET(0, &rfds); // adiciona teclado ao set rfds
         // maxfd = 1;
      }

      counter = select(FD_SETSIZE, &rfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)NULL);
      if (counter <= 0)
      {
         printf("Erro counter\n");
         fecha(res_UDP, res_TCP, res_TCP_client, fd_UDP, fd_TCP, fd_TCP_client, newfd);
         // exit(1);
      }

      switch (counter)
      {
      case -1:
         printf("Erro select -1\n");
         fecha(res_UDP, res_TCP, res_TCP_client, fd_UDP, fd_TCP, fd_TCP_client, newfd);
         // exit(1);
         break;

      default:
         if (FD_ISSET(0, &rfds))
         {
            if ((n = read(0, teclado, 127)) != 0)
            {
               if (n == -1)
               {
                  exit(1);
               }

               teclado[n] = 0;
               printf("From keyboard: %s\n", teclado);
               if (strcmp(teclado, "new\n") == 0)
               {
                  new(prd,suc_TCP,next_TCP_ip,next_TCP_port,prd_TCP_ip,prd_TCP_port, ip,TCP_port,chave);

               }

               else if (strcmp(teclado, "next\n") == 0)
               {
                  splitter(teclado, next_TCP_port, NULL, NULL, NULL, NULL);
               }

               else if (strcmp(teclado, "sendudp\n") == 0)
               {
                  if(strcmp(suc_UDP,"") != 0)
                  {
                        // strcpy(buffer, "Novo");
                      UDP_code = send_to_shortcut_UDP(fd_UDP, chave, suc_UDP, next_UDP_ip, next_UDP_port, hints_UDP, res_UDP);
                      if (UDP_code == 0)
                      {
                         printf("No UDP server reply\n");
                      }
                  }
                  else printf("Não posso enviar msg porque não existe sucessor UDP!\n");

               }

               else if (strcmp(teclado, "sendtcp\n") == 0)
               {
                  send_TCP(&newfd, chave, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP_client);
                  // keyboard_sendtcp = 1;
               }

               else if (strcmp(teclado, "show\n") == 0)
               {
                  fprintf(stdout, "Key: %s\nServer on: %s , UDP port: %s   TCP port: %s\n", chave, ip, UDP_port, TCP_port);
                  fprintf(stdout, "UDP shortcut: %s on port: %s\n", next_UDP_ip, next_UDP_port);
                  fprintf(stdout, "Prd: %s\n   - IP: %s     - Port: %s\n", prd, prd_TCP_ip,prd_TCP_port);
                  fprintf(stdout, "Suc: %s\n   - IP: %s     - Port: %s\n", suc_TCP, next_TCP_ip, next_TCP_port);
               }

               else if (strstr(teclado, "find") != NULL) // This function returns a pointer to the first occurrence in haystack of any of the entire sequence of characters specified in needle, or a null pointer if the sequence is not present in haystack.
               {
                  splitter(teclado, chave_find, NULL, NULL, NULL, NULL);
                  add_one_to_char(tag);
                   // devolve a tag incrementada de 1, serve como nº de sequência associado á pesquisa
                   // se a chave pretendida estiver mais proxima do nó TCP seguinte ... FND 15 n 10 10.IP 10.Port \n
                     if (d_32(suc_UDP,chave_find) < d_32(suc_TCP,chave_find) && d_32(suc_UDP,chave_find) < d_32(chave,chave_find))
                     {
                        sprintf(comando,"FND %s %s %s %s %s\n",chave_find,tag,chave,ip,TCP_port);
                        UDP_code = send_to_shortcut_UDP(fd_UDP,comando,suc_UDP,next_UDP_ip,next_UDP_port,hints_UDP,res_UDP);
                        if (UDP_code == 0)
                        {
                           sprintf(comando,"FND %s %s %s %s %s\n",chave_find,tag,chave,ip,TCP_port);
                           send_TCP(&newfd, comando, ip, next_TCP_port, hints_TCP, res_TCP_client);
                        }
                     }

                     else if(d_32(suc_TCP,chave_find) < d_32(chave,chave_find))
                     {
                        sprintf(comando,"FND %s %s %s %s %s\n",chave_find,tag,chave,ip,TCP_port);
                        send_TCP(&newfd, comando, ip, next_TCP_port, hints_TCP, res_TCP_client);
                     }
                     else
                     {
                        printf("Chave %s: nó %s (%s:%s)\n", chave_find, chave, ip, TCP_port);
                        strcpy(chave_find,"");
                     }

                  // se ele tiver a chave notifica o utilizador (este caso pois é keyboard), ou nó entrante


               }

               else if (strstr(teclado, "pentry") != NULL)
               {
                  //splitter(teclado, aux, next_TCP_ip, next_TCP_port, NULL);
                  splitter(teclado, aux, prd_TCP_ip, prd_TCP_port, NULL, NULL);

                  if (atoi(aux) >= atoi(chave)   )
                  {
                     printf("Erro: pdr >= chave\n");
                  }

                  else if(atoi(aux) < atoi(chave))
                  {
                     strcpy(prd,aux);
                     sprintf(comando, "SELF %s %s %s\n", chave, ip, TCP_port);
                     send_TCP(&fd_TCP_client, comando, prd_TCP_ip, prd_TCP_port, hints_TCP, res_TCP_client);
                  }

                  // é criado um fd_tcp_client(pois está == -1) dá connect, o nó i dá accept cria um newfd,
                  // e cria-se uma conexão entre eles
                  //printf("Mensagem TCP escrita: %s\n\n", comando);
               }

                else if (strstr(teclado, "echord") != NULL)
               {
                  //splitter(teclado, aux, next_TCP_ip, next_TCP_port, NULL);
                  strcpy(suc_UDP,"");
                  strcpy(next_UDP_ip,"");
                  strcpy(next_UDP_port,"");
               }

               else if (strstr(teclado, "chord") != NULL)
               {
                  //splitter(teclado, aux, next_TCP_ip, next_TCP_port, NULL);
                  splitter(teclado, suc_UDP, next_UDP_ip, next_UDP_port, NULL, NULL);
               }

               else if((strstr(teclado, "bentry") != NULL))
               {
                  splitter(teclado, UDP_special, UDP_ip_special,UDP_port_special,NULL,NULL);
                  sprintf(comando,"EFND %s\n",chave);
                  send_to_shortcut_UDP(fd_UDP,comando,UDP_special,UDP_ip_special,UDP_port_special,hints_UDP,res_UDP);
               }

               else if (strstr(teclado, "leave") != NULL)
               {
                  if (strcmp(prd_TCP_ip,"") != 0 && strcmp(prd_TCP_port,"") != 0 && strcmp(suc_TCP,chave) != 0)
                  {
                     strcpy(suc_TCP,"");
                     sprintf(comando, "PRED %s %s %s\n", prd, prd_TCP_ip, prd_TCP_port);
                     send_TCP(&newfd, comando, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP);
                     reset_values(prd,suc_TCP,next_TCP_ip,next_TCP_port,prd_TCP_ip,prd_TCP_port);
                     if( n=read(newfd,buffer,128)==0 )
                     {
                        close(newfd);
                        newfd = -1;
                     }

                     close(fd_TCP_client);
                     fd_TCP_client = -1;

                  }

                  else if (strcmp(suc_TCP,chave) == 0)
                  {
                     printf("Anel apagado\n\n");
                     reset_values(prd,suc_TCP,next_TCP_ip,next_TCP_port,prd_TCP_ip,prd_TCP_port);
                  }

                  else
                  {
                     printf("Não estou num no!\n");
                  }

               }

               else if (strstr(teclado, "exit") != NULL)
               {
                  if (running == 1)
                  {
                     fecha(res_UDP, res_TCP, res_TCP_client, fd_UDP, fd_TCP, fd_TCP_client, newfd);
                  }
                  exit(0);
               }
               else
               {
                  printf("Comando inválido\n");
               }

               break;
            }
         }

         else if (FD_ISSET(fd_UDP, &rfds) && !FD_ISSET(0, &rfds))
         {
            // UDP wait
            memset(buffer, 0, sizeof(buffer));
            addrlen_UDP = sizeof(addr_UDP);

            nread = recvfrom(fd_UDP, buffer, 128, 0, &addr_UDP, &addrlen_UDP);
            if (nread == -1)
            {
               exit(1);
            }
            printf("Rec: %s\n", buffer);


            if (strstr(buffer, "EFND") != NULL)
            {
               n = sendto(fd_UDP,"ACK",nread,0,&addr_UDP,addrlen_UDP);
               if(n==-1)
               {
                  exit(1);
               }
               splitter(buffer, chave_find, NULL, NULL, NULL, NULL);
               strcpy(UDP_special,chave_find);
               add_one_to_char(tag);
                  // devolve a tag incrementada de 1, serve como nº de sequência associado á pesquisa
                  // se a chave pretendida estiver mais proxima do nó TCP seguinte ... FND 15 n 10 10.IP 10.Port \n
                  if (d_32(suc_UDP,chave_find) < d_32(suc_TCP,chave_find) && d_32(suc_UDP,chave_find) < d_32(chave,chave_find))
                  {
                     sprintf(comando,"FND %s %s %s %s %s\n",chave_find,tag,chave,ip,TCP_port);
                     UDP_code = send_to_shortcut_UDP(fd_UDP,comando,suc_UDP,next_UDP_ip,next_UDP_port,hints_UDP,res_UDP);
                     if (UDP_code == 0)
                     {
                        sprintf(comando,"FND %s %s %s %s %s\n",chave_find,tag,chave,ip,TCP_port);
                        send_TCP(&newfd, comando, ip, next_TCP_port, hints_TCP, res_TCP_client);
                     }
                  }

                  else if(d_32(suc_TCP,chave_find) < d_32(chave,chave_find))
                  {
                     sprintf(comando,"FND %s %s %s %s %s\n",chave_find,tag,chave,ip,TCP_port);
                     send_TCP(&newfd, comando, ip, next_TCP_port, hints_TCP, res_TCP_client);
                  }
                  else
                  {
                     sprintf(comando,"EPRED %s %s %s\n", chave,ip,next_TCP_port);
                     n = sendto(fd_UDP,comando,nread,0,&addr_UDP_special,addrlen_UDP_special);
                     if(n==-1)
                     {
                        exit(1);
                     }
                     strcpy(chave_find,"");
                  }
               splitter(buffer,UDP_special,NULL,NULL,NULL,NULL);
               addr_UDP_special = addr_UDP;
               addrlen_UDP_special = addrlen_UDP;
            }

            else if (strstr(buffer, "FND") != NULL)
            {
               n = sendto(fd_UDP,"ACK",nread,0,&addr_UDP,addrlen_UDP);
               if(n==-1)
               {
                  exit(1);
               }
               //printf("FND recebido\n");
               splitter(buffer,chave_find,ori_tag,ori,ori_ip,ori_port);

               if(d_32(chave,chave_find) > d_32(suc_TCP,chave_find) || d_32(chave,chave_find) > d_32(suc_UDP,chave_find))
               {
                  if(d_32(suc_UDP,chave_find) < d_32(suc_TCP,chave_find))
                  {
                     UDP_code = send_to_shortcut_UDP(fd_UDP,buffer,suc_UDP,next_UDP_ip,next_UDP_port,hints_UDP,res_UDP);
                     if(UDP_code == 0)
                     {
                        send_TCP(&newfd, buffer, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP);
                     }

                  }

                  else if(d_32(suc_TCP,chave_find) < d_32(suc_UDP,chave_find))
                  {
                     send_TCP(&newfd, buffer, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP);
                  }
               }

               else
               {
                  sprintf(comando,"RSP %s %s %s %s %s\n",ori,ori_tag,chave,ip,TCP_port);
                  send_TCP(&newfd, comando, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP);
               }
            }

            else if (strstr(buffer, "EPRED") != NULL)
            {
               splitter(buffer, aux, prd_TCP_ip, prd_TCP_port, NULL, NULL);

                  if (atoi(aux) >= atoi(chave)   )
                  {
                     printf("Erro: pdr >= chave\n");
                  }

                  else if(atoi(aux) < atoi(chave))
                  {
                     strcpy(prd,aux);
                     sprintf(comando, "SELF %s %s %s\n", chave, ip, TCP_port);
                     send_TCP(&fd_TCP_client, comando, prd_TCP_ip, prd_TCP_port, hints_TCP, res_TCP_client);
                  }

                  // é criado um fd_tcp_client(pois está == -1) dá connect, o nó i dá accept cria um newfd,
                  // e cria-se uma conexão entre eles
                  //printf("Mensagem TCP escrita: %s\n\n", comando);
            }
         }
         else if (FD_ISSET(fd_TCP, &rfds))
         {
            addrlen_TCP = sizeof(addr_TCP);
            if (newfd != -1) //se já existe newfd... guarda esse newfd na oldfd para poder usar a conexão antiga mais uma vez
            {
               old_newfd = newfd;
            }

            // estrutura accept - int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
            // The  accept()  system  call  is used with connection-based socket types (SOCK_STREAM, SOCK_SEQPACKET).  It extracts the first connection request on the queue of pending connections for the listening socket, sockfd, creates  a  new  connected socket, and returns a new file descriptor referring to that socket.
            // The newly created socket is not in the listening state.  The original socket sockfd is unaffected by this call.
            if ((newfd = accept(fd_TCP, &addr_TCP, &addrlen_TCP)) == -1)
            {
               printf("Erro newfd accept!\n");
              fecha(res_UDP, res_TCP, res_TCP_client, fd_UDP, fd_TCP, fd_TCP_client, newfd);
            }

            memset(buffer, 0, sizeof(buffer));

            //i=0;
            if ((n = read(newfd, buffer, 128)) != 0)
            //while(i!=1)
            {
               //memset(aux, 0, sizeof(aux));
               //strcpy(aux, buffer);
               //splitter(aux, chave_recebida, NULL, NULL, NULL);

               // perror("Error: ");
               // n = read(newfd, buffer, 128);
               //n = read(newfd, buffer, 128);

               if (n == -1) /*error*/exit(1);

               //if(strstr(buffer,"\n")!=NULL) i=1;
               // ptr = &buffer[0];
               // while (n > 0)
               // {
               //  if ((nw = write(newfd, ptr, n)) <= 0) /*error*/
               //    exit(1);
               // On success, the number of bytes written is returned.  On error,  -1  is returned, and errno is set to indicate the cause of the error.
               // n -= nw;
               // ptr += nw;
               //}
            }
            printf("Mensagem TCP recebida: %s\n", buffer);

            if (strstr(buffer, "SELF") != NULL) // quando já há pelo menos 2 nós...
            {

               // 1º self- i aceitou o connect de j e criou uma ligação entre newfd dele e fd_TCP_cliet do nó j, também atualizou o seu newfd antigo com o no u para old_newfd (1º self)
               if (strcmp(suc_TCP, chave) != 0 && strcmp(suc_TCP,"") != 0)
               {// se tem predecessor diferente dele mesmo...
                  // if(d_32(chave,"0") > d_32(suc_TCP,"0"))// se a minha chave é menor que a chave recebida então...(1º self), se a chave recebida for maior que a minha chave tcp então...(2º self)
                     strcpy(TCP_ip_special, next_TCP_ip);
                     strcpy(TCP_port_special, next_TCP_port);
                     splitter(buffer, suc_TCP, next_TCP_ip, next_TCP_port, NULL, NULL);
                     sprintf(comando, "PRED %s %s %s\n", suc_TCP, next_TCP_ip, next_TCP_port);
                     //printf("A enviar PRED\n");
                     send_TCP(&old_newfd, comando, TCP_ip_special, TCP_port_special, hints_TCP, res_TCP);
                     if( n=read(old_newfd,buffer,128)==0 )//fica á espera que o no seguinte receba o pred e feche o seu fd_tcp_client
                     {// só depois podemos finalizar o fecho
                     // Note however that, if the connection is closed, by the peer
                     //process, in an orderly fashion, while read is blocking your
                     //process, then read would return 0, as a sign of EOF(end-of-file).
                        close(old_newfd);
                     }


               }
               else
                  {//self recebido no fim pelo 2º pentry
                     splitter(buffer, suc_TCP, next_TCP_ip, next_TCP_port, NULL, NULL);
                     if(strcmp(prd,chave) == 0)
                     {
                        strcpy(prd,suc_TCP);
                        strcpy(prd_TCP_ip,next_TCP_ip);
                        strcpy(prd_TCP_port,next_TCP_port);
                        printf("Nao tinha prd diferente de mim!\n");
                        sprintf(comando, "SELF %s %s %s\n", chave, ip, TCP_port);
                        send_TCP(&fd_TCP_client, comando, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP);
                     }
                  }

               if (strcmp(suc_TCP, "") == 0)
               {
                  splitter(buffer, suc_TCP, next_TCP_ip, next_TCP_port, NULL, NULL);
                  //strcpy(prd, suc_TCP); // o predecessor é o mesmo que o sucessor
                  if (strcmp(suc_TCP,chave) !=0 )
                  {
                     sprintf(comando, "SELF %s %s %s\n", chave, ip, TCP_port);
                     //send_TCP(&fd_TCP_client, comando, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP_client);
                  }


               }

            }



         }

         else if (FD_ISSET(fd_TCP_client, &rfds))
         {
            memset(buffer, 0, sizeof(buffer)); //limpa buffer
            if(n = read(fd_TCP_client, buffer, 128) == 0)
            {
               printf("Erro read fd_TCP\n");
               fecha(res_UDP, res_TCP, res_TCP_client, fd_UDP, fd_TCP, fd_TCP_client, newfd);
            }  //lê mensagem e poe no buffer
            printf("Anel usado para enviar informação... Mensagem recebida = %s\n",buffer);
            if (strstr(buffer, "PRED") != NULL)
            { // acaba-se de abrir o anel ao fechar a conexão com i, ou seja, fechar o
               // fd_tcp_client, depois pôe-se o fd_tcp_client = -1 para
               // quando entrar no send_tcp criar uma nova socket fd_tcp_client, associada
               // ao nó u , que vai pedir connect com o nó j, este como nao tem nenhum
               // newfd vai criar um novo
               close(fd_TCP_client);
               fd_TCP_client = -1;
               if(strcmp(suc_TCP,"") != 0)
               {
                  splitter(buffer, prd, TCP_ip_special, TCP_port_special, NULL, NULL);
                  splitter(buffer, prd, prd_TCP_ip, prd_TCP_port, NULL, NULL);

                  if(strcmp(prd,chave) != 0)
                  {
                  sprintf(comando, "SELF %s %s %s\n", chave, ip, TCP_port);
                  send_TCP(&fd_TCP_client, comando, TCP_ip_special, TCP_port_special, hints_TCP, res_TCP);
                  }
                  else
                  {
                     if( n=read(newfd,buffer,128)==0 )
                     {
                        close(newfd);
                        newfd = -1;
                     }
                     strcpy(suc_TCP,prd);
                     strcpy(next_TCP_ip, prd_TCP_ip);
                     strcpy(next_TCP_port, prd_TCP_port);
                  }
               }
            }

            else if (strstr(buffer, "FND") != NULL)
            {
               //printf("FND recebido\n");
               splitter(buffer,chave_find,ori_tag,ori,ori_ip,ori_port);

               if(d_32(chave,chave_find) > d_32(suc_TCP,chave_find) || d_32(chave,chave_find) > d_32(suc_UDP,chave_find))
               {
                  if(d_32(suc_UDP,chave_find) < d_32(suc_TCP,chave_find))
                  {
                     UDP_code = send_to_shortcut_UDP(fd_UDP,buffer,suc_UDP,next_UDP_ip,next_UDP_port,hints_UDP,res_UDP);
                     if(UDP_code == 0)
                     {
                        send_TCP(&newfd, buffer, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP);
                     }

                  }

                  else if(d_32(suc_TCP,chave_find) < d_32(suc_UDP,chave_find))
                  {
                     send_TCP(&newfd, buffer, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP);
                  }
               }

               else
               {
                  sprintf(comando,"RSP %s %s %s %s %s\n",ori,ori_tag,chave,ip,TCP_port);
                  send_TCP(&newfd, comando, next_TCP_ip, next_TCP_port, hints_TCP, res_TCP);
               }
            }

            else if (strstr(buffer, "RSP") != NULL)
            {
               splitter(buffer,ori,NULL,NULL,NULL,NULL);

               if(strcmp(ori,chave) == 0)
               {
                  if(strcmp(UDP_special,chave_find) == 0)
                  {
                     splitter(buffer,aux,ori_tag, ori, ori_ip,ori_port);
                     sprintf(comando,"EPRED %s %s %s\n", ori,ori_ip,ori_port);
                     n = sendto(fd_UDP,comando,strlen(comando),0,&addr_UDP,addrlen_UDP);
                     if(n==-1)
                     {
                        exit(1);
                     }
                     strcpy(chave_find,"");
                     strcpy(UDP_special,"");
                  }
                  else
                  {
                     splitter(buffer,aux,ori_tag, ori, ori_ip,ori_port);
                     printf("Chave %s: nó %s (%s:%s)\n", chave_find, ori, ori_ip, ori_port);
                  }

               }

               else
               {
                  send_TCP(&newfd,buffer,next_TCP_ip,next_TCP_port,hints_TCP,res_TCP);
               }
            }
         }

         break;
      }
   }

   fecha(res_UDP, res_TCP, res_TCP_client, fd_UDP, fd_TCP, fd_TCP_client, newfd);
}
