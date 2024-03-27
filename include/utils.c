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


void sqrt_bignum(const BIGNUM *num,BIGNUM * x) {

    BIGNUM *x_prev = BN_CTX_get(CTX);
    BIGNUM *tmp1 =  BN_CTX_get(CTX);
    BIGNUM *tmp2 =  BN_CTX_get(CTX);
    BIGNUM *two =  BN_CTX_get(CTX);
    BN_set_word(two, 2);
    BN_copy(x, num);
    // 牛顿迭代法计算平方根
    do {
        BN_copy(x_prev, x);
        BN_div(tmp1, tmp2, num, x, CTX);
        BN_add(x, x, tmp1);
        BN_div(x, NULL, x, two, CTX);
    } while (BN_cmp(x, x_prev) != 0);

    BN_clear(x_prev);
    BN_clear(tmp1);
    BN_clear(tmp2);
    BN_clear(two);

}