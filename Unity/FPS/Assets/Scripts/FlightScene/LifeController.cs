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
	private NetworkSocket _network;


    // Start is called before the first frame update
    void Start()
    {
		_currLifeValue = lifeMaxValue;
		_clientDead = false;
		_playerAnimator = gameObject.GetComponent<Animator>();
		_entityController = GameObject.Find("GameController").GetComponent<EntityController>();
		_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();

		if(gameObject.CompareTag("Player"))
		{
			lifeSlider = GameObject.Find("GameController").GetComponent<GameController>().lifeSlider;
			lifeText = GameObject.Find("GameController").GetComponent<GameController>().lifeText;
		}

		// 初始化UI
		SetUI();
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetButtonDown("Reload")){
            _playerAnimator.SetTrigger("Dead");
        }
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

			_currLifeValue -= damage;
			if(_currLifeValue <= 0)
			{
				_clientDead = true;
			}

			EntityHit entityHit;
			entityHit.eid = gameObject.GetComponent<Entity>().eid;
			entityHit.bulletEid = obj.GetComponent<BulletController>().eid;
			entityHit.bulletDamage = damage;
			_network.send(Config.COMMAND_HIT, JsonUtility.ToJson(entityHit));

			Destroy(obj);
		}
	}

	public void ProcessLifeRecv(int lifeValue)
	{
		_currLifeValue = lifeValue;
		if(lifeValue > 0)
		{
			_clientDead = false;
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

	public void Dead()
	{
		_playerAnimator.SetTrigger("Dead");
		_currLifeValue = 0;
		_clientDead = true;
		SetUI();
	}

	public int CurrLife
	{
		get {
			return _currLifeValue;
			}
	}
}
