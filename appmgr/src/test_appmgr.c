#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include "capi.h"
#include "test_appmgr.h"        
#include "result_check_util.h"
#include "command_util.h"
#define pressure_num 3

				
FILE *fp;
CtSgwAppStatus_t app_status; //define one struct: app_status 
CtSgwAppInfo_t *apps = NULL; //define one struct: *apps
CtSgwAppInfo_t *papp = NULL;
int i = 0;
int size  = 0;
int  return_list=-1;
char *result[]={"pass","fail"};
char *target_status[]={"RUNNING","STOPPED","EEXIST"};
char *function_name[]={"install","upgrade","start","stop",
                       "reload","uninstall","UCIWriteOption","UCIReadSection",
                       "UCIReadOption","Log_Open","Log_Close","Log_Cover",
                       "LOG_USER","LOG_NOTICE","LOG_ERR","Root_Limit",
                       "Memory_Limit","CPU_Limit","ListApp",		
                      };

char *row_name[]={"Moduel","Function","App_name","Return_value",
                  "Return_target","State_value","State_target","Action","Result",
                 };
char *module_name[]={"One_Test","Install_Test","Upgrade_Test","Start_test",
                     "Stop_Test","Reload_Test","Uninstall_Test","UCI_Test",
                     "LOG_Test","Safety_Test","ListApp_Test","Pressure_Test",
                     "Appnum_test",
                    };
static int PASS=0,FAIL=0;

void* install_1(void* m);
void* install_2(void* m);
void* install_3(void* m);
void* install_4(void* m);
void* install_5(void* m);
int Process_test_safe();
void check_string(void);
void Process_test_appnumber(void);	
/*
 AMRET_DISCONNECT = -1002,
 AMRET_UNKNOW_DEVICE = -1001,
 AMRET_INVALID_TOKEN = -1000,
 AMRET_OP_TIMEOUT = -100,
 AMRET_APP_INSTALL_FAILED = -17,
 AMRET_APP_NOT_RUNNING = -16,
 AMRET_APP_UNINSTALL_NOT_ALLOWED = -15,
 AMRET_APP_DISABLE_NOT_ALLOWED = -14,
 AMRET_APP_IS_RUNNING = -13,
 AMRET_APP_NOT_EXIST = -12,
 AMRET_APP_INSTALLED = -11,
 AMRET_APP_RUN_FAILED = -10,
 AMRET_APP_DL_FAILED = -9,
 AMRET_APP_INVALID_SIG = -8,
 AMRET_APP_INVALID_PLATFORM = -7,
 AMRET_APP_INVALID_VERSION = -6,
 AMRET_APP_BUSY = -5,
 AMRET_APP_NO_SPACE = -4,
 AMRET_APP_URL_UNREACHABLE = -3,
 AMRET_ILLEGAL_DEVICE = -2,
 AMRET_GENERAL_ERROR = -1,
 AMRET_SUCCESS = 0,

*/
char* value_transfrom(int value)
{
    switch(value){
        case  AMRET_DISCONNECT  : return "AMRET_DISCONNECT";
        case  AMRET_UNKNOW_DEVICE  : return "AMRET_UNKNOW_DEVICE";
        case  AMRET_INVALID_TOKEN  : return "AMRET_INVALID_TOKEN";
        case  AMRET_OP_TIMEOUT  : return "AMRET_OP_TIMEOUT";
        case  AMRET_APP_INSTALL_FAILED: return "AMRET_APP_INSTALL_FAILED";
        case  AMRET_APP_NOT_RUNNING: return "AMRET_APP_NOT_RUNNING";
        case  AMRET_APP_UNINSTALL_NOT_ALLOWED: return "AMRET_APP_UNINSTALL_NOT_ALLOWED";
        case  AMRET_APP_DISABLE_NOT_ALLOWED: return "AMRET_APP_DISABLE_NOT_ALLOWED";
        case  AMRET_APP_IS_RUNNING: return "AMRET_APP_IS_RUNNING";
        case  AMRET_APP_NOT_EXIST: return "AMRET_APP_NOT_EXIST";
        case  AMRET_APP_INSTALLED: return "AMRET_APP_INSTALLED";
        case  AMRET_APP_RUN_FAILED  : return "AMRET_APP_RUN_FAILED";
        case  AMRET_APP_DL_FAILED: return "AMRET_APP_DL_FAILED";
        case  AMRET_APP_INVALID_SIG: return "AMRET_APP_INVALID_SIG";
        case  AMRET_APP_INVALID_PLATFORM: return "AMRET_APP_INVALID_PLATFORM";
        case  AMRET_APP_INVALID_VERSION: return "AMRET_APP_INVALID_VERSION";
        case  AMRET_APP_BUSY: return "AMRET_APP_BUSY";
        case  AMRET_APP_NO_SPACE: return "AMRET_APP_NO_SPACE";
        case  AMRET_APP_URL_UNREACHABLE: return "AMRET_APP_URL_UNREACHABLE";
        case  AMRET_ILLEGAL_DEVICE: return "AMRET_ILLEGAL_DEVICE";
        case  AMRET_GENERAL_ERROR: return "AMRET_GENERAL_ERROR";
        case  AMRET_SUCCESS : return "AMRET_SUCCESS";
        default : return NULL;
    }
    return NULL;
}

/**********write_xls*********/
int write_xls(char *module,char *function,char *app_name,int return_value,
                int return_target,char *state_value,char *state_target,char* action, char *result)
 {
   fp=fopen("/tmp/appmgr/appmgr.xls","a");
   if(fp){
      char* return_print=NULL;
      return_print=value_transfrom(return_value);
      char* return_target_print=NULL;
      return_target_print=value_transfrom(return_target);
      if(return_print){
         if(return_target_print)
             fprintf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",module,function,app_name,return_print,return_target_print,state_value,state_target,action, result);
         else
             fprintf(fp,"%s\t%s\t%s\t%s\t%d\t%s\t%s\t%s\t%s\n",module,function,app_name,return_print,return_target,state_value,state_target,action,result);

      }
      else
         fprintf(fp,"%s\t%s\t%s\t%d\t%d\t%s\t%s\t%s\t%s\n",module,function,app_name,return_value,return_target,state_value,state_target,action,result);
      fclose(fp);
   }
   fp=NULL;
 }

/**********listapp*************/
void applist(void)
{
    
   return_list = CtSgwListApp(&apps, &size);  //output:apps(list the installed apps),size(the number of the installed apps)
   g_print ("List App returns: %d number: %d\n", return_list, size);
   papp = apps;
   for  (i = 0; i < size; ++i)
    {
       g_print ("ListApp is : %s\n", papp->name);
       ++papp;
                        
    }
   g_free (apps);
   papp = NULL;

}
/****************************/
int start(gchar *command){

   g_print("command%s\n",command);
   //g_print("start\n");											
   int mode=0; 
   fp=fopen("/tmp/appmgr/appmgr.xls","a");
   if(fp){
   fprintf(fp,"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",row_name[0],row_name[1],row_name[2],row_name[3],row_name[4],row_name[5],row_name[6],row_name[7],row_name[8]);
   fclose(fp);
   }
   fp=NULL;
   
  mode =Get_mode(command);//Get_mode(command);    		
  g_print("mode=%d\n",mode);

  switch(mode){
   case APP_ALL:
    Process_test_all();
        break; 
   case APP_ONE:
     Process_test_one();
        break;
   case APP_INSTALL:
     Process_test_install();
   	break;   
   case APP_START:
     Process_test_start();
	 break;	  
   case APP_UPGRADE:
     Process_test_upgrade();
	 break; 
   case APP_STOP:
     Process_test_stop();
         break;
   case APP_RELOAD:		
     Process_test_reload();
         break;
  case APP_UNINSTALL:		
     Process_test_uninstall();
         break;  
  case SAFE_TEST:
     Process_test_safe();
         break;
  case UCI_CAPI:
     Process_test_UCI();
         break;
 case TEST_LOG:
     Process_test_log();         
         break;
 case PRESSURE_TEST:
     Process_test_pressure();
         break;
 case APPNUM_TEST:
     Process_test_appnumber();
  default:break;

  }

}


/********Test All **********/
int Process_test_all(){
  
    Process_test_log();
  
    Process_test_UCI();  

    Process_test_one();
   		
    Process_test_install();

    Process_test_upgrade();
    
    Process_test_start();     

    Process_test_stop();

    Process_test_reload();
    
    Process_test_uninstall(); 

    //Process_test_appnumber();

    Process_test_safe();
    

}  

/********Pressure Test********/
int Process_test_pressure(){
  int i=-1,j=-1;
 

  for(i=0;i<pressure_num;i++){
     Process_test_UCI();
  }  

  for(i=0;i<pressure_num;i++){
     Process_test_install();
  }
  for(i=0;i<pressure_num;i++){
     Process_test_uninstall();
  }
  for(i=0;i<pressure_num;i++){
     Process_test_stop();
  }
  for(i=0;i<pressure_num;i++){
    Process_test_appnumber();
  }		
  
  for(i=0;i<pressure_num;i++){
     Process_test_start();
  }				
    	
}

#ifdef Realtek
                    char *appnum_install[]={"/tmp/appmgr/applist/app_install/normal/tsappaa_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappab_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappac_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappad_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappae_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaf_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappag_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappah_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappai_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaj_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappak_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappal_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappam_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappan_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappao_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappap_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaq_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappar_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappas_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappat_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappau_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappav_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaw_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappax_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappay_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaz_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappba_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbb_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbc_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbd_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbe_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbf_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbg_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbh_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbi_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbj_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbk_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbl_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbm_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbn_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbp_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbq_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbr_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbs_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbt_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbu_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbv_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbw_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbx_1_rtl9600.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappby_1_rtl9600.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbz_1_rtl9600.ipk",
                                          
                                           };        
		    

#elif Broadcom
                   char *appnum_install[]={"/tmp/appmgr/applist/app_install/normal/tsappaa_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappab_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappac_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappad_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappae_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaf_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappag_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappah_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappai_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaj_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappak_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappal_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappam_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappan_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappao_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappap_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaq_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappar_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappas_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappat_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappau_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappav_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaw_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappax_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappay_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaz_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappba_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbb_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbc_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbd_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbe_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbf_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbg_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbh_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbi_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbj_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbk_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbl_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbm_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbn_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbp_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbq_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbr_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbs_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbt_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbu_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbv_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbw_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbx_1_bcm6838.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappby_1_bcm6838.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbz_1_bcm6838.ipk",

                                           };        
#elif Huawei
                    char *appnum_install[]={"/tmp/appmgr/applist/app_install/normal/tsappaa_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappab_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappac_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappad_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappae_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaf_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappag_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappah_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappai_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaj_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappak_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappal_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappam_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappan_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappao_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappap_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaq_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappar_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappas_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappat_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappau_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappav_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaw_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappax_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappay_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaz_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappba_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbb_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbc_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbd_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbe_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbf_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbg_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbh_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbi_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbj_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbk_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbl_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbm_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbn_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbp_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbq_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbr_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbs_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbt_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbu_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbv_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbw_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbx_1_sd5115.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappby_1_sd5115.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbz_1_sd5115.ipk",
                                          
                                           };    
#elif MTK
                    char *appnum_install[]={"/tmp/appmgr/applist/app_install/normal/tsappaa_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappab_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappac_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappad_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappae_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaf_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappag_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappah_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappai_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaj_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappak_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappal_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappam_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappan_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappao_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappap_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaq_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappar_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappas_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappat_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappau_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappav_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaw_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappax_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappay_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaz_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappba_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbb_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbc_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbd_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbe_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbf_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbg_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbh_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbi_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbj_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbk_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbl_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbm_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbn_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbp_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbq_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbr_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbs_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbt_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbu_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbv_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbw_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbx_1_mt7526.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappby_1_mt7526.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbz_1_mt7526.ipk",
                                          
                                           };    

