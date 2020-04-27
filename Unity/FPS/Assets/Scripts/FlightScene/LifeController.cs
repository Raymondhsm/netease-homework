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
	private bool _clientDead;
	private Animator _playerAnimator;
	private EntityController _entityController;


    // Start is called before the first frame update
    void Start()
    {
		_currLifeValue = lifeMaxValue;
		_clientDead = false;
		_playerAnimator = gameObject.GetComponent<Animator>();
		_entityController = GameObject.Find("GameController").GetComponent<EntityController>();

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
				_clientDead = true;
			}

			Destroy(obj);
		}
	}

	public void ProcessLifeRecv(int lifeValue)
	{
		_currLifeValue = lifeValue;
		if(lifeValue == 0)
		{
			Dead();
		}
		SetUI();
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
		_playerAnimator.SetTrigger("Dead");
		Destroy(gameObject, 5);
		_entityController.EntityDead(gameObject.GetComponent<Entity>().eid);
	}

	public int CurrLife
	{
		get {return _currLifeValue;}
	}
}
