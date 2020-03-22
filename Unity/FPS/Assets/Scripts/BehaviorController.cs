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
	public GameObject OriginPos;

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

	[Header("Other")]
	public GameObject Player;

	private bool _attackMode;
	private float _nextFireTime;
	private bool _reset;
	private bool _IsReloading;

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
		_animator = GetComponent<Animator>();
		_collider = GetComponent<CapsuleCollider>();

		moveAudio.clip = moveClip;
		//MoveToPoint(new Vector3(0.3f, 0.0f, 1.6f));
	}

    // Update is called once per frame
    void Update()
    {
		_animator.SetBool("IsGround", CheckIfOnGround());

		Behavior();
	}

	private void Behavior()
	{
		if(!_attackMode)
		{
			if(_reset)
			{
				_reset = Vector3.Distance(transform.position, OriginPos.transform.position) > 1;
			}
			else if (CheckIfDiscoverPlayer())
			{
				_attackMode = true;
				if (_moveCoroutine != null) StopCoroutine(_moveCoroutine);
				if (_rotateCoroutine != null) StopCoroutine(_rotateCoroutine);
			}
			else
			{

			}
		}
		else if(_attackMode && !CheckToFar())
		{
			Attack();
		}
		else if(_attackMode && CheckToFar())
		{
			_attackMode = false;
			_reset = true;
			MoveToPoint(OriginPos.transform.position);
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

	public bool CheckIfDiscoverPlayer()
	{
		// 检测距离
		float distance = Vector3.Distance(transform.position, Player.transform.position);
		if (distance > discoverDistance)
			return false;

		// 检测角度
		var direction = Player.transform.position - transform.position;
		float angle = Vector3.Angle(transform.forward, direction);
		if (angle > discoverAngle)
			return false;

		// 检测有无障碍物
		RaycastHit hit;
		var point = transform.position + _collider.center;
		var layerMask = LayerMask.GetMask("Ground");
		if (Physics.CapsuleCast(point, point, _collider.radius, direction, out hit, distance, layerMask))
		{
			return false;
		}
		else return true;
	}

	public void Attack()
	{
		var point = transform.position + _collider.center;
		var direction = Player.transform.position - transform.position;
		var distance = Vector3.Distance(transform.position, Player.transform.position);
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
		var endPoint = Player.transform.position + Player.GetComponent<CapsuleCollider>().center / 2;
		var direction = endPoint - startPoint;

		GameObject bulletPrefab = (GameObject)Resources.Load("Prefabs/bullet");
		bulletPrefab.transform.position = startPoint;
		GameObject bullet = Instantiate(bulletPrefab);
		bullet.GetComponent<Rigidbody>().AddForce(direction.normalized * 10f, ForceMode.Impulse);

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

	public bool CheckToFar()
	{
		var distance = Vector3.Distance(transform.position, OriginPos.transform.position);
		return distance > removeAttackDistance;
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
