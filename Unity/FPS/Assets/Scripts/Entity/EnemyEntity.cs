using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyEntity : Entity
{
    private BehaviorController m_behaviorController;

    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        Type = Config.ENTITY_ENEMY;
        m_behaviorController = gameObject.GetComponent<BehaviorController>();
    }

    // Update is called once per frame
    void Update()
    {
        
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

    public override void EnemyBehavior(int command, EnemyBehavior eh)
    {
        if(m_behaviorController)
        {
            switch(command)
            {
                case Config.COMMAND_NPC_COMMON:
                    m_behaviorController.CommonBehavior(eh);
                    break;
                
                case Config.COMMAND_NPC_ATTACK:
                    m_behaviorController.AttackBehavior(eh);
                    break;

                case Config.COMMAND_NPC_RESET:
                    m_behaviorController.ResetBehavior(eh);
                    break;

                case Config.COMMAND_NPC_DIZZY:
                    m_behaviorController.Dizzy();
                    break;
            }
        }
    }

    public override void EnemyBehavior(NPCShoot ns)
    {
        m_behaviorController.Shoot(ns);
    }

}
