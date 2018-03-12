#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char*argv[])
{
    JavaVM *jvm;
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVMOption options[1];

    jobjectArray applicationArgs;
    jstring appArg;

    /* Setting VM arguments */
    vm_args.version = JNI_VERSION_1_2;
    vm_args.ignoreUnrecognized = JNI_TRUE;
    vm_args.nOptions = 0;

     /* Setting classpath */
    char classpath[1024] = "-Djava.class.path=";
    char *env_classpath = getenv("CLASSPATH");

    int mainclass_index = 1;
    if (argc >= 3 && !strcmp("-classpath", argv[1])) {
        options[0].optionString = strcat(classpath, argv[2]);
        vm_args.nOptions++;
        mainclass_index += 2;
    } else if (env_classpath) {
        options[0].optionString = strcat(classpath, env_classpath);
        vm_args.nOptions++;
    }

    if (vm_args.nOptions > 0) {
        vm_args.options = options;
    }

    if (mainclass_index >= argc) {
        printf("Main class not found, please specify it\n");
        return 0;
    }

    jint res = JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);
    if (res < 0) {
        printf("Create VM error, code = %d\n", res);
        return -1;
    }

    jclass cls = (*env)->FindClass(env, argv[mainclass_index]);
    if (!cls) {
        printf("Class %s not found\n", argv[mainclass_index]);
        return -1;
    }

    jmethodID mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");

    if (!mid) {
        printf("Method %s of Class %s not found\n", "main", argv[mainclass_index]);
        return -1;
    }
    applicationArgs = (*env)->NewObjectArray(env, argc - mainclass_index - 1,
                              (*env)->FindClass(env, "java/lang/String"),
                              NULL);

    int i = 0;
    for (i = mainclass_index + 1; i < argc; i ++) {
        appArg = (*env)->NewStringUTF(env, argv[i]);
        (*env)->SetObjectArrayElement(env, applicationArgs, i - mainclass_index - 1, appArg);
    }

    (*env)->CallStaticVoidMethod(env, cls, mid, applicationArgs);

    printf("before destroy\n");

    /*
     * Destroy the JVM.
     * This is necessary, otherwise if the called method exits,
     * this program will return immediately.
     */
    (*jvm)->DestroyJavaVM(jvm);

    printf("after destroy\n");

    return 0;
 }
