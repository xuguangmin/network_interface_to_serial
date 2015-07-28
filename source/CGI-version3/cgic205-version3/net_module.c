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
#include <subFunction.h>
#include <cgic.h>

#define		DEVICE_NUM			NET


int cgiMain() {
	CONFIG_T newcfg;
	int sock;
#ifdef DEBUG
	afreshOrient("cgi.txt", "a", stdout); //test
#endif /* DEBUG */

	if((sock = MyConnect()) < 0) {
		if(cgi_sprint("FAILED", "", INTERNAL_SERVERERR, "MyConnect failed") != SUCCESS){
			exit(EFAILED);
		}
		exit(-1);
	}
	/* If a submit button has already been clicked, act on the submission of the form. */
	if (cgiFormSubmitClicked("savesetup") == cgiFormSuccess)
	{
		/*load the configuration file*/
		ReceiveSubmit(DEVICE_NUM, &newcfg);
		SaveConf(sock, DEVICE_NUM, newcfg);
	}  else if(LoadConf(sock, DEVICE_NUM, &newcfg) != 0) {
		if(cgi_sprint("FAILED", "", INTERNAL_SERVERERR, "LoadConf failed") != SUCCESS){
			exit(EFAILED);
		}
		exit(-1);
	} 

	/* Now send the data for form */
	SendSubmit(DEVICE_NUM, newcfg);

	CloseSocket(sock);
	exit(0);
}
