using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerEntity : Entity
{
    private FpsInput m_fpsInput;
    private MoveController m_moveController;
    private ShootController m_shootController;

    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        Type = Config.ENTITY_PLAYER;
        m_fpsInput = new FpsInput();
        m_moveController = gameObject.GetComponent<MoveController>();
        m_shootController = gameObject.GetComponent<ShootController>();
    }

    // Update is called once per frame
    void Update()
    {
        StatusUpload();
        UseProp();
    }

    private void OnCollisionEnter(Collision other)
    {
        GameObject obj = other.gameObject;
        Debug.Log(obj.tag);
        if(obj.CompareTag("Reward"))
        {
            PickUpReward pickUpReward;
            pickUpReward.eid = m_eid;
            pickUpReward.rewardEid = obj.GetComponent<Entity>().eid;
            m_network.send(Config.COMMAND_PICK_UP, JsonUtility.ToJson(pickUpReward));
        }
    }

    public void StatusUpload()
    {
        if(m_status == 1) return;
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

    public override void ProcessUpdateInfoRecv(PlayerUpdateRecv pur)
    {
        Debug.Log("child" + pur.totalBullet);
        if(m_lifeController)
            m_lifeController.ProcessLifeRecv(pur.life);
        if(m_shootController)
            m_shootController.UpdateTotalBullet(pur.totalBullet);
    }

    public override void ProcessMoveRecv(EntityMoveInfo entity)
    {
        m_moveController.moveCharactor(entity.strafe, entity.move, entity.run);
        if(entity.jump) m_moveController.Jump();
        m_moveController.RotateCharactor(entity.rotateX, entity.rotateY);
    }

    public void ShootUpload(Vector3 endPoint)
    {
        if(m_status == 1) return;

        EntityShootInfo esi;
        esi.eid = m_eid;
        esi.bulletEid = -1;
        esi.endPointX = endPoint.x;
        esi.endPointY = endPoint.y;
        esi.endPointZ = endPoint.z;

        string data = JsonUtility.ToJson(esi);
        m_network.send(Config.COMMAND_SHOOT, data);
    }

    public override void ProcessShootRecv(EntityShootInfo esi)
    {
        Vector3 endPoint = new Vector3(esi.endPointX, esi.endPointY, esi.endPointZ);
        m_shootController.ShootRecv(esi.bulletEid, endPoint);
    }

    public void ReloadUpload()
    {
        if(m_status == 1) return;

        EntityEid er;
        er.eid = m_eid;
        m_network.send(Config.COMMAND_RELOAD, JsonUtility.ToJson(er));
    }

    public override void ProcessReloadRecv(EntityEid er)
    {
        m_shootController.ReloadRecv();
    }

    public override void UpdateInfo()
    {
        PlayerUpdateInfo playerUpdateInfo;
        playerUpdateInfo.eid = m_eid;
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

    
    void UseProp()
    {
        if(m_fpsInput.P1 || m_fpsInput.P2)
        {
            UsePropStruct usePropStruct;
            usePropStruct.eid = m_eid;
            usePropStruct.type = m_fpsInput.P1 ? Config.ENTITY_REWARD_MEDICINE : Config.ENTITY_REWARD_BULLET;
            m_network.send(Config.COMMAND_USE_PROP, JsonUtility.ToJson(usePropStruct));
        }
    }
}
