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
	char buffer_in[1024], buffer_out[1024],input[1024],sb[150],fr[50],to[50],dt[80],cc[200],cco[200],aux[1];
	int recibidos=0,enviados=0;
	int a=0,b=0,c=0;
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
						if (estado>1)
						{
							printf("CLIENTE> Por favor introduzca: ");
							printf("CLIENTE> 1. Para añadir un destinatario. ");
							printf("CLIENTE> 2. Para enviar un mail a el/los destinatario/s.");
							printf("CLIENTE> 3. Para reiniciar la sesion.");
							printf("CLIENTE> 4. Para finalizar la sesion.");
							gets(input);
							if(atoi(input)==1){
							estado=2;
							}
							else if (atoi(input)==2)
							{
								estado=3;
							}else if (atoi(input)==3)
							{
								estado=5;
							}else if (atoi(input)==4)
							{
								estado=6;
							}
						}
					case S_HELO:
						do{
						printf("CLIENTE> Introduzca el usuario: ");
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
						strcat(fr,input);					
						break;
					case S_RCPT:
						
						do
						{
							printf("CLIENTE> Introduzca el destinatario: ");
						gets(input);
						} while (strlen(input)==0);
						
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",RE,input,CRLF);
						c++;
						if (c>1)
						{
							printf("CLIENTE> Indique si se trata de copia oculta [s/n]: ");
							do
							{
								gets(aux);
							} while (aux!="s"&&aux!="n");
							if (aux=="s")
							{
								strcat(cco,input);
							}else
							{
								strcat(cc,input);
							}

						}
						break;
					
					case S_DATA:
						if (b==0)
						{
							printf("CLIENTE> Por favor introduzca primero un destinatario.");
							estado=S_RCPT;
							break;
						}
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s",DA,CRLF);
						break;

					case S_MSGE:
						printf("CLIENTE> Indique el asunto: ");
						gets(sb);
				 
				
					
					case S_RSET:
						printf("CLIENTE> Se reiniciara la sesion.");
						estado=S_MAIL;
						b=0;
						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s",RS,CRLF);
						break;

					//Envio
					if(estado==S_RCPT&&b==0){
					continue;
					}
					
					if(estado!=NULL){
					

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
						if(estado<S_RCPT && strncmp(buffer_in,OK,1)==0){
							estado++;
							continue;
							}
						if (estado==S_RCPT&& strncmp(buffer_in,OK,1)!=0)
						{
							b++;
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
						if (estado==S_MSGE && strncmp(buffer_in,OK,1)!=0){
						printf("CLIENTE> Ha habido algun problema con el envio del mensaje, por favor vuelva a intentarlo");
						estado=S_RSET;
						   continue;
						}
						/**if((estado!=S_DATA && estado!=S_RCPT) && strncmp(buffer_in,OK,1)==0) {
							
							estado++;  
							continue;
						}*/
						/**
						if(estado==S_RCPT && (strncmp(buffer_in,NEP,1)==0 || strncmp(buffer_in,POS,1)==0 )){
						printf("CLIENTE> Ha habido algun error, intente introducir al receptor de nuevo sin errores.");
						continue;
						}
						if(strncmp(buffer_in,NE,1)==0){
							printf("CLIENTE> No se ha aceptado el comando y no se puede volver a intentar");
							
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
							estado++;

						}*/
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