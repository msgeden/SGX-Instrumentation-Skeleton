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


#ifndef _APP_H_
#define _APP_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "sgx_error.h"       /* sgx_status_t */
#include "sgx_eid.h"     /* sgx_enclave_id_t */

#ifndef TRUE
# define TRUE 1
#endif

#ifndef FALSE
# define FALSE 0
#endif

# define TOKEN_FILENAME   "enclave.token"
# define ENCLAVE_FILENAME "enclave.signed.so"

extern sgx_enclave_id_t global_eid;    /* global enclave id */

#if defined(__cplusplus)
extern "C" {
void vi_call_u1(int ID,int access,bool value, char* var_name);
void vi_call_u16(int ID,int access,u_int16_t value, char* var_name);
void vi_call_u32(int ID,int access,u_int32_t value, char* var_name);
void vi_call_u64(int ID,int access,u_int64_t value, char* var_name);
void vi_call_1(int ID,int access,bool value, char* var_name);
void vi_call_8(int ID,int access,int8_t value, char* var_name);
void vi_call_16(int ID,int access,int16_t value, char* var_name);
void vi_call_32(int ID,int access,int32_t value, char* var_name);
void vi_call_64(int ID,int access,int64_t value, char* var_name);
void vi_call_p1(int ID,int access, bool* pointer, char* var_name);
void vi_call_p8(int ID,int access,u_int8_t* pointer, char* var_name);
void vi_call_p16(int ID,int access,u_int16_t* pointer, char* var_name);
void vi_call_p32(int ID,int access,u_int32_t* pointer, char* var_name);
void vi_call_p64(int ID,int access,u_int64_t* pointer, char* var_name);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* !_APP_H_ */
