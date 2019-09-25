/*
 * Copyright (C) 2011-2018 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include <sgx_urts.h>
#include <sgx_uswitchless.h>

/*#include "sgx_urts.h"*/
#include "App.h"
#include "Enclave_u.h"

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
    	printf("Error code is 0x%X. Please refer to the \"Intel SGX SDK Developer Reference\" for more details.\n", ret);
}

int initialize_switchless_enclave(const sgx_uswitchless_config_t* us_config)
{
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;

    /* Call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */

    const void* enclave_ex_p[32] = { 0 };

    enclave_ex_p[SGX_CREATE_ENCLAVE_EX_SWITCHLESS_BIT_IDX] = (const void*)us_config;

    ret = sgx_create_enclave_ex(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, NULL, NULL, &global_eid, NULL, SGX_CREATE_ENCLAVE_EX_SWITCHLESS, enclave_ex_p);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        return -1;
    }

    return 0;
}
int initialize_enclave(void)
{
        /* Configuration for Switchless SGX */
    sgx_uswitchless_config_t us_config = SGX_USWITCHLESS_CONFIG_INITIALIZER;
    us_config.num_uworkers = 2;
    us_config.num_tworkers = 2;
        /* Initialize the enclave */
    if(initialize_switchless_enclave(&us_config) < 0)
    {
        printf("Error: enclave initialization failed\n");
        return -1;
    }
    return 0;

}

int destroy_enclave(void){
    sgx_destroy_enclave(global_eid);
    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}


int ocall_open(const char* path) {
    return open(path, O_RDONLY);
}

int ocall_create(const char* path) {
    return open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
}

ssize_t ocall_read(int fd, void* buf, size_t size) {
    return read(fd, buf, size);
}

ssize_t ocall_write(int fd, const void* buf, size_t size) {
    return write(fd, buf, size);
}

int ocall_close(int fd) {
    return close(fd);
}

/* Wrapper functions for different variable types*/ 
void vi_call_u1(int ID,int access,bool value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_u8(int ID,int access,u_int8_t value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_u16(int ID,int access,u_int16_t value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_u32(int ID,int access,u_int32_t value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_u64(int ID,int access,u_int64_t value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_1(int ID,int access,bool value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_8(int ID,int access,int8_t value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_16(int ID,int access,int16_t value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_32(int ID,int access,int32_t value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_64(int ID,int access,int64_t value, char* var_name){
    int cast_value=(int)value;
    vi_call(global_eid,ID,access,cast_value,var_name);
    return;
}
void vi_call_p1(int ID,int access, bool* pointer, char* var_name){
    bool value=(__intptr_t)pointer;
    vi_call_u1(ID,access,value,var_name);
    return;
}
void vi_call_p8(int ID,int access,u_int8_t* pointer, char* var_name){
    u_int8_t value=(__intptr_t)pointer;
    vi_call_u8(ID,access,value,var_name);
    return;
}
void vi_call_p16(int ID,int access,u_int16_t* pointer, char* var_name){
    u_int16_t value=(__intptr_t)pointer;
    vi_call_u16(ID,access,value,var_name);
    return;
}
void vi_call_p32(int ID,int access,u_int32_t* pointer, char* var_name){
    u_int32_t value=(__intptr_t)pointer;
    vi_call_u32(ID,access,value,var_name);
    return;
}
void vi_call_p64(int ID,int access,u_int64_t* pointer, char* var_name){
    u_int64_t value=(__intptr_t)pointer;
    vi_call_u64(ID,access,value,var_name);
    return;
}

int SGX_CDECL main(int argc, char *argv[]){return 0;}
