using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class BehaviorController : MonoBehaviour
{
	[Header("Enemy Attribute")]
	public float attackDistance;
	public float discoverDistance = 10.0f;
	public float discoverAngle = 90.0f;
	public float removeAttackDistance = 5.0f;
	public float groundOffset = 0.01f;

	[Header("Move Attribute")]
	public float moveSpeed;
	public float rotateSpeed;

	[Header("Weapon Attribute")]
	public float fireInterval;

	[Header("Audio Attribute")]
	public AudioSource moveAudio; 
	public Text dizzyText;

	protected bool _attackMode;
	protected float _nextFireTime;
	protected bool _reset;
	protected Vector3 _targetPos;
	protected bool _invincible;

	protected Animator _animator;
	protected Coroutine _moveCoroutine;
	protected Coroutine _rotateCoroutine;
	protected CapsuleCollider _collider;

	protected NetworkSocket _network;

	protected bool _isDazzy;

    // Start is called before the first frame update
    public virtual void Start()
    {
		_attackMode = false;
		_reset = false;
		_targetPos = new Vector3();
		_isDazzy = false;
		_animator = GetComponent<Animator>();
		_collider = GetComponent<CapsuleCollider>();
		_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();

	}

    // Update is called once per frame
    public virtual void Update()
    {
		_animator.SetBool("IsGround", CheckIfOnGround());
	}

	public void CommonBehavior(EnemyBehavior eh)
	{
		_targetPos = eh.pos;
		_invincible = true;
		_reset = false;	
		_attackMode = false;
		dizzyText.text = "";
		if (_moveCoroutine != null) StopCoroutine(_moveCoroutine);
		if (_rotateCoroutine != null) StopCoroutine(_rotateCoroutine);
	}

	public void AttackBehavior(EnemyBehavior eh)
	{
		_targetPos = eh.pos;
		_invincible = false;
		dizzyText.text = "";
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
		_targetPos = eh.pos;
		dizzyText.text = "";
		if(!_reset)
		{
			_attackMode = false;
			_reset = true;
			_invincible = true;
			MoveToPoint(_targetPos);
		}
	}

	public void Dizzy()
	{
		_isDazzy = true;
		dizzyText.text = "眩晕";
	}

	protected virtual void Attack()
	{
		return;
	}

	public virtual void Shoot(NPCShoot ns)
	{
		return;
	}

	public virtual void Prick()
	{
		return;
	}
	
	public void MoveToPoint(Vector3 point)
	{
		_rotateCoroutine = StartCoroutine(SmoothRotateToVector(point));
		
	}

	IEnumerator SmoothMoveToPoint(Vector3 point)
	{
		var stand = transform.position;

		_animator.SetBool("Forward", true);
		PlayMoveAudio(true);
		while (Vector3.Distance(transform.position, point) > 0.5f) 
		{
			if(_isDazzy)
				yield return 0;

			transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);
			yield return 0;
		}
		_animator.SetBool("Forward", false);
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
			if(_isDazzy)
				yield return 0;
			float rotateAngle = Mathf.Abs(angle - currAngle) > rotateSpeed * Time.deltaTime ? rotateSpeed * Time.deltaTime : angle - currAngle;
			transform.Rotate(0.0f, angle > 0 ? rotateAngle : -rotateAngle, 0.0f);
			currAngle += rotateAngle;
			yield return 0;
		}
		_moveCoroutine = StartCoroutine(SmoothMoveToPoint(point));
	}

	protected void PlayMoveAudio(bool play)
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

	protected bool CheckIfOnGround()
	{
		var point = transform.position + _collider.center;
		var layerMask = LayerMask.GetMask("Ground");
		return Physics.CapsuleCast(point, point, _collider.radius, Vector3.down, _collider.height / 2 + groundOffset, layerMask);
	}

	public bool Invincible
	{
		set { _invincible = value; }
		get { return _invincible; }
	}
}
