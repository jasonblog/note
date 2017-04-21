#include "StructPass.h"
/* Header for class StructPass */

static struct {
    jfieldID name;
    jfieldID age;
    jfieldID height;
} gStructPassClassInfo;


JNIEXPORT void JNICALL Java_StructPass_toNativeStruct
(JNIEnv* env, jobject obj)
{
    jstring jstr;
    int age;
    float height;
    const char* str;

    /* Get a reference to obj's class */
    printf("In C:\n");

    /* Read the instance field name */
    jstr = (jstring)(*env)->GetObjectField(env, obj, gStructPassClassInfo.name);
    str = (*env)->GetStringUTFChars(env, jstr, NULL);

    age = (*env)->GetIntField(env, obj, gStructPassClassInfo.age);
    height = (*env)->GetFloatField(env, obj, gStructPassClassInfo.height);

    if (str == NULL) {
        return; /* out of memory */
    }

    printf("  c.name = \"%s\"\n", str);
    printf("  c.age = %d\n", age);
    printf("  c.height = %f\n", height);
    (*env)->ReleaseStringUTFChars(env, jstr, str);

}

JNIEXPORT void JNICALL Java_StructPass_fromNativeStruct
(JNIEnv* env, jobject obj)
{
    jstring jstr;

    /* Create a new string and overwrite the instance field */
    jstr = (*env)->NewStringUTF(env, "123");

    (*env)->SetObjectField(env, obj, gStructPassClassInfo.name, jstr);
    (*env)->SetIntField(env, obj, gStructPassClassInfo.age, 18);
    (*env)->SetFloatField(env, obj, gStructPassClassInfo.height, 179.0);
}

JNIEXPORT void JNICALL Java_StructPass_initNative
(JNIEnv* env, jobject obj)
{
    /* Get a reference to obj's class */
    jclass cls = (*env)->GetObjectClass(env, obj);

    gStructPassClassInfo.name = (*env)->GetFieldID(env, cls, "name", "Ljava/lang/String;");
    gStructPassClassInfo.age = (*env)->GetFieldID(env, cls, "age", "I");
    gStructPassClassInfo.height = (*env)->GetFieldID(env, cls, "height", "F");
}


