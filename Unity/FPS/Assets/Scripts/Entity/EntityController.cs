using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EntityController : MonoBehaviour
{
    public GameObject playerPrefab;
    public GameObject playerOther;
    public GameObject enemyPrefab;
    public float updateInterval;

    private Dictionary<int, Entity> m_entities;
    private List<int> m_updateEntity;
    private int m_entityIndex;
    private NetworkSocket m_network;
    private float m_time;


    // Start is called before the first frame update
    void Start()
    {
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
        Service.Instance().EntityNewRecvCallback = new Service.RecvHandler(this.RegisterEntityCallback);
        Service.Instance().EntityMoveCallback = new Service.RecvHandler(this.EntityMoveCallback);
        Service.Instance().EntityShootCallback = new Service.RecvHandler(this.EntityShootCallback);
        Service.Instance().EntityReloadCallback = new Service.RecvHandler(this.EntityReloadCallback);
        Service.Instance().EnemyBehaviorCallback = new Service.ComRecvHandler(this.EnemyBehaviorCallback);

        m_entities = new Dictionary<int, Entity>();
        m_updateEntity = new List<int>();
        m_entityIndex = 0;
        m_time = 0;
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetButtonDown("Mode")){
            Vector3[] a = {new Vector3(),new Vector3(),new Vector3()};
            RegisterEntity(Config.ENTITY_PLAYER, a);
        }

        m_time += Time.deltaTime;
        if(m_time > updateInterval)
        {
            this.UpdateEntityInfo();
            m_time = 0;
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
                    go = Instantiate(playerPrefab);
                else
                    go = Instantiate(playerOther);

                go.transform.position = entity.pos;
                go.GetComponent<Entity>().eid = entity.eid;

                m_entities.Add(entity.eid, go.GetComponent<Entity>());
                m_updateEntity.Add(entity.eid);
                break;

            case Config.ENTITY_ENEMY:
                GameObject enemy = Instantiate(enemyPrefab);
                enemy.transform.position = entity.pos;
                enemy.GetComponent<Entity>().eid = entity.eid;

                m_entities.Add(entity.eid, enemy.GetComponent<Entity>());
                m_updateEntity.Add(entity.eid);
                break;
        }
    }

    public void EntityDead(int eid)
    {
        if(m_entities.ContainsKey(eid))
            m_entities.Remove(eid);

        for(int i=0; i<m_updateEntity.Count; i++)
        {
            if(m_updateEntity[i] == eid){
                m_updateEntity.Remove(i);
                break;
            }
        }

        DeadInfo deadInfo;
        deadInfo.eid = eid;
        deadInfo.privateID = PlayerPrefs.GetString("privateID");
        m_network.send(Config.COMMAND_DEAD, JsonUtility.ToJson(deadInfo));
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

    public void UpdateEntityInfo()
    {
        for(int i = 0; i<m_updateEntity.Count; i++)
        {
            m_entities[m_updateEntity[i]].UpdateInfo();
        }
    }

    public void UpdateEntityCallback(string data)
    {
        PlayerUpdateInfo playerUpdateInfo = JsonUtility.FromJson<PlayerUpdateInfo>(data);
        m_entities[playerUpdateInfo.eid].ProcessUpdateInfoRecv(playerUpdateInfo);
    }

    public void EnemyBehaviorCallback(int command, string data)
    {
        EnemyBehavior enemyBehavior = JsonUtility.FromJson<EnemyBehavior>(data);
        m_entities[enemyBehavior.eid].EnemyBehavior(command, enemyBehavior);
    }
}
