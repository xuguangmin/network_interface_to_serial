#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <errno.h>
#include "cgic.h"
#include "subFunction.h"

static int SetDeviceHeader(head_t *headp, int deviceNum, int len, int flag)
{
	if(headp == NULL) {
		fprintf(cgiOut, "device_rt()-> rtp: NULL<br>\r\n");
		return EPARAMETER;
	} else if(deviceNum < 0 || deviceNum > DEVICE_TOTAL) {
		fprintf(cgiOut, "device_rt()-> device: %d not register<br>\r\n", deviceNum);
		return EPARAMETER;
	}

	headp->flag	  = flag; // receive or send
	headp->device = deviceNum;
	headp->length = len;

	return SUCCESS;
}//SetDeviceHeader
static char* deviceName(int x)
{
	if(x < 0 || x > DEVICE_TOTAL) {
		fprintf(cgiOut, "deviceName() -> x: %d<br>\r\n", x);
		return NULL;
	}
	switch(x)
	{
		case SERIAL_ONE:return DEVICE_NAME_1;
		case SERIAL_TWO:return DEVICE_NAME_2;
		case SERIAL_THREE:return DEVICE_NAME_3;
		case SERIAL_FOUR:return DEVICE_NAME_4;
		default:return NULL;
	}

	return (char*)1;
}//deviceName

/*----------------------    serial setup   ------------------*/
static char *parity[] = {
	"N",
	"E",
	"O",
};
static int SerialParity(char *serial, char *result)
{
	int i;

	if(serial == NULL || parity == NULL)
		return EPARAMETER;
	if(cgiFormSelectSingle(serial, parity, 3, &i, 0) != cgiFormSuccess)
		return ERETVAL;
	else
		*result = *parity[i];

	return cgiFormSuccess;
}
static int SerialSpeed(char *serial, int *speed)
{
	if(serial == NULL || speed == NULL)
		return EPARAMETER;
	return cgiFormInteger(serial, speed, 0);
}
static int SerialData(char *serial, int *data)
{
	if(serial == NULL || data == NULL)
		return EPARAMETER;
	return cgiFormInteger(serial, data, 0);
}
static int SerialStop(char *serial, int *stop)
{
	if(serial == NULL || stop == NULL)
		return EPARAMETER;
	return cgiFormInteger(serial, stop, 0);
}
static int GetDataSerial(CONFIG_T *nCfg)
{
	int err;

	if(nCfg == NULL) {
		fprintf(cgiOut, "GetDateSerial(): parameter is NULL<br>\r\n");
		return EPARAMETER;
	}

	if((err = SerialSpeed("baudRate",  &(nCfg->serial.speed))) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDateSerial()->SerialSpeed(): %d return value error<br>\r\n", err);
		return ERETVAL;
	}
	if((err = SerialData("dataBit",     &(nCfg->serial.data))) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDateSerial()->SerialData(): %d return value error<br>\r\n", err);
		return ERETVAL;
	}
	if((err = SerialStop("stopBit",     &(nCfg->serial.stop))) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDateSerial()->SerialStop(): %d return value error<br>\r\n", err);
		return ERETVAL;
	}
	if((err =SerialParity("parityBit",&(nCfg->serial.parity))) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDateSerial()->SerialParity(): %d return value error<br>\r\n", err);
		return ERETVAL;
	}

	return SUCCESS;
}//END serial setup