#elif Zhongxing
                    char *appnum_install[]={"/tmp/appmgr/applist/app_install/normal/tsappaa_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappab_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappac_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappad_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappae_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaf_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappag_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappah_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappai_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaj_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappak_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappal_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappam_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappan_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappao_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappap_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaq_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappar_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappas_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappat_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappau_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappav_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappaw_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappax_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappay_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappaz_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappba_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbb_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbc_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbd_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbe_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbf_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbg_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbh_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbi_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbj_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbk_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbl_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbm_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbn_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbo_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbp_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbq_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbr_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbs_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbt_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbu_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbv_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbw_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappbx_1_zx279100.ipk",
                                            "/tmp/appmgr/applist/app_install/normal/tsappby_1_zx279100.ipk",  
                                            "/tmp/appmgr/applist/app_install/normal/tsappbz_1_zx279100.ipk",
                                          
                                           };    

#endif 

                   char *appnum_name[]={"tsappaa tsappab tsappac tsappad tsappae tsappaf tsappag tsappah tsappai tsappaj tsappak tsappal tsappam tsappan tsappao tsappap tsappaq tsappar tsappas tsappat tsappau tsappav tsappaw tsappax tsappay tsappaz tsappba tsappbc tsappbd tsappbe tsappbf tsappbg tsappbh tsappbi tsappbj tsappbk tsappbl tsappbm tsappbn tsappbo tsappbp tsappbq tsappbr tsappbs tsappbt tsappbu tsappbv tsappbw tsappbx tsappby tsappbz tsappca "};
                   
                   int appnum_array=sizeof(appnum_install)/sizeof(char*);

/******thread1**********/             
void* install_1(void*m)
{
                    int i=0;
                    int target_value=0;
                    int return_value=-1;
                    int Num_value;                    
                    for(i=0;i<appnum_array/5;i++)
                      {
                        return_value=CtSgwInstallApp(appnum_name[i]);                        
                        g_print("%s return_value is: %d\n",appnum_name[i],return_value);   
                        Num_value=CheckCtSgwtest_one(return_value,target_value);

                        if(Num_value==1){
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_1 install",result[0]);
                         g_print("Pass\n");
                        }
                        else{
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_1 install",result[1]);
                         g_print("Fail\n");
                        }

                      } 
}

/*****thread2******/
void* install_2(void*m)
{
                    int i=0;
                    int target_value=0;
                    int return_value=-1;
                    int Num_value;                    
                    for(i=appnum_array/5;i<2*appnum_array/5;i++)
                      {
                        return_value=CtSgwInstallApp(appnum_name[i]);                        
                        g_print("%s return_value is: %d\n",appnum_name[i],return_value);   
                        Num_value=CheckCtSgwtest_one(return_value,target_value);

                        if(Num_value==1){
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_2 install",result[0]);
                         g_print("Pass\n");
                        }
                        else{
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_2 install", result[1]);
                         g_print("Fail\n");
                        }

                      } 
}
/*****thread3********/
void* install_3(void*m)
{                  
                   int i=0;
                   int target_value=0;
                   int return_value=-1;
                   int Num_value; 
                   for(i=2*appnum_array/5;i<3*appnum_array/5;i++)
                      {
                        return_value=CtSgwInstallApp(appnum_name[i]);                        
                        g_print("%s return_value is: %d\n",appnum_name[i],return_value);   
                        Num_value=CheckCtSgwtest_one(return_value,target_value);

                        if(Num_value==1){
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_3 install",result[0]);
                         g_print("Pass\n");
                        }
                        else{
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_3 install",result[1]);
                         g_print("Fail\n");
                        }

                      } 


}
/*****thread4********/
void* install_4(void*m)
{
                   int i=0;
                   int target_value=0;
                   int return_value=-1;
                   int Num_value; 
                   for(i=3*appnum_array/5;i<4*appnum_array/5;i++)
                      {
                        return_value=CtSgwInstallApp(appnum_name[i]);                        
                        g_print("%s return_value is: %d\n",appnum_name[i],return_value);   
                        Num_value=CheckCtSgwtest_one(return_value,target_value);

                        if(Num_value==1){
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_4 install", result[0]);
                         g_print("Pass\n");
                        }
                        else{
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_4 install", result[1]);
                         g_print("Fail\n");
                        }

                      } 

}
/*****thread5********/
void* install_5(void*m)
{ 
                  int i=0;
                  int target_value=0;
                  int return_value=-1;
                  int Num_value; 
                  for(i=4*appnum_array/5;i<appnum_array/5;i++)
                      {
                        return_value=CtSgwInstallApp(appnum_name[i]);                        
                        g_print("%s return_value is: %d\n",appnum_name[i],return_value);   
                        Num_value=CheckCtSgwtest_one(return_value,target_value);

                        if(Num_value==1){
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_5 install",result[0]);
                         g_print("Pass\n");
                        }
                        else{
                         write_xls(module_name[12],function_name[0],appnum_name[i],return_value,target_value,NULL,NULL,"install_5 install",result[1]);
                         g_print("Fail\n");
                        }

                      } 


}                    
/****app_number测试******/
void Process_test_appnumber(void){
                    g_print("###Process_test_appnumber beginning###");
                    g_print("\n");   
                    g_print("appnum_array=%d\n",appnum_array);     
                    int i=0;              				
                    int c1=-1,c2=-1,c3=-1,c4=-1,c5=-1;
                    pthread_t t1,t2,t3,t4,t5;                   
                               
                  
                    applist();
                    c1=pthread_create(&t1,NULL,install_1,NULL);
                    c2=pthread_create(&t2,NULL,install_2,NULL);
                    c3=pthread_create(&t3,NULL,install_3,NULL);
                    c4=pthread_create(&t4,NULL,install_4,NULL);
                    c5=pthread_create(&t5,NULL,install_5,NULL);

                    pthread_join(t1,NULL);
                    pthread_join(t2,NULL);
                    pthread_join(t3,NULL);
                    pthread_join(t4,NULL);
                    pthread_join(t5,NULL);

                    applist();
                    if(appnum_array==size){
                      write_xls(module_name[12],function_name[0],NULL,appnum_array,size,NULL,NULL,"test_appnumber ",result[0]);
                      g_print("appnum_array=%d,size=%d\n",appnum_array,size);
                      g_print("appnumber test pass\n");
                    }
                    else{
                      write_xls(module_name[12],function_name[0],NULL,appnum_array,size,NULL,NULL,"test_appnumber", result[1]);
                      g_print("appnum_array=%d,size=%d\n",appnum_array,size);
                      g_print("appnumber test fail\n");
                    }

                    for(i=0;i<appnum_array;i++)
                     {
                       CtSgwUninstallApp(appnum_name[i]);

                     }
                    applist();
                    g_print("###Process_test_appnumber ending###");
                    g_print("\n"); 

}

                                  

