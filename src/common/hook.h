// Copyright 2022. All Rights Reserved.
// Author: Bruce-Lee-LY
// Date: 15:04:22 on Sun, May 29, 2022
//
// Description: hook

#ifndef __CUDA_HOOK_HOOK_H__
#define __CUDA_HOOK_HOOK_H__

#include <dlfcn.h>

#include <string>

#include "macro_common.h"
typedef void *(*fp_dlsym)(void *, const char *);
#define HOOK_GET_SYMBOL(type, real_dlsym, symbol_name)                                             \
    do {                                                                                           \
        static void *type##_handle = dlopen(s_##type##_dso, RTLD_NOW | RTLD_LOCAL);                \
        HOOK_CHECK(type##_handle);                                                                 \
        Dl_info info;                                                                              \
        void *symbol = real_dlsym(type##_handle, symbol_name.c_str());                             \
        HOOK_CHECK(symbol);                                                                        \
        HOOK_CHECK(dladdr(symbol, &info));                                                         \
        HLOG("Symbol: %s, Address: %p, Library: %s", symbol_name.c_str(), symbol, info.dli_fname); \
        return symbol;                                                                             \
    } while (0)

class Hook {
public:
    // 获取唯一实例的静态方法
    static Hook &getInstance() {
        static Hook instance;
        return instance;
    }

    void *GetCUDASymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(cuda, real_dlsym, symbol_name);
    }

    void *GetNVMLSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(nvml, real_dlsym, symbol_name);
    }

    void *GetCUDARTSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(cudart, real_dlsym, symbol_name);
    }

    void *GetCUDNNSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(cudnn, real_dlsym, symbol_name);
    }

    void *GetCUBLASSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(cublas, real_dlsym, symbol_name);
    }

    void *GetCUBLASLTSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(cublasLt, real_dlsym, symbol_name);
    }

    void *GetCUFFTSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(cufft, real_dlsym, symbol_name);
    }

    void *GetNVTXSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(nvtx, real_dlsym, symbol_name);
    }

    void *GetNVRTCSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(nvrtc, real_dlsym, symbol_name);
    }

    void *GetCURANDSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(curand, real_dlsym, symbol_name);
    }

    void *GetCUSPARSESymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(cusparse, real_dlsym, symbol_name);
    }

    void *GetCUSOLVERSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(cusolver, real_dlsym, symbol_name);
    }

    void *GetNVJPEGSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(nvjpeg, real_dlsym, symbol_name);
    }

    void *GetNVBLASSymbol(const std::string &symbol_name) {
        HOOK_GET_SYMBOL(nvblas, real_dlsym, symbol_name);
    }

private:
    // nvidia native cuda dynamic library can be modified to any other unambiguous name, or moved to any path
    static constexpr const char *s_cuda_dso = "/usr/lib/x86_64-linux-gnu/libcuda.so";
    static constexpr const char *s_nvml_dso = "/usr/lib/x86_64-linux-gnu/libnvidia-ml.so";
    static constexpr const char *s_cudart_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libcudart.so";
    static constexpr const char *s_cudnn_dso = "/usr/local/cudnn/lib64/libcudnn.so";
    static constexpr const char *s_cublas_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libcublas.so";
    static constexpr const char *s_cublasLt_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libcublasLt.so";
    static constexpr const char *s_cufft_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libcufft.so";
    static constexpr const char *s_nvtx_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libnvToolsExt.so";
    static constexpr const char *s_nvrtc_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libnvrtc.so";
    static constexpr const char *s_curand_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libcurand.so";
    static constexpr const char *s_cusparse_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libcusparse.so";
    static constexpr const char *s_cusolver_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libcusolver.so";
    static constexpr const char *s_nvjpeg_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libnvjpeg.so";
    static constexpr const char *s_nvblas_dso = "/usr/local/cuda/targets/x86_64-linux/lib/libnvblas.so";

private:
    // 私有构造函数，防止外部实例化
    Hook() {
        if (real_dlsym == NULL) {
            void *handle = dlopen("libc.so.6", RTLD_LAZY);
            if (!handle) {
                HLOG("Failed to open libc.so.6: %s", dlerror());
                HOOK_CHECK(handle);
            }
            real_dlsym = (fp_dlsym)dlvsym(handle, "dlsym", "GLIBC_2.2.5");
            if (real_dlsym == NULL) {
                HLOG("Failed to find real dlsym function: %s", dlerror());
                HOOK_CHECK(real_dlsym);
            }
        }
    }
    ~Hook() {
        real_dlsym = NULL;
    }
    fp_dlsym real_dlsym = NULL;
    HOOK_DISALLOW_COPY_AND_ASSIGN(Hook);
};

#define HOOK_CUDA_SYMBOL(symbol_name) Hook::getInstance().GetCUDASymbol(symbol_name)
#define HOOK_NVML_SYMBOL(symbol_name) Hook::getInstance().GetNVMLSymbol(symbol_name)
#define HOOK_CUDART_SYMBOL(symbol_name) Hook::getInstance().GetCUDARTSymbol(symbol_name)
#define HOOK_CUDNN_SYMBOL(symbol_name) Hook::getInstance().GetCUDNNSymbol(symbol_name)
#define HOOK_CUBLAS_SYMBOL(symbol_name) Hook::getInstance().GetCUBLASSymbol(symbol_name)
#define HOOK_CUBLASLT_SYMBOL(symbol_name) Hook::getInstance().GetCUBLASLTSymbol(symbol_name)
#define HOOK_CUFFT_SYMBOL(symbol_name) Hook::getInstance().GetCUFFTSymbol(symbol_name)
#define HOOK_NVTX_SYMBOL(symbol_name) Hook::getInstance().GetNVTXSymbol(symbol_name)
#define HOOK_NVRTC_SYMBOL(symbol_name) Hook::getInstance().GetNVRTCSymbol(symbol_name)
#define HOOK_CURAND_SYMBOL(symbol_name) Hook::getInstance().GetCURANDSymbol(symbol_name)
#define HOOK_CUSPARSE_SYMBOL(symbol_name) Hook::getInstance().GetCUSPARSESymbol(symbol_name)
#define HOOK_CUSOLVER_SYMBOL(symbol_name) Hook::getInstance().GetCUSOLVERSymbol(symbol_name)
#define HOOK_NVJPEG_SYMBOL(symbol_name) Hook::getInstance().GetNVJPEGSymbol(symbol_name)
#define HOOK_NVBLAS_SYMBOL(symbol_name) Hook::getInstance().GetNVBLASSymbol(symbol_name)

#endif  // __CUDA_HOOK_HOOK_H__
