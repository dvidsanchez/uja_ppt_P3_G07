#ifndef protocolostpte_practicas_headerfile
#define protocolostpte_practicas_headerfile
#endif

// COMANDOS DE APLICACION
//#define SC "USER"  // SOLICITUD DE CONEXION USER usuario 
//#define PW "PASS"  // Password del usuario  PASS password
#define HE "HELO" 
#define MA "MAIL FROM:"
#define RE "RCPT TO:"
#define DA "DATA"
#define RS "RSET"
// RESPUESTAS A COMANDOS DE APLICACION
#define PRE "1"
#define OK  "2"
#define POS "3"
#define NEP "4"
#define NE  "5"

#define SD  "QUIT"  // Finalizacion de la conexion de aplicacion
#define SD2 "EXIT"  // Finalizacion de la conexion de aplicacion y finaliza servidor



#define S_MAILt 9
#define S_pHELO 8
//FIN DE RESPUESTA
#define CRLF "\r\n"

//ESTADOS
#define S_HELO 0
#define S_MAIL 1
#define S_RCPT 2
#define S_DATA 3
#define S_MSGC 4
#define S_MSGB 5
#define S_RSET 6
#define S_QUIT 7


//PUERTO DEL SERVICIO
#define TCP_SERVICE_PORT	25

// NOMBRE Y PASSWORD AUTORIZADOS
#define USER		"alumno"
#define PASSWORD	"123456"

//COMANDO SUMA
//#define SU "SUMA"