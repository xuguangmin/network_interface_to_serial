/* Change this if the SERVER_NAME environment variable does not report
	the true name of your web server. */
#if 1
#define SERVER_NAME cgiServerName
#endif
#if 0
#define SERVER_NAME "www.boutell.com"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgic.h>
#include <subFunction.h>

#define		DEVICE_NUM			SERIAL_THREE

int cgiMain() 
{
	CONFIG_T newcfg;
	int sock;
	int err;

#ifdef DEBUG
	afreshOrient("cgi.txt", "a", stdout);
#endif /* DEBUG */

	//AUTHENTICATION

	if((sock = MyConnect()) < 0){ 
		if(cgi_sprint("FAILED", "NULL", INTERNAL_SERVERERR, "Service program failed\r\n") != SUCCESS) {
			exit(EFAILED);
		}
		exit(EFAILED);
	}
	/* If a submit button has already been clicked, act on the submission of the form. */
	if(cgiFormSubmitClicked("savesetup") == cgiFormSuccess) {
		if((err = ReceiveSubmit(DEVICE_NUM, &newcfg)) != SUCCESS) {
			if(cgi_sprint("FAILED", "NULL", NOTMODIFY, "ReceiveSubmit error\r\n") != SUCCESS) {
				exit(EFAILED);
			}
			exit(EFAILED);
		}
		if((err = SaveConf(sock, DEVICE_NUM, newcfg)) != SUCCESS) {
			if(cgi_sprint("FAILED", "NULL", NOTMODIFY, "SaveConf error\r\n") != SUCCESS) {
				exit(EFAILED);
			}
			exit(EFAILED);
		}
	} else if((err = LoadConf(sock, DEVICE_NUM, &newcfg)) != 0) {
		/*load the configuration file*/
		if(cgi_sprint("FAILED", "NULL", INTERNAL_SERVERERR, "LoadConf failed\r\n") != SUCCESS) {
			exit(EFAILED);
		}
		
		exit(EFAILED);
	} 
	
	/* Now send the data for form */
	SendSubmit(DEVICE_NUM, newcfg);

	CloseSocket(sock);
	exit(SUCCESS);
}