/****输入"APP_ONE"进行单个app测试***/		
int Process_test_one(){		

                 						
                    g_print("###%s is begining###\n",__func__);   
                    g_print("\n");
                   
#ifdef Realtek
                    char app_install[]= "/tmp/appmgr/applist/app_install/normal/testapp_1_rtl9600.ipk";         
		    char app_upgrade[]= "/tmp/appmgr/applist/app_upgrade/testapp_1_rtl9600.ipk";


#elif Broadcom
                    char app_install[]= "/tmp/appmgr/applist/app_install/normal/testapp_1_bcm6838.ipk";         
		    char app_upgrade[]= "/tmp/appmgr/applist/app_upgrade/testapp_1_bcm6838.ipk";
#elif Huawei
                    char app_install[]= "/tmp/appmgr/applist/app_install/normal/testapp_1_sd5115.ipk";         
		    char app_upgrade[]= "/tmp/appmgr/applist/app_upgrade/testapp_1_sd5115.ipk";
#elif Zhongxing
		// TODO: replace the file with real ZTE ipk package
                    char app_install[]= "/tmp/appmgr/applist/app_install/normal/testapp_1_zx279100.ipk";         
		    char app_upgrade[]= "/tmp/appmgr/applist/app_upgrade/testapp_1_zx279100.ipk";
#elif MTK
		// TODO: replace the file with real MTK ipk package
                    char app_install[]= "/tmp/appmgr/applist/app_install/normal/testapp_1_mt7526.ipk";         
		    char app_upgrade[]= "/tmp/appmgr/applist/app_upgrade/testapp_1_mt7526.ipk";
#endif 		
                    char *app_name = "testapp";                  				
                    //int  target_value[]={-12,0,0,0,0,-16,0};  //目标返回值

                    int  target_value[]={AMRET_APP_NOT_EXIST,AMRET_SUCCESS,AMRET_SUCCESS,AMRET_SUCCESS,AMRET_SUCCESS,AMRET_APP_NOT_RUNNING,AMRET_SUCCESS};  //目标返回值
                    int  return_value=-1;  
                    int  Num_value=-1,Num_status=-1,Num_array=0;
                    int pass=0,fail=0;
                    int  flag=0;
                    pid_t fpid;
                    Num_array=sizeof(target_value)/sizeof(int);                

                    /*********uninstall all app************/
                    CtSgwListApp(&apps, &size); 
                    g_print ("Want uninstall all %d apps\n",size);
                    papp = apps;
                    for  (i = 0; i < size; ++i)
                     {
                        g_print ("Uninstall %s.\n", papp->name);
                        CtSgwUninstallApp(papp->name);
                        ++papp;
                     }
                    g_free (apps);
                    papp = NULL;
                    g_print("%s Unstall all finish\n",__func__);                     

                    /**********applist************/
                    g_print("Applist is begining\n");
                    applist();                    
                    
                    Num_value=CheckCtSgwtest_one(return_list,target_value[1]);                     
                    if(Num_value==1){
                      write_xls(module_name[10],function_name[18],NULL,return_list,target_value[1],NULL,NULL,"list ipk with no ipk",result[0]);
                    }
                    else{
                      write_xls(module_name[10],function_name[18],NULL,return_list,target_value[1],NULL,NULL,"list ipk with no ipk",result[1]);
                    }
                    g_print("\n");   

                    /***********uninstall*********/
                    g_print("Uninstall is begining\n");
                    return_value = CtSgwUninstallApp(app_name);
                    g_print("%s return_value is: %d\n",app_name,return_value);                        
                    
                    strcpy (app_status.name, app_name);                  
                    CtSgwGetAppStatus(&app_status);
                    g_print("%s state is: %s\n",app_name,app_status.state); 

                    Num_value=CheckCtSgwtest_one(return_value,target_value[0]); 
                    Num_status=CheckCtSgwStatus(app_status,target_status[2]);
                    g_print("Num_value=%d,Num_status=%d\n",Num_value,Num_status);
                    
                    
                    if(Num_value==1 && Num_status==1){
                     pass++;
                     write_xls(module_name[0],function_name[5],app_name,return_value,target_value[0],app_status.state,target_status[2],"uninstall ipk with no ipk",result[0]);
                     
                     g_print("Pass\n");
                    }
                    else{
                     fail++;
                     write_xls(module_name[0],function_name[5],app_name,return_value,target_value[0],app_status.state,target_status[2],"uninstall ipk with no ipk",result[1]);
                     
                     g_print("Fail\n");
                    }
                    
                    g_print("\n");          
                   
                    /***************install**************/
                    g_print("Installing is begining\n");
                    return_value = CtSgwInstallApp(app_install);
                    g_print("%s return_value is: %d\n",app_name,return_value);                   
                    applist(); 
                    sleep(2);
                    strcpy (app_status.name, app_name);                  
                    CtSgwGetAppStatus(&app_status);
                    g_print("%s state is: %s\n",app_name,app_status.state);              
 			                   
                    Num_value=CheckCtSgwtest_one(return_value,target_value[1]); 
                    Num_status=CheckCtSgwStatus(app_status,target_status[1]);
                    g_print("Num_value=%d,Num_status=%d\n",Num_value,Num_status);

                    if(Num_value==1 && Num_status==1){
                     pass++;
                     write_xls(module_name[0],function_name[0],app_name,return_value,target_value[1],app_status.state,target_status[1],"install ipk ", result[0]);
                     g_print("Pass\n");
                    }
                    else{
                     fail++;
                     write_xls(module_name[0],function_name[0],app_name,return_value,target_value[1],app_status.state,target_status[1],"install ipk ",result[1]);
                     g_print("Fail\n");
                    }

                    g_print("\n");
	            sleep (3);
                    /***********upgrade************/
                    g_print("Upgrade is begining\n"); 
                    return_value = CtSgwUpgradeApp(app_upgrade); 
                    g_print("%s return_value is: %d\n",app_name,return_value);     
                    sleep(2);
                    strcpy (app_status.name, app_name);                  
                    CtSgwGetAppStatus(&app_status);
                    g_print("%s state is: %s\n",app_name,app_status.state);     
                    sleep(2);            
                    
                    Num_value=CheckCtSgwtest_one(return_value,target_value[2]); 
                    Num_status=CheckCtSgwStatus(app_status,target_status[1]);
                    g_print("Num_value=%d,Num_status=%d\n",Num_value,Num_status);

                    if(Num_value==1 && Num_status==1){
                     pass++;
                     write_xls(module_name[0],function_name[1],app_name,return_value,target_value[2],app_status.state,target_status[1],"upgrade ipk",result[0]);
                     g_print("Pass\n");  
                    }         
                    else{
                     fail++;   
                     write_xls(module_name[0],function_name[1],app_name,return_value,target_value[2],app_status.state,target_status[1],"upgrade ipk",result[1]);
                     g_print("Fail\n");
                    }

                    g_print("\n");
                    /***********start**************/ 
                    g_print("Starting is begining\n"); 	            
                    return_value = CtSgwStartApp(app_name);
		    g_print("%s return_value is: %d\n",app_name,return_value);
                    sleep(2);
                    strcpy (app_status.name, app_name);
                    CtSgwGetAppStatus(&app_status);
                    g_print ("%s state is: %s\n",app_name,app_status.state);

                    Num_value=CheckCtSgwtest_one(return_value,target_value[3]); 
                    Num_status=CheckCtSgwStatus(app_status,target_status[0]);
                    g_print("Num_value=%d,Num_status=%d\n",Num_value,Num_status);

                    if(Num_value==1 && Num_status==1){
                     pass++;
                     write_xls(module_name[0],function_name[2],app_name,return_value,target_value[3],app_status.state,target_status[0],"start ipk",result[0]);
                     g_print("Pass\n");
                    }
                    else{
                     fail++;
                     write_xls(module_name[0],function_name[2],app_name,return_value,target_value[3],app_status.state,target_status[0],"start ipk",result[1]);
                     g_print("Fail\n");
                    }
              
                    g_print("\n");
                    /***********stop***************/
                    g_print("Stopping is begining\n"); 	 
                    sleep(2); 
                    return_value = CtSgwStopApp(app_name);
                    g_print("%s return_value is: %d\n",app_name,return_value);

                    sleep(2);
                    strcpy (app_status.name, app_name);		
                    CtSgwGetAppStatus(&app_status);
                    g_print ("%s state is: %s\n",app_name,app_status.state);

                    Num_value=CheckCtSgwtest_one(return_value,target_value[4]); 
                    Num_status=CheckCtSgwStatus(app_status,target_status[1]);
                    g_print("Num_value=%d,Num_status=%d\n",Num_value,Num_status);

                    if(Num_value==1 && Num_status==1){
                     pass++;
                     write_xls(module_name[0],function_name[3],app_name,return_value,target_value[4],app_status.state,target_status[1],"stop ipk",result[0]);
                     g_print("Pass\n");
                    }
                    else{
                     fail++;
                     write_xls(module_name[0],function_name[3],app_name,return_value,target_value[4],app_status.state,target_status[1],"stop ipk",result[1]);
                     g_print("Fail\n");
                    }
                     
                    g_print("\n");
                    /***********reload*************/
                    g_print("Reloading is begining\n"); 	  
                    return_value = CtSgwReloadApp(app_name);                    
                    g_print("%s return_value is: %d\n",app_name,return_value);
                    sleep(2);
                    strcpy (app_status.name, app_name);
                    CtSgwGetAppStatus(&app_status);
                    g_print ("%s state is: %s\n",app_name,app_status.state);         
                    
                    Num_value=CheckCtSgwtest_one(return_value,target_value[5]); 
                    Num_status=CheckCtSgwStatus(app_status,target_status[1]);
                    g_print("Num_value=%d,Num_status=%d\n",Num_value,Num_status);

                    if(Num_value==1 && Num_status==1){
                     pass++;
                     write_xls(module_name[0],function_name[4],app_name,return_value,target_value[5],app_status.state,target_status[1],"reload ipk",result[0]);
                     g_print("Pass\n");  
                    }         
                    else{
                     fail++;
                     write_xls(module_name[0],function_name[4],app_name,return_value,target_value[5],app_status.state,target_status[1],"reload ipk",result[1]);
                     g_print("Fail\n");
                    }

                    g_print("\n");
                    /***********uninstall*********/
                    applist();
                    g_print("Uninstalling is begining\n"); 	  
                    return_value = CtSgwUninstallApp(app_name);
                    g_print("%s return_value is: %d\n",app_name,return_value);
                    applist();
                    sleep(2);
                    strcpy (app_status.name, app_name);
                    CtSgwGetAppStatus(&app_status);
                    g_print ("%s state is: %s\n",app_name,app_status.state); 

                    Num_value=CheckCtSgwtest_one(return_value,target_value[6]); 
                    Num_status=CheckCtSgwStatus(app_status,target_status[2]);
                    g_print("Num_value=%d,Num_status=%d\n",Num_value,Num_status);
                               
                    if(Num_value==1 && Num_status==1){
                     pass++;
                     write_xls(module_name[0],function_name[5],app_name,return_value,target_value[6],app_status.state,target_status[2],"uninstall ipk",result[0]);
                     
                     g_print("Pass\n");  
                    }         
                    else{
                     fail++;
                     write_xls(module_name[0],function_name[5],app_name,return_value,target_value[6],app_status.state,target_status[2],"uninstall ipk",result[1]);
                    
                     g_print("Fail=%d\n",fail);
                    }
                    g_print("\n");
             
                    /**********check the function*********/ 
                    g_print("Num_array=%d,pass=%d\n",Num_array,pass);      
                    if(pass==Num_array){
                     PASS++;
                     g_print("Pass=%d\n",pass);
                     g_print("Process_test_one: Pass\n");
                     
                    }
                    else{
                     FAIL++;
                     g_print("Pass=%d,Fail=%d\n",pass,fail);
                     g_print("Process_test_one: Fail\n");
                     
                    }
                    g_print("#######End of the %s#######\n",__func__);                  
                    g_print("\n\n"); 

                    
                  
 return 0;  
   
}

	
/*******test install***********/
int  Process_test_install(){

             
                    //测试app:不存在的app,不完整的app，接口不存在的app,占用空间很大的app，
                    //apk格式的app,目录内容错误的app，首次安装正常app，再次安装正常app
                    g_print("###%s is begining###\n",__func__);   
                 
#ifdef Realtek         
             char *app_install[] = { "/tmp/appmgr/applist/app_install/abnormal/tsappno_1_rtl9600.ipk",                                           
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappcrack_1_rtl9600.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsappinterface_1_rtl9600.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsapplarge_1_rtl9600.ipk",
                                                        							                    
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappapk_1_rtl9600.apk", 
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappwrong_1_rtl9600.ipk",    
                                     "/tmp/appmgr/applist/app_install/normal/install_1/tsappa_1_rtl9600.ipk",
                                     "/tmp/appmgr/applist/app_install/normal/install_2/tsappa_1_rtl9600.ipk",
                              
                                     };  
#elif Broadcom
             char *app_install[] = { "/tmp/appmgr/applist/app_install/abnormal/tsappno_1_bcm6838.ipk",                                           
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappcrack_1_bcm6838.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsappinterface_1_bcm6838.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsapplarge_1_bcm6838.ipk",
                                                        							                    
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappapk_1_bcm6838.apk", 
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappwrong_1_bcm6838.ipk",    
                                     "/tmp/appmgr/applist/app_install/normal/install_1/tsappa_1_bcm6838.ipk",
                                     "/tmp/appmgr/applist/app_install/normal/install_2/tsappa_1_bcm6838.ipk",                                    
                                  
                                     };      
 
#elif Huawei

             char *app_install[] = { "/tmp/appmgr/applist/app_install/abnormal/tsappno_1_sd5115.ipk",                                           
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappcrack_1_sd5115.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsappinterface_1_sd5115.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsapplarge_1_sd5115.ipk",
                                                        							                    
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappapk_1_sd5115.apk", 
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappwrong_1_sd5115.ipk",    
                                     "/tmp/appmgr/applist/app_install/normal/install_1/tsappa_1_sd5115.ipk",
                                     "/tmp/appmgr/applist/app_install/normal/install_2/tsappa_1_sd5115.ipk",
                             
                                     };  
             
#elif Zhongxing
		// TODO:

             char *app_install[] = { "/tmp/appmgr/applist/app_install/abnormal/tsappno_1_zx279100.ipk",                                           
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappcrack_1_zx279100.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsappinterface_1_zx279100.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsapplarge_1_zx279100.ipk",                            							                    
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappapk_1_zx279100.apk", 
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappwrong_1_zx279100.ipk",    
                                     "/tmp/appmgr/applist/app_install/normal/install_1/tsappa_1_zx279100.ipk",
                                     "/tmp/appmgr/applist/app_install/normal/install_2/tsappa_1_zx279100.ipk",
                             
                                     };  
             
#elif MTK
		// TODO:

             char *app_install[] = { "/tmp/appmgr/applist/app_install/abnormal/tsappno_1_mt7526.ipk",                                           
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappcrack_1_mt7526.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsappinterface_1_mt7526.ipk",
				     "/tmp/appmgr/applist/app_install/abnormal/tsapplarge_1_mt7526.ipk",
                                                        							                    
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappapk_1_mt7526.apk", 
                                     "/tmp/appmgr/applist/app_install/abnormal/tsappwrong_1_mt7526.ipk",    
                                     "/tmp/appmgr/applist/app_install/normal/install_1/tsappa_1_mt7526.ipk",
                                     "/tmp/appmgr/applist/app_install/normal/install_2/tsappa_1_mt7526.ipk",
                             
                                     };  
             
#endif
                
           		   
                    char *app_name[] = {"tsappno","tsappcrack","tsappinterface","tsapplarge",
                                        "tsappapk","tsappwrong","tsappa","tsappa"};
                    //int target_value[]={-12,-12,-17,-4,-12,-12,0,-11};
                    int target_value[]={AMRET_APP_NOT_EXIST,AMRET_APP_NOT_EXIST,AMRET_APP_INSTALL_FAILED,AMRET_APP_NO_SPACE,AMRET_APP_NOT_EXIST,AMRET_APP_NOT_EXIST,AMRET_SUCCESS,AMRET_APP_INSTALLED};
                    int return_install=-1;       
                    int return_uninstall=-1;               
                    int Num_value=0;
                    int Num_array=0;
                   
                    Num_array=sizeof(app_install)/sizeof(char*);

                    g_print("app_insall=%d,char*=%d,Num_array=%d\n",sizeof(app_install),sizeof(char*),Num_array);
                    applist();                   
                    for(i=0;i<Num_array;i++)
                    {
                      if(i>0) // no ipk
                      {
                          if (access((app_install+i), 0) != 0)
                          {
                              g_print("%s %s is not exist!\n",__func__,*(app_install+i));
                          }
                      }
//liuxy
                      return_install = CtSgwInstallApp(*(app_install+i));        
                      g_print("%s case%d: %s return_install is: %d\n",__func__, i+1,*(app_install+i),return_install);
                      Num_value = CheckCtSgwInstallApp_t(return_install,target_value[i]);  

                      if(return_install==target_value[i]){  
                           write_xls(module_name[1],function_name[0],app_name[i],return_install,target_value[i],NULL,NULL,"install ipk",result[0]);   
                      }
                      else write_xls(module_name[1],function_name[0],app_name[i],return_install,target_value[i],NULL,NULL,"install ipk",result[1]);

                    }  
                    sleep(2);                
                    applist();  
                    g_print("Num_value=%d\n",Num_value);
                    
              
                    /**********check the function**********/
                    if(Num_value==Num_array){
                      g_print("Pass:%d\n",Num_value);
                      g_print("All install test: Pass\n");
                      g_print("\n");
                    }
                    else{
                      g_print("Fail:%d\n",Num_array-Num_value);
                      g_print("All install test:Fail\n");
                      g_print("\n");
                    }                     
                    
                    /*******Uninstall all the app after installing**/
                    for(i=0;i<Num_array;i++){
                      return_uninstall = CtSgwUninstallApp(app_name[i]);
                      g_print("Uninstall %s return_value is: %d\n",app_name[i],return_uninstall);
                    }
                    applist();                		
                    g_print("#######End of the %s#######\n",__func__);                  
               
                        
                      
}   
   

