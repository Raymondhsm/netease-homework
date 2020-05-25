using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class StartGameController : MonoBehaviour
{
    public GameObject loginPlane;
    public InputField inpAccount;
    public InputField inpPassword;
    public Text txtTips;
    public Text txtInfo;

    private NetworkSocket m_network;
    private bool isLogin;
    private float _nextUpdateTime;

    // Start is called before the first frame update
    void Start()
    {
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
        Service.Instance().loginRecvCallback = new Service.RecvHandler(this.LoginCallback);
        Service.Instance().registerRecvCallback = new Service.RecvHandler(this.RegisterCallback);
        Service.Instance().updateInfoCallback = new Service.RecvHandler(this.updateInfoCallback);
        Service.Instance().beginGameCallback = new Service.RecvHandler(this.BeginGameCallback);
        Service.Instance().checkLogin = new Service.RecvHandler(this.CheckLogin);

        if(PlayerPrefs.HasKey("sessionID") && PlayerPrefs.HasKey("fightEnd"))
        {
            string sID = PlayerPrefs.GetString("sessionID");
            m_network.send(Config.COMMAND_CHECK_LOGIN, "{\"sessionID\":\""+sID+"\"}");
            PlayerPrefs.DeleteKey("fightEnd");
        }
        else
        {
            loginPlane.SetActive(true);
        }

        isLogin = false;
        _nextUpdateTime = 0;
    }

    void Update()
    {
        _nextUpdateTime += Time.deltaTime;
        if(_nextUpdateTime >= 5)
        {
            string sID = PlayerPrefs.GetString("sessionID");
            m_network.send(Config.COMMAND_UPDATE_INFO, "{\"sessionID\":\""+sID+"\"}");
            _nextUpdateTime = 0;
        }
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
            PlayerPrefs.SetString("sessionID", loginRecv.sessionID);
            m_network.send(Config.COMMAND_UPDATE_INFO, "{\"sessionID\":\""+loginRecv.sessionID+"\"}");
            isLogin = true;
        }
        else
        {
            txtTips.text = loginRecv.sessionID;
        }
    }

    public void CheckLogin(string recv)
    {
        StatusStruct ss = JsonUtility.FromJson<StatusStruct>(recv);
        if(ss.status)
        {
            loginPlane.SetActive(false);
            string sID = PlayerPrefs.GetString("sessionID");
            m_network.send(Config.COMMAND_UPDATE_INFO, "{\"sessionID\":\""+sID+"\"}");
            isLogin = true;
        }
        else
        {
            loginPlane.SetActive(true);
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
        RegisterRecv rr = JsonUtility.FromJson<RegisterRecv>(registerRecvStr);
        if(rr.registerStatus)
        {
            txtTips.text = "注册成功";
        }
        else
        {
            txtTips.text = rr.registerInfo;
        }
    }

    public void updateInfoCallback(string RecvStr)
    {
        UserInfo info = JsonUtility.FromJson<UserInfo>(RecvStr);
        txtInfo.text = "血量：" + info.blood.ToString() + "/100    子弹：" + info.bullet.ToString() + "/210    等级：" + info.level.ToString() + "    经验：" + info.experience.ToString() + "/" + (500 + info.level * 100).ToString();
    }

    public void BeginGame()
    {
        if(!isLogin)return;
        string sessionId = PlayerPrefs.GetString("sessionID");
        m_network.send(Config.COMMAND_ATTEND_GAME, "{\"sessionID\":\""+sessionId+"\"}");
    }

    public void BeginGameCallback(string RecvStr)
    {
        StatusStruct ss = JsonUtility.FromJson<StatusStruct>(RecvStr);
        if(ss.status)
            SceneManager.LoadSceneAsync(1);
    }

}
