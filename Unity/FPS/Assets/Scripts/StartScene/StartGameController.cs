using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class StartGameController : MonoBehaviour
{
    public GameObject loginPlane;
    public InputField inpAccount;
    public InputField inpPassword;
    public Text txtTips;

    private NetworkSocket m_network;

    // Start is called before the first frame update
    void Start()
    {
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
        Service.Instance().loginRecvCallback = new Service.RecvHandler(this.LoginCallback);
        Service.Instance().registerRecvCallback = new Service.RecvHandler(this.RegisterCallback);
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

    public void LoginCallback(string loginRecvStr)
    {
        LoginRecv loginRecv = JsonUtility.FromJson<LoginRecv>(loginRecvStr);
        
        if(loginRecv.loginStatus)
        {
            loginPlane.SetActive(false);
            PlayerPrefs.SetString("sessionId", loginRecv.sessionId);
        }
        else
        {
            txtTips.text = loginRecv.sessionId;
        }
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

    public void RegisterCallback(string registerRecvStr)
    {

    }

    public void BeginGame()
    {

    }

    public struct LoginInfo
    {
        public string account;
        public string password;
    }

    public struct LoginRecv
    {
        public bool loginStatus;
        public string sessionId;
    }
}
