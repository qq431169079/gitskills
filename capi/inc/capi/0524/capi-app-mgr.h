

#ifndef _CAPI_AM_H_
#define _CAPI_AM_H_


#define _GNU_SOURCE
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <gio/gio.h>
#include <syslog.h>

#include "capi-dbus-service.h"
#include "capi-sys-service.h"

extern CtSgwDbusServiceContext_t *sgw_dbus_service_context;

#define DBUS_BUS_ADDRESS "/var/run/dbus/server_address"

#ifdef USE_SESSION_BUS
#define DBUS_BUS_NAME "DBUS_SESSION_BUS_ADDRESS"
#define DBUS_CONF "/etc/dbus-1/session.conf"
#define DBUS_CONF_DIR "/etc/dbus-1/session.d"
#define DBUS_TYPE G_BUS_TYPE_SESSION
#else
#define DBUS_BUS_NAME "DBUS_SYSTEM_BUS_ADDRESS"
#define DBUS_CONF "/etc/dbus-1/system.conf"
#define DBUS_CONF_DIR "/etc/dbus-1/system.d"
#define DBUS_TYPE G_BUS_TYPE_SYSTEM
#endif


#define MAX_UCI_OPT_NAME_LEN 16
#define MAX_UCI_OPT_VAL_LEN 16

typedef char CtSgwUCIOptName_t[MAX_UCI_OPT_NAME_LEN + 1];
typedef char CtSgwUCIOptVal_t[MAX_UCI_OPT_VAL_LEN + 1];
typedef struct {
	CtSgwUCIOptName_t name;
	CtSgwUCIOptVal_t value;
} CtSgwUCIOpt_t;

typedef struct {
	char name[STR_LEN_128];
	char appid[STR_LEN_128];
	char version[STR_LEN_16];
	unsigned int state;
} CtSgwAppInfo_t;

int CtSgwSetDbusEnv(void);

int CtSgwUCIReadOption(const char *pkg_name, const char *section_name, const char *option_name, CtSgwUCIOptVal_t option_val);
int CtSgwUCIReadSection(const char *pkg_name, const char *section_type, const char *section_name, CtSgwUCIOpt_t **opts, uint32_t *optsize);
int CtSgwUCIWriteOption(const char *pkg_name, const char *section_name, const char *option_name, const char *option_val);

void CtSgwLogOpen(int facility, const char *ident);
void CtSgwLogClose(void);
void CtSgwLog(int priority, const char *fmt, ...);
int CtSgwIsLogInitialized(void);


typedef struct {
	/* TODO */
	char name[STR_LEN_32];
	char state[STR_LEN_16];
	pid_t pid;
} CtSgwAppStatus_t;

int CtSgwStartApp(const char *app_name);

int CtSgwUninstallApp(const char *app_name);

int CtSgwInstallApp(const char *app_file);
int CtSgwUpgradeApp(const char *app_file);

int CtSgwStopApp(const char *app_name);
int CtSgwReloadApp(const char *app_name);

int CtSgwGetAppStatus(CtSgwAppStatus_t *app_status);
int CtSgwListApp(CtSgwAppInfo_t **apps, uint32_t *size);

int CtSgwHandleKernelModules(const char *pkg_name, gboolean insert, const char *mod_name, const char *mod_opts);

int CtSgwPostMsg(const char *app_name, const char *msg);
int CtSgwFactoryPlugin(const char *app_name);

CtSgwDBusNodeInfo_t *CtSgwAppRegisterMgtFuncs(CtSgwAppMgtCallbacks_t *cbs);

int CtSgwAppMountUsbDevice(char *mountCmd, char *mountSrc, char *mountDest, char *opts);

int CtSgwAppUnmountUsbDevice(char *umountCmd, char *umountPath, char *opts);

#endif
