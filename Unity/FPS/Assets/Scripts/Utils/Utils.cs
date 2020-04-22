using System;
class Utils
{
    // 小端转换为int32
    public static int CharArrayToInt32(char[] input)
    {
        if(input.Length != 4) throw (new ArgumentOutOfRangeException("char array is expected the length of 4."));

        int result;
        result = input[0] + input[1]*256 + input[2]*65536 + input[3]*16777216;
        return result;
    }

    public static char[] Int32ToCharArray(int input)
    {
        char[] re = new char[4];
        re[0] = (char)(input & 0x000000ff);
        re[1] = (char)((input >> 8) & 0x0000ff);
        re[2] = (char)((input >> 16) & 0x00ff);
        re[3] = (char)(input >> 24);
        return re;
    }

    public static byte[] MergeByteArray(byte[] a, byte[] b)
    {
        byte[] re = new byte[a.Length + b.Length];
        a.CopyTo(re, 0);
        b.CopyTo(re, a.Length);
        return re;
    }
}