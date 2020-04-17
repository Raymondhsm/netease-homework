using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class StartGameController : MonoBehaviour
{
    public InputField inpAccount;
    public InputField inpPassword;

    private NetworkSocket m_network;

    // Start is called before the first frame update
    void Start()
    {
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void Login()
    {
        string account = inpAccount.text;
        string pwd = inpPassword.text;

        LoginInfo loginInfo;
        loginInfo.account = account;
        loginInfo.password = pwd;

        string data = JsonUtility.ToJson(loginInfo);
        m_network.send(Config.COMMAND_LOGIN,data);
    }

    public void Register()
    {
        string account = inpAccount.text;
        string pwd = inpPassword.text;

        LoginInfo loginInfo;
        loginInfo.account = account;
        loginInfo.password = pwd;

        string data = JsonUtility.ToJson(loginInfo);
        m_network.send(Config.COMMAND_REGISTER,data);
    }

    public void BeginGame()
    {

    }

    public struct LoginInfo
    {
        public string account;
        public string password;
    }
}
