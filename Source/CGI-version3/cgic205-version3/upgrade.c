#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include "cgic.h"
#include "subFunction.h"

const char *upFileType[] = {
	".tar.gz",
	".tar.bz2",
	".zip",
	"NULL"
};

#define SUBMITNAME		"uploadFileName"
#define SAVEFILEPATH	"/upgrade/"
#define NEWFILENAME		"upgrade.tar.gz"

int UiReturn(int stat);
int UploadFile();

int cgiMain() 
{
	afreshOrient("cgi.txt", "a", stdout); //test
	/* Send the content type, letting the browser know this is HTML */
	cgiHeaderContentType("text/html");

	/* If a submit button has already been clicked, act on the 
		submission of the form. */
	if (cgiFormSubmitClicked("uploadSubmit") == cgiFormSuccess) {
		if(UiReturn(UploadFile()) != SUCCESS)
			exit(EFAILED);
	} else if (cgiFormSubmitClicked("rebootSubmit") == cgiFormSuccess) {
		my_reboot();
	}

	exit(SUCCESS);
}

static int isChecked(const char *fileName, const char **upFileType)
{
	int i, len;

	if(upFileType == NULL)
		return EFAILED;

	for(i = 0; strcmp(upFileType[i], "NULL") != 0; i++) {
		if(strlen(fileName) <= strlen(upFileType[i]))
			continue;

		len = strlen(fileName) - strlen(upFileType[i]);
		if(strcmp(upFileType[i], fileName + len) == 0) {
			return SUCCESS;
		}
	}

	return EFAILED;
}

//get file of upload
int UploadFile()
{
	int fd;
	int got;
	cgiFilePtr filep;
	char pathFname[1024], fileName[1024], fileBuf[1024];

	//get file name
	if (cgiFormFileName(SUBMITNAME, fileName, sizeof(fileName)) != cgiFormSuccess) {
		fprintf(stdout, "No file was uploaded.\r\n");
		return EFAILED;
	} 

	if(isChecked(fileName, upFileType) != 0) {
		fprintf(stdout, "Upload file type is not support\r\n");	
		return EFAILED;
	}

	if (cgiFormFileOpen(SUBMITNAME, &filep) != cgiFormSuccess) {
		fprintf(cgiOut, "Can't open the file.\r\n");
		return EFAILED;
	}

	sprintf(pathFname, "%s%s", SAVEFILEPATH, NEWFILENAME);

	if((fd = open(pathFname, O_TRUNC | O_CREAT | O_WRONLY, 444)) < 0) {
		fprintf(stdout, "UploadFile()->open():%s\r\n", strerror(errno));	
		return EFAILED;
	}

	while (cgiFormFileRead(filep, fileBuf, sizeof(fileBuf), &got) == cgiFormSuccess) 
		write(fd, fileBuf, got);

	sync();

	close(fd);
	cgiFormFileClose(filep);
	return SUCCESS;
}

//return the stat to brower;
int UiReturn(int stat)
{
	return SUCCESS;
}
