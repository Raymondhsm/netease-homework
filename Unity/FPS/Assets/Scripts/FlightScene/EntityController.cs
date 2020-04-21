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
        m_entities = new Dictionary<int, Entity>();
        m_entityIndex = 0;
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetButtonDown("Mode")){
            Debug.Log("click");
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
        Debug.Log(data);
        EntityRecv entity = JsonUtility.FromJson<EntityRecv>(data);

        GameObject go = Instantiate(player);
        go.transform.position = entity.pos;
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
        public Vector3 pos;
        public Vector3 direction;
        public Vector3 velocity;
        public float life;
    }
}
