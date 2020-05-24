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
    public Text txtInfo;

    private NetworkSocket m_network;

    // Start is called before the first frame update
    void Start()
    {
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
        Service.Instance().loginRecvCallback = new Service.RecvHandler(this.LoginCallback);
        Service.Instance().registerRecvCallback = new Service.RecvHandler(this.RegisterCallback);
        Service.Instance().updateInfoCallback = new Service.RecvHandler(this.updateInfoCallback);
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
            m_network.send(Config.COMMAND_UPDATE_INFO, "{\"sessionID\":\""+loginRecv.sessionId+"\"}");
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

    public void updateInfoCallback(string RecvStr)
    {
        UserInfo info = JsonUtility.FromJson<UserInfo>(RecvStr);
        txtInfo.text = "血量：" + info.blood.ToString() + "/100    子弹：" + info.bullet.ToString() + "/140    等级：" + info.level.ToString() + "    经验：" + info.experience.ToString() + "/" + (500 + info.level * 100).ToString();
    }

    public void BeginGame()
    {

    }

}
