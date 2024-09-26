package com.example.minilzo;

public class JNITools {

    static {
        System.loadLibrary("jnidemo");
    }

    //加法
    public static native byte[] lzo_compress(byte[] a);
    public static native byte[] lzo_decompress(byte[] a);

}