//target_value
int Process_test_upgrade()
{                  
                  
                   //接口不存在的更新包，不完整的更新包，占用空间很大的更新包，
                   //正常更新包，再次更新
                   g_print("###%s is begining###\n",__func__);   
#ifdef Realtek
                   char *app_install[] = { "/tmp/appmgr/applist/app_install/normal/tsappcinterface_1_rtl9600.ipk",
                                           "/tmp/appmgr/applist/app_install/normal/tsappccrack_1_rtl9600.ipk",
                                           "/tmp/appmgr/applist/app_install/normal/tsappclarge_1_rtl9600.ipk",     
                                           "/tmp/appmgr/applist/app_install/normal/taappcwrong_1_rtk9600.ipk", 
                                           "/tmp/appmgr/applist/app_install/normal/tsappc_1_rtl9600.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappcup_1_rtl9600.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappjup_1_rtl9600.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappkup_1_rtl9600.ipk",
                                          };

                   char *app_grade[]  =  { "/tmp/appmgr/applist/app_upgrade/tsappcinterface_1_rtl9600.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappccrack_1_rtl9600.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappclarge_1_rtl9600.ipk",     
                                           "/tmp/appmgr/applist/app_upgrade/taappcwrong_1_rtk9600.ipk",                                       
                                           "/tmp/appmgr/applist/app_upgrade/grade_1/tsappc_1_rtl9600.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/grade_2/tsappc_1_rtl9600.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappcup_1_rtl9600.ipk",   
                                           "/tmp/appmgr/applist/app_upgrade/tsappjup_1_rtl9600.ipk",  
                                           "/tmp/appmgr/applist/app_upgrade/tsappkup_1_rtl9600.ipk",                    
                                                                           
                                           };
#elif Broadcom   
                  char *app_install[] = {  "/tmp/appmgr/applist/app_install/normal/tsappcinterface_1_bcm6838.ipk",
                                           "/tmp/appmgr/applist/app_install/normal/tsappccrack_1_bcm6838.ipk",
                                           "/tmp/appmgr/applist/app_install/normal/tsappclarge_1_bcm6838.ipk",     
                                           "/tmp/appmgr/applist/app_install/normal/taappcwrong_1_bcm6838.ipk",  
                                           "/tmp/appmgr/applist/app_install/normal/tsappc_1_bcm6838.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappcup_1_bcm6838.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappjup_1_bcm6838.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappkup_1_bcm6838.ipk",
                                          };

                  char *app_grade[]  =  { "/tmp/appmgr/applist/app_upgrade/tsappcinterface_1_bcm6838.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappccrack_1_bcm6838.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappclarge_1_bcm6838.ipk",     
                                           "/tmp/appmgr/applist/app_upgrade/taappcwrong_1_bcm6838.ipk",                                       
                                           "/tmp/appmgr/applist/app_upgrade/grade_1/tsappc_1_bcm6838.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/grade_2/tsappc_1_bcm6838.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappcup_1_bcm6838.ipk",   
                                           "/tmp/appmgr/applist/app_upgrade/tsappjup_1_bcm6838.ipk",  
                                           "/tmp/appmgr/applist/app_upgrade/tsappkup_1_bcm6838.ipk",                    
                                                                           
                                           };

#elif Huawei
                char *app_install[] = {    "/tmp/appmgr/applist/app_install/normal/tsappcinterface_1_sd5115.ipk",
                                           "/tmp/appmgr/applist/app_install/normal/tsappccrack_1_sd5115.ipk",
                                           "/tmp/appmgr/applist/app_install/normal/tsappclarge_1_sd5115.ipk",     
                                           "/tmp/appmgr/applist/app_install/normal/taappcwrong_1_sd5115.ipk",  
                                           "/tmp/appmgr/applist/app_install/normal/tsappc_1_sd5115.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappcup_1_sd5115.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappjup_1_sd5115.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappkup_1_sd5115.ipk",
                                          };

                 char *app_grade[]  =  {   "/tmp/appmgr/applist/app_upgrade/tsappcinterface_1_sd5115.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappccrack_1_sd5115.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappclarge_1_sd5115.ipk",     
                                           "/tmp/appmgr/applist/app_upgrade/taappcwrong_1_sd5115.ipk",                                       
                                           "/tmp/appmgr/applist/app_upgrade/grade_1/tsappc_1_sd5115.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/grade_2/tsappc_1_sd5115.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappcup_1_sd5115.ipk",   
                                           "/tmp/appmgr/applist/app_upgrade/tsappjup_1_sd5115.ipk",  
                                           "/tmp/appmgr/applist/app_upgrade/tsappkup_1_sd5115.ipk",                    
                                                                           
                                           };
#elif Zhongxing
		// TODO:
                char *app_install[] = {   "/tmp/appmgr/applist/app_install/normal/tsappcinterface_1_zx279100.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappccrack_1_zx279100.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappclarge_1_zx279100.ipk",     
                                          "/tmp/appmgr/applist/app_install/normal/taappcwrong_1_zx279100.ipk",  
                                          "/tmp/appmgr/applist/app_install/normal/tsappc_1_zx279100.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappcup_1_zx279100.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappjup_1_zx279100.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappkup_1_zx279100.ipk",
                                          };

                 char *app_grade[]  =  {   "/tmp/appmgr/applist/app_upgrade/tsappcinterface_1_zx279100.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappccrack_1_zx279100.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappclarge_1_zx279100.ipk",     
                                           "/tmp/appmgr/applist/app_upgrade/taappcwrong_1_zx279100.ipk",                                       
                                           "/tmp/appmgr/applist/app_upgrade/grade_1/tsappc_1_zx279100.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/grade_2/tsappc_1_zx279100.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappcup_1_zx279100.ipk",   
                                           "/tmp/appmgr/applist/app_upgrade/tsappjup_1_zx279100.ipk",  
                                           "/tmp/appmgr/applist/app_upgrade/tsappkup_1_zx279100.ipk",                    
                                                                           
                                           };
#elif MTK
                char *app_install[] = {   "/tmp/appmgr/applist/app_install/normal/tsappcinterface_1_mt7526.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappccrack_1_mt7526.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappclarge_1_mt7526.ipk",     
                                          "/tmp/appmgr/applist/app_install/normal/taappcwrong_1_mt7526.ipk",  
                                          "/tmp/appmgr/applist/app_install/normal/tsappc_1_zx279100.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappcup_1_mt7526.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappjup_1_mt7526.ipk",
                                          "/tmp/appmgr/applist/app_install/normal/tsappkup_1_mt7526.ipk",
                                          };

                 char *app_grade[]  =  {   "/tmp/appmgr/applist/app_upgrade/tsappcinterface_1_mt7526.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappccrack_1_mt7526.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappclarge_1_mt7526.ipk",     
                                           "/tmp/appmgr/applist/app_upgrade/taappcwrong_1_mt7526.ipk",                                       
                                           "/tmp/appmgr/applist/app_upgrade/grade_1/tsappc_1_mt7526.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/grade_2/tsappc_1_mt7526.ipk",
                                           "/tmp/appmgr/applist/app_upgrade/tsappcup_1_mt7526.ipk",   
                                           "/tmp/appmgr/applist/app_upgrade/tsappjup_1_mt7526.ipk",  
                                           "/tmp/appmgr/applist/app_upgrade/tsappkup_1_mt7526.ipk",                    
                                                                           
                                           };
#endif

                   char *app_name[]={"tsappcinterface","tsappccrack","tsappclarge","taappcwrong","tsappc","tsappc","tsappcup","tsappjup","tsappkup"};  

                   //int  target_value[]={-17,-12,-4,-12,0,0,-17,-17,-17};                  
                   //int  target_value[]={AMRET_APP_INSTALL_FAILED,AMRET_APP_NOT_EXIST,AMRET_APP_NO_SPACE,AMRET_APP_NOT_EXIST,AMRET_SUCCESS,AMRET_SUCCESS,AMRET_APP_INSTALL_FAILED,AMRET_APP_INSTALL_FAILED,AMRET_APP_INSTALL_FAILED};                  
                   int  target_value[]={AMRET_APP_INSTALL_FAILED,AMRET_APP_NOT_EXIST,AMRET_APP_NO_SPACE,AMRET_APP_NOT_EXIST,AMRET_SUCCESS,AMRET_SUCCESS,AMRET_SUCCESS, AMRET_SUCCESS, AMRET_APP_INSTALL_FAILED};                  
                   int  return_install=-1;
                   int  return_grade=-1; 
                   int  Num_value=-1,Num_array=0;      
                   int pass=0,fail=0;
                   Num_array=sizeof(app_grade)/sizeof(char*);    
                   g_printf("app_grade=%d,char*=%d,Num_array=%d\n",sizeof(app_grade),sizeof(char*),Num_array);      
                  
                   /*******install the app at first*******/   
                    applist();
                    //for(i=0;i<3;i++){                
                    for(i=0;i<(3+5);i++){                
                      return_install = CtSgwInstallApp(app_install[i]);
                      g_print("%s return_install is: %d\n",app_name[i],return_install);  
                    
                    } 
                    applist();       
                                
                   /******upgrade the app at last**********/        
                   /*for(i=0;i<Num_array-3;i++)
                    {
                       return_grade = CtSgwUpgradeApp(*(app_grade+i));                   
                       g_print("%s case%d: %s return_grade is: %d\n",__func__,i+1,app_name,return_grade);                       
                       
                       Num_value=CheckCtSgwtest_one(return_grade,target_value[i]);
                       if(Num_value==1){
                         pass++;
                         g_print("Pass\n");
                         write_xls(module_name[2],function_name[1],app_name[0],return_grade,target_value[i],NULL,NULL,"upgrade ipk",result[0]); 
                         
                       }
                       else{
                         fail++;
                         g_print("Fail\n");
                         write_xls(module_name[2],function_name[1],app_name[0],return_grade,target_value[i],NULL,NULL,"upgrade ipk",result[1]); 
                         
                       }
                     
                    } 

                   for(i=0;i<3;i++){
 
                       return_grade = CtSgwUpgradeApp(*(app_grade+i+6));                   
                       g_print("%s case%d: %s return_grade is: %d\n",__func__, i+7,app_name[i],return_grade);                     
                       
                       Num_value=CheckCtSgwtest_one(return_grade,target_value[i+6]);
                       if(Num_value==1){
                         pass++;
                         g_print("Pass\n");
                         write_xls(module_name[2],function_name[1],app_name[i],return_grade,target_value[i+6],NULL,NULL,"upgrade ipk",result[0]); 
                         
                       }
                       else{
                         fail++;
                         g_print("Fail\n");
                         write_xls(module_name[2],function_name[1],app_name[i],return_grade,target_value[i+6],NULL,NULL,"upgrade ipk",result[1]); 
                         
                       }  

                    }*/
                    for(i=0;i<Num_array;i++)
                    {
                          if (access((app_grade+i), 0) != 0)
                          {
                              g_print("%s %s is not exist!\n",__func__,*(app_grade+i));
                          }
                       return_grade = CtSgwUpgradeApp(*(app_grade+i));                   
                       g_print("%s case%d: %s return_grade is: %d\n",__func__,i+1,app_name[i],return_grade);                       
                       
                       Num_value=CheckCtSgwtest_one(return_grade,target_value[i]);
                       if(Num_value==1){
                         pass++;
                         g_print("Pass\n");
                         write_xls(module_name[2],function_name[1],app_name[i],return_grade,target_value[i],NULL,NULL,"upgrade ipk",result[0]);
                       }
                       else{
                         fail++;
                         g_print("Fail\n");
                         write_xls(module_name[2],function_name[1],app_name[i],return_grade,target_value[i],NULL,NULL,"upgrade ipk",result[1]); 
                       }
                     
                    }
                    g_print("\n");

                    /******check the function***********/
                    if(pass==Num_array && fail!=Num_array){
                        g_print("Pass=%d\n",pass);
                        g_print("Process_test_upgrade: Pass\n");
                    }
                    else{
                        g_print("Pass=%d,Fail=%d\n",pass,fail);
                        g_print("Process_test_upgrade: Fail\n");   
                    }                    

                    for(i=0;i<Num_array;i++){
                      CtSgwUninstallApp(*(app_name+i));
                    }
                    applist();                       
                    g_print("#######End of the %s#######\n",__func__);                  
                    g_print ("\n");
}
              
 
int Process_test_start(){
                    
                   //运行不存在的app，运行安装后的app，再次运行app

                   g_print("###%s is begining###\n",__func__);   
#ifdef Realtek         
                               
        char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappb_1_rtl9600.ipk";   
  
#elif Broadcom  

        char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappb_1_bcm6838.ipk";
#elif Huawei

        char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappb_1_sd5115.ipk";

#elif Zhongxing

	// TODO:
        char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappb_1_zx279100.ipk";
#elif MTK

        char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappb_1_mt7526.ipk";
#endif        
                   char *app_name[] = {"tsappb","tsappb"};                 
                   int return_start=-1;       
                   int return_install=-1;

                   //int target_value[3]={-12,0,-13};
                   int target_value[3]={AMRET_APP_NOT_EXIST,AMRET_SUCCESS,AMRET_APP_IS_RUNNING};
                   int Num_value=-1;
                   int Num_status=-1,Num_array=0;
                   static int pass=0,fail=0;
                   Num_array=sizeof(target_value)/sizeof(int);
                    
                   /*******start the app before installing*/                 
                   applist();
                   return_start =  CtSgwStartApp(app_name[0]);
                   g_print("%s case1: %s return_start is: %d\n",__func__,app_name[0],return_start);
                      
                   strcpy (app_status.name, app_name[0]);
                   CtSgwGetAppStatus(&app_status);
                   g_print ("%s status is: %s\n",app_name[0],app_status.state);            

                   Num_value=CheckCtSgwtest_one(return_start,target_value[0]); 
                   Num_status=CheckCtSgwStatus(app_status,target_status[2]);
                   g_print("Num_value=%d,Num_status=%d\n",Num_value,Num_status);
                   if(Num_value==1 && Num_status==1){
                    pass++;
                    write_xls(module_name[3],function_name[2],app_name[0],return_start,target_value[0],app_status.state,target_status[2],"start ipk",result[0]);
                    g_print("Pass\n");           
                   }
                   else{
                    fail++;
                    write_xls(module_name[3],function_name[2],app_name[0],return_start,target_value[0],app_status.state,target_status[2],"start ipk",result[1]);
                    g_print("Fail\n");
                   }                  
                   /*******start the app after installing*******/                 
                   return_install = CtSgwInstallApp(app_install);
                   g_print("%s return_install is: %d\n",app_install,return_install);                  
                   applist();                            
                      
                   for(i=0;i<2;i++)
                    {
                      return_start = CtSgwStartApp(*(app_name+i));              // all the app's return value
                      g_print("%s case%d: %s return_start is: %d\n",__func__, i+2,*(app_name+i),return_start);

                      strcpy (app_status.name, *(app_name+i));
                      CtSgwGetAppStatus(&app_status);
                      g_print ("%s status is: %s\n",*(app_name+i),app_status.state);
                     
                      Num_value=CheckCtSgwtest_one(return_start,target_value[i+1]);
                      Num_status=CheckCtSgwStatus(app_status,target_status[0]);
                      if(Num_value==1 && Num_status==1){   
                         pass++;                   
                         write_xls(module_name[3],function_name[2],app_name[0],return_start,target_value[i+1],app_status.state,target_status[0],"start ipk",result[0]);
                         g_print("Pass\n");           
                      }
                      else{
                         fail++;
                         write_xls(module_name[3],function_name[2],app_name[0],return_start,target_value[i+1],app_status.state,target_status[0],"start ipk",result[1]);
                         g_print("Fail\n");
                      }                  
                      
                    }                                     
                   g_print("\n");

                   /************check the function************/         
                   if(pass==Num_array && fail!=Num_array){
                        g_print("Pass=%d\n",pass);
                        g_print("Process_test_start: Pass\n");
                   }
                   else{
                        g_print("Pass=%d,Fail=%d\n",pass,fail);
                        g_print("Process_test_start: Fail\n");
                   }
                   /*****Unistalling all the app after stating**/
                    CtSgwUninstallApp(app_name[0]);
                    applist(); 
                    g_print("#######End of the %s#######\n",__func__);                  
                    g_print("\n");           
                    


}                       
          