/*---------------------  network setup   --------------------*/
static int GetIpstr(char *name, char *result, int len)
{
	if(name == NULL || result == NULL || len <= 0)
		return EPARAMETER;
	return cgiFormStringNoNewlines(name, result, len);
}
static int PortNum(char *name, int *portNum)
{
	if(name == NULL || portNum <= 0)
		return EPARAMETER;
	return cgiFormInteger(name, portNum, 0);
}
static int NetMask(char *name, char *result, int len)
{
	if(name == NULL || result == NULL || len <= 0)
		return EPARAMETER;
	return cgiFormStringNoNewlines(name, result, len);
}
static int GateWay(char *name, char *result, int len)
{
	if(name == NULL || result == NULL || len <= 0)
		return EPARAMETER;

	return cgiFormString(name, result, len);
}
static int GetDataNet(CONFIG_T *nCfg)
{
	int err;

	if(nCfg == NULL) {
		fprintf(cgiOut, "GetDateNet(): parameter is NULL<br>\r\n");
		return EPARAMETER;
	}

	if((err = GateWay("gateWay", nCfg->netdata.gateway, IP_LEN)) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDateNet()->GateWay(): %d return value error<br>\r\n", err);
		return ERETVAL;
	}
	if((err = NetMask("subnetMask", nCfg->netdata.netmask, IP_LEN)) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDateNet()->NetMask(): %d return value error<br>\r\n", err);
		return ERETVAL;
	}
	if((err = PortNum("serPort", &nCfg->netdata.port)) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDataNet()->PortNum(): %d return value error<br>\r\n", err);
		return ERETVAL;
	}
	if((err = GetIpstr("ip", nCfg->netdata.ipstr, IP_LEN)) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDataNet()->GetIpstr(): %d return value error<br>\r\n", err);
		return ERETVAL;
	}

	return SUCCESS;
}//END network

/*-----------------------   GET PASSWORD    ----------------------*/

int GetPasswordBrow(char *name, char *result, int len)
{
	if(name == NULL || result == NULL || len == 0)
		return EPARAMETER;
	return cgiFormStringNoNewlines(name, result, len);
}

static int GetDataPasswd(char *pwd)
{
	int err;
	char oldRec[PWDLEN];
	char new1[PWDLEN], new2[PWDLEN]; 

	if(pwd == NULL) {
		fprintf(cgiOut, "GetDataPasswd()->pwd: NULL<br>\r\n");
		return EPARAMETER;
	}

	if((err = GetPasswordBrow("oldPasswd", oldRec, PWDLEN)) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDataPasswd()->GetPasswordBrow() 1st: %d return error value<br>\r\n", err);
		return ERETVAL;
	}
	if((err = GetPasswordBrow("newPasswd", new1, PWDLEN)) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDataPasswd()->GetPasswordBrow() 2nd: %d return error value<br>\r\n", err);
		return ERETVAL;
	}
	if((err = GetPasswordBrow("confirmNewPasswd", new2, PWDLEN)) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDataPasswd()->GetPasswordBrow() 2thr: %d return error value<br>\r\n", err);
		return ERETVAL;
	}

	if((err = strcmp(new1, new2)) != 0) {
		fprintf(cgiOut, "GetDataPasswd(): new passworddisaccord<br>\r\n");
		return ERETVAL;
	}
	if((err = strcmp(oldRec, pwd)) != 0) {
		fprintf(cgiOut, "GetDataPasswd(): authentication fail<br>\r\n");
		return ERETVAL;
	}
	if(strcpy(pwd, new1) == NULL) {
		fprintf(cgiOut, "GetDataPasswd(): new passworddisaccord<br>\r\n");
		return ERETVAL;
	}

	return SUCCESS;
}//END GET PASSWORD

/*------------------   UPLOAD FILE  ----------------------*/
int my_reboot()
{
	if(reboot(LINUX_REBOOT_CMD_RESTART) != 0) {
		fprintf(stdout, "my_reboot()->reboot():%s\r\n", strerror(errno));	
		return ERETVAL;
	}

	return SUCCESS;
}//UPLOAD FILE

/*------------------   LOGIN PAGE  ----------------------*/
int GetSuperName(char *name, char *result, int len)
{
	if(name == NULL || result == NULL || len <= 0)
		return EPARAMETER;
	return cgiFormStringNoNewlines(name, result, len);
}
static int GetDataLogin(void *user)
{
	user_t *puser = (user_t *)user;

	if(user == NULL) {
		fprintf(stdout, "GetDataLogin():user is NULL\r\n");	
		return EPARAMETER;
	}

	//比对用户名
	if(GetSuperName("userName_loginpage", puser->uname, UNAME-1) != SUCCESS) {
		fprintf(cgiOut, "GetDataLogin()->GetSuperName() failed\r\n");	
		return EFAILED;
	} else if(GetPasswordBrow("passWord_loginpage", puser->key, PWDLEN-1) != cgiFormSuccess) {
		fprintf(cgiOut, "GetDataLogin()->GetPasswordBrow(): return error value<br>\r\n");
		return ERETVAL;
	} 

	return SUCCESS;
}
int CheckLogin(user_t ruser, user_t ouser)
{
	if(strcmp(ruser.uname, ouser.uname) || strcmp(ruser.key, ouser.key)) {
		return EFAILED;
	}

	return SUCCESS;
}


