using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MagicArrowController : MonoBehaviour
{
    public ParticleSystem explosion;
    public ParticleSystem fly;
    private int m_eid;
    private bool m_explosed;
    private Vector3 m_direction;

    private NetworkSocket m_network;

    // Start is called before the first frame update
    void Start()
    {
        m_explosed = false;
        m_network = GameObject.Find("NetworkController").GetComponent<NetworkSocket>();
    }

    // Update is called once per frame
    void Update()
    {
        if(!m_explosed)
        {
            Vector3 localDir = transform.InverseTransformDirection(Vector3.Normalize(m_direction));
            transform.Translate(Vector3.Normalize(m_direction));
        }
    }

    void OnTriggerEnter(Collider other)
    {
        GameObject obj = other.gameObject;
        if(obj.CompareTag("Player") || m_explosed)
            return;

        Explosion();

        Destroy(gameObject,2);
    }

    private void Explosion()
    {
        m_explosed = true;
        fly.Stop();
        explosion.Play();
        EnemyBehavior explosePos;
        explosePos.eid = m_eid;
        explosePos.pos = transform.position;
        m_network.send(Config.COMMAND_MAGIC_EXPLOSE, JsonUtility.ToJson(explosePos));
    }

    public Vector3 Direction
    {
        set { m_direction = value; }
        get { return m_direction; }
    }

    public int Eid
    {
        set { m_eid = value; }
        get { return m_eid; }
    }
}
