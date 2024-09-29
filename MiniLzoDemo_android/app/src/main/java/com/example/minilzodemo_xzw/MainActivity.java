package com.example.minilzodemo_xzw;

import android.os.Bundle;


import androidx.appcompat.app.AppCompatActivity;


import android.view.View;

import com.example.minilzo.JNITools;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);





    }

    public void string(View view){

        System.out.println("string-----");
        lzo_compress_decompress();


    }

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




    /**
     * 16进制字符串转换成字节数组
     * @param strIn
     * @return
     */
    public static byte[] hexStr2ByteArr(String strIn) {
        strIn = strIn.replaceAll(" ", "");
        strIn = strIn.replaceAll("\n", "");
        byte[] arrB = strIn.getBytes();
        int iLen = arrB.length;

//        System.out.println("字符数据------1-0");
        // 两个字符表示一个字节，所以字节数组长度是字符串长度除以2
        byte[] arrOut = new byte[iLen / 2];
//        System.out.println("字符数据------1-1");
        for (int i = 0; i < iLen; i = i + 2) {
            String strTmp = new String(arrB, i, 2);
            arrOut[i / 2] = (byte) Integer.parseInt(strTmp, 16);
        }
        return arrOut;
    }

    /**
     * 将字节数组转换成16进制字符串
     * @param bytes
     * @return
     */

    public static String byteTobyteStr(byte[] bytes) {
        if (bytes == null) {
            return null;
        }
        StringBuilder builder = new StringBuilder();
        // 遍历byte[]数组，将每个byte数字转换成16进制字符，再拼接起来成字符串
        for (int i = 0; i < bytes.length; i++) {
            // 每个byte转换成16进制字符时，bytes[i] & 0xff如果高位是0，输出将会去掉，所以+0x100(在更高位加1)，再截取后两位字符
            builder.append(Integer.toString((bytes[i] & 0xff) + 0x100, 16).substring(1));
        }
        return builder.toString().toUpperCase();
    }
}