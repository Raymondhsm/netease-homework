using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EntityController : MonoBehaviour
{
    public GameObject player;

    private Dictionary<int, Entity> m_entities;
    private int m_entityIndex;
    private NetworkSocket m_network;


    // Start is called before the first frame update
    void Start()
    {
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
        Service.Instance().EntityNewRecvCallback = new Service.RecvHandler(this.RegisterEntityCallback);
        Service.Instance().EntityMoveCallback = new Service.RecvHandler(this.EntityMoveCallback);
        Service.Instance().EntityShootCallback = new Service.RecvHandler(this.EntityShootCallback);

        m_entities = new Dictionary<int, Entity>();
        m_entityIndex = 0;
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetButtonDown("Mode")){
            Vector3[] a = {new Vector3(),new Vector3(),new Vector3()};
            RegisterEntity(Config.ENTITY_PLAYER, a);
        }
    }

    public void RegisterEntity(int entity, Vector3[] pdv, int life = 0)
    {
        EntityInfo entityInfo;
        entityInfo.entityCommand = entity;
        entityInfo.pos = pdv[0];
        entityInfo.direction = pdv[1];
        entityInfo.velocity = pdv[2];
        entityInfo.life = life;

        string sendMsg = JsonUtility.ToJson(entityInfo);
        m_network.send(Config.COMMAND_NEW_ENTITY, sendMsg);
    }

    public void RegisterEntityCallback(string data)
    {
        EntityRecv entity = JsonUtility.FromJson<EntityRecv>(data);

        switch(entity.Type)
        {
            case Config.ENTITY_PLAYER:
                GameObject go = Instantiate(player);
                go.transform.position = entity.pos;
                go.GetComponent<Entity>().eid = entity.eid;

                m_entities.Add(entity.eid, go.GetComponent<Entity>());
                break;
        }
    }

    public void EntityMoveCallback(string data)
    {
        Entity.EntityMoveInfo dataRecv = JsonUtility.FromJson<Entity.EntityMoveInfo>(data);
        m_entities[dataRecv.eid].ProcessMoveRecv(dataRecv);
    }

    public void EntityShootCallback(string data)
    {
        Entity.EntityShootInfo dataRecv = JsonUtility.FromJson<Entity.EntityShootInfo>(data);
        m_entities[dataRecv.eid].ProcessShootRecv(dataRecv);
    }

    public struct EntityInfo
    {
        public int entityCommand;
        public Vector3 pos;
        public Vector3 direction;
        public Vector3 velocity;
        public float life;
    }

    public struct EntityRecv
    {
        public int eid;
        public int Type;
        public Vector3 pos;
        public Vector3 direction;
        public Vector3 velocity;
        public float life;
    }
}
