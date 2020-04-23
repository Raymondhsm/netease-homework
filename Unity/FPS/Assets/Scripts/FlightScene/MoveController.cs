using System;
using UnityEngine;

public class MoveController : MonoBehaviour
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
	public Camera camera;
	public AudioSource audioSource;
	public AudioClip moveClip;
	public AudioClip runClip;
	private CapsuleCollider _collider;

	[Header("Other")]
	public float groundOffset;

	[Tooltip("The names of the axes and buttons for Unity's Input Manager."), SerializeField]
	private FpsInput input;



	private Rigidbody _rigidbody;
	private SmoothRotation _rotationX;
	private SmoothRotation _rotationY;
	private SmoothVelocity _velocityX;
	private SmoothVelocity _velocityY;
	private SmoothVelocity _velocityZ;

	private bool _isGround;
	private bool _isJump;

	// Start is called before the first frame update
	void Start()
    {
		_rigidbody = GetComponent<Rigidbody>();
		_collider = GetComponent<CapsuleCollider>();

		//初始化运动参数
		_rotationX = new SmoothRotation(RotationXRaw);
		_rotationY = new SmoothRotation(RotationYRaw);
		_velocityX = new SmoothVelocity();
		_velocityY = new SmoothVelocity();
		_velocityZ = new SmoothVelocity();

		_isGround = true;
		_isJump = false;

		Cursor.visible = false;
	}

    // Update is called once per frame
    void Update()
    {
		CheckIfOnGround();
	}

	void FixedUpdate()
	{
		AnimatorChange();
		playerAnimator.SetBool("IsGround", _isGround);
	}

	public void moveCharactor(float strafe, float move, bool run)
	{
		var direction = new Vector3(strafe, 0.0f, move).normalized;
		var velocity = direction * (run ? runSpeed : walkSpeed);
		var smoothX = _velocityX.Update(velocity.x, movementSmoothness);
		var smoothZ = _velocityZ.Update(velocity.z, movementSmoothness);
		var rigidbodyVelocity = _rigidbody.velocity;
		var force = new Vector3(smoothX - rigidbodyVelocity.x, 0.0f, smoothZ - rigidbodyVelocity.z);
		transform.Translate(force * Time.fixedDeltaTime);
		//_rigidbody.AddForce(force, ForceMode.Force);
	}

	private void AnimatorChange()
	{
		// change animation
		playerAnimator.SetBool("IsForward", input.Move > 0);
		playerAnimator.SetBool("IsBack", input.Move < 0);
		playerAnimator.SetBool("IsLeft", input.Strafe < 0);
		playerAnimator.SetBool("IsRight", input.Strafe > 0);
		playerAnimator.SetBool("IsRun", input.Run);

		// 播放声音
		bool isMove = input.Move != 0 || input.Strafe != 0;
		if(audioSource && isMove)
		{
			if (input.Run) audioSource.clip = runClip;
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
		if (_isGround && input.Jump && !_isJump) 
		{
			_rigidbody.AddForce(jumpSpeed * Vector3.up, ForceMode.Impulse);
			playerAnimator.SetTrigger("Jump");
			_isJump = true;
		}
	}

	public void RotateCharactor(float RotateX, float RotateY)
	{
		RotateX *= mouseSensitivity;
		RotateY *= mouseSensitivity;

		Quaternion cameraRotation = camera.GetComponent<Transform>().rotation;
		float camXAngle = cameraRotation.eulerAngles.x;

		//将camXAngle转换至[-180,180]区间
		if (camXAngle < 180) camXAngle = -camXAngle;
		else camXAngle = 360 - camXAngle;

		float yRot = _rotationX.Update(RotationXRaw, rotationSmoothness);
		float xRot = _rotationY.Update(RotationYRaw, rotationSmoothness);
		this.transform.Rotate(new Vector3(0f, yRot, 0f));

		camXAngle += xRot;

		//判断旋转角度
		if (camXAngle < maxVerticalAngle && camXAngle > minVerticalAngle)
			camera.transform.RotateAround(transform.position, transform.right, -xRot);
			//camera.transform.localRotation *= Quaternion.Euler(-xRot, 0f, 0f);

	}

	void CheckIfOnGround()
	{
		var point = transform.position + _collider.center;
		var layerMask = LayerMask.GetMask("Ground");
		_isGround = Physics.CapsuleCast(point, point, _collider.radius, Vector3.down, _collider.height / 2 + groundOffset, layerMask);
	}

	/// Returns the target rotation of the camera around the y axis with no smoothing.
	private float RotationXRaw
	{
		get { return input.RotateX * mouseSensitivity; }
	}

	/// Returns the target rotation of the camera around the x axis with no smoothing.
	private float RotationYRaw
	{
		get { return input.RotateY * mouseSensitivity; }
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
