using System;
using System.Collections;
using UnityEngine;
using UnityEngine.UI;

public class ShootController : MonoBehaviour
{
	[Header("Component")]
	public Animator playerAnimator;
	public Camera camera;
	public GameObject bulletPos;

	[Header("Shoot Effect")]
	public AudioSource shootAudio;
	public AudioClip shootClip;
	public AudioClip reloadClip;
	public ParticleSystem shootEffect;

	[Header("Shooting Attribute")]
	public float weaponRange = 500f;
	public float fireInterval = 100f;

	[Header("UI Component")]
	private Text textBullet;
	private Text textWeaponMode;

	[SerializeField]
	private ShootingInput input;

	[SerializeField]
	private WeaponController weapon;

	private GameObject _bulletPrefab;	
	private bool _singleShooting;
	private float _nextFireTime;
	private Coroutine _reloadCompleted;
	private bool _IsReloading;
	private bool _reloadRecv;

    // Start is called before the first frame update
    void Start()
    {
		_singleShooting = true;
		_nextFireTime = Time.time;
		_IsReloading = false;
		_reloadRecv = false;

		textBullet = GameObject.Find("GameController").GetComponent<GameController>().textBullet;
		textWeaponMode = GameObject.Find("GameController").GetComponent<GameController>().textWeaponMode;

		_bulletPrefab = (GameObject)Resources.Load("Prefabs/bullet");
		// 初始化UI
		SetWeaponBulletText(weapon.CurrBullet);
    }

    // Update is called once per frame
    void Update()
    {
		if (input.Reload) Reload();
		if (input.Mode) ChangeMode();

		Shooting();
	}

	private void Reload()
	{
		playerAnimator.SetTrigger("Reload");

		if(shootAudio)
		{
			shootAudio.clip = reloadClip;
			shootAudio.Play();
		}

		_IsReloading = true;
		if(_reloadCompleted != null)
			StopCoroutine(_reloadCompleted);
		_reloadCompleted = StartCoroutine(ReloadCompleted());

		_reloadRecv = false;
		gameObject.GetComponent<PlayerEntity>().ReloadUpload();
	}

	private IEnumerator ReloadCompleted()
	{
		bool preState = playerAnimator.GetCurrentAnimatorStateInfo(1).IsTag("reload");
		bool currState = playerAnimator.GetCurrentAnimatorStateInfo(1).IsTag("reload");
		float timeout = 0;

		while ((preState || currState) || !_reloadRecv)
		{
			preState = currState;
			currState = playerAnimator.GetCurrentAnimatorStateInfo(1).IsTag("reload");
			timeout += Time.deltaTime;
			if(timeout > 5) StopCoroutine(this._reloadCompleted);
			yield return 0;
		}
		int currBullet = weapon.Reload();
		SetWeaponBulletText(currBullet);
		_IsReloading = false;
	}

	public void ReloadRecv()
	{
		_reloadRecv = true;
	}

	private void SetWeaponBulletText(int currBullet)
	{
		textBullet.text = currBullet + "/" + weapon.TotalBullet;
	}

	private void ChangeMode()
	{
		weapon.ChangeMode();
		if (weapon.WeaponMode == 0)
		{
			_singleShooting = true;
			textWeaponMode.text = "单点";
		}
		else if (weapon.WeaponMode == 1)
		{
			_singleShooting = false;
			textWeaponMode.text = "连射";
		}
	}

	private void Shooting()
	{
		if (_nextFireTime > Time.time || _IsReloading || !weapon.canShoot()) return;

		// 两种模式
		if((_singleShooting && input.LeftButtonDown) || (!_singleShooting && input.LeftButtonHold))
		{
			// if (!weapon.Shoot()) return;
			_nextFireTime = Time.time + fireInterval;

			Vector3 rayOrigin = camera.ViewportToWorldPoint(new Vector3(0.5f, 0.5f, 0.0f));
			RaycastHit hit;
			Vector3 endPoint;

			// 检测射线是否碰撞到对象
			if (Physics.Raycast(rayOrigin, camera.transform.forward, out hit, weaponRange))
			{
				endPoint = hit.point;
			}
			else
			{
				endPoint = rayOrigin + (camera.transform.forward * weaponRange);
			}

			gameObject.GetComponent<PlayerEntity>().ShootUpload(endPoint);
		}

	}

	public void ShootRecv(int bulletEid, Vector3 endPoint)
	{
		weapon.Shoot();
		SetWeaponBulletText(weapon.CurrBullet);
		// 计算方向
		var startPoint = bulletPos.transform.position;
		var direction = endPoint - startPoint;

		_bulletPrefab.transform.position = startPoint;
		GameObject bullet= Instantiate(_bulletPrefab);
		BulletController bc = bullet.GetComponent<BulletController>();
		bc.StaticHitPos = endPoint;
		bc.eid = bulletEid;
		bc.owner = 0;
		bc.AddForce(direction.normalized);

		// 播放声音
		if (shootAudio)
		{
			shootAudio.clip = shootClip;
			shootAudio.Play();
		}
		shootEffect.Play();

		if (_singleShooting) playerAnimator.SetTrigger("IsShoot");
		else playerAnimator.SetBool("IsShootBrust", true);
	}

	public int CurrBullet 
	{
		get { return weapon.CurrBullet; }
	}

	public int TatolBullet
	{
		get { return weapon.TotalBullet; }
	}

	[Serializable]
	private class ShootingInput
	{
		[Tooltip("shoot"), SerializeField]
		private string shoot = "Fire1";

		[Tooltip("reload"), SerializeField]
		private string reload = "Reload";

		[Tooltip("Mode"), SerializeField]
		private string mode = "Mode";

		public bool Shoot
		{
			get { return LeftButtonDown || LeftButtonHold; }
		}

		public bool LeftButtonDown
		{
			get { return Input.GetButtonDown(shoot); }
		}

		public bool LeftButtonHold
		{
			get { return Input.GetButton(shoot); }
		}

		public bool Mode
		{
			get { return Input.GetButtonDown(mode); }
		}

		public bool Reload
		{
			get { return Input.GetButtonDown(reload); }
		}
	}
}
