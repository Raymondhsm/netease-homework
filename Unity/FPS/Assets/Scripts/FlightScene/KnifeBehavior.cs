using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class KnifeBehavior : BehaviorController
{
    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        _nextFireTime = 0;
    }

    protected override void Attack()
    {
		var direction = _targetPos - transform.position;
		var distance = Vector3.Distance(transform.position, _targetPos);

		// 在攻击范围
		if (distance <= attackDistance)
		{
            if(_nextFireTime <= Time.time)
            {
                _animator.SetTrigger("Attack");
                _nextFireTime = Time.time + fireInterval;
                _animator.SetBool("Forward", false);
            }
		}

		// 不在攻击范围 前方无障碍物 移动
		else
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
    }
}
