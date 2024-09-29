#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>


#include "minilzo.h"

#define IN_LEN      (128*1024ul)
#define OUT_LEN     (IN_LEN + IN_LEN / 16 + 64 + 3)

#define LOG_TAG "MyTag"

static unsigned char __LZO_MMODEL in1  [ IN_LEN ];
static unsigned char __LZO_MMODEL out1 [ OUT_LEN ];

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);


jbyteArray compress(JNIEnv *env,jclass clazz,jbyteArray a);
jbyteArray decompress(JNIEnv *env,jclass clazz,jbyteArray a);





JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){

    //打印日志，说明已经进来了
    __android_log_print(ANDROID_LOG_DEBUG,"JNITag","enter jni_onload");

    JNIEnv* env = NULL;
    jint result = -1;

    // 判断是否正确
    if((*vm)->GetEnv(vm,(void**)&env,JNI_VERSION_1_6)!= JNI_OK){
        return result;
    }

    //注册四个方法，注意签名
    const JNINativeMethod method[]={
            {"lzo_compress","([B)[B",(void*)compress},
            {"lzo_decompress","([B)[B",(void*)decompress},
//            {"sub","(II)I",(void*)subNumber},
//            {"mul","(II)I",(void*)mulNumber},
//            {"div","(II)I",(void*)divNumber},
//            {"ad","(II)I",(void*)adNumberr}


    };

    //找到对应的JNITools类
    jclass jClassName=(*env)->FindClass(env,"com/example/minilzo/JNITools");

    //开始注册
    jint ret = (*env)->RegisterNatives(env,jClassName,method, 2);

     //如果注册失败，打印日志
    if (ret != JNI_OK) {
        __android_log_print(ANDROID_LOG_DEBUG, "JNITag", "jni_register Error");
        return -1;
    }

    return JNI_VERSION_1_6;
}

jbyteArray compress(JNIEnv *env,jclass clazz,jbyteArray a){
    //将jbyteArray转成jbyte *格式
    jsize length = (*env)->GetArrayLength(env,a);
    jbyte *bytes = (*env)->GetByteArrayElements(env,a, NULL);


    //将jbyte *格式转成unsigned char *
    unsigned char *unsignedChars = (unsigned char *)malloc(length * sizeof(unsigned char));
    memcpy(unsignedChars, bytes, length * sizeof(unsigned char));
    (*env)->ReleaseByteArrayElements(env,a, bytes, 0);


    //执行lzo压缩
    int length1 = lzo_compress1(unsignedChars, (int )length);
    jbyteArray jArray = (*env)->NewByteArray(env, length1);
    // 复制数据从cArray到jArray
    (*env)->SetByteArrayRegion(env, jArray, 0, length1, (jbyte*)out1);

    //释放内存
//    (*env)->ReleaseByteArrayElements(env,a, bytes, 0);
//    free(unsignedChars);

    return jArray;
}


jbyteArray decompress(JNIEnv *env,jclass clazz,jbyteArray a){
    //将jbyteArray转成jbyte *格式
    jsize length = (*env)->GetArrayLength(env,a);
    jbyte *bytes = (*env)->GetByteArrayElements(env,a, NULL);

    //将jbyte *格式转成unsigned char *
    unsigned char *unsignedChars = (unsigned char *)malloc(length * sizeof(unsigned char));
    memcpy(unsignedChars, bytes, length * sizeof(unsigned char));
    (*env)->ReleaseByteArrayElements(env,a, bytes, 0);

    //执行lzo解压
    int length1 = lzo_decompress1(unsignedChars, (int )length);
    jbyteArray jArray = (*env)->NewByteArray(env, length1);
    // 复制数据从cArray到jArray
    (*env)->SetByteArrayRegion(env, jArray, 0, length1, (jbyte*)in1);
    //释放内存
//    (*env)->ReleaseByteArrayElements(env,a, bytes, 0);
//    free(unsignedChars);

    return jArray;
}



int  lzo_compress1(unsigned char * data,int datalen)
{
//    __android_log_print(ANDROID_LOG_DEBUG, "JNITag", "datalen-%d",datalen);
    int r;
    lzo_uint in_len;
    lzo_uint out_len;

/*
 * Step 1: initialize the LZO library
 */
    if (lzo_init() != LZO_E_OK)
    {
        printf("internal error - lzo_init() failed !!!\n");
        printf("(this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
    }


    in_len = IN_LEN;
    in_len = datalen;

//    lzo_memset(in,0,in_len);
//    in = data;

//    unsigned char __LZO_MMODEL newout [ OUT_LEN ];
    printf("out----%s\n",out1);
    //__android_log_print(ANDROID_LOG_DEBUG, "JNITag", "out0---%s",out1);
    r = lzo1x_1_compress(data,datalen,out1,&out_len,wrkmem);
    //__android_log_print(ANDROID_LOG_DEBUG, "JNITag", "out1---%s",out1);

//    printf("out----%s\n",out1);
    if (r == LZO_E_OK){
        printf("compressed %lu bytes into %lu bytes\n",
               (unsigned long) in_len, (unsigned long) out_len);
        __android_log_print(ANDROID_LOG_DEBUG, "JNITag", "compressed %lu bytes into %lu bytes\n",
                            (unsigned long) in_len, (unsigned long) out_len);
    }

    else
    {
        /* this should NEVER happen */
        printf("internal error - compression failed: %d\n", r);
        __android_log_print(ANDROID_LOG_DEBUG, "JNITag", "internal error - compression failed: %d\n", r);
    }
    /* check for an incompressible block */
//    if (out_len >= in_len)
//    {
//        printf("This block contains incompressible data.\n");
//        return 0;
//    }
//    unsigned char *a = lzo_decompress(out,in, out_len);
    return out_len;
}





int lzo_decompress1(unsigned char * data,int datalen)
{
    int r;
    lzo_uint out_len;

    if (lzo_init() != LZO_E_OK)
    {
        printf("internal error - lzo_init() failed !!!\n");
        printf("(this usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
    }

    out_len = 0;

//    printf("%s",in1);
    r = lzo1x_decompress(data,datalen,in1,&out_len,NULL);
//    printf("%s",in);
//    printf("%s",in1);

    if (r == LZO_E_OK)
        printf("decompressed %lu bytes back into %lu bytes\n",
               (unsigned long) datalen, (unsigned long) out_len);
    else
    {
        /* this should NEVER happen */
        printf("internal error - decompression failed: %d\n", r);
    }

    return out_len;
}