int Process_test_stop(){

                  //停止未存在的app,停止未运行的app,停止已运行的app

                  g_print("###%s is begining###\n",__func__);   
#ifdef Realtek                 
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappd_1_rtl9600.ipk";
#elif Broadcom
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappd_1_bcm6838.ipk";
#elif Huawei
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappd_1_sd5115.ipk";
#elif Zhongxing
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappd_1_zx279100.ipk";
#elif MTK
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappd_1_mt7526.ipk";

#endif  
                  char app_name[] = "tsappd";                  

                  //int target_value[]={-12,-16,0,-16};
                  int target_value[]={AMRET_APP_NOT_EXIST,AMRET_APP_NOT_RUNNING,AMRET_SUCCESS,AMRET_APP_NOT_RUNNING};
                  int return_install=-1;
                  int return_start=-1;
                  int return_stop=-1;
                  int return_reload=-1;
                  static int pass=0,fail=0;
                  int Num_value=-1,Num_status=-1,Num_array=0;
                  Num_array=sizeof(target_value)/sizeof(int);
                  
                  /********stop the app without installing****/
                  applist();                                
                  return_stop = CtSgwStopApp(app_name);
                  g_print("%s case1: %s return_stop is: %d\n",__func__, app_name,return_stop);

                  strcpy (app_status.name, app_name);
                  CtSgwGetAppStatus(&app_status);
                  g_print("%s status is: %s\n",app_name,app_status.state);                 

                  Num_value=CheckCtSgwtest_one(return_stop,target_value[0]);
                  Num_status=CheckCtSgwStatus(app_status,target_status[2]);
                  if(Num_value==1 && Num_status==1){
                    pass++;
                    write_xls(module_name[4],function_name[3],app_name,return_stop,target_value[0],app_status.state,target_status[2],"stop ipk",result[0]);
                    g_print("Pass\n");
                  }
                  else{
                    fail++;
                    write_xls(module_name[4],function_name[3],app_name,return_stop,target_value[0],app_status.state,target_status[2],"stop ipk",result[1]);
                    g_print("Fail\n");
                  }

                  /*******stop the app without starting*******/   
                  return_install = CtSgwInstallApp(app_install);
                  g_print("%s return_install is: %d\n",app_name,return_install);                  
                  applist();
                  
                  return_stop = CtSgwStopApp(app_name);
                  g_print("%s case2: %s return_stop is: %d\n",__func__, app_name,return_stop);
                  sleep(5);
                  strcpy (app_status.name, app_name);
                  CtSgwGetAppStatus(&app_status);
                  g_print("%s status is: %s\n",app_name,app_status.state); 

                  Num_value=CheckCtSgwtest_one(return_stop,target_value[1]);
                  Num_status=CheckCtSgwStatus(app_status,target_status[1]);
                  if(Num_value==1 && Num_status==1){
                    pass++;
                    write_xls(module_name[4],function_name[3],app_name,return_stop,target_value[1],app_status.state,target_status[1],"stop ipk after install",result[0]);
                    g_print("Pass\n");
                  }
                  else{
                    fail++;
                    write_xls(module_name[4],function_name[3],app_name,return_stop,target_value[1],app_status.state,target_status[1],"stop ipk after install",result[1]);
                    g_print("Fail\n");
                  }
                                  

                  /********stop the app after starting********/
             
                  return_start = CtSgwStartApp(app_name);
                  g_print("%s return_start is: %d\n",app_name,return_start);
                  sleep(3);
                  strcpy (app_status.name, app_name);
                  CtSgwGetAppStatus(&app_status);
                  g_print("%s status is: %s\n",app_name,app_status.state);
                  sleep(3);           

                  return_stop = CtSgwStopApp(app_name);
                  g_print("%s case3: %s return_stop is: %d\n",__func__, app_name,return_stop);
                  sleep(3);
                  strcpy (app_status.name, app_name);
                  CtSgwGetAppStatus(&app_status);
                  g_print("%s status is: %s\n",app_name,app_status.state); 

                  Num_value=CheckCtSgwtest_one(return_stop,target_value[2]);
                  Num_status=CheckCtSgwStatus(app_status,target_status[1]);
                  if(Num_value==1 && Num_status==1){
                    pass++;
                    write_xls(module_name[4],function_name[3],app_name,return_stop,target_value[2],app_status.state,target_status[1],"stop ipk",result[0]);
                    g_print("Pass\n");
                  }
                  else{
                    fail++;
                    write_xls(module_name[4],function_name[3],app_name,return_stop,target_value[2],app_status.state,target_status[1],"stop ipk",result[1]);
                    g_print("Fail\n");
                  }
                  

                  /*********stop the app after stopping******/
                  return_stop = CtSgwStopApp(app_name);
                  g_print("%s case4: %s return_stop is: %d\n",__func__, app_name,return_stop);
                  sleep(5);

                  strcpy (app_status.name, app_name);
                  CtSgwGetAppStatus(&app_status);
                  g_print("%s status is: %s\n",app_name,app_status.state); 
                  
                  Num_value=CheckCtSgwtest_one(return_stop,target_value[3]);
                  Num_status=CheckCtSgwStatus(app_status,target_status[1]);
                  if(Num_value==1 && Num_status==1){
                    pass++;
                    write_xls(module_name[4],function_name[3],app_name,return_stop,target_value[3],app_status.state,target_status[1],"stop ipk",result[0]);
                    g_print("Pass\n");
                  }
                  else{
                    fail++;
                    write_xls(module_name[4],function_name[3],app_name,return_stop,target_value[3],app_status.state,target_status[1],"stop ipk",result[1]);
                    g_print("Fail\n");
                  }
                  g_print("\n");
                  
                  /*********check the funcion**************/
                  if(pass==Num_array && fail!=Num_array){
                    g_print("Pass=%d\n",pass);
                    g_print("Process_test_stop: Pass\n");
                  }
                  else{
                    g_print("Pass=%d,Fail=%d\n",pass,fail);
                    g_print("Process_test_stop: Fail\n");
                  }
                
                  CtSgwUninstallApp(app_name);
                  applist();

                  g_print("#######End of the %s#######\n",__func__);                  
                  g_print ("\n");      
                  
              

}

