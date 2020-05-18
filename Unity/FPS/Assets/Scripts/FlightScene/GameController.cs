using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GameController : MonoBehaviour
{
    [Header("UI Component")]
	public Text textBullet;
	public Text textWeaponMode;
	public Slider lifeSlider;
	public Text lifeText;

    public Text RewardMtext;
    public Text RewardBtext;

    private FpsInput m_input;

    // Start is called before the first frame update
    void Start()
    {
        
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
}
