using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LifeController : MonoBehaviour
{
	public int lifeMaxValue;
	public Slider lifeSlider;
	public Text lifeText;

	private int _currLifeValue;

    // Start is called before the first frame update
    void Start()
    {
		_currLifeValue = lifeMaxValue;

		// 初始化UI
		SetUI();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

	private void OnTriggerEnter(Collider other)
	{
		var obj = other.gameObject;
		if (obj.tag.Equals("Bullet"))
		{
			var controller = obj.GetComponent<BulletController>();
			int damage = 0;
			if (controller)
			{
				damage = controller.BulletDamage;
			}
			if (_currLifeValue > damage)
				_currLifeValue -= damage;
			else
			{
				_currLifeValue = 0;
				Dead();
			}

			// set UI
			SetUI();

			Destroy(obj);
		}
	}

	private void SetUI()
	{
		if (lifeSlider)
			lifeSlider.value = (100 / lifeMaxValue) * _currLifeValue;
		if (lifeText)
			lifeText.text = _currLifeValue.ToString();
	}

	private void Dead()
	{

	}
}
