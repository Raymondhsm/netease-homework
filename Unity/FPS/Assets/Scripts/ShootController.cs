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
	public Text textBullet;
	public Text textWeaponMode;

	[SerializeField]
	private ShootingInput input;

	[SerializeField]
	private WeaponController weapon;

	private bool _singleShooting;
	private float _nextFireTime;
	private Coroutine _reloadCompleted;
	private bool _IsReloading;

    // Start is called before the first frame update
    void Start()
    {
		_singleShooting = true;
		_nextFireTime = Time.time;
		_IsReloading = false;

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
	}

	private IEnumerator ReloadCompleted()
	{
		bool preState = playerAnimator.GetCurrentAnimatorStateInfo(1).IsTag("reload");
		bool currState = playerAnimator.GetCurrentAnimatorStateInfo(1).IsTag("reload");

		while (!(preState && !currState))
		{
			preState = currState;
			currState = playerAnimator.GetCurrentAnimatorStateInfo(1).IsTag("reload");
			yield return 0;
		}
		int currBullet = weapon.Reload();
		SetWeaponBulletText(currBullet);
		_IsReloading = false;
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
		bool hasShoot = false;
		if (_nextFireTime > Time.time || _IsReloading) return;

		// 两种模式
		if((_singleShooting && input.LeftButtonDown) || (!_singleShooting && input.LeftButtonHold))
		{
			if (!weapon.Shoot()) return;
			_nextFireTime = Time.time + fireInterval;
			SetWeaponBulletText(weapon.CurrBullet);

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

			// 计算方向
			var startPoint = bulletPos.transform.position;
			var direction = endPoint - startPoint;

			GameObject bulletPrefab = (GameObject)Resources.Load("Prefabs/bullet");
			bulletPrefab.transform.position = startPoint;
			GameObject bullet= Instantiate(bulletPrefab);
			bullet.GetComponent<Rigidbody>().AddForce(direction.normalized*10f ,ForceMode.Impulse);

			// 播放声音
			if (shootAudio)
			{
				shootAudio.clip = shootClip;
				shootAudio.Play();
			}
			shootEffect.Play();

			if (_singleShooting) playerAnimator.SetTrigger("IsShoot");
			else hasShoot = true;
		}

		playerAnimator.SetBool("IsShootBrust", hasShoot);
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
