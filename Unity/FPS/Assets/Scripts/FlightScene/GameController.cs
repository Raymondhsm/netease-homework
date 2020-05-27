using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class GameController : MonoBehaviour
{
    [Header("UI Component")]
	public Text textBullet;
	public Text textWeaponMode;
	public Slider lifeSlider;
	public Text lifeText;

    public Text RewardMtext;
    public Text RewardBtext;
    public Text MagicText;

    public GameObject endPlane;

    private FpsInput m_input;
    private ShootingInput m_sInput;
    private AsyncOperation endLoad;
    private NetworkSocket m_network;
    private float m_magicTime;
    private Controller m_controller;
    private bool m_endGame;

    // Start is called before the first frame update
    void Start()
    {
        Service.Instance().endGameCallback = new Service.RecvHandler(this.EndGame);
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
        m_controller = GameObject.Find("Controller").GetComponent<Controller>();
        m_magicTime = 0;
        m_input = new FpsInput();
        m_sInput = new ShootingInput();

        Cursor.lockState = CursorLockMode.Locked;
    }

    // Update is called once per frame
    void Update()
    {
        SetMagicTime();

        if(m_input.ESC)
        {
            Cursor.lockState = CursorLockMode.None;
            Cursor.visible = true;
        }
        if(m_sInput.Shoot && !m_endGame)
        {
            if(Cursor.lockState == CursorLockMode.None)
                Cursor.lockState = CursorLockMode.Confined;
            else
                Cursor.lockState = CursorLockMode.Locked;
            Cursor.visible = false;
        }
    }

    public void UpdateProp(int m, int b)
    {
        RewardMtext.text = m.ToString();
        RewardBtext.text = b.ToString();
    }

    public void EndGame(string recv)
    {
        endGameInfo info = JsonUtility.FromJson<endGameInfo>(recv);

        endPlane.SetActive(true);
        Text[] txtEnd = endPlane.GetComponentsInChildren<Text>();
        txtEnd[0].text = "使用子弹：" + info.useBullet;
        txtEnd[1].text = "剩余子弹：" + info.leftBullet;
        txtEnd[2].text = "剩余生命：" + info.leftLife;
        txtEnd[3].text = "对敌伤害：" + info.damage;
        txtEnd[4].text = "获得经验：" + info.experience;

        endLoad = SceneManager.LoadSceneAsync(0);
        endLoad.allowSceneActivation = false;

        Cursor.lockState = CursorLockMode.None;
        Cursor.visible = true;

        m_endGame = true;
    }

    public void EndButtonClick()
    {
        endLoad.allowSceneActivation = true;
        m_controller.fightEnd = true;
    }

    public void QuitButtonClick()
    {
        m_network.send(Config.COMMAND_PLAYER_QUIT,"{}");
    }

    public void SetMagicTime()
    {
        m_magicTime -= Time.deltaTime;
        if(m_magicTime <=0 ) m_magicTime = 0;
        MagicText.text = ((int)m_magicTime).ToString();
    }

    public float MagicTime
    {
        set { m_magicTime = value; }
    }
}
