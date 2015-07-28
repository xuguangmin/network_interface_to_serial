#if 1
#define SERVER_NAME cgiServerName
#endif
#if 0
#define SERVER_NAME "www.boutell.com"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <cgic.h>
#include <subFunction.h>

#define		DEVICE_NUM			PWD

int cgiMain()
{
	CONFIG_T newcfg;
	int sock;
	int err;

	if((sock = MyConnect()) < 0) {
		if(cgi_sprint("FAILED", "", INTERNAL_SERVERERR, "MyConnect failed") != SUCCESS){
			exit(EFAILED);
		}
		exit(EFAILED);
	}

	/* If a submit button has already been clicked, act on the submission of the form. */
	if (cgiFormSubmitClicked("savesetup") == cgiFormSuccess) {
		/*load the configuration file*/
		if((err = LoadConf(sock, DEVICE_NUM, &newcfg)) != 0) {
			/*load the configuration file*/
			if(cgi_sprint("FAILED", "", INTERNAL_SERVERERR, "LoadConf failed") != SUCCESS){
				exit(EFAILED);
			}
			exit(EFAILED);
		}
		CloseSocket(sock);

		if((err = ReceiveSubmit(DEVICE_NUM, &newcfg)) != SUCCESS) {
			if(cgi_sprint("FAILED", "", BADREQUIST, "ReceiveSubmit failed") != SUCCESS){
				exit(EFAILED);
			}
			exit(EFAILED);
		}

		if((sock = MyConnect()) < 0) {
			if(cgi_sprint("FAILED", "", INTERNAL_SERVERERR, "MyConnect failed") != SUCCESS){
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

		CloseSocket(sock);
	} 

	return 0;
}
