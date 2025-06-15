#ifdef __ANDROID__

#include "Native/AndroidHelpers.h"
#include "Exceptions/ExternalException.h"
#include "Exceptions/InvalidArgumentException.h"
#include "Exceptions/InvalidOperationException.h"

namespace Heph::Native
{
    void AndroidHelpers::ValidateJVM()
    {
        if (AndroidHelpers::jvm == nullptr)
        {
            HEPH_EXCEPTION_RAISE_AND_THROW(InvalidOperationException, HEPH_FUNC, "Heph::AndroidHelpers::jvm must be set.");
        }
    }

    JNIEnv* AndroidHelpers::GetEnv()
    {
        AndroidHelpers::ValidateJVM();

        JNIEnv* env;
        jint jniResult = AndroidHelpers::jvm->GetEnv(reinterpret_cast<void**>(&env), AndroidHelpers::JNI_VERSION);

        if (jniResult == JNI_EDETACHED)
        {
            jniResult = AndroidHelpers::jvm->AttachCurrentThread(&env, nullptr);
            if (jniResult != JNI_OK)
            {
                HEPH_EXCEPTION_RAISE_AND_THROW(ExternalException, HEPH_FUNC, "Failed to attach to the current thread.", "JNI", "");
            }
        }
        else if (jniResult != JNI_OK)
        {
            HEPH_EXCEPTION_RAISE_AND_THROW(ExternalException, HEPH_FUNC, "Could not get the current jni environment.", "JNI", "");
        }

        return env;
    }

    std::string AndroidHelpers::JStringToStdString(JNIEnv* env, jstring jstr)
    {
        if (env == nullptr)
        {
            HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "evn cannot be nullptr.");
        }

        std::string stdstr;

        const jchar* raw = env->GetStringChars(jstr, 0);
        const jsize len = env->GetStringLength(jstr);
        stdstr.assign(raw, raw + len);
        env->ReleaseStringChars(jstr, raw);

        return stdstr;
    }

    jstring AndroidHelpers::StdStringToJString(JNIEnv* env, const std::string& stdstr)
    {
        if (env == nullptr)
        {
            HEPH_EXCEPTION_RAISE_AND_THROW(InvalidArgumentException, HEPH_FUNC, "evn cannot be nullptr.");
        }
        return env->NewStringUTF(stdstr.c_str());
    }
}

extern "C" __attribute__((weak)) jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    Heph::Native::AndroidHelpers::jvm = vm;
    return Heph::Native::AndroidHelpers::JNI_VERSION;
}

#endif