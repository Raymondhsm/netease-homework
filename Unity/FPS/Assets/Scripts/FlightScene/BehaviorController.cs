using System.Collections;
using UnityEngine;

public class BehaviorController : MonoBehaviour
{
	[Header("Enemy Attribute")]
	public float attackDistance;
	public float discoverDistance;
	public float discoverAngle;
	public float removeAttackDistance;
	public float groundOffset = 0.01f;

	[Header("Move Atrribute")]
	public float moveSpeed;
	public float rotateSpeed;

	[Header("Weapon Attribute")]
	public GameObject bulletPos;
	public float fireInterval;
	public WeaponController weapon;

	[Header("Effect")]
	public AudioSource shootAudio;
	public AudioSource moveAudio;
	public AudioClip shootClip;
	public AudioClip reloadClip;
	public AudioClip moveClip;
	public ParticleSystem shootEffect;

	private bool _attackMode;
	private float _nextFireTime;
	private bool _reset;
	private bool _IsReloading;
	private Vector3 _targetPos;

	private Animator _animator;
	private Coroutine _moveCoroutine;
	private Coroutine _rotateCoroutine;
	private CapsuleCollider _collider;
	private Coroutine _reloadCompleted;

    // Start is called before the first frame update
    void Start()
    {
		_attackMode = false;
		_reset = false;
		_IsReloading = false;
		_targetPos = new Vector3();
		_animator = GetComponent<Animator>();
		_collider = GetComponent<CapsuleCollider>();

		moveAudio.clip = moveClip;
	}

    // Update is called once per frame
    void Update()
    {
		_animator.SetBool("IsGround", CheckIfOnGround());

		// Behavior();
	}

	public void CommonBehavior(EnemyBehavior eh)
	{
		Debug.Log("common");
		_targetPos = eh.pos;
		_reset = false;	
		_attackMode = false;
		if (_moveCoroutine != null) StopCoroutine(_moveCoroutine);
		if (_rotateCoroutine != null) StopCoroutine(_rotateCoroutine);
	}

	public void AttackBehavior(EnemyBehavior eh)
	{
		Debug.Log("attack");
		_targetPos = eh.pos;
		if(_reset)
		{
			_attackMode = true;
			_reset = false;
			if (_moveCoroutine != null) StopCoroutine(_moveCoroutine);
			if (_rotateCoroutine != null) StopCoroutine(_rotateCoroutine);
		}
		else 
			Attack();
	}

	public void ResetBehavior(EnemyBehavior eh)
	{
		Debug.Log("reset");
		_targetPos = eh.pos;
		if(!_reset)
		{
			_attackMode = false;
			_reset = true;
			MoveToPoint(_targetPos);
		}
	}

	public void MoveToPoint(Vector3 point)
	{
		_rotateCoroutine = StartCoroutine(SmoothRotateToVector(point));
		
	}

	IEnumerator SmoothMoveToPoint(Vector3 point)
	{
		var stand = transform.position;

		_animator.SetBool("IsForward", true);
		PlayMoveAudio(true);
		while (Vector3.Distance(transform.position, point) > 0.5f) 
		{
			transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);
			yield return 0;
		}
		_animator.SetBool("IsForward", false);
		PlayMoveAudio(false);
	}

	IEnumerator SmoothRotateToVector(Vector3 point)
	{
		// 计算角度
		var stand = transform.position;
		var direction = (point - stand).normalized;
		direction.y = 0f;
		float angle = Vector3.Angle(transform.forward, direction);
		Vector3 cross = Vector3.Cross(transform.forward, direction);
		angle = cross.y > 0 ? angle : -angle;
		
		// 旋转
		float currAngle = 0;
		while(Mathf.Abs(currAngle) < Mathf.Abs(angle))
		{
			float rotateAngle = Mathf.Abs(angle - currAngle) > rotateSpeed * Time.deltaTime ? rotateSpeed * Time.deltaTime : angle - currAngle;
			transform.Rotate(0.0f, angle > 0 ? rotateAngle : -rotateAngle, 0.0f);
			currAngle += rotateAngle;
			yield return 0;
		}
		_moveCoroutine = StartCoroutine(SmoothMoveToPoint(point));
	}


	public void Attack()
	{
		var point = transform.position + _collider.center;
		var direction = _targetPos - transform.position;
		var distance = Vector3.Distance(transform.position, _targetPos);
		var layerMask = LayerMask.GetMask("Ground");
		var hasObstacle = Physics.CapsuleCast(point, point, _collider.radius, direction, distance, layerMask);

		// 在攻击范围 前方无障碍物
		if (distance <= attackDistance && !hasObstacle)
		{
			direction.y = 0;
			float angle = Vector3.Angle(transform.forward, direction);
			Vector3 cross = Vector3.Cross(transform.forward, direction);
			angle = cross.y > 0 ? angle : -angle;
			transform.Rotate(0.0f, angle, 0.0f);
			ShootPlayer();
			_animator.SetBool("IsForward", false);
			PlayMoveAudio(false);
		}

		// 不在攻击范围 前方无障碍物 移动
		else if(distance > attackDistance && !hasObstacle)
		{
			direction.y = 0;
			float angle = Vector3.Angle(transform.forward, direction);
			Vector3 cross = Vector3.Cross(transform.forward, direction);
			angle = cross.y > 0 ? angle : -angle;
			transform.Rotate(0.0f, angle, 0.0f);
			transform.Translate(transform.forward * moveSpeed * Time.deltaTime);
			_animator.SetBool("IsForward", true);
			PlayMoveAudio(true);
		}
		
		// 前方有障碍物
		else if(hasObstacle)
		{
			transform.Translate(transform.forward * moveSpeed * Time.deltaTime);
			_animator.SetBool("IsForward", true);
			PlayMoveAudio(true);
		}
	}

	public void ShootPlayer()
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

		GameObject bulletPrefab = (GameObject)Resources.Load("Prefabs/bullet");
		bulletPrefab.transform.position = startPoint;
		GameObject bullet = Instantiate(bulletPrefab);
		bullet.GetComponent<BulletController>().AddForce(direction.normalized);

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

	private void PlayMoveAudio(bool play)
	{
		if(play)
		{
			if(!moveAudio.isPlaying)
			{
				moveAudio.Play();
			}
		}
		else
		{
			moveAudio.Stop();
		}
	}

	public void Dead()
	{

	}

	bool CheckIfOnGround()
	{
		var point = transform.position + _collider.center;
		var layerMask = LayerMask.GetMask("Ground");
		return Physics.CapsuleCast(point, point, _collider.radius, Vector3.down, _collider.height / 2 + groundOffset, layerMask);
	}

}
