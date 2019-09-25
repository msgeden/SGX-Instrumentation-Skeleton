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

#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include <stdbool.h>
#include <string.h>
#include <sgx_trts.h>
#include <sgx_tprotected_fs.h>

#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */


int *shadow_variable_cells;
int *shadow_init_vector;
int shadow_set_success=0;
//int file_descriptor=0;

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf_on_terminal(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}

void printf_helloworld()
{
    printf_on_terminal("Hello World\n");
}


int upfs_open(const char* path) {
    int fd = 0;
    ocall_open(&fd, path);
    return fd;
}

int upfs_create(const char* path) {
    int fd = 0;
    ocall_create(&fd, path);
    return fd;
}

ssize_t upfs_read(int fd, void* buf, size_t size) {
    ssize_t ret = 0;
    ocall_read(&ret, fd, buf, size);
    return ret;
}

ssize_t upfs_write(int fd, const void* buf, size_t size) {
    ssize_t ret = 0;
    ocall_write(&ret, fd, buf, size);
    return ret;
}

int upfs_close(int fd) {
    int ret = 0;
    ocall_close(&ret, fd);
    return ret;
}

void printf_on_file(const char *fmt, ...)
{
    int fdes=upfs_create("vi_output.log");
    char buf[BUFSIZ] = {'\0'};
    ssize_t ret = 0;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_write(&ret,fdes,buf,strlen(buf));
    upfs_close(fdes);
}
/*create heap space for shadow cells and corresponding initialisation vector for enumerated variables*/
void set_shadow_memory(int size){
    /*file_descriptor = upfs_create("vi_output.log");
    if (file_descriptor < 0) {
        return;
    }*/
    shadow_variable_cells = (int*)calloc(size, sizeof(int));
    shadow_init_vector = (int*)calloc(size, sizeof(int));
    if (shadow_variable_cells!=NULL && shadow_init_vector!=NULL){
        shadow_set_success=1;
        printf_on_terminal("INFO:ALLOCATION->Shadow memory for %d cells is successfully allocated\n",size);
        printf_on_file("INFO:ALLOCATION->Shadow memory for %d cells is successfully allocated\n",size);
    }    
    return;
}
/*copies the variable value to allocated cell in case of a memory write or checks the variable value with the one previously stored in case of a memory read*/
void vi_call(int ID,int access,int value, const char* var_name){
    if (shadow_set_success==1){
        if (access==1){
            //printf_on_file(file_descriptor,"INFO:STORE->\tVar:%s\tAccess:WRITE\tValue:%d\n", var_name, value);
            shadow_variable_cells[ID]=value;
            shadow_init_vector[ID]=1;
        }
        else if (access==0){
            //printf_on_file(file_descriptor,"INFO:LOAD->\tVar:%s\tAccess:READ\tValue:%d\n", var_name, value);
            if (shadow_init_vector[ID]==1){
                if (shadow_variable_cells[ID]!=value){
                    printf_on_terminal("WARN:VIOLATION->\tVar:%s\tShadow:%d\tActual:%d\n", var_name, shadow_variable_cells[ID],  value);
                    printf_on_file("WARN:VIOLATION->\tVar:%s\tShadow:%d\tActual:%d\n", var_name, shadow_variable_cells[ID],  value);
                }
            }
            //else{
            //    //printf_on_file(file_descriptor,"WARN:UNINITIALISED USE->\tVar:%s\tActual:%d\n", var_name,value);
            //}
        }
    }
    return;
}

/*frees the allocated shadow memory in the heap*/
void free_shadow_memory(){
    free(shadow_variable_cells);
    free(shadow_init_vector);
    printf_on_terminal("INFO:FREE->Shadow memory is successfully freed\n");
    printf_on_file("INFO:FREE->Shadow memory is successfully freed\n");    
    return;
}