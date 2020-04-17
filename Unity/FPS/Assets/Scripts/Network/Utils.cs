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
}