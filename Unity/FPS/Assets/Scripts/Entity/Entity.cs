using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Entity : MonoBehaviour
{
    public int m_eid;
    protected NetworkSocket m_network;
    protected LifeController m_lifeController;

    // Start is called before the first frame update
    public virtual void Start()
    {
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

    public virtual void ProcessReloadRecv(EntityReload er)
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
        m_lifeController.ProcessLifeRecv(pur.life);
    }

    public int eid
    {
        set { m_eid = value; }
        get { return m_eid; }
    }


}
