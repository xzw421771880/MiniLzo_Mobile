# MiniLzo_Mobile
移动端实现miniLzo压缩算法
# LZO官方源码
<font style="color:rgb(25, 25, 25);">http://www.oberhumer.com/opensource/lzo</font>

找到miniLZO点击Dowload miniLZO下载源码

[http://www.oberhumer.com/opensource/lzo/download/minilzo-2.10.tar.gz](http://www.oberhumer.com/opensource/lzo/download/minilzo-2.10.tar.gz)

# demo源码(包含安卓和iOS)
https://github.com/xzw421771880/MiniLzo_Mobile.git

# 1.安卓部分
## 1.1.测试用例
```java
public void lzo_compress_decompress(){
    String ss = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
    System.out.println("原16进制数据"+ss);

    //原数据字节流格式
    byte[] bytes = hexStr2ByteArr(ss);

    //开始压缩
    bytes = JNITools.lzo_compress(bytes);
    System.out.println("Lzo压缩后16进制数据"+byteTobyteStr(bytes));

    //开始解压
    bytes = JNITools.lzo_decompress(bytes);
    System.out.println("Lzo解压后16进制数据"+byteTobyteStr(bytes));
}

```

## 
## 1.2实现流程
### 1.2.1.新建工程(参考源码中名为minilzo的moudle)
### 1.2.2.在src/main目录下新建文件jni
### 1.2.3.将从lzo官方下载的文件中（lzoconf.h,lzodefs.h,minilzo.h,minilzo.c）放入jni文件
### 1.2.4.新建jnitools.c文件(或者直接复制)
### 1.2.5.引入相关文件头
```c
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
```

### 1.2.6.实现压缩
```c
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

```

### 1.2.7.实现解压
```c
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

```

### 1.2.8.java部分（新建JNItools）
```c
public class JNITools {
    static {
        System.loadLibrary("jnidemo");
    }
    //加法
    public static native byte[] lzo_compress(byte[] a);
    public static native byte[] lzo_decompress(byte[] a);
}

```


