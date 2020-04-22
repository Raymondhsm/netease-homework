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

    public virtual void StatusUpload()
    {
        return;
    }
    public virtual void ProcessMoveRecv(EntityMoveInfo entity)
    {
        return;
    }

    public int eid
    {
        set { m_eid = value; }
        get { return m_eid; }
    }

    public struct EntityMoveInfo
    {
        public int eid;
        public float move;
        public float strafe;
        public bool run;
        public bool jump;
    }

}
