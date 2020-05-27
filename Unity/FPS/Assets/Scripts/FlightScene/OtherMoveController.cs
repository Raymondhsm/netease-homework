using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OtherMoveController : MonoBehaviour
{
	[Header("Component")]
	public Animator playerAnimator;
	public AudioSource audioSource;
	public AudioClip moveClip;
	public AudioClip runClip;
	private CapsuleCollider _collider;

	[Header("Other")]
	public float groundOffset;


	private Rigidbody _rigidbody;

	private bool _isGround;
	private bool _isJump;

    private float[] _moveData;
    private bool _isRun;

	private float _nextPosTime;
	private float _nextDirTime;

	// Start is called before the first frame update
	void Start()
    {
		_rigidbody = GetComponent<Rigidbody>();
		_collider = GetComponent<CapsuleCollider>();

		_isGround = true;
		_isJump = false;

        _moveData = new float[2];

		_nextPosTime = Time.time;
		_nextDirTime = Time.time;
	}

    // Update is called once per frame
    void Update()
    {
		CheckIfOnGround();
        AnimatorChange(_moveData[0], _moveData[1], _isRun);

		_moveData[0] = 0;
		_moveData[1] = 0;
		_isRun = false;
	}

	void FixedUpdate()
	{
		playerAnimator.SetBool("IsGround", _isGround);
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

	void CheckIfOnGround()
	{
		var point = transform.position + _collider.center;
		var layerMask = LayerMask.GetMask("Ground");
		_isGround = Physics.CapsuleCast(point, point, _collider.radius, Vector3.down, _collider.height / 2 + groundOffset, layerMask);
	}
}
