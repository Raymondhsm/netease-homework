using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Entity : MonoBehaviour
{
    public int m_eid;
    public int Type;
    protected int m_status;
    protected NetworkSocket m_network;
    protected LifeController m_lifeController;

    // Start is called before the first frame update
    public virtual void Start()
    {
        m_status = 0;

        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
        m_lifeController = gameObject.GetComponent<LifeController>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    public virtual void ProcessMoveRecv(EntityMoveInfo entity)
    {
        return;
    }

    public virtual void ProcessShootRecv(EntityShootInfo esi)
    {
        return;
    }

    public virtual void ProcessReloadRecv(EntityEid ee)
    {
        return;
    }

    public virtual void UpdateInfo()
    {
        return;
    }

    public virtual void EnemyBehavior(int command, EnemyBehavior eh)
    {
        return;
    }

    public virtual void EnemyBehavior(NPCShoot ns)
    {
        return;
    }

    public virtual void PickedUp(PickUpRecv pickUpRecv){}

    public virtual void ProcessMagicRecv(EntityShootInfo esi){}

    public virtual void ProcessUpdateInfoRecv(PlayerUpdateRecv pur)
    {
        // if(Vector3.Distance(pur.pos, transform.position) > 10f)
        // {
        //     transform.position = pur.pos;
        // }
        // if(Vector3.Angle(pui.direction, transform.forward) > Mathf.PI / 2)
        // {
        //     transform.forward = pui.direction;
        // }
        if(m_lifeController)
            m_lifeController.ProcessLifeRecv(pur.life);
    }

    public virtual void ProcessEntityDead()
    {
        m_status = 1;
        m_lifeController.Dead();
    }

    public int eid
    {
        set { m_eid = value; }
        get { return m_eid; }
    }

    public int Status
    {
        set { m_status = value; }
        get { return m_status; }
    }
}