//return_value, status.state
int Process_test_reload(){
                 
                 //重载未存在的app,重载未运行的app,重载运行后的app
                 //重载停止后的app
                 g_print("###%s is begining###\n",__func__);   
#ifdef Realtek 
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappe_1_rtl9600.ipk";  
#elif Broadcom
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappe_1_bcm6838.ipk";
#elif Huawei
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappe_1_sd5115.ipk";
#elif MTK
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappe_1_mt7526.ipk";
#elif Zhongxing
          char app_install[] = "/tmp/appmgr/applist/app_install/normal/tsappe_1_zx279100.ipk";
#endif
                 char app_name[] = "tsappe";

                 //int  target_value[]={-12,-16,0,-16};
                 int  target_value[]={AMRET_APP_NOT_EXIST,AMRET_APP_NOT_RUNNING,AMRET_APP_NOT_RUNNING};
         
                 int return_install=-1;
                 int return_start=-1;
                 int return_stop=-1;
                 int return_reload=-1;

                 int Num_value=-1,Num_status=-1,Num_array=0;
                 static int pass=0,fail=0;
                 Num_array=sizeof(target_value)/sizeof(int);
                                
                 /********reload the app without installing********/  
                 applist();       
                 return_reload = CtSgwReloadApp(app_name);
                 g_print("%s case1: %s return_reload is: %d\n",__func__, app_name,return_reload);
                
                 strcpy (app_status.name, app_name);
                 CtSgwGetAppStatus(&app_status);
                 g_print("%s status is: %s\n",app_name,app_status.state); 
                 sleep(2);

                 Num_value=CheckCtSgwtest_one(return_reload,target_value[0]);
                 Num_status=CheckCtSgwStatus(app_status,target_status[2]);
                 if(Num_value==1 && Num_status==1){
                    pass++; 
                    write_xls(module_name[5],function_name[4],app_name,return_reload,target_value[0],app_status.state,target_status[2],"reload no install ipk",result[0]);
                    g_print("Pass\n");
                 }
                 else{
                    fail++;
                    write_xls(module_name[5],function_name[4],app_name,return_reload,target_value[0],app_status.state,target_status[2],"reload no install ipk",result[1]);
                    g_print("Fail\n");
                 }
                 /********reload the app after installing ********/ 
                 return_install = CtSgwInstallApp(app_install);
                 g_print("%s return_install is: %d\n",app_name,return_install);
                 applist();
                 
                 return_reload = CtSgwReloadApp(app_name);
                 g_print("%s case2: %s return_reload is: %d\n",__func__, app_name,return_reload);

                 strcpy (app_status.name,app_name);
                 CtSgwGetAppStatus(&app_status);
                 g_print("%s status is: %s\n",app_name,app_status.state); 

                 Num_value=CheckCtSgwtest_one(return_reload,target_value[1]);
                 Num_status=CheckCtSgwStatus(app_status,target_status[1]);
                 if(Num_value==1 && Num_status==1){
                    pass++; 
                    write_xls(module_name[5],function_name[4],app_name,return_reload,target_value[1],app_status.state,target_status[1],"reload ipk after install", result[0]);
                    g_print("Pass\n");
                 }
                 else{
                    fail++;
                    write_xls(module_name[5],function_name[4],app_name,return_reload,target_value[1],app_status.state,target_status[1],"reload ipk after install", result[1]);
                    g_print("Fail\n");
                 }

                    /********reload the app after stopping the app********/ 
                 return_start = CtSgwStartApp(app_name);
                 g_print("%s return_start is: %d\n",app_name,return_start);
                 sleep(5);
                 strcpy (app_status.name,app_name);
                 CtSgwGetAppStatus(&app_status);
                 g_print("%s status is: %s\n",app_name,app_status.state);  

                 return_stop = CtSgwStopApp(app_name);
                 g_print("%s return_stop is: %d\n",app_name,return_stop);
                 sleep(10);

                 strcpy(app_status.name,app_name);
                 CtSgwGetAppStatus(&app_status);
                 g_print("%s status is: %s\n",app_name,app_status.state);
          
                 return_reload = CtSgwReloadApp(app_name);
                 g_print("%s case3: %s return_reload is: %d\n",__func__, app_name,return_reload);

                 strcpy(app_status.name,app_name);
                 CtSgwGetAppStatus(&app_status);
                 g_print("%s status is: %s\n",app_name,app_status.state); 
             
                 Num_value=CheckCtSgwtest_one(return_reload,target_value[2]);
                 Num_status=CheckCtSgwStatus(app_status,target_status[1]);
                 if(Num_value==1 && Num_status==1){
                    pass++; 
                    write_xls(module_name[5],function_name[4],app_name,return_reload,target_value[2],app_status.state,target_status[1],"reload ipk after stop",result[0]);
                    g_print("Pass\n");
                 }
                 else{
                    fail++;
                    write_xls(module_name[5],function_name[4],app_name,return_reload,target_value[2],app_status.state,target_status[1],"reload ipk after stop",result[1]);
                    g_print("Fail\n");
                 }   
                 g_print("\n"); 
                 /********check the function*****************/
                 if(pass==Num_array && fail!=Num_array){
                    g_print("Pass=%d\n",pass);
                    g_print("Process_test_reload: Pass\n");
                 }
                 else{
                    g_print("Pass=%d,Fail=%d\n",pass,fail);
                    g_print("Process_test_reload: Fail\n");
                 }
                 CtSgwUninstallApp(app_name);
                 applist();
                                      
                 g_print("#######End of the %s#######\n",__func__);                  
                 g_print ("\n");                            


}

//return_value                          
int Process_test_uninstall()
{

               //卸载不存在的app,卸载被其他app依赖的app,
               //卸载运行中的app,卸载无依赖的app

               g_print("###%s is begining###\n",__func__);   
#ifdef Realtek                             
        char *app_install[ ]={"/tmp/appmgr/applist/app_install/normal/tsappg_1_rtl9600.ipk",  
                              "/tmp/appmgr/applist/app_install/normal/tsappf_1_rtl9600.ipk",                                  
                              "/tmp/appmgr/applist/app_install/normal/tsappdependf_1_rtl9600.ipk"};
        
#elif Broadcom
        char *app_install[ ]={"/tmp/appmgr/applist/app_install/normal/tsappg_1_bcm6838.ipk",  
                              "/tmp/appmgr/applist/app_install/normal/tsappf_1_bcm6838.ipk",                                  
                              "/tmp/appmgr/applist/app_install/normal/tsappdependf_1_bcm6838.ipk"};
        
#elif Huawei
        char *app_install[ ]={"/tmp/appmgr/applist/app_install/normal/tsappg_1_sd5115.ipk",  
                              "/tmp/appmgr/applist/app_install/normal/tsappf_1_sd5115.ipk",                                  
                              "/tmp/appmgr/applist/app_install/normal/tsappdependf_1_sd5115.ipk"};  
#elif MTK
        char *app_install[ ]={"/tmp/appmgr/applist/app_install/normal/tsappg_1_mt7526.ipk",  
                              "/tmp/appmgr/applist/app_install/normal/tsappf_1_mt7526.ipk",                                  
                              "/tmp/appmgr/applist/app_install/normal/tsappdependf_1_mt7526.ipk"};  
#elif Zhongxing
        char *app_install[ ]={"/tmp/appmgr/applist/app_install/normal/tsappg_1_zx279100.ipk",  
                              "/tmp/appmgr/applist/app_install/normal/tsappf_1_zx279100.ipk",                                  
                              "/tmp/appmgr/applist/app_install/normal/tsappdependf_1_zx279100.ipk"};  
#endif
               char *app_name[]={"tsappg","tsappf","tsappdependf"};
               int return_install=-1;
               int return_uninstall=-1;
	       int return_start=-1;


               //int target_value[]={-12,-15,0,0,0};
               int target_value[]={AMRET_APP_NOT_EXIST,AMRET_APP_UNINSTALL_NOT_ALLOWED,AMRET_SUCCESS,AMRET_SUCCESS,AMRET_SUCCESS};
               int Num_value=-1,Num_status=-1,Num_array=0;
               static int pass=0,fail=0;
               Num_array=sizeof(target_value)/sizeof(int);
             
               /*************uninstall the tsappg without installing**********/ 
               applist();               
               return_uninstall = CtSgwUninstallApp(app_name[0]);        
               g_print("%s case1: %s return_uninstall is: %d\n",__func__, app_name[0],return_uninstall);
               applist(); 

               Num_value=CheckCtSgwtest_one(return_uninstall,target_value[0]);
               //Num_status=CheckCtSgwStatus(app_status.state,target_status[2]);
               if(Num_value==1){
                 pass++;
                 write_xls(module_name[6],function_name[5],app_name[0],return_uninstall,target_value[0],NULL,NULL,"uninstall ipk before install",result[0]);
                 g_print("Pass\n"); 
               }
               else{
                 fail++;
                 write_xls(module_name[6],function_name[5],app_name[0],return_uninstall,target_value[0],NULL,NULL,"uninstall ipk before install",result[1]);
                 g_print("Fail\n");
               }
               /************Innstall all the app***************/
               for(i=0;i<3;i++)
                {                  
                   return_install = CtSgwInstallApp(app_install[i]);
		   g_print("%s return_install is: %d\n",app_name[i],return_install);
                  
                }  
               applist();
               /************Uninstall the tsappf**********/
   	       return_uninstall = CtSgwUninstallApp(*(app_name+1));
               g_print("%s case2: %s return_uninstall is: %d\n",__func__, *(app_name+1),return_uninstall);                   
               applist(); 

               Num_value=CheckCtSgwtest_one(return_uninstall,target_value[1]);
               //Num_status=CheckCtSgwStatus(app_status.state,target_status[1]);
               if(Num_value==1){
                 pass++;
                 write_xls(module_name[6],function_name[5],app_name[1],return_uninstall,target_value[1],NULL,NULL,"uninstall ipk before undependent",result[0]);
                 g_print("Pass\n"); 
               }
               else{
                 fail++;
                 write_xls(module_name[6],function_name[5],app_name[1],return_uninstall,target_value[1],NULL,NULL,"uninstall ipk before undependent",result[1]);
                 g_print("Fail\n");
               }
               /************Uninstall the tsappdependf***************/
               return_uninstall = CtSgwUninstallApp(app_name[2]);        // all the app's return value
               g_print("%s case3: %s return_uninstall is: %d\n",__func__, app_name[2],return_uninstall);
               applist(); 

               Num_value=CheckCtSgwtest_one(return_uninstall,target_value[2]);
               //Num_status=CheckCtSgwStatus(app_status.state,target_status[1]);
               if(Num_value==1){
                 pass++;
                 write_xls(module_name[6],function_name[5],app_name[2],return_uninstall,target_value[2],NULL,NULL,"uninstall normal ipk",result[0]);
                 g_print("Pass\n"); 
               }
               else{
                 fail++;
                 write_xls(module_name[6],function_name[5],app_name[2],return_uninstall,target_value[2],NULL,NULL,"uninstall normal ipk",result[1]);
                 g_print("Fail\n");
               }

               /************Uninstall the tsappf **************/
               return_uninstall = CtSgwUninstallApp(app_name[1]);        // all the app's return value
               g_print("%s case4: %s return_uninstall is: %d\n",__func__, app_name[1],return_uninstall);
               applist();   

               Num_value=CheckCtSgwtest_one(return_uninstall,target_value[3]);
               //Num_status=CheckCtSgwStatus(app_status.state,target_status[1]);
               if(Num_value==1){
                 pass++;
                 write_xls(module_name[6],function_name[5],app_name[1],return_uninstall,target_value[3],NULL,NULL,"uninstall ipk after undependent",result[0]);
                 g_print("Pass\n"); 
               }
               else{
                 fail++;
                 write_xls(module_name[6],function_name[5],app_name[1],return_uninstall,target_value[3],NULL,NULL,"uninstall ipk after undependent",result[1]);
                 g_print("Fail\n");
               }              

               /***********Uninstall the tsappg after starting******/
               return_start = CtSgwStartApp(app_name[0]);
               g_print("%s return_start is: %d\n",app_name[0],return_start); 
               strcpy (app_status.name, app_name[0]);
               CtSgwGetAppStatus(&app_status);
               g_print("%s status is: %s\n",app_name[0],app_status.state);

               return_uninstall = CtSgwUninstallApp(app_name[0]);
               g_print("%s case5: %s return_uninstall is: %d\n",__func__, app_name[0],return_uninstall);                     
               applist();      

               Num_value=CheckCtSgwtest_one(return_uninstall,target_value[4]);
               //Num_status=CheckCtSgwStatus(app_status.state,target_status[1]);
               if(Num_value==1){
                 pass++;
                 write_xls(module_name[6],function_name[5],app_name[0],return_uninstall,target_value[4],app_status.state,target_status[0],"uninstall ipk after start",result[0]);
                 g_print("Pass\n"); 
               }
               else{
                 fail++;
                 write_xls(module_name[6],function_name[5],app_name[0],return_uninstall,target_value[4],app_status.state,target_status[0],"uninstall ipk after start",result[1]);
                 g_print("Fail\n");
               }            
               g_print("\n");
               /************check the function**************/
               if(pass==Num_array && fail!=Num_array){
                  g_print("Pass=%d\n",pass);
                  g_print("Process_test_uninstall: Pass\n");

               }
               else{
                  g_print("Pass=%d,Fail=%d\n",pass,fail);
                  g_print("Process_test_uninstall: Fail\n");
               }
               g_print("#######End of the %s#######\n",__func__);                  
               g_print ("\n");            
                      			
   
}                       
                              

