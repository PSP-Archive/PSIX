#include "ezxml.h"
#include "psix.h"
#include "zlib.h"

#define PACKAGE_APP 1
#define PACKAGE_GAME 2
#define PACKAGE_UTIL 4
#define PACKAGE_OTHER 8

int httpSocket = -1;
ezxml_t xmlFile;
struct package packageList[512];
int packageListSize = 0;

void psix_util_md5sum(const char* string, char *result)
{
  u8 digest[16];
  sceKernelUtilsMd5Digest((u8*)string, strlen(string), digest);
  int i;
  //char result[33];
  for (i = 0; i < 16; i++) sprintf(result + 2 * i, "%02x", digest[i]);
  //return sprintf("%s", result);
}

void psix_net_getconnlist()
{
  struct connection connList[10];
  int i;
  int numConfigs = 0;
  while (sceUtilityCheckNetParam(numConfigs) == 0) {
    numConfigs++;
  }
  numConfigs--;
  for (i = 1; i < numConfigs; i++)
    {
      struct connection newConn;
      sceUtilityGetNetParam(i, 0, &newConn.name);
      sceUtilityGetNetParam(i, 1, &newConn.data);
      sceUtilityGetNetParam(i, 4, &newConn.detail);
      connList[i] = newConn;
      pspDebugScreenPrintf("Connection Found! Name: %s\n", newConn.name.asString);
    }
  if (i == 1)
    {
      psix_net_init(0);
    }
  else if (i > 1)
    {
      //Return the connection list or something
    }
}

void psix_net_init(int config)
{
  int err = pspSdkInetInit();
  if (err < 0)
    return;
  pspDebugScreenPrintf("Inet Inited.\n");
  err = sceNetApctlConnect(config);
  if (err < 0)
    return;
  pspDebugScreenPrintf("AP Connecting");
  psix_net_connbox_open();
  char* progressBar = "Connecting";
  while (1)
    {
      int state;
      sceNetApctlGetState(&state);
      pspDebugScreenPrintf(".");
      //sprintf(progressBar, ".");
      psix_net_connbox_set(progressBar);
      if (state == 4)
	break;
      sceKernelDelayThread(1000000);
    }
  psix_net_connbox_close();
  pspDebugScreenPrintf(" Done!\n");
  httpSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (httpSocket < 0)
    return;
  pspDebugScreenPrintf("Socket Inited.\n\n");
}

void psix_net_connect(char* ip, int port)
{
  struct sockaddr_in httpProvider;
  httpProvider.sin_family = AF_INET;
  httpProvider.sin_port = htons(port);
  int err = sceNetInetInetAton(ip, &httpProvider.sin_addr);
  if (err < 0)
    return;
  connect(httpSocket, (struct sockaddr*) &httpProvider, sizeof(httpProvider));
  pspDebugScreenPrintf("Connected.\n\n");
  return;
}

void psix_net_disconnect()
{
  httpSocket = -1;
}

void psix_net_term()
{
  sceNetApctlDisconnect();
  pspSdkInetTerm();
}

int psix_pdc_getxml()
{
  //const char* xmlRequest = "GET /user/getxml.php HTTP/1.1\r\nHost: www.psixonline.com\r\nMAC=00:00:00:00:00:00\r\n";
  const char* xmlRequest = "GET /~naoneo/pdc/pdc.xml HTTP/1.1\r\nHost: www.lostnovice.com\r\n\r\n";
  send(httpSocket, xmlRequest, strlen(xmlRequest), 0);
  pspDebugScreenPrintf("Query sent.\n");
  
  int xmlCapacity = 1024;
  int xmlLen = 0;
  u8* xmlBuffer = (u8*) malloc(xmlCapacity);
  pspDebugScreenPrintf("Recieve");
  while (1)
    {
      int xmlCount = recv(httpSocket, (u8*) &xmlBuffer[xmlLen], xmlCapacity - xmlLen, 0);
      pspDebugScreenPrintf(".");
      if (xmlCount <= 0)
	{
	  break;
	}
      xmlLen += xmlCount;
      if (xmlLen > xmlCapacity)
	{
	  xmlCapacity *= 2;
	  xmlBuffer = realloc(xmlBuffer, xmlCapacity);
	  if (!xmlBuffer) break;
	}
      if (xmlBuffer) {
	xmlBuffer[xmlLen] = 0;
	char* xmlPage = strstr((char*) xmlBuffer, "\r\n\r\n");
	if (xmlPage) {
	  xmlPage += 4;
	  pspDebugScreenPrintf(" Done!\n\n");
	  pspDebugScreenInit();
	  pspDebugScreenPrintf("%s", xmlPage);
	  sceKernelDelayThread(5000000);
	  xmlFile = ezxml_parse_str(xmlPage, strlen(xmlPage));			
	  return 1;
	}
	free(xmlBuffer);
      }
    }
  return 0;
}

