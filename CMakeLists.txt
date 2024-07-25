#cmake_minimum_required(VERSION 3.25)
#project(security_similarity_query C)
#
#set(CMAKE_C_STANDARD 11)
## 设置编译选项
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
#
## 添加源文件目录
#set(OPENSSL_INCLUDE_DIR "D:\\software\\lib\\OpenSSL\\include")
#set(OPENSSL_CRYPTO_LIBRARY "D:\\software\\lib\\OpenSSL\\lib\\vc\\x64\\MT\\libcrypto_static.lib")
#set(OPENSSL_SSL_LIBRARY "D:\\software\\lib\\OpenSSL\\lib\\vc\\x64\\MT\\libssl_static.lib")
#aux_source_directory(include SOURCE_FILES)
#aux_source_directory(test SOURCE_FILES)
#include_directories(include ${OPENSSL_INCLUDE_DIR})
#
#
#add_executable(${PROJECT_NAME} ${SOURCE_FILES} include/eTPSS.c)
#add_executable(SSQ_test ${SOURCE_FILES} include/eTPSS.c)
#target_include_directories(${PROJECT_NAME} PUBLIC include)
#target_include_directories(SSQ_test PUBLIC include)
## 链接 OpenSSL 库
#target_link_libraries(${PROJECT_NAME}  ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ws2_32 Crypt32)
#target_link_libraries(SSQ_test ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ws2_32 Crypt32)


#----------------------------------------------------------------------------------------------------------------


#cmake_minimum_required(VERSION 3.25)
#project(security_similarity_query C)
#
#set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
#
## 强制使用静态链接的 OpenSSL 库
#set(OPENSSL_INCLUDE_DIR "D:\\software\\lib\\OpenSSL\\include")
#set(OPENSSL_CRYPTO_LIBRARY "D:\\software\\lib\\OpenSSL\\lib\\vc\\x64\\MT\\libcrypto_static.lib")
#set(OPENSSL_SSL_LIBRARY "D:\\software\\lib\\OpenSSL\\lib\\vc\\x64\\MT\\libssl_static.lib")
#
#aux_source_directory(include SOURCE_FILES)
#include_directories(include ${OPENSSL_INCLUDE_DIR})
#
#
## 编译为动态链接库（DLL）
#add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} include/eTPSS.c)
#set_target_properties(${PROJECT_NAME} PROPERTIES
#        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
## 链接 OpenSSL 和 eTPSS 库
#target_link_libraries(${PROJECT_NAME}  ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} ws2_32 Crypt32)

cmake_minimum_required(VERSION 3.25)
project(security_similarity_query C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
#定义两个变量，表示头文件路径和库路径
find_package(OpenSSL REQUIRED)

# 添加源文件目录
aux_source_directory(include SOURCE_FILES)
aux_source_directory(test SOURCE_FILES)
# 显示的包含头文件
include_directories(include)

if(OPENSSL_FOUND)
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} include/eTPSS.c)

    add_executable(SSQ_test ${SOURCE_FILES} include/eTPSS.c)

    target_include_directories(${PROJECT_NAME} PUBLIC include)
    target_include_directories(SSQ_test PUBLIC include)
    # 链接 OpenSSL 库
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
    target_link_libraries(SSQ_test OpenSSL::SSL OpenSSL::Crypto)

endif (OPENSSL_FOUND)