using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerEntity : Entity
{
    private FpsInput m_fpsInput;
    private MoveController m_moveController;
    private ShootController m_shootController;
    private LifeController m_lifeController;

    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        m_fpsInput = new FpsInput();
        m_moveController = gameObject.GetComponent<MoveController>();
        m_shootController = gameObject.GetComponent<ShootController>();
        m_lifeController = gameObject.GetComponent<LifeController>();
    }

    // Update is called once per frame
    void Update()
    {
        StatusUpload();
    }

    public void StatusUpload()
    {
        if(!m_fpsInput.isChange) return;

        EntityMoveInfo en;
        en.move = m_fpsInput.Move;
        en.strafe = m_fpsInput.Strafe;
        en.run = m_fpsInput.Run;
        en.jump = m_fpsInput.Jump;
        en.eid = m_eid;
        en.rotateX = m_fpsInput.RotateX;
        en.rotateY = m_fpsInput.RotateY;

        string data = JsonUtility.ToJson(en);
        m_network.send(Config.COMMAND_MOVE, data);
    }

    public override void ProcessMoveRecv(EntityMoveInfo entity)
    {
        m_moveController.moveCharactor(entity.strafe, entity.move, entity.run);
        m_moveController.Jump();
        m_moveController.RotateCharactor(entity.rotateX, entity.rotateY);
    }

    public void ShootUpload(Vector3 endPoint)
    {
        EntityShootInfo esi;
        esi.eid = m_eid;
        esi.endPointX = endPoint.x;
        esi.endPointY = endPoint.y;
        esi.endPointZ = endPoint.z;

        string data = JsonUtility.ToJson(esi);
        m_network.send(Config.COMMAND_SHOOT, data);
    }

    public override void ProcessShootRecv(EntityShootInfo esi)
    {
        Vector3 endPoint = new Vector3(esi.endPointX, esi.endPointY, esi.endPointZ);
        m_shootController.ShootRecv(endPoint);
    }

    public void ReloadUpload()
    {
        EntityReload er;
        er.eid = m_eid;
        m_network.send(Config.COMMAND_RELOAD, JsonUtility.ToJson(er));
    }

    public override void ProcessReloadRecv(EntityReload er)
    {
        m_shootController.ReloadRecv();
    }

    public override void UpdateInfo()
    {
        PlayerUpdateInfo playerUpdateInfo;
        playerUpdateInfo.eid = m_eid;
        // playerUpdateInfo.currBullet = m_shootController.CurrBullet;
        // playerUpdateInfo.totalBullet = m_shootController.TatolBullet;
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

    public override void ProcessUpdateInfoRecv(PlayerUpdateInfo pui)
    {
        
    }
}
