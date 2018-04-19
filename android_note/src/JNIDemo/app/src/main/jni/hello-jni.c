//
// Created by Administrator on 2017/11/20.
//

#include <jni.h>

//jstring: 返回值类型 String
//规则: Java_包名_类名_方法名（jvm虚拟机的指针，调用者对象）
jstring Java_com_tengmuz_jnidemo_MainActivity_hellojni(JNIEnv* env,jobject thiz)
{
    return (*env)->NewStringUTF(env, "Hello JNI, 你好 JNI !");
}