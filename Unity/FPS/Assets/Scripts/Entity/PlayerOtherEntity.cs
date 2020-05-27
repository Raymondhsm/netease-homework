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
    public GameObject _magicPrefab;
    public GameObject _magicPos;

    private FpsInput m_fpsInput;
    private OtherMoveController m_moveController;

    private float _nextUpdateTime;

    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        Type = Config.ENTITY_PLAYER_OTHER;
        m_fpsInput = new FpsInput();
        m_moveController = gameObject.GetComponent<OtherMoveController>();
        _nextUpdateTime = Time.time;
    }
    public override void ProcessMoveRecv(EntityMoveInfo entity)
    {
        m_moveController.SetMoveData(entity.move, entity.strafe, entity.run);
        // if(entity.jump) m_moveController.Jump();
    }

    public override void ProcessUpdateInfoRecv(PlayerUpdateRecv pur)
    { 
        transform.position = pur.pos;
        transform.forward = pur.direction;
    }

    public override void ProcessShootRecv(EntityShootInfo esi)
    {
        Vector3 endPoint = new Vector3(esi.endPointX, esi.endPointY, esi.endPointZ);

        // 计算方向
		var startPoint = _bulletPos.transform.position;
		var direction = endPoint - startPoint;

		_bulletPrefab.transform.position = startPoint;
		GameObject bullet= Instantiate(_bulletPrefab);
		BulletController bc = bullet.GetComponent<BulletController>();
		bc.StaticHitPos = endPoint;
		bc.eid = esi.bulletEid;
		bc.owner = 0;
		bc.AddForce(direction.normalized);

		// 播放声音
		if (shootAudio)
		{
			shootAudio.clip = shootClip;
			shootAudio.Play();
		}
		shootEffect.Play();
    }

    public override void ProcessReloadRecv(EntityEid er)
    {
        gameObject.GetComponent<Animator>().SetTrigger("Reload");
        if(shootAudio)
		{
			shootAudio.clip = reloadClip;
			shootAudio.Play();
		}
    }

    public override void ProcessMagicRecv(EntityShootInfo esi)
    {
        Vector3 endPoint = new Vector3(esi.endPointX, esi.endPointY, esi.endPointZ);

        // 计算方向
		var startPoint = _magicPos.transform.position;
		var direction = endPoint - startPoint;

		GameObject magic = Instantiate(_magicPrefab);
		magic.transform.position = startPoint;
		MagicArrowController ma = magic.GetComponent<MagicArrowController>();
		ma.Direction = direction;
		ma.Eid = esi.bulletEid;
    }

}
