using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Entity : MonoBehaviour
{
    public int m_eid;
    protected NetworkSocket m_network;

    // Start is called before the first frame update
    public virtual void Start()
    {
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
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

    public virtual void ProcessUpdateInfoRecv(PlayerUpdateInfo pui)
    {
        return;
    }

    public int eid
    {
        set { m_eid = value; }
        get { return m_eid; }
    }


}
