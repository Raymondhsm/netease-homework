using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerOtherEntity : Entity
{
    public GameObject _bulletPrefab;
    public GameObject _bulletPos;
    
    [Header("Shoot Effect")]
	public AudioSource shootAudio;
	public AudioClip shootClip;
	public AudioClip reloadClip;
	public ParticleSystem shootEffect;

    private FpsInput m_fpsInput;
    private MoveController m_moveController;
    private LifeController m_lifeController;

    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        m_fpsInput = new FpsInput();
        m_moveController = gameObject.GetComponent<MoveController>();
        m_lifeController = gameObject.GetComponent<LifeController>();
    }
    public override void ProcessMoveRecv(EntityMoveInfo entity)
    {
        m_moveController.moveCharactor(entity.strafe, entity.move, entity.run);
        m_moveController.Jump();
        m_moveController.RotateCharactor(entity.rotateX, entity.rotateY);
    }

    public override void ProcessShootRecv(EntityShootInfo esi)
    {
        Vector3 endPoint = new Vector3(esi.endPointX, esi.endPointY, esi.endPointZ);

        // 计算方向
		var startPoint = _bulletPos.transform.position;
		var direction = endPoint - startPoint;

		_bulletPrefab.transform.position = startPoint;
		GameObject bullet= Instantiate(_bulletPrefab);
		bullet.GetComponent<BulletController>().StaticHitPos = endPoint;
		bullet.GetComponent<BulletController>().AddForce(direction.normalized);

		// 播放声音
		if (shootAudio)
		{
			shootAudio.clip = shootClip;
			shootAudio.Play();
		}
		shootEffect.Play();
    }

    public override void ProcessReloadRecv(EntityReload er)
    {
        gameObject.GetComponent<Animator>().SetTrigger("Reload");
        if(shootAudio)
		{
			shootAudio.clip = reloadClip;
			shootAudio.Play();
		}
    }

    public override void UpdateInfo()
    {
        PlayerUpdateInfo playerUpdateInfo;
        playerUpdateInfo.eid = m_eid;
        // playerUpdateInfo.currBullet = -1;
        // playerUpdateInfo.totalBullet = -1;
        playerUpdateInfo.life = m_lifeController.CurrLife;
        playerUpdateInfo.pos = transform.position;
        playerUpdateInfo.direction = transform.forward;

        Rigidbody rigidbody = gameObject.GetComponent<Rigidbody>();
        if(rigidbody == null)
            playerUpdateInfo.velocity = new Vector3(0,0,0);
        else
            playerUpdateInfo.velocity = rigidbody.velocity;

        string data = JsonUtility.ToJson(playerUpdateInfo);
        m_network.send(Config.COMMAND_UPDATE_ENTITY, data);
    }
}
