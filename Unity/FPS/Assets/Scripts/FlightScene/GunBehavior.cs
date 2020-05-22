using System.Collections;
using UnityEngine;

public class GunBehavior : BehaviorController
{
	[Header("Weapon Attribute")]
	public GameObject bulletPos;
	public WeaponController weapon;

	[Header("Effect")]
	public AudioSource shootAudio;
	public AudioClip shootClip;
	public AudioClip reloadClip;
	public ParticleSystem shootEffect;

	private bool _IsReloading;

	private Coroutine _reloadCompleted;
	private GameObject _bulletPrefab;


    // Start is called before the first frame update
    public override void Start()
    {
		base.Start();
		_IsReloading = false;
		_bulletPrefab = (GameObject)Resources.Load("Prefabs/bullet");
	}

	protected override void Attack()
	{
		var point = transform.position + _collider.center;
		var direction = _targetPos - transform.position;
		var distance = Vector3.Distance(transform.position, _targetPos);
		var layerMask = LayerMask.GetMask("Ground");
		var hasObstacle = Physics.CapsuleCast(point, point, _collider.radius, direction, distance, layerMask);

		// 在攻击范围 前方无障碍物
		if (distance <= attackDistance && !hasObstacle)
		{
			int eid = gameObject.GetComponent<EnemyEntity>().eid;
			EntityEid ee;
			ee.eid = eid;
			_network.send(Config.COMMAND_NPC_SHOOT, JsonUtility.ToJson(ee));
		}

		// 不在攻击范围 前方无障碍物 移动
		else if(distance > attackDistance && !hasObstacle)
		{
			direction.y = 0;
			float angle = Vector3.Angle(transform.forward, direction);
			Vector3 cross = Vector3.Cross(transform.forward, direction);
			angle = cross.y > 0 ? angle : -angle;
			transform.Rotate(0.0f, angle, 0.0f);
			transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);
			_animator.SetBool("Forward", true);
			PlayMoveAudio(true);
		}
		
		// 前方有障碍物
		else if(hasObstacle)
		{
			transform.Translate(transform.forward * moveSpeed * Time.deltaTime);
			_animator.SetBool("Forward", true);
			PlayMoveAudio(true);
		}
	}

	public override void Shoot(NPCShoot ns)
	{
		_targetPos = ns.pos;
		var direction = _targetPos - transform.position;
		direction.y = 0;
		float angle = Vector3.Angle(transform.forward, direction);
		Vector3 cross = Vector3.Cross(transform.forward, direction);
		angle = cross.y > 0 ? angle : -angle;
		transform.Rotate(0.0f, angle, 0.0f);
		ShootPlayer(ns.eid, ns.bulletEid);
		_animator.SetBool("Forward", false);
		PlayMoveAudio(false);
	}

	public void ShootPlayer(int eid, int bulletEid)
	{
		if (_nextFireTime > Time.time) return;
		if (weapon.CurrBullet == 0 && !_IsReloading)
		{
			Reload();
			return;
		}
		if (!weapon.Shoot()) return;

		_nextFireTime = Time.time + fireInterval;

		// 计算方向
		var startPoint = bulletPos.transform.position;
		var endPoint = _targetPos + new Vector3(0, 0.5f, 0);
		var direction = endPoint - startPoint;

		
		_bulletPrefab.transform.position = startPoint;
		GameObject bullet = Instantiate(_bulletPrefab);
		BulletController bc = bullet.GetComponent<BulletController>();
		bc.eid = bulletEid;
		bc.owner = eid;
		bc.AddForce(direction.normalized);

		// 播放声音
		if (shootAudio)
		{
			shootAudio.clip = shootClip;
			shootAudio.Play();
		}
		shootEffect.Play();
	}

	public void Reload()
	{
		_animator.SetTrigger("Reload");
		if (shootAudio)
		{
			shootAudio.clip = reloadClip;
			shootAudio.Play();
		}

		_IsReloading = true;

		if (_reloadCompleted != null)
			StopCoroutine(_reloadCompleted);
		_reloadCompleted = StartCoroutine(ReloadCompleted());
	}

	private IEnumerator ReloadCompleted()
	{
		bool preState = _animator.GetCurrentAnimatorStateInfo(1).IsTag("reload");
		bool currState = _animator.GetCurrentAnimatorStateInfo(1).IsTag("reload");

		while (!(preState && !currState))
		{
			preState = currState;
			currState = _animator.GetCurrentAnimatorStateInfo(1).IsTag("reload");
			yield return 0;
		}
		weapon.Reload();
		_IsReloading = false;
	}

}
