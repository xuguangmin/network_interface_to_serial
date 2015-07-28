#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <cgic.h>
#include <subFunction.h>

#define DEBUG	0
//device number
#define		DEVICE_NUM			LOGIN

//用户名
#define SUPERNAME					"admin"

int cgiMain() {
	CONFIG_T cfg, rcfg;
	int sock;
	int err;
	int checks;

#if DEBUG
	afreshOrient("cgi.txt", "a", stdout); //test
#endif /* DEBUG */

	if((sock = MyConnect()) < 0) {
		if(cgi_sprint("FAILED", "", INTERNAL_SERVERERR, "MyConnect failed") != SUCCESS){
			exit(EFAILED);
		}
		exit(EFAILED);
	}
	/*load the configuration file*/
	if((err = LoadConf(sock, DEVICE_NUM, &cfg)) != 0) {
		/*load the configuration file*/
		if(cgi_sprint("FAILED", "", INTERNAL_SERVERERR, "LoadConf failed") != SUCCESS){
			exit(EFAILED);
		}
		exit(EFAILED);
	}
	/* If a submit button has already been clicked, act on the submission of the form. */
	if (cgiFormSubmitClicked("login_loginpage") == cgiFormSuccess) {
		strcpy(cfg.user.uname, SUPERNAME);

		if((err = ReceiveSubmit(DEVICE_NUM, &rcfg)) != SUCCESS) {
			if(cgi_sprint("FAILED", "", BADREQUIST, "ReceiveSubmit failed") != SUCCESS){
				exit(EFAILED);
			}
			exit(EFAILED);
		}

		if((checks = CheckLogin(rcfg.user, cfg.user)) != SUCCESS) {
			if(cgi_sprint("FAILED", "", UNAUTHENTICATION, "authentication failed") != SUCCESS){
				exit(EFAILED);
			}
			exit(EFAILED);
		} else {
			//AUTHENTICATION
			if(setenv(CGI_AUTH_IP, CGICLIENTIP, 1) != 0) {
				if(cgi_sprint("FAILED", "", 100, "authentication save failed") != SUCCESS){
					exit(EFAILED);
				}
				exit(EFAILED);
			}
			if(cgi_sprint("SUCCESS", "../main.html", SUSSHAND, "request success") != SUCCESS){
				exit(EFAILED);
			}
		}

		//SendSubmit(DEVICE_NUM, rcfg);

	}

	CloseSocket(sock);
	return SUCCESS;
}

