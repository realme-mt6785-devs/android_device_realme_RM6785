#include <dlfcn.h>
#include <stdint.h>

namespace android {
    struct Rect { int32_t left, top, right, bottom; };
    struct native_handle;
}

extern "C" {
    // Android 11 removed GraphicBufferMapper::lock(..., int*, int*). We forward it to the modern 4-argument lock.
    __attribute__((visibility("default")))
    int _ZN7android19GraphicBufferMapper4lockEPK13native_handlejRKNS_4RectEPPvPiS9_(
            void* mapper_this, 
            const android::native_handle* handle, 
            unsigned int usage, 
            const android::Rect& bounds, 
            void** vaddr, 
            int* outBytesPerPixel, 
            int* outBytesPerStride) {
        
        static void* libui = []() { void* h = dlopen("libui-v32.so", RTLD_NOW); return h ? h : dlopen("libui.so", RTLD_NOW); }();
        // Modern 4-argument lock: GraphicBufferMapper::lock(native_handle const*, unsigned int, Rect const&, void**)
        static void* sym = libui ? dlsym(libui, "_ZN7android19GraphicBufferMapper4lockEPK13native_handlejRKNS_4RectEPPv") : nullptr;
        
        int err = -1; // Fallback to error if symbol not found
        if (sym) {
            typedef int (*LockFunc)(void*, const android::native_handle*, unsigned int, const android::Rect&, void**);
            LockFunc lock_func = (LockFunc)sym;
            err = lock_func(mapper_this, handle, usage, bounds, vaddr);
        }
        
        if (outBytesPerPixel) *outBytesPerPixel = 0;
        if (outBytesPerStride) *outBytesPerStride = 0;
        
        return err;
    }

    // Android 11 removed GraphicBufferMapper::unlock(native_handle const*). We forward it to the modern unlock.
    __attribute__((visibility("default")))
    int _ZN7android19GraphicBufferMapper6unlockEPK13native_handle(
            void* mapper_this,
            const android::native_handle* handle) {
        
        static void* libui = []() { void* h = dlopen("libui-v32.so", RTLD_NOW); return h ? h : dlopen("libui.so", RTLD_NOW); }();
        // Modern 2-argument unlock: GraphicBufferMapper::unlock(native_handle const*, unique_fd*)
        static void* sym = libui ? dlsym(libui, "_ZN7android19GraphicBufferMapper6unlockEPK13native_handlePNS_4base14unique_fd_implINS4_13DefaultCloserEEE") : nullptr;
        
        if (sym) {
            typedef int (*UnlockFunc)(void*, const android::native_handle*, void*);
            UnlockFunc unlock_func = (UnlockFunc)sym;
            return unlock_func(mapper_this, handle, nullptr);
        }
        
        return -1;
    }
}
