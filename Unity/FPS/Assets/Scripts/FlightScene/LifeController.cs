﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class LifeController : MonoBehaviour
{
	public int lifeMaxValue;
	public Slider lifeSlider;
	public Text lifeText;
	public BoxCollider knifeCollider;

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

		knifeCollider = null;
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
			if(gameObject.CompareTag("Player") && obj.GetComponent<BulletController>().owner == 0)
				return;
			if(gameObject.CompareTag("Enemy") && obj.GetComponent<BulletController>().owner == 1)
				return;

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
		else if(obj.CompareTag("Knife"))
		{
			if(gameObject.CompareTag("Enemy"))
				return;

			EntityHit entityHit;
			entityHit.eid = gameObject.GetComponent<Entity>().eid;
			entityHit.bulletEid = obj.GetComponentInParent<Entity>().eid;
			entityHit.bulletDamage = 3;
			_network.send(Config.COMMAND_HIT, JsonUtility.ToJson(entityHit));

			if(knifeCollider != null)
				knifeCollider.enabled = false;
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
		CapsuleCollider cc = gameObject.GetComponent<CapsuleCollider>();
		if(cc)
			cc.enabled = false;
		Rigidbody gb = gameObject.GetComponent<Rigidbody>();
		if(gb)
			gb.useGravity = false;
		_playerAnimator.SetTrigger("Dead");
		_currLifeValue = 0;
		_clientDead = true;
		SetUI();
		Destroy(gameObject, 5);
	}

	public int CurrLife
	{
		get {
			return _currLifeValue;
			}
	}
}