/*-----------------------------------------------------------------   
 *
 *					PUBLIC FUNCTION  
 *
-----------------------------------------------------------------*/

/*-----------------    SOCKET   --------------------*/
int CloseSocket(int sock)
{
	close(sock);
	return 0;
}
static int CreateSocket()
{
	int sockfd;
	if ((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
		fprintf(cgiOut, "CreatreSocket()->socket(): %d %s<br>\r\n", sockfd, strerror(errno));
		return sockfd;
	}

	return sockfd;
}
int MyConnect()
{
	struct sockaddr_un address;
	size_t addrLength;
	int sock;
	int err;

	if((sock = CreateSocket()) < 0) {
		fprintf(cgiOut, "MyConnect()->CreateSocket(): %d %s<br>\r\n", sock, strerror(errno));
		return ERETVAL;
	}

	address.sun_family = AF_UNIX;    /* Unix domain socket */
	strcpy(address.sun_path, DOMAIN_SOCKET);

	/* The total length of the address includes the sun_family element */
	addrLength = sizeof(address.sun_family) + strlen(address.sun_path);

	if ((err = connect(sock, (struct sockaddr *) &address, addrLength)) != 0) {
		fprintf(cgiOut, "MyConnect()->connect(): %d %s<br>\r\n", err, strerror(errno));
		return ERETVAL;
	}

	return sock;
}//END SOCKET

/*----------------------   SAVE  LOAD  RECEIVE    ------------------------*/
int LoadConf(int sock, int deviceNum, void *saveCfg)
{
	int ret = 0;
	commu_t recvData;
	CONFIG_T *psaveCfg = (CONFIG_T *)saveCfg;

	if(saveCfg == NULL) {
		fprintf(cgiOut, "LoadConf() -> newCfg: is NULL<br>\r\n");
		return EPARAMETER;
	} else if(deviceNum <= 0 || deviceNum > DEVICE_TOTAL) {
		fprintf(cgiOut, "LoadConf() -> deviceNum: %d not register<br>\r\n", deviceNum);
		return EPARAMETER;
	} else if(sock < 0) {
		fprintf(cgiOut, "LoadConf() -> sock: %d<br>\r\n", sock);
		return EPARAMETER;
	}

	memcpy(&recvData, "\0", sizeof(commu_t));
	memcpy(saveCfg, "\0", sizeof(CONFIG_T));
	SetDeviceHeader(&recvData.head, deviceNum, 0, RECEIVE);
	if((ret = send(sock, &recvData, sizeof(commu_t), 0)) <= 0) {
		fprintf(cgiOut, "LoadConf()->send(): %d %s<br>\r\n", ret, strerror(errno));
		return ERETVAL;
	}

	if((ret = recv(sock, &recvData, sizeof(commu_t), 0)) < 0) {
		fprintf(cgiOut, "LoadConf() -> recv(): %d %s<br>\r\n", ret, strerror(errno));
		return ERETVAL;
	}

	memcpy(saveCfg, "\0", sizeof(CONFIG_T));
	switch(deviceNum)
	{
		case SERIAL_ONE: 
		case SERIAL_TWO: 
		case SERIAL_THREE: 
		case SERIAL_FOUR: 
			memcpy(&(psaveCfg->serial), &recvData.body, recvData.head.length);
			break;
		case NET: 
			memcpy(&(psaveCfg->netdata), &recvData.body, recvData.head.length);
			break;
		case PWD: 
			memcpy(psaveCfg->user.key, &recvData.body, recvData.head.length);
			psaveCfg->user.key[recvData.head.length] = '\0';
			break;
		case LOGIN: 
			memcpy(psaveCfg->user.key, &recvData.body, recvData.head.length);
			psaveCfg->user.key[recvData.head.length] = '\0';
			break;
		default:
			fprintf(cgiOut, "SaveConf()->deviceNum: %d not support<br>\r\n", deviceNum);
			return EPARAMETER;
	}

	return SUCCESS;
}//LOAD

int ReceiveSubmit(int deviceNum, void *newCfg)
{
	int err;
	CONFIG_T *pnewCfg = (CONFIG_T *)newCfg;

	if(newCfg == NULL) {
		fprintf(cgiOut, "ReceiveSubmit() -> newCfg: NULL<br>\r\n");
		return EPARAMETER;
	} else if(deviceNum < 0 || deviceNum > DEVICE_TOTAL) {
		fprintf(cgiOut, "ReceiveSubmit() -> deviceNum: %d<br>\r\n", deviceNum);
		return EPARAMETER;
	}

	switch(deviceNum)
	{
		/* get data of serial from brower*/
		case SERIAL_ONE: 
		case SERIAL_TWO: 
		case SERIAL_THREE: 
		case SERIAL_FOUR: 
			if((err = GetDataSerial(pnewCfg)) != SUCCESS) {
				fprintf(cgiOut, "ReceiveSubmit()->GetDataSerial(): %d<br>\r\n", err);
				return ERETVAL;
			}
			break;
		/* get data of netsetup from brower*/
		case NET: 
			if((err = GetDataNet(pnewCfg)) != SUCCESS) {
				fprintf(cgiOut, "ReceiveSubmit()->GetDataNet(): %d<br>\r\n", err);
				return ERETVAL;
			}
			break;
			/* get data of passwd from brower*/
		case PWD: 
			if((err = GetDataPasswd(pnewCfg->user.key)) != SUCCESS) {
				fprintf(cgiOut, "ReceiveSubmit()->GetDataPasswd(): %d<br>\r\n", err);
				return ERETVAL;
			}
			break;
		case LOGIN: 
			if(GetDataLogin(&(pnewCfg->user)) != SUCCESS){
				fprintf(cgiOut, "ReceiveSubmit()->GetDataLogin()\r\n");
				return ERETVAL;
			}
			break;
		default:
			fprintf(cgiOut, "ReceiveSubmit()->deviceNum: %d not support<br>\r\n", deviceNum);
			return EPARAMETER;
	}

	return SUCCESS;
}//RECEIVE

int SendSubmit(int deviceNum, CONFIG_T newCfg)
{
	if(deviceNum < 0 || deviceNum > DEVICE_TOTAL) {
		fprintf(cgiOut, "SendSubmit() -> deviceNum: %d<br>\r\n", deviceNum);
		return EPARAMETER;
	}

	/* Send the content type, letting the browser know this is HTML */
	switch(deviceNum)
	{
		case SERIAL_ONE: 
		case SERIAL_TWO: 
		case SERIAL_THREE: 
		case SERIAL_FOUR: 
			cgiHeaderContentType("text/html");
			fprintf(cgiOut, "{serialName:'%s',baudRate:'%d',dataBit:'%d',stopBit:'%d',parityBit:'%c'}", 
				deviceName(deviceNum),
				newCfg.serial.speed, 
				newCfg.serial.data, 
				newCfg.serial.stop,
				newCfg.serial.parity
			);
			break;
		case NET: 
			cgiHeaderContentType("text/html");
			fprintf(cgiOut, "{ip:'%s', subnetMask:'%s', serPort:'%d', gateWay:'%s'}",
				newCfg.netdata.ipstr,
				newCfg.netdata.netmask,
				newCfg.netdata.port,
				newCfg.netdata.gateway
			);
			break;
		case PWD: 
			break;
		case LOGIN: 
			cgiHeaderContentType("text/html");
			//cgiHeaderLocation("../main.html");
			fprintf(cgiOut, "HelloWorld!!!");
			break;
		default:
			fprintf(cgiOut, "SendSubmit()->deviceNum: %d not support<br>\r\n", deviceNum);
			return EPARAMETER;
	}

	return SUCCESS;
}//Send

//Save the data of from brower to the config file of ntos_server program
int SaveConf(int sock, int deviceNum, CONFIG_T newCfg)
{
	int ret, len = 0;
	commu_t rdata;

	if(deviceNum <= 0 || deviceNum > DEVICE_TOTAL) {
		fprintf(cgiOut, "SaveConf() -> deviceNum: %d not support<br>\r\n", deviceNum);
		return EPARAMETER;
	} else if(sock < 0) {
		fprintf(cgiOut, "SaveConf() -> sock: %d error<br>\r\n", sock);
		return EPARAMETER;
	}

	memcpy(&rdata, "\0", sizeof(commu_t));
	switch(deviceNum)
	{
		case SERIAL_ONE: 
		case SERIAL_TWO: 
		case SERIAL_THREE: 
		case SERIAL_FOUR: 
			memcpy(&rdata.body, &newCfg.serial, (len = sizeof(serial_t)));
			break;
		case NET: 
			memcpy(&rdata.body, &newCfg.netdata, (len = sizeof(net_t)));
			break;
		case PWD: 
			memcpy(&rdata.body, newCfg.user.key, (len = strlen(newCfg.user.key)));
			break;
		case LOGIN: 
			return SUCCESS;
		default:
			fprintf(cgiOut, "SaveConf()->deviceNum: %d not support<br>\r\n", deviceNum);
			return EPARAMETER;
	}
	SetDeviceHeader(&rdata.head, deviceNum, len, SEND);

	if((ret = send(sock, &rdata, sizeof(commu_t), 0)) < 0) {
		fprintf(cgiOut, "SaveConf()->send(): %d %s<br>\r\n", ret, strerror(errno));
		return ERETVAL;
	}

	return SUCCESS;
}//Save 
/* ---------------------------   END SAVE LOAD RECEIVE  ---------------------------*/
int cgi_sprint(const char *result, const char *url, int statuscode, const char *describ)
{
	if(result == NULL || url == NULL) {
		return EFAILED;
	}
	cgiHeaderContentType("text/html");
	fprintf(cgiOut, "{result:'%s',url:'%s',statuscode:'%d',describe:'%s'}",
		result,
		url,
		statuscode,
		describ
	);

	return SUCCESS;
}


/* ---------------------------    FOR DEBUG    ---------------------------------*/
//reset standard output
FILE* afreshOrient(const char *fname, const char *mode, FILE *stream)
{
	FILE *nfp;

	nfp = freopen(fname, mode, stream);
	if(nfp == NULL) {
		perror("freopen(): NULL\r\n");	
		return NULL;
	}

	return  nfp;
}//afreshOrient

int print_cfg(int deviceNum, CONFIG_T cfg)
{	
	if(deviceNum < 0 || deviceNum > DEVICE_TOTAL) {
		fprintf(cgiOut, "print_cfg() -> deviceNum: %d<br>\r\n", deviceNum);
		return EPARAMETER;
	}

	switch(deviceNum)
	{
		case SERIAL_ONE: 
		case SERIAL_TWO: 
		case SERIAL_THREE: 
		case SERIAL_FOUR: 
			fprintf(cgiOut, "cfg.serial[%d].speed  = %d\r\n", deviceNum, cfg.serial.speed);
			fprintf(cgiOut, "cfg.serial[%d].data   = %d\r\n", deviceNum, cfg.serial.data);
			fprintf(cgiOut, "cfg.serial[%d].stop   = %d\r\n", deviceNum, cfg.serial.stop);
			fprintf(cgiOut, "cfg.serial[%d].parity = %c\r\n", deviceNum, cfg.serial.parity);
			return SUCCESS;
		case NET: 
			fprintf(cgiOut, "cfg.netdata.subnetmask = %s\r\n", cfg.netdata.netmask);
			fprintf(cgiOut, "cfg.netdata.gateway = %s\r\n", cfg.netdata.gateway);
			fprintf(cgiOut, "cfg.netdata.port = %d\r\n", cfg.netdata.port);
			return SUCCESS;
		case PWD: 
			return SUCCESS;
		default:
			fprintf(cgiOut, "print_cfg()->deviceNum: %d not support<br>\r\n", deviceNum);
			return EPARAMETER;
	}

	return SUCCESS;
}//print_cfg
/*-----------------------------  END FOR DEBUG  --------------------------*/

