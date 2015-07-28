#ifndef SUBFUNCTION_C
#define SUNFUNCTION_C 1

/*---------------------  注册新功能 ---------------------*/
#define DEVICE_TOTAL		7

#define SERIAL_ONE				1
#define SERIAL_TWO				2
#define SERIAL_THREE			3
#define SERIAL_FOUR				4
#define NET						5
#define PWD						6
#define LOGIN					7

#define		DEVICE_NAME_1		"串口 1"
#define		DEVICE_NAME_2		"串口 2"
#define		DEVICE_NAME_3		"串口 3"
#define		DEVICE_NAME_4		"串口 4"
#define		DEVICE_NAME_5		"网络配置"
#define		DEVICE_NAME_6		"密码管理"
/*-------------------  END 注册新功能 -------------------*/

#define		SUCCESS					0		//success
#define		EFAILED					-1		//faile
#define		ENOTOPENFILE			-2		//open file error, serial_device or net device
#define		EBADDEVICENU			-3		//device num is not support
#define		EPARAMETER			    -4		//device num is not support
#define		ERETVAL					-5		//device num is not support


#define	DOMAIN_SOCKET	"/etc/ntos/sample-socket"

//for authentication
#define CGI_AUTH_IP					"CGI_AUTH_CLIENT_IP"
#define CGICLIENTIP					"REMOTE_ADDR"

//status code
#define BADREQUIST			400
#define UNAUTHENTICATION	401
#define SUSSHAND			200
#define	NOTMODIFY			304
#define CONFLICT			409
#define INTERNAL_SERVERERR	500
#define SERVICE_UNAVAILABLE	503


//what to do?  receive or transmission
#define		RECEIVE				0
#define		SEND				1

#define CONFIG_T		body_t
//配置文件结构体
typedef struct {
	int  speed;
	int  data;
	int  stop;
	char parity;
}serial_t;
typedef struct {
#define		IP_LEN		64			//IP长度
	char ipstr[IP_LEN];
	int  port;
	char gateway[IP_LEN];
	char netmask[IP_LEN];
}net_t;
typedef struct {
	int	flag;		// 0获取,1保存
	int	device;	//device number	
	int length;	//length of body_t
}head_t;
typedef struct {
#define UNAME	16
#define PWDLEN	16
	char uname[UNAME];
	char key[PWDLEN];	//密码
}user_t;
typedef union _body_t{
	serial_t serial;
	net_t	 netdata;
	user_t   user;
}body_t;

typedef struct {
	head_t head;
	body_t body;
}commu_t;

/*--------------   FUNCTION   -----------------*/
//socket
int  CloseSocket(int sock);
int  MyConnect();

//save and load
int LoadConf(int sock, int deviceNum, void *saveCfg);
int SaveConf(int sock, int deviceNum, CONFIG_T newCfg);

//Receive and Send
int ReceiveSubmit(int deviceNum, void *newCfg);
int SendSubmit(int deviceNum, CONFIG_T newCfg);
int cgi_sprint(const char *result, const char *url, int statuscode, const char *describ);

int CheckLogin(user_t ruser, user_t ouser);
int my_reboot();
int GetPasswordBrow(char *name, char *result, int len);

int print_cfg(int deviceNum, CONFIG_T cfg);
FILE* afreshOrient(const char *fname, const char *mode, FILE *stream);
/*--------------  END FUNCTION   -----------------*/

#endif /* SUBFUNCTION_C */
