using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyEntity : Entity
{
    private LifeController m_lifeController;

    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();

        m_lifeController = gameObject.GetComponent<LifeController>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public override void ProcessMoveRecv(EntityMoveInfo entity)
    {

    }

    public override void ProcessShootRecv(EntityShootInfo esi)
    {
        
    }

    public override void UpdateInfo()
    {
        PlayerUpdateInfo playerUpdateInfo;
        playerUpdateInfo.eid = m_eid;
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
