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





int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[1024], buffer_out[1024],input[1024];
	int recibidos=0,enviados=0;
	int a=0;
	int estado=S_HELO;
	char option;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

    char ipdest[16];
	char default_ip[16]="127.0.0.1";
	
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

		
			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets(ipdest);

			if(strcmp(ipdest,"")==0)
				strcpy(ipdest,default_ip);


			server_in.sin_family=AF_INET;
			server_in.sin_port=htons(TCP_SERVICE_PORT);
			server_in.sin_addr.s_addr=inet_addr(ipdest);
			
			estado=S_HELO;
		
			// establece la conexion de transporte
			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0)
			{
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			
		
				//Inicio de la máquina de estados
				do{
					switch(estado)
					{
					case S_HELO:
						do{
						printf("CLIENTE> Introduzca el host al que desea conectar: ");
						gets(input);
						}while(strlen(input)==0);
						sprintf_s(buffer_out,sizeof(buffer_out), "%s %s",HE,input);
						
						// Se recibe el mensaje de bienvenida
						break;
					case S_MAIL:
						// establece la conexion de aplicacion 
						do{
						printf("CLIENTE> Identifique al remitente: ");
						gets(input);

						}while(strlen(input)==0);
						
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",MA,input,CRLF);
											
						break;
					case S_RCPT:
						printf("CLIENTE> Introduzca el destinatario, para no introducir mas destinatarios pulse (enter): ");
						gets(input);
						if(strlen(input)==0)
						{
							/**sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",DA,CRLF);
							estado=S_DATA;*/
							a=1;
							break;
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",RE,input,CRLF);
						break;
					case S_DATA:
						printf("CLIENTE> Introduzca la informacion, finalice con CRLF.CRLF: ");
						gets(input);
						//Hay ke añadir un punto al comienzo de cada linea, y ver komo se va a finalizar la introduccion de datos en el cliente.
						/**if(strlen(input)==0||strcmp(input,SD)==0) 
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);
							estado=S_QUIT;
						}
						else if (strlen(input)==1)
						{
							printf("CLIENTE> Introduzca los numeros a sumar, formato(XXXX XXXX): ");
						gets(input);
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",SU,input,CRLF);
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",input,CRLF);*/
						break;
				 
				
					}
					//Envio
					if(estado!=NULL){
					// Ejercicio: Comprobar el estado de envio

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
					//Recibo
					recibidos=recv(sockfd,buffer_in,512,0);
					
					if(recibidos<=0)
					{
						DWORD error=GetLastError();
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
					else
					{
						buffer_in[recibidos]=0x00;
						printf(buffer_in);
						if(estado==S_RCPT && strncmp(buffer_in,OK,1)==0){
							if(a==1){
							estado++;
							continue;
							}
							printf("CLIENTE> Se ha aceptado el comando y el receptor.");
						}
						if(estado==S_RCPT && (strncmp(buffer_in,NEP,1)==0 || strncmp(buffer_in,POS,1)==0 )){
						printf("CLIENTE> Ha habido algun error, intente introducir al receptor de nuevo sin errores.");
						continue;
						}
						if(strncmp(buffer_in,NE,1)==0){
							printf("CLIENTE> No se ha aceptado el comando y no se puede volver a intentar");
							estado=S_QUIT;
							continue;
						}
						if(strncmp(buffer_in,NEP,1)==0){
							printf("CLIENTE> No se ha aceptado el comando pero se puede volver a intentar");
							continue;
						}
						if((estado!=S_DATA && estado!=S_RCPT) && strncmp(buffer_in,OK,1)==0) {
							
							estado++;  
							continue;
						}
						if (estado==S_DATA && strncmp(buffer_in,POS,1)==0)
						{

						}
					}
					
				}while(estado!=S_QUIT);
				
	
		
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