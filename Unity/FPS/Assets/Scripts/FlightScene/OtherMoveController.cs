using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OtherMoveController : MonoBehaviour
{
    [Header("Move Attribute")]
	public float walkSpeed = 5f;
	public float runSpeed = 9f;
	public float jumpSpeed = 5f;
	[Tooltip("Approximately the amount of time it will take for the player to reach maximum running or walking speed."), SerializeField]
	private float movementSmoothness = 0.125f;

	[Header("Look Settings")]
	[Tooltip("Rotation speed of the fps controller."), SerializeField]
	private float mouseSensitivity = 4f;
	[Tooltip("Approximately the amount of time it will take for the fps controller to reach maximum rotation speed."), SerializeField]
	private float rotationSmoothness = 0.05f;
	[Tooltip("Minimum rotation of the arms and camera on the x axis."), SerializeField]
	private float minVerticalAngle = -90f;
	[Tooltip("Maximum rotation of the arms and camera on the axis."), SerializeField]
	private float maxVerticalAngle = 90f;

	[Header("Component")]
	public Animator playerAnimator;
	public AudioSource audioSource;
	public AudioClip moveClip;
	public AudioClip runClip;
	private CapsuleCollider _collider;

	[Header("Other")]
	public float groundOffset;


	private Rigidbody _rigidbody;
	private SmoothRotation _rotationX;
	private SmoothRotation _rotationY;
	private SmoothVelocity _velocityX;
	private SmoothVelocity _velocityY;
	private SmoothVelocity _velocityZ;

	private bool _isGround;
	private bool _isJump;

    private float[] _moveData;
    private bool _isRun;

	private Vector3 _rightPos;
	private Vector3 _rightDir;

	// Start is called before the first frame update
	void Start()
    {
		_rigidbody = GetComponent<Rigidbody>();
		_collider = GetComponent<CapsuleCollider>();

		//初始化运动参数
		_velocityX = new SmoothVelocity();
		_velocityY = new SmoothVelocity();
		_velocityZ = new SmoothVelocity();

		_isGround = true;
		_isJump = false;

        _moveData = new float[2];
	}

    // Update is called once per frame
    void Update()
    {
		CheckIfOnGround();
        AnimatorChange(_moveData[0], _moveData[1], _isRun);
		moveCharactor();

		_moveData[0] = 0;
		_moveData[1] = 0;
		_isRun = false;
	}

	void FixedUpdate()
	{
		playerAnimator.SetBool("IsGround", _isGround);
	}

	public void moveCharactor()
	{
		if(Vector3.Distance(_rightPos, transform.position) <= 0.01f) 
		{
			Debug.Log(_rightPos);
			Debug.Log(transform.position);
			return;
		}
		var velocity = _isRun ? runSpeed : walkSpeed;
		var d = Vector3.Lerp(transform.position, _rightPos, velocity*Time.deltaTime);
		d.y = transform.position.y;
		transform.Translate(d);
	}

	public void SetMoveData(float move, float strafe, bool run)
	{
		_moveData[0] = move;
        _moveData[1] = strafe;
        _isRun = run;
	}

	public void AnimatorChange(float move, float strafe, bool run)
	{
		// change animation
		float speed = (move != 0 || strafe != 0) ? 1 : 0;
		speed += (speed != 0 && run) ? 1 : 0;

		float Vertical = move == 1 ? 0 : (move == -1 ? 2 : -1);
		float Horizonal = strafe == 1 ? 3 : (strafe == -1 ? 1 : -1);
		float direction = 0;
		if (Vertical != -1 && Horizonal != -1)
		{
			if(Vertical == 0)
				direction = Horizonal == 3 ? 3.5f : 0.5f;
			else
				direction = Horizonal == 3 ? 1.5f : 2.5f;
		}
		else 
			direction = (Vertical != -1) ? Vertical : (Horizonal != -1 ? Horizonal : 0);

		playerAnimator.SetFloat("Speed", speed);
		playerAnimator.SetFloat("Direction", direction);

		// 播放声音
		bool isMove = move != 0 || strafe != 0;
		if(audioSource && isMove)
		{
			if (run) audioSource.clip = runClip;
			else audioSource.clip = moveClip;
			if (!audioSource.isPlaying) audioSource.Play();
		}
		else if(audioSource)
		{
			audioSource.Stop();
		}
	}

	public void Jump()
	{
		_isJump = !(_rigidbody.velocity.y <= 0);
		if (_isGround && !_isJump) 
		{
			_rigidbody.AddForce(jumpSpeed * Vector3.up, ForceMode.Impulse);
			playerAnimator.SetTrigger("Jump");
			_isJump = true;
		}
	}

	public void RotateCharactor(float RotateX, float RotateY)
	{
		RotateX *= mouseSensitivity;
		this.transform.Rotate(new Vector3(0f, RotateX, 0f));
	}

	void CheckIfOnGround()
	{
		var point = transform.position + _collider.center;
		var layerMask = LayerMask.GetMask("Ground");
		_isGround = Physics.CapsuleCast(point, point, _collider.radius, Vector3.down, _collider.height / 2 + groundOffset, layerMask);
	}

	public Vector3 RightPos
	{
		set { _rightPos = value; }
	}

	public Vector3 RightDir
	{
		set { _rightDir = value; }
	}


	/// A helper for assistance with smoothing the camera rotation.
	private class SmoothRotation
	{
		private float _current;
		private float _currentVelocity;

		public SmoothRotation(float startAngle)
		{
			_current = startAngle;
		}

		/// Returns the smoothed rotation.
		public float Update(float target, float smoothTime)
		{
			return _current = Mathf.SmoothDampAngle(_current, target, ref _currentVelocity, smoothTime);
		}

		public float Current
		{
			set { _current = value; }
		}
	}

	/// A helper for assistance with smoothing the movement.
	private class SmoothVelocity
	{
		private float _current;
		private float _currentVelocity;

		/// Returns the smoothed velocity.
		public float Update(float target, float smoothTime)
		{
			return _current = Mathf.SmoothDamp(_current, target, ref _currentVelocity, smoothTime);
		}

		public float Current
		{
			set { _current = value; }
		}
	}
}
