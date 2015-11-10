/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: cliente.c
Versión: 1.0
Fecha: 23/09/2012
Descripción:
	Cliente de eco sencillo TCP.

Autor: Juan Carlos Cuevas Martínez
Modificado por: Antonio Perez Pozuelo y David Sanchez Fernandez
*******************************************************/
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>


#include "protocolSMTP.h"

int getTimeZone()
{
   TIME_ZONE_INFORMATION tziOld;
	DWORD dwRet;

	dwRet = GetTimeZoneInformation(&tziOld);

	 if(dwRet == TIME_ZONE_ID_STANDARD || dwRet == TIME_ZONE_ID_UNKNOWN)    
		 return tziOld.StandardBias/60;
   else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
      return tziOld.DaylightBias/60;
   else
   {
      printf("GTZI failed (%d)\n", GetLastError());
      return 0;
   }

  

}
/**char[] getaddr(){


}*/


int main(int *argc, char *argv[])
{
	time_t t=time(0);
	struct tm *tlocal=localtime(&t);
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[1024], buffer_out[1024],input[1024],sb[150],fr[50],to[50],dt[150],cc[200],cco[200],aux[30];
	int recibidos=0,enviados=0;
	int a=0,b=0,c=0,d=0,i;
	int estado=S_HELO;
	char option;
	char hostname[128];
	struct hostent      *returned_host;//Para obtener la IP
	struct in_addr **addr_list;


	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

    char ipdest[16];
	char default_ip[16]="127.0.0.1";
	cc[0]=0x00;
	cco[0]=cc[0];

	//Inicialización Windows sockets
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();
		return(0);
	}
	//Fin: Inicialización Windows sockets

	do{
		sockfd=socket(AF_INET,SOCK_STREAM,0);

		if(sockfd==INVALID_SOCKET)
		{
			printf("CLIENTE> ERROR AL CREAR SOCKET\r\n");
			exit(-1);
		}
		else
		{
			printf("CLIENTE> SOCKET CREADO CORRECTAMENTE\r\n");

			do
			{

			
			printf("CLIENTE>introduzca un nombre de host o una IP\r\n");
			gets(input);
			if (inet_addr(ipdest)==INADDR_NONE)
				{
					returned_host=gethostbyname(input);
				if (returned_host==NULL)
				{
					d=0;

				}else
				{
					strcpy(ipdest, inet_ntoa(*(struct in_addr*)returned_host->h_addr));
					d=1;
					}
				
					
				}
			

			else
			{
				
				if(strcmp(ipdest,"")==0)
				strcpy(ipdest,default_ip);
				d=1;
				
				
			}
			

			

			} while (d!=1);
			server_in.sin_family=AF_INET;
			server_in.sin_port=htons(TCP_SERVICE_PORT);
			server_in.sin_addr.s_addr=inet_addr(ipdest);
			
			


			estado=S_pHELO;
		
			// establece la conexion de transporte
			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0)
			{

				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			
		
				//Inicio de la máquina de estados
				do{
					

					
					if (estado==S_RCPT)//Menu una vez identificado el usuario
						{
							printf("CLIENTE> Por favor introduzca: \r\n");
							printf("CLIENTE> 1. Para anadir un destinatario.\r\n ");
							printf("CLIENTE> 2. Para enviar un mail a el/los destinatario/s.\r\n");
							printf("CLIENTE> 3. Para reiniciar la sesion.\r\n");
							printf("CLIENTE> 4. Para finalizar la sesion.\r\n");
							gets(input);
							if(atoi(input)==1){
							estado=2;
							}
							else if (atoi(input)==2)
							{
								estado=3;
							}else if (atoi(input)==3)
							{
								estado=6;
							}else if (atoi(input)==4)
							{
								estado=7;
								continue;
							}
						}
					
					switch(estado)
					{
						
					case S_HELO:
						{
						do{
						printf("CLIENTE> Pulse enter para continuar: \r\n");
						gets(input);
						}while(strlen(input)!=0);
						gethostname(hostname, sizeof hostname);//Enviamos el nombre de la maquina en la que se ejecuta el cliente.
						sprintf_s(buffer_out,sizeof(buffer_out),"%s %s%s",HE,hostname,CRLF);
						//Se establece la conexion de aplicacion
						break;
						}
					case S_MAIL:
						{
						 //Identificamos al usuario
						do{
						printf("CLIENTE> Identifique al remitente: \r\n");
						gets(input);
						}while(strlen(input)==0);
						
						
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",MA,input,CRLF);
						strcpy(fr,input);		//Se copia el remitente al campo from.			
						break;
						}
					case S_RCPT:	//Se añaden destinatarios, si hay mas de uno se preguntara si es CC o BCC. 
						
						{
						do{
							printf("CLIENTE> Introduzca el destinatario: \r\n");
						gets(input);
						}while(strlen(input)==0);
						
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",RE,input,CRLF);
						
						if (c>0)
						{
							printf("CLIENTE> Indique si se trata de copia oculta [s/n]: \r\n");
							do
							{
								gets(aux);
							} while (!strcmp(aux,"s")==0&&!strcmp(aux,"n")==0);
							if (strcmp(aux,"s"))
							{
								strcpy(aux,input);
								strcat(cco,aux);
								
							}else
							{
								strcpy(aux,input);
								strcat(cc,aux);
							}

						}
						else
						{
							strcpy(to,input);
						}
						break;
					}
					case S_DATA://Se envia el comando DATA para pasar al estado de envio del mensaje.
						{
						if (c<=0)
						{
							printf("CLIENTE> Por favor introduzca primero un destinatario.\r\n");
							estado=S_RCPT;
							c-=1;
							break;
						}
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s",DA,CRLF);
						
						break;
						}
					case S_MSGC:
						{//Se envian las cabeceras del mensaje
						printf("CLIENTE> Indique el asunto: \r\n");
						gets(sb);
						strftime(dt,sizeof(dt),"%a, %d %b %y %H:%M:%S",tlocal);
						if (getTimeZone()>0)
						{
							strcat(dt,"+");
						}
						else
						{
							strcat(dt,"-");
						}
						sprintf_s(dt,sizeof(dt),"%s%d",dt,getTimeZone()); 
						
						sprintf_s(buffer_out,sizeof(buffer_out),"From: %s\r\nTo: %s\r\nSubject: %s\r\nCC: %s\r\nBCC: %s\r\nDate: %s\r\n%s",fr,to,sb,cc,cco,dt,CRLF);
						
						break;
						}
					case S_MSGB:
						{//Se envia el cuerpo del mensaje, se estan enviando lineas hasta que se introduce un punto.
						printf("CLIENTE> Escriba el cuerpo del mensaje, para finalizar ingrese un punto unicamente: \r\n");
						gets(input);
						if (strcmp(input,".")==0)
						{
							input[0]=0x2E;
							estado=S_MAILt;

							printf("%d",estado);						
						}
						sprintf_s(buffer_out,sizeof(buffer_out),"%s%s",input,CRLF);
						
						break;
						}
					case S_RSET:
						{//Se vacian todas las variables trascendentales y se pasa al estado MAIL FROM
						printf("CLIENTE> Se reiniciara la sesion.");
						strcpy(to,"");
						strcpy(cc,"");
						strcpy(cco,"");
						b=0;
						c=0;
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s",RS,CRLF);
						
						break;
						}
					}//Envio
					
					//Se envia siempre que no sea el estado "pre HELO"
					
					if(estado!=S_QUIT+1)
					{
					
						printf("a");
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);
						if (enviados==SOCKET_ERROR)
						{
							printf("CLIENTE> Error enviando datos\r\n");
							estado=S_QUIT;
						}else if (enviados==0)
						{
							printf("CLIENTE> Se ha liberado la conexion de forma acordada");
							estado=S_QUIT;
						}
						else
						{
						printf("CLIENTE> Se han enviado los datos: %s",buffer_out);
						}
					}
					printf("1f");
					if (estado==S_MSGB)//Si es el cuerpo del mensaje solo se envia hasta que se introduzca un punto.
					{
						
						continue;
					}
					
					if (estado==S_MSGC)
					{
						estado++;
						continue;
					}
					printf("1");
				//Recibo
					recibidos=recv(sockfd,buffer_in,sizeof(buffer_in),0);
					printf("recibe");
					if(recibidos<=0)
					{
						
						DWORD error=GetLastError();
						printf("fail");
						if(recibidos<0)
						{
							printf("CLIENTE> Error %d en la recepción de datos\r\n",error);
							estado=S_QUIT;
							
						}
						else
						{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;
							
						}
					}
					else//Comprobacion de las respuestas recibidas por el servidor en funcion de los distintos estados.
					{
						buffer_in[recibidos]=0x00;
						printf(buffer_in);
						if (estado==S_pHELO&&strncmp(buffer_in,OK,1)==0)
						{
							estado=S_HELO;
							continue;
						}
						if (estado==S_MAILt&&strncmp(buffer_in,OK,1)==0)
					{
						estado=S_MAIL;
						continue;
					}
					
						if(estado==S_RSET && strncmp(buffer_in,OK,1)==0){
							estado=S_MAIL;
							continue;
						}
						if(estado<S_RCPT && strncmp(buffer_in,OK,1)==0){
							estado++;
							continue;
							}
						if (estado==S_RCPT&& strncmp(buffer_in,OK,1)==0)
						{
							c++;
							continue;
						}	
						if (estado==S_RCPT&& strncmp(buffer_in,OK,1)!=0)
						{
							printf("CLIENTE> Ha habido algun problema o no se reconoce el destinatario. Si persiste reinicie la sesion.\r\n");
							continue;
						}
						if (estado==S_DATA && strncmp(buffer_in,POS,1)==0)
						{
							estado++;
							continue;
						}
						if (estado==S_MAILt && strncmp(buffer_in,OK,1)==0){
						printf("CLIENTE> Se ha enviado el mensaje");
						estado=S_RSET;
						   continue;
						}
						
						else{
							estado=S_RSET;
							printf("CLIENTE>Ha habido algun problema, se reiniciara la sesion.");
						}
							
					}
					
				}while(estado!=S_QUIT);
						strcpy(aux,"");
						strcpy(dt,"");
						strcpy(to,"");
						strcpy(fr,"");
						strcpy(cc,"");
						strcpy(cco,"");
						b=0;
						c=0;
	
		
			}
			else
			{
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			}		
			// fin de la conexion de transporte
			closesocket(sockfd);
			
		}
		do
		{
			printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();
		} while (option!='n'&&option!='N'&&option!='s'&&option!='S');
		

	}while(option!='n' && option!='N');

	
	
	return(0);

}
/**

incluir windows.h
TIME_ZONE_INFORMATION TzIold;
DWORD duRet;
duRet=GetTimeZoneInformation



*/