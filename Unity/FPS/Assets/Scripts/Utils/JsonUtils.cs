using UnityEngine;

class JsonUtils
{
    public static string ToJson(object obj, object[] innerObj = null, string[] name = null)
    {
        string first = JsonUtility.ToJson(obj);
        if(innerObj == null || name == null || innerObj.Length != name.Length) return first;

        string second = first.Substring(0, first.Length - 1);
        for(int i=0; i<innerObj.Length; i++)
        {
            second += (",\"" + name[i] + "\":" + JsonUtility.ToJson(innerObj[i]));
        }
        string re = second + '}';
        return re;
    }

}