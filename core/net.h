typedef struct version *version_t;
typedef struct package *package_t;

struct version
{
	int id;
	int maj;
	int min;
	int rev;
	char* md5;
	char* changes;
};

struct package
{
	int id;
	char* name;
	char* description;
	char* author;
	int category;
	int firmware;
	version_t* versions;
};

struct connection
{
	netData data;
	netData name;
	netData detail;
};

void psix_net_getconnlist();
void psix_net_init(int config);
void psix_net_connect(char* ip, int port);
void psix_net_disconnect();
void psix_net_term();

int psix_pdc_getxml();
void psix_pdc_parsexml(int packageFilter);
int psix_pdc_getpkgcount();
struct package psix_pdc_getpkg(int index);
int psix_pdc_installpkg(int id);

void psix_net_connbox_open();
void psix_net_connbox_set();
void psix_net_connbox_close();
