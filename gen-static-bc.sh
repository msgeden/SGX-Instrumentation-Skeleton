#rm sgx-app.h
cat App/App.h App/Enclave_u.h > sgx-app.h
# sudo cp ./sgx-app.h /usr/include/

#rm sgx-app.c
#rm sgx-app.bc
#rm sgx-app-32
cat App/App.cpp App/Enclave_u.c > sgx-app.c
sed -i 's/#include "App.h"/#include "sgx-app.h"/g' sgx-app.c
sed -i '/#include "Enclave_u.h"/d' ./sgx-app.c
sed -i '/^int SGX_CDECL main/ d' ./sgx-app.c

App_Include_Paths=" -IInclude -IApp -I$SGX_SDK/include"
#App_C_Flags=" -O2 -fPIC -Wno-attributes $App_Include_Paths -DNDEBUG -UEDEBUG -UDEBUG" #release
App_C_Flags=" -O0 -g -fPIC -Wno-attributes $App_Include_Paths -DDEBUG -UNDEBUG -UEDEBUG" #debug
clang -S -emit-llvm -m64 $App_C_Flags  sgx-app.c  -o sgx-app.bc

cp ./enclave.so $HOME/SGX/lib
cp ./enclave.signed.so $HOME/SGX/lib
cp ./sgx-app.bc $HOME/SGX/lib