int Process_test_UCI(){

              g_print("###%s is begining###\n",__func__);   
              g_print("\n");
              int return_uci=-1;
              int target_value=0;
              int return_start=-1;
              int return_install=-1;	
              int return_uninstall=-1;	
              int num=0;					
              int optsize=0;
           
              CtSgwUCIOpt_t *opts=NULL;
	      CtSgwUCIOpt_t *popt=NULL;            		
              CtSgwUCIOptVal_t option_val;             
#ifdef Realtek                      
       char app_install[]={"/tmp/appmgr/applist/app_install/normal/tsapph_1_rtl9600.ipk"};
#elif Broadcom
       char app_install[]={"/tmp/appmgr/applist/app_install/normal/tsapph_1_bcm6838.ipk"};
#elif Huawei
       char app_install[]={"/tmp/appmgr/applist/app_install/normal/tsapph_1_sd5115.ipk"};
#elif MTK
       char app_install[]={"/tmp/appmgr/applist/app_install/normal/tsapph_1_mt7526.ipk"};
#elif Zhongxing
       char app_install[]={"/tmp/appmgr/applist/app_install/normal/tsapph_1_zx279100.ipk"};
#endif
              char *app_name= "tsapph";              
             
              struct uci_write_param{
                             char pkg_name[32];                              
                             char section_name[32];                              
                             char option_name[32];                       
                             char option_value[32];                             
                           };             

             struct uci_read_section{
                             char pkg_name[32];			
                             char section_type[32];
                             char section_name[32];                              																
                          };             
             
             struct uci_read_option{
                             char pkg_name[32];                             
                             char section_name[32];
                             char option_name[32];                             
                           };
             	
             struct uci_write_param  uciwrite ={ "tsapph", "Man", "gender","33"};
             struct uci_read_section ucisection={"tsapph","Weather","secweather"};
             struct uci_read_option ucioption={"tsapph","Yellow", "shape"};
             char *opt_name[]={"day","date","weath"};
             char *opt_value[]={"Sunday","3.29","Cloudy"};
             /***************install tsapph****************/
             applist();
             return_install = CtSgwInstallApp(app_install);
             g_print("%s return_install is %d\n",app_name,return_install);
             applist();  
             /***************start  tsapph****************/  
             return_start = CtSgwStartApp(app_name);
             g_print("%s return_start is: %d\n",app_name,return_start);
             strcpy (app_status.name,app_name);
             CtSgwGetAppStatus(&app_status);
             g_print("%s status is: %s\n",app_name,app_status.state);    
        					  
             /**************WriteOption****************/
             return_uci=CtSgwUCIWriteOption(uciwrite.pkg_name, uciwrite.section_name, uciwrite.option_name, uciwrite.option_value);            		
             g_print("UCI_WriteOption: return_value= %d,target_value=%d\n",return_uci,target_value);
             if(return_uci==target_value){
               g_print("CtSgwUCIWriteOption: Pass\n");
               write_xls(module_name[7],function_name[6],app_name,return_uci,target_value,app_status.state,target_status[0],"write after start",result[0]);
               g_print("\n");
             }
             else{
               g_print("CtSgwUCIWriteOption: Fail\n");
               write_xls(module_name[7],function_name[6],app_name,return_uci,target_value,app_status.state,target_status[0],"write after start",result[1]);
               g_print("\n");
             }            
             /**************ReadSection*************/
             return_uci =CtSgwUCIReadSection(ucisection.pkg_name, ucisection.section_type, ucisection.section_name, &opts, &optsize);
             g_print("UCI_ReadSection: return_value is=%d,target_value=%d\n",return_uci,target_value);

	     g_print ("optsize = %d\n", optsize);
     	     popt = opts;           
	     for (i = 0; i < optsize; ++i)
	     {
             	g_print("\topts[%d] : %s = %s\n", i, popt->name, popt->value);
                if(return_uci==0 && (strcmp(popt->name,opt_name[i])==0) && (strcmp(popt->value,opt_value[i])==0)){
                  g_print("Pass\n");
                  num++;                  
                }
                else{
                  g_print("Fail\n");                  
                }
		++popt;    
	     }            
             if(num==optsize && optsize!=0){
                  g_print("CtSgwUCIReadSection: Pass\n");
                  write_xls(module_name[7],function_name[7],app_name,return_uci,target_value,app_status.state,target_status[0],"read section after start",result[0]);
                  g_print("\n");
             }        
             else{
                  g_print("CtSgwUCIReadSection: Fail\n");
                  write_xls(module_name[7],function_name[7],app_name,return_uci,target_value,app_status.state,target_status[0],"read section after start",result[1]);   
                  g_print("\n");
             } 
	     memset (option_val, 0, sizeof(option_val));

             /***********ReadOption******************/
             return_uci=CtSgwUCIReadOption(ucioption.pkg_name, ucioption.section_name, ucioption.option_name ,option_val);  
             g_print("UCI_ReadOption: return_value= %d,target_value=%d\n",return_uci,target_value);         
          
             g_print("CtSgwUCIReadOption: [%s] ==> [%s]\n",ucioption.option_name, option_val);
             if(strcmp(ucioption.option_name,"shape")==0 && strcmp(option_val,"round")==0){
               g_print("UCIReadOption: Pass\n");
               write_xls(module_name[7],function_name[8],app_name,return_uci,target_value,app_status.state,target_status[0],"read option after start",result[0]);
               g_print("\n");
             }
             else{
               g_print("UCIReadOption: Fail\n");
               write_xls(module_name[7],function_name[8],app_name,return_uci,target_value,app_status.state,target_status[0],"read option after start",result[1]);
               g_print("\n");
             }
 
             /***********stop and uninstall tsapph********/  
             CtSgwStopApp(app_name);
             sleep(3);             
	     return_uninstall = CtSgwUninstallApp(app_name);
	     g_print("%s return_uninstall is %d\n",app_name,return_uninstall);	
	     applist();
             g_print("#######End of the %s#######\n",__func__);                  
             g_print ("\n");            												
       
}                         
         		
 
           

int Process_test_log(){

        g_print("###%s is begining###\n",__func__);   
        #define LOGSIZE 200*1024
   	gchar *log1="LogOpen,this log should exist in system log";
	gchar *log2="LogClose,this log should not exist in system log";
	gchar *log3="test_appmgr write the log beign";
	gchar *log4="1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
        gchar *log5="log debug test";
        gchar *log6="log notice test";
        gchar *log7="log err test";

        int return_value[]={0,1};
        int target_value[]={0,1};
       	/*********Opent log test******/
	CtSgwLogOpen(LOG_USER, "test_appmgr ");
	CtSgwLog(LOG_USER,"%s",log1);    
        if(check_log_exist(log1)){
	 g_print("Open log test\n Pass\n");
         g_print("\n");
         write_xls(module_name[8],function_name[9],NULL,return_value[1],target_value[1],NULL,NULL,"open log",result[0]);	
	}else{
         write_xls(module_name[8],function_name[9],NULL,return_value[0],target_value[1],NULL,NULL,"open log",result[1]);	
	 g_print("OPen log test\n Fail\n");
         g_print("\n");
	}

        /**********Close log test****/
        CtSgwLogClose();        
   	CtSgwLog(LOG_NOTICE,"%s",log2);

        if(check_log_exist(log2)){
          write_xls(module_name[8],function_name[10],NULL,return_value[0],target_value[1],NULL,NULL,"close log",result[1]);	
	  g_print("Close log test\n Fail\n");
          g_print("\n");
	}else{
          write_xls(module_name[8],function_name[10],NULL,return_value[1],target_value[1],NULL,NULL,"close log",result[0]);
	  g_print("Close log test\n Pass\n");
          g_print("\n");
	}
       
        /**********Cover log test***/
   	CtSgwLogOpen(LOG_USER, "test_appmgr ");
  
	CtSgwLog(LOG_NOTICE,"%s",log3);//write log3
	if(check_log_exist(log3)){
	  g_print("ok,continue\n");    //read log3
	}else{
	  g_print("can't test\n");
	}
	int index=0;
	int len = strlen(log4);
        //g_print("Test log4 %d\n",len);
        CtSgwLog(LOG_NOTICE,"test_appmgr write the log begin:\n");
	for(index=0;index<LOGSIZE/len;index++){
	   CtSgwLog(LOG_NOTICE,"%s",log4);
           usleep(5);
	}
	CtSgwLog(LOG_NOTICE,"test_appmgr write the log end:\n");
	if(check_file_size("/opt/apps/ctsgw.log")>LOGSIZE){
          write_xls(module_name[8],function_name[11],NULL,return_value[0],target_value[1],NULL,NULL,"log too long",result[1]);
	  g_print("Cover log test: size too big\n Fail\n");
          g_print("\n");
      
	}
	if(check_log_exist(log3)){
          write_xls(module_name[8],function_name[11],NULL,return_value[0],target_value[1],NULL,NULL,"write log begin",result[1]);
	  g_print("Cover log test: the log do not overlay\n Fail\n");
          g_print("\n");
	}else{
          write_xls(module_name[8],function_name[11],NULL,return_value[1],target_value[1],NULL,NULL,"write log begin",result[0]);
	  g_print("Cover log test: the log overlay\n Pass\n");
          g_print("\n");
	}

        /******Level log test*******/
       /*****log_user test*****/
       if(check_log_exist(log5)){
         g_print("log_user has been exit,Stopping \n");

       }else{
         g_print("log_user test Continue\n");
       }
       for(index=0;index<10;index++)
       CtSgwLog(LOG_USER,"  %s ",log5);
       sleep(2);
       if(check_log_exist(log5)){
         write_xls(module_name[8],function_name[12],NULL,return_value[1],target_value[1],NULL,NULL,"log debug",result[0]);
         g_print("LOG_USER test \n Pass\n");
         g_print("\n");

       }else{
         write_xls(module_name[8],function_name[12],NULL,return_value[0],target_value[1],NULL,NULL,"log debug",result[1]);
         g_print("LOG_USER test \n Fail\n");
         g_print("\n");
       }

       /******log_notice test***/
      
       if(check_log_exist(log6)){

         g_print("log_notice has been exit,Stopping\n");

       }else{
         g_print("log_notice test continue\n");
       }
       for(index=0;index<10;index++)
       CtSgwLog(LOG_NOTICE,"%s  @@@",log6);
       sleep(2);
       if(check_log_exist(log6)){
         write_xls(module_name[8],function_name[13],NULL,return_value[1],target_value[1],NULL,NULL,"log notice",result[0]);
         g_print("LOG_NOTICE test \n Pass\n");
         g_print("\n");

       }else{
         write_xls(module_name[8],function_name[13],NULL,return_value[0],target_value[1],NULL,NULL,"log notice",result[1]);
         g_print("LOG_NOTICE test \n Fail\n");
         g_print("\n");
       }
      
       
       /******log_err test*****/
       if(check_log_exist(log7)){
         g_print("log_err has been exit,Stopping\n");

       }else{
         g_print("log_err test continue\n");
       }
       for(index=0;index<10;index++)
       CtSgwLog(LOG_ERR,"%s",log7);
       sleep(2);
       if(check_log_exist(log7)){
         write_xls(module_name[8],function_name[14],NULL,return_value[1],target_value[1],NULL,NULL,"log err",result[0]);
         g_print("LOG_ERR test\n Pass\n");
         g_print("\n");

       }else{
         write_xls(module_name[8],function_name[14],NULL,return_value[1],target_value[1],NULL,NULL,"log err",result[1]);
         g_print("LOG_ERR test\n Fail\n");
         g_print("\n");
       }					
       g_print("#######End of the %s#######\n",__func__);                  
       g_print ("\n\n"); 
                     

}

															
gboolean check_log_exist(char * info){
    char cmd_string[128];
	char tmpfile[128];
    tmpnam(tmpfile);
    //sprintf(cmd_string, "grep -F \"%s\" /opt/apps/ctsgw.log> %s", info, tmpfile);
    sprintf(cmd_string, "grep \"%s\" /opt/apps/ctsgw.log -wn > %s", info, tmpfile);
    //sprintf(cmd_string, "grep \"user.debug\" /opt/apps/ctsgw.log -wn > %s", tmpfile);
    g_print("check_log_exist %s\n",cmd_string);
    g_print("check_log_exist tmpfile %s\n",tmpfile);
    system(cmd_string);
    g_print("sleep before check tmpfile %s\n",tmpfile);
    sleep(10);
   if(check_file_size(tmpfile)>0){
     g_print("log \"%s\" exist\n",info);
     return TRUE;
   }
   else {
     g_print("log \"%s\" not exist\n",info);
     return FALSE;
   }
}
gboolean check_debug_log_exist(char * info){
    char cmd_string[128];
	char tmpfile[128];
    tmpnam(tmpfile);
    //sprintf(cmd_string, "grep -F \"%s\" /opt/apps/ctsgw.log> %s", info, tmpfile);
    //sprintf(cmd_string, "grep \"%s\" /opt/apps/ctsgw.log -wn > %s", info, tmpfile);
    sprintf(cmd_string, "grep \"user.debug\" /opt/apps/ctsgw.log -wn > /tmp/k1");
    g_print("check_log_exist %s\n",cmd_string);
    g_print("check_log_exist tmpfile %s\n",tmpfile);
    system(cmd_string);
   if(check_file_size("/tmp/k1")>0){
     g_print("log \"%s\" exist\n",info);
     return TRUE;
   }
   else {
     g_print("log \"%s\" not exist\n",info);
     return FALSE;
   }
}
long check_file_size(char* filename){
    unsigned long filesize = -1;      
    struct stat statbuff;  
    if(stat(filename, &statbuff) < 0){  
        return filesize;  
    }
    else{  
        filesize = statbuff.st_size;  
    }  
    g_print("check_file_size %ld \n",filesize);
    return filesize;  
 }                                              
                     

