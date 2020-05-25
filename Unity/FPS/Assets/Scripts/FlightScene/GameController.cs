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

    public GameObject endPlane;

    private FpsInput m_input;
    private AsyncOperation endLoad;

    // Start is called before the first frame update
    void Start()
    {
        Service.Instance().endGameCallback = new Service.RecvHandler(this.EndGame);
    }

    // Update is called once per frame
    void Update()
    {
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

        Cursor.visible = true;
        PlayerPrefs.SetString("fightEnd","end");
    }

    public void EndButtonClick()
    {
        endLoad.allowSceneActivation = true;
    }
}
