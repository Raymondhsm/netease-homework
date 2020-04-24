using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EntityController : MonoBehaviour
{
    public GameObject player;
    public GameObject playerOther;

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
        Service.Instance().EntityReloadCallback = new Service.RecvHandler(this.EntityReloadCallback);

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
        entityInfo.privateID = PlayerPrefs.GetString("privateID");
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
                GameObject go;
                Debug.Log(entity.publicID);
                Debug.Log(PlayerPrefs.GetString("publicID"));
                if(entity.publicID == PlayerPrefs.GetString("publicID"))
                    go = Instantiate(player);
                else
                    go = Instantiate(playerOther);

                go.transform.position = entity.pos;
                go.GetComponent<Entity>().eid = entity.eid;

                m_entities.Add(entity.eid, go.GetComponent<Entity>());
                break;

            case Config.ENTITY_ENEMY:
                GameObject enemy = Instantiate(playerOther);
                enemy.transform.position = entity.pos;
                enemy.GetComponent<Entity>().eid = entity.eid;

                m_entities.Add(entity.eid, enemy.GetComponent<Entity>());
                break;
        }
    }

    public void EntityMoveCallback(string data)
    {
        EntityMoveInfo dataRecv = JsonUtility.FromJson<EntityMoveInfo>(data);
        m_entities[dataRecv.eid].ProcessMoveRecv(dataRecv);
    }

    public void EntityShootCallback(string data)
    {
        EntityShootInfo dataRecv = JsonUtility.FromJson<EntityShootInfo>(data);
        m_entities[dataRecv.eid].ProcessShootRecv(dataRecv);
    }

    public void EntityReloadCallback(string data)
    {
        EntityReload dataRecv = JsonUtility.FromJson<EntityReload>(data);
        m_entities[dataRecv.eid].ProcessReloadRecv(dataRecv);
    }
}
