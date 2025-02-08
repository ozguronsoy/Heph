#ifdef __ANDROID__
#ifndef HEPH_ANDROID_HELPERS_H
#define HEPH_ANDROID_HELPERS_H

#include "HephShared.h"
#include <string>
#include <jni.h>

/** @file */

namespace Heph::Native
{
    /** @brief Implements helper methods for Android. */
    class HEPH_API AndroidHelpers final
    {
    public:
        /** Default JNI version.  */
        static constexpr jint JNI_VERSION = JNI_VERSION_1_6;
        /** Java Virtual Machine, must be set manually if JNI_OnLoad method is overridden. */
        static inline JavaVM* jvm = nullptr;

    public:
        HEPH_DISABLE_INSTANCE(AndroidHelpers);

        /** 
         * Checks if the AndroidHelpers::jvm is set and throws an exception if not. 
         *
         * @exception InvalidOperationException
         */
        static void ValidateJVM();

        /** 
         * Gets the JNIEnv variable. 
         * 
         * @exception InvalidOperationException
         * @exception ExternalException
         */
        static JNIEnv* GetEnv();
        
        /** 
         * Converts jstring to std::string. 
         * 
         * @exception InvalidArgumentException
         */
        static std::string JStringToStdString(JNIEnv* env, jstring jstr);
        
        /** 
         * Converts std::string to jstring. 
         * 
         * @exception InvalidArgumentException
         */
        static jstring StdStringToJString(JNIEnv* env, const std::string& stdstr);
    };
}

#endif
#endif