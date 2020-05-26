using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EntityController : MonoBehaviour
{
    public GameObject playerPrefab;
    public GameObject playerOther;
    public GameObject gunEnemyPrefab;
    public GameObject knifeEnemyPrefab;
    public GameObject medicinePrefab;
    public GameObject bulletPrefab;
    public float updateInterval;

    private Dictionary<int, Entity> m_entities;
    private List<int> m_updateEntity;
    private int m_entityIndex;
    private NetworkSocket m_network;
    private float m_time;
    private Entity m_player;

    public string publicID;
    public string privateID;


    // Start is called before the first frame update
    void Start()
    {
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
        Service.Instance().EntityNewRecvCallback = new Service.RecvHandler(this.RegisterEntityCallback);
        Service.Instance().EntityMoveCallback = new Service.RecvHandler(this.EntityMoveCallback);
        Service.Instance().EntityShootCallback = new Service.RecvHandler(this.EntityShootCallback);
        Service.Instance().EntityReloadCallback = new Service.RecvHandler(this.EntityReloadCallback);
        Service.Instance().EntityUpdateCallback = new Service.RecvHandler(this.UpdateEntityCallback);
        Service.Instance().EnemyBehaviorCallback = new Service.ComRecvHandler(this.EnemyBehaviorCallback);
        Service.Instance().EntityDeadCallback = new Service.RecvHandler(this.EntityDead);
        Service.Instance().PlayerPickUp = new Service.RecvHandler(this.PlayerPickUp);
        Service.Instance().EntityMagicCallback = new Service.RecvHandler(this.EntityMagicCallback);
        Service.Instance().newClientCallback = new Service.RecvHandler(this.ProcessNewClient);

        m_entities = new Dictionary<int, Entity>();
        m_updateEntity = new List<int>();
        m_entityIndex = 0;
        m_time = 0;
    }

    // Update is called once per frame
    void Update()
    {
        m_time += Time.deltaTime;
        if(m_time > updateInterval)
        {
            this.UpdateEntityInfo();
            m_time = 0;
        }
    }

    // public void RegisterEntity(int entity, Vector3[] pdv, int life = 100)
    // {
    //     EntityInfo entityInfo;
    //     entityInfo.privateID = PlayerPrefs.GetString("privateID");
    //     entityInfo.entityCommand = entity;
    //     entityInfo.pos = pdv[0];
    //     entityInfo.direction = pdv[1];
    //     entityInfo.velocity = pdv[2];
    //     entityInfo.life = life;

    //     string sendMsg = JsonUtility.ToJson(entityInfo);
    //     m_network.send(Config.COMMAND_NEW_ENTITY, sendMsg);
    // }

    public void RegisterEntityCallback(string data)
    {
        EntityRecv entity = JsonUtility.FromJson<EntityRecv>(data);

        switch(entity.Type)
        {       
            case Config.ENTITY_PLAYER:
                GameObject go;
                if(entity.publicID == publicID)
                {
                    go = Instantiate(playerPrefab);
                    m_player = go.GetComponent<PlayerEntity>();
                }
                else
                    go = Instantiate(playerOther);

                go.transform.position = entity.pos;
                go.GetComponent<Entity>().eid = entity.eid;

                m_entities.Add(entity.eid, go.GetComponent<Entity>());
                m_updateEntity.Add(entity.eid);
                break;

            case Config.ENTITY_ENEMY:
                GameObject enemy;
                if(entity.EnemyType == Config.ENTITY_GUN_ENEMY)
                    enemy = Instantiate(gunEnemyPrefab);
                else
                    enemy = Instantiate(knifeEnemyPrefab);
                enemy.transform.position = entity.pos;
                enemy.GetComponent<Entity>().eid = entity.eid;

                m_entities.Add(entity.eid, enemy.GetComponent<EnemyEntity>());
                m_updateEntity.Add(entity.eid);
                break;

            case Config.ENTITY_REWARD_MEDICINE:
                GameObject me = Instantiate(medicinePrefab);
                entity.pos.y += 1f;
                entity.pos.x += 0.5f;
                me.transform.position = entity.pos;
                me.GetComponent<Entity>().eid = entity.eid;

                m_entities.Add(entity.eid, me.GetComponent<RewardEntity>());
                break;

            case Config.ENTITY_REWARD_BULLET:
                GameObject bu = Instantiate(bulletPrefab);
                entity.pos.y += 1f;
                entity.pos.x += 0.5f;
                bu.transform.position = entity.pos;
                bu.GetComponent<Entity>().eid = entity.eid;

                m_entities.Add(entity.eid, bu.GetComponent<RewardEntity>());
                break;
        }
    }

    public void EntityDead(string dataStr)
    {
        EntityEid data = JsonUtility.FromJson<EntityEid>(dataStr);
        if(m_entities[data.eid].Status == 0)
            m_entities[data.eid].ProcessEntityDead();
    }

    public void EntityMoveCallback(string data)
    {
        EntityMoveInfo dataRecv = JsonUtility.FromJson<EntityMoveInfo>(data);
        if(m_entities[dataRecv.eid].Status == 0)
            m_entities[dataRecv.eid].ProcessMoveRecv(dataRecv);
    }

    public void EntityShootCallback(string data)
    {
        EntityShootInfo dataRecv = JsonUtility.FromJson<EntityShootInfo>(data);
        if(m_entities[dataRecv.eid].Status == 0)
            m_entities[dataRecv.eid].ProcessShootRecv(dataRecv);
    }

    public void EntityMagicCallback(string data)
    {
       EntityShootInfo dataRecv = JsonUtility.FromJson<EntityShootInfo>(data);
        if(m_entities[dataRecv.eid].Status == 0)
            m_entities[dataRecv.eid].ProcessMagicRecv(dataRecv);
    }

    public void EntityReloadCallback(string data)
    {
        EntityEid dataRecv = JsonUtility.FromJson<EntityEid>(data);
        if(m_entities[dataRecv.eid].Status == 0)
            m_entities[dataRecv.eid].ProcessReloadRecv(dataRecv);
    }

    public void UpdateEntityInfo()
    {
        for(int i = 0; i<m_updateEntity.Count; i++)
        {
            if(m_entities[m_updateEntity[i]].Status == 0)
                m_entities[m_updateEntity[i]].UpdateInfo();
        }
    }

    public void UpdateEntityCallback(string data)
    {
        PlayerUpdateRecv playerUpdateRecv = JsonUtility.FromJson<PlayerUpdateRecv>(data);
        if(m_entities[playerUpdateRecv.eid].Status == 0)
            m_entities[playerUpdateRecv.eid].ProcessUpdateInfoRecv(playerUpdateRecv);
    }

    public void EnemyBehaviorCallback(int command, string data)
    {
        if(command == Config.COMMAND_NPC_SHOOT)
        {
            NPCShoot npcShoot = JsonUtility.FromJson<NPCShoot>(data);
            if(m_entities[npcShoot.eid].Status == 0)
                m_entities[npcShoot.eid].EnemyBehavior(npcShoot);
        }
        else
        {
            EnemyBehavior enemyBehavior = JsonUtility.FromJson<EnemyBehavior>(data);
            if(m_entities[enemyBehavior.eid].Status == 0)
                m_entities[enemyBehavior.eid].EnemyBehavior(command, enemyBehavior);
        }
    }

    public void PlayerPickUp(string data)
    {
        PickUpRecv pickUpRecv = JsonUtility.FromJson<PickUpRecv>(data);

        if(m_entities[pickUpRecv.eid].Type == Config.ENTITY_REWARD)
            m_entities[pickUpRecv.eid].PickedUp(pickUpRecv, publicID);
    }
    
    public void ProcessNewClient(string data)
    {
        IDs ids = JsonUtility.FromJson<IDs>(data);
        publicID = ids.publicID;
        privateID = ids.privateID;
    }

    public GameObject Player
    {
        get { 
            if(m_player != null)
                return m_player.gameObject; 
            else return null;
        }
    }

    public Dictionary<int, Entity> Entities
    {
        get { return m_entities; }
    }
}