/********safety test********/
int Process_test_safe(){
   g_print("####Process_safe_test beginning\n####");
   
   int i;
   int Num_array=0;
   int return_install=-1;
   int return_start=-1;
   int return_uninstall=-1;

   //int target_value=-10;
   //int target_value=AMRET_APP_RUN_FAILED;
   int target_value[]={AMRET_APP_RUN_FAILED,AMRET_SUCCESS,AMRET_SUCCESS,AMRET_SUCCESS,AMRET_SUCCESS};
   int Num_value=-1,Num_status=-1;
#ifdef Realtek   		
   char *app_install[]={"/tmp/appmgr/applist/app_install/normal/appmemory_1_rtl9600.ipk",                        
                        "/tmp/appmgr/applist/app_install/normal/approotn_1_rtl9600.ipk",
                        "/tmp/appmgr/applist/app_install/normal/approot_1_rtl9600.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpul_1_rtl9600.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpus_1_rtl9600.ipk",
                       };   
#elif Broadcom
   char *app_install[]={"/tmp/appmgr/applist/app_install/normal/appmemory_1_bcm6838.ipk",                        
                        "/tmp/appmgr/applist/app_install/normal/approotn_1_bcm6838.ipk",
                        "/tmp/appmgr/applist/app_install/normal/approot_1_bcm6838.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpul_1_bcm6838.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpus_1_bcm6838.ipk",                        
                       };    
#elif Huawei
   char *app_install[]={"/tmp/appmgr/applist/app_install/normal/appmemory_1_sd5115.ipk",                        
                        "/tmp/appmgr/applist/app_install/normal/approotn_1_sd5115.ipk",
                        "/tmp/appmgr/applist/app_install/normal/approot_1_sd5115.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpul_1_sd5115.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpus_1_sd5115.ipk",
                       };   
#elif Zhongxing
   char *app_install[]={"/tmp/appmgr/applist/app_install/normal/appmemory_1_zx279100.ipk",                        
                        "/tmp/appmgr/applist/app_install/normal/approotn_1_zx279100.ipk",
                        "/tmp/appmgr/applist/app_install/normal/approot_1_zx279100.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpul_1_zx279100.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpus_1_zx279100.ipk",
                       };   
#elif MTK
   char *app_install[]={"/tmp/appmgr/applist/app_install/normal/appmemory_1_mt7526.ipk",                        
                        "/tmp/appmgr/applist/app_install/normal/approotn_1_mt7526.ipk",
                        "/tmp/appmgr/applist/app_install/normal/approot_1_mt7526.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpul_1_mt7526.ipk",
                        "/tmp/appmgr/applist/app_install/normal/appcpus_1_mt7526.ipk",
                       };   
#endif
   char *app_name[]={"appmemory","approotn","approot","appcpul","appcpus"};
   char *safe_target_status[]={"STOPPED", "RUNNING","RUNNING", "RUNNING","RUNNING"};
   Num_array=sizeof(app_install)/sizeof(char*);
   g_print("Num_array=%d\n",Num_array);   
  
   /*******uninstall app before install******/
   for(i=0;i<Num_array;i++){
     CtSgwStopApp(app_name[i]);	
     sleep(1);	
     CtSgwUninstallApp(app_name[i]);  
   }  		
   applist();
  #if 0 
   /*******check appmemory********/
   return_install=CtSgwInstallApp(app_install[0]);
   g_print("%s return_install=%d\n",app_name[0],return_install);

   return_start = CtSgwStartApp(app_name[0]); 	
   g_print("%s return_start= %d\n",app_name[0],return_start);	
   
   strcpy(app_status.name,app_name[0]);
   CtSgwGetAppStatus(&app_status);
   g_print("%s status is: %s\n",app_name[0],app_status.state);

   Num_value=CheckCtSgwtest_one(return_start,target_value);
   Num_status=CheckCtSgwStatus(app_status,target_status[1]);
   if(Num_value==1 && Num_status==1){
       write_xls(module_name[9],function_name[16],app_name[0],return_start,target_value,app_status.state,target_status[1],"start ipk",result[0]);
   }
   else write_xls(module_name[9],function_name[16],app_name[0],return_start,target_value,app_status.state,target_status[1],"start ipk",result[1]); 
   
   /*******start tsroot approotn approot*******/	
   for(i=3;i<Num_array;i++){
     return_install=CtSgwInstallApp(app_install[i]);
     g_print("%s return_install=%d\n",app_name[i],return_install);
     sleep(1);
   }	
   for(i=3;i<Num_array;i++){
     return_start = CtSgwStartApp(app_name[i]);     
     g_print("%s return_start= %d\n",app_name[i],return_start);					
     sleep(1); 
     strcpy(app_status.name,app_name[i]);
     CtSgwGetAppStatus(&app_status);
     g_print("%s status is: %s\n",app_name[i],app_status.state);
     sleep(1);
   }    

   if((access("/tmp/appmgr/file",F_OK)!=-1)||(access("/tmp/appmgr/folder",F_OK)!=-1)){
      g_print("/tmp/appmgr/file or /tmp/appmgr/folder wasn't removed\n");
      
   }
   else{
     check_string();  //在file与folder已被delete条件下执行log check
   }

   /******appcpu test ********/
#endif
   /******uninstall app*******/




   for(i=0;i<Num_array;i++){
    if (access((app_install+i), 0) != 0)
    {
        g_print("%s %s is not exist!\n",__func__,*(app_install+i));
    }
   return_install=CtSgwInstallApp(app_install[i]);
   g_print("%s return_install=%d\n",app_name[i],return_install);

   return_start = CtSgwStartApp(app_name[i]); 	
   g_print("%s return_start= %d\n",app_name[i],return_start);	
   
   strcpy(app_status.name,app_name[i]);
   CtSgwGetAppStatus(&app_status);
   g_print("%s status is: %s\n",app_name[i],app_status.state);

   Num_value=CheckCtSgwtest_one(return_start,target_value[i]);
   Num_status=CheckCtSgwStatus(app_status,safe_target_status[i]);//"RUNNING"
   //if(Num_value==1 && Num_status==1){
   if(Num_value==1 ){
       write_xls(module_name[9],function_name[16],app_name[i],return_start,target_value[i],app_status.state,safe_target_status[i],"safe run ipk",result[0]);
   }
   else write_xls(module_name[9],function_name[16],app_name[i],return_start,target_value[i],app_status.state,safe_target_status[i],"safe run ipk",result[1]); 
   }



       g_print("#######End of the %s#######\n",__func__);                  
       g_print ("\n\n"); 

   for(i=0;i<Num_array;i++){
     CtSgwStopApp(app_name[i]);
     sleep(1);		
     CtSgwUninstallApp(app_name[i]);  		 
     sleep(1);		
   }  	
   applist();	 
}

void check_string(void){
  int target_value=0,return_value=0;
  char *log1="approot test pass";
  char *log2="approotn test pass";
  char *log3="approot test fail";
  char *log4="approotn test fail";
  char *filename="/opt/apps/ctsgw.log";
  char buf[1024];
  int flag1=0,flag2=0;

  FILE* fp=fopen(filename, "r");
  FILE* out=fopen("/opt/apps/out.txt","w");

  while(fgets(buf, 1024, fp) != NULL)
   {
    char *p1=strstr(buf,log1);
    char *p2=strstr(buf,log2);
    char *p3=strstr(buf,log3);
    char *p4=strstr(buf,log4);
    /*****check the removed log*****/
    if(p1!=NULL){
      flag1++;
      printf("log1 exit\n");
      g_print("flag1=%d\n",flag1);
     }
     else if(p2!=NULL){
      flag2++;
      g_print("log2 exit\n");
      g_print("flag2=%d\n",flag2);
     }
    /*****rewrite the other byte that doesn't beyond log avoid the next test******/
    else{
      fputs(buf,out);    
      g_print("log has been removed\n");
     }
   
   } 
  
  if(flag1==1 && flag2==1){
      write_xls(module_name[9],function_name[15],NULL,return_value,target_value,NULL,NULL,"log exit",result[0]); 
      g_print("root test pass\n");
      g_print("flag1=%d,flag2=%d",flag1,flag2);
  }
  else{
      write_xls(module_name[9],function_name[15],NULL,return_value,target_value,NULL,NULL,"log exit",result[1]); 
      g_print("root test fail\n");
      g_print("flag1=%d,flag2=%d",flag1,flag2);
  }   
   fclose(fp);
   fclose(out);
   fp=NULL;
   out=NULL;
   remove("/opt/apps/ctsgw.log");
   rename("/opt/apps/out.txt","/opt/apps/ctsgw.log");
   system("chmod 777 /opt/apps/ctsgw.log");
   system("/etc/init.d/log restart");
   
}

		