void psix_pdc_parsexml(int packageFilter)
{
  packageListSize = 0;
  //realloc(&packageList, sizeof(struct package) * 512);
  if (xmlFile->child)
    {
      ezxml_t xmlPackage = xmlFile->child;
      int i, j = 0;
      for (i = 0; i < 512; i++)
	{
	  if (xmlPackage)
	    {
	      //packageList[i] = malloc(sizeof(struct package));
	      struct package newPackage;
	      newPackage.id = atoi(ezxml_child(xmlPackage, "id")->txt);
	      newPackage.name = ezxml_child(xmlPackage, "name")->txt;
	      newPackage.description = ezxml_child(xmlPackage, "desc")->txt;
	      newPackage.category = atoi(ezxml_child(xmlPackage, "cat")->txt);
	      if (packageFilter)
		{
		  if (packageFilter != newPackage.category)
		    {
		      pspDebugScreenPrintf("Package was filtered out.\n");
		      i--;
		      xmlPackage = ezxml_next(xmlPackage);
		      continue;
		    }
		}
	      newPackage.author = ezxml_child(xmlPackage, "author")->txt;
	      newPackage.firmware = atoi(ezxml_child(xmlPackage, "firm")->txt);
	      pspDebugScreenPrintf("Package: %s - by %s\n", newPackage.name, newPackage.author);
	      ezxml_t xmlVersion = ezxml_child(ezxml_child(xmlPackage, "versions"), "version");
	      sceKernelDelayThread(5000000);
	      while(xmlVersion != NULL)
		{
		  newPackage.versions[j] = malloc(sizeof(struct version));
		  newPackage.versions[j]->maj = atoi(ezxml_attr(xmlVersion, "maj"));
		  newPackage.versions[j]->min = atoi(ezxml_attr(xmlVersion, "min"));
		  newPackage.versions[j]->rev = atoi(ezxml_attr(xmlVersion, "rev"));
		  newPackage.versions[j]->md5 = ezxml_child(xmlVersion, "md5")->txt;
		  newPackage.versions[j]->changes = ezxml_child(xmlVersion, "changes")->txt;
		  pspDebugScreenPrintf("Latest Version: v%d.%d%d\n\n",newPackage.versions[j]->maj, newPackage.versions[j]->min, newPackage.versions[j]->rev);
		  xmlVersion = ezxml_next(xmlVersion);
		  j++;
		}
	      
	      packageList[i] = newPackage;
	      packageListSize++;
	    }
	  else
	    break;
	  
	  xmlPackage = ezxml_next(xmlPackage);
	}
    }
  else
    return;
}

int psix_pdc_getpkgcount()
{
  return packageListSize;
}

struct package psix_pdc_getpkg(int index)
{
  return packageList[index];
}

int psix_pdc_installpkg(int id)
{
  char fileRequest[256];
  sprintf(fileRequest, "GET /apps/%d HTTP/1.1\r\nHost: cocot.sytes.net\r\n\r\n", id);
  send(httpSocket, fileRequest, strlen(fileRequest), 0);
  pspDebugScreenPrintf("Query sent.\n");
  
  int fileCapacity = 1024;
  int fileLen = 0;
  u8* fileBuffer = (u8*) malloc(fileCapacity);
  pspDebugScreenPrintf("Recieve");
  while (1)
    {
      int fileCount = recv(httpSocket, (u8*) &fileBuffer[fileLen], fileCapacity - fileLen, 0);
      pspDebugScreenPrintf(".");
      if (fileCount <= 0)
	{
	  break;
	}
      fileLen += fileCount;
      if (fileLen > fileCapacity)
	{
	  fileCapacity *= 2;
	  fileBuffer = realloc(fileBuffer, fileCapacity);
	  if (!fileBuffer) break;
	}
      if (fileBuffer)
	{
	  fileBuffer[fileLen] = 0;
	  char* filePage = strstr((char*) fileBuffer, "\r\n\r\n");
	  if (filePage)
	    {
	      filePage += 4;
	      pspDebugScreenPrintf(" Done!\n\n");
	      char fileStore[256];
	      sprintf(fileStore, "ms0:/PSP/GAME/psix/%d.app", id);
	      int fd = sceIoOpen(fileStore, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	      sceIoWrite(fd, filePage, strlen(filePage));
	      sceIoClose(fd);
	      //Here we extract it to the correct place
	      sceIoRemove(fileStore);
	      return 1;
	    }
	  free(fileBuffer);
	}
    }
  return 0;
}

int psix_pdc_updatepkg(char* path)
{
  int fd = sceIoOpen(path, PSP_O_RDONLY, 0777);
  pspDebugScreenPrintf("Opened file...\n");
  if (fd >= 0)
    {
      char fileData[1024];
      sceIoRead(fd, fileData, sizeof(fileData));
      sceIoClose(fd);
      char md5[33];
      psix_util_md5sum(fileData, md5);

      int i, j;
      ezxml_t xmlPackage = xmlFile->child;
      for (i = 0; i < packageListSize; i++)
	{
	  //ezxml_t xmlVersion = ezxml_child(xmlPackage, "versions");
	  ezxml_t xmlVersion = ezxml_child(ezxml_child(xmlPackage, "versions"), "version");
	  
	  while(xmlVersion != NULL)
	    {
	      if(!strcmp(ezxml_child(xmlVersion, "md5")->txt,md5))
		//if (xmlVersion->md5 == *md5)
		{
		  //if (j != 0)  quoi?
		    {
		      psix_net_connbox_open();
		      psix_net_connbox_set("found match");

		      //ok, order an install
		      //psix_pdc_installpkg(xmlPackage.id);
		    }
		  break;
		}
	      xmlVersion = ezxml_next(xmlVersion);
	    }
	  xmlPackage = ezxml_next(xmlPackage);
	}
    } else
      return 0 ;
  return 0;
}

void psix_net_connbox_open()
{
  sdi_set_enabled("net_conn_main",1);
  sdi_new("net_conn_text");
  sdi_set_overlay("net_conn_text",1);
  sdi_set_enabled("net_conn_text",1);
}

void psix_net_connbox_close()
{
  sdi_set_enabled("net_conn_main",0);
  sdi_del("net_conn_text");
}

void psix_net_connbox_set(char *str)
{
  sdi_print_line_max("net_conn_text","font_black",str,168,0);
  int w = sdi_get_width("net_conn_text");
  sdi_set_position("net_conn_text",161+(168-w)/2,143);
}
