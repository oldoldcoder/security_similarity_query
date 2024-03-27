#include "utils.h"
#include "openssl/bn.h"
#include "eTPSS.h"


void printDebugInfo(BIGNUM * res,eTPSS * et,const char * funcName,int line,char * paramName){
    if(res == NULL && et == NULL){
        fprintf(stderr,"%s[%d]:传入参数不符合\n",funcName,line );
        goto end;
    }
    if(res != NULL && et != NULL){
        fprintf(stderr,"%s[%d]:传入参数不符合\n",funcName,line );
        goto end;
    }
    if(res != NULL){
        char * str = BN_bn2dec(res);
        fprintf(stdout,"%s[%d]: %s的值为-->%s\n",funcName,line,paramName,str);
    }else{
        BIGNUM * tmp = BN_CTX_get(CTX);
        et_Recover(tmp,et);
        char * str = BN_bn2dec(tmp);
        fprintf(stdout,"%s[%d]: %s的值为-->%s\n",funcName,line,paramName,str);
        BN_clear(tmp);
    }

    end:
    fflush(stdout);
    fflush(stderr);
}
