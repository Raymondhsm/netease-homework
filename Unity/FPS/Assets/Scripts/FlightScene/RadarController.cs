using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;
using System;

public class RadarController : MonoBehaviour
{
    public GameObject Point;

    private EntityController m_entityController;
    private List<GameObject> m_pointList;
    private int m_index;

    // Start is called before the first frame update
    void Start()
    {
        m_entityController = GameObject.Find("GameController").GetComponent<EntityController>();
        m_pointList = new List<GameObject>();
        m_index = 0;
    }

    // Update is called once per frame
    void Update()
    {
        m_index = 0;
        setRadar();
    }

    public void setRadar()
    {
        GameObject player = m_entityController.Player;
        if(player == null) return;
        Vector3 playerPos = player.transform.position;

        foreach (KeyValuePair<int, Entity> entity in m_entityController.Entities)
        {   
            if(entity.Value == null || entity.Value.Status != 0) continue;
            int type = entity.Value.Type;
            if(type == Config.ENTITY_PLAYER_OTHER || type == Config.ENTITY_ENEMY || type == Config.ENTITY_REWARD)
            {
                Vector3 enemyPos = entity.Value.transform.position;
                Vector3 dir = enemyPos - playerPos;

                float angle = Vector3.Angle(player.transform.forward, dir);
                Vector3 cross = Vector3.Cross(player.transform.forward, dir);
                angle = cross.y < 0 ? -angle : angle;

                float distance = Vector3.Distance(playerPos, enemyPos);
                float virtualDis = distance >= 25 ? 75 : distance * 3;

                float x = Mathf.Sin(angle * Mathf.PI / 180) * virtualDis;
                float y = Mathf.Cos(angle * Mathf.PI / 180) * virtualDis;

                setPoint(type).GetComponent<RectTransform>().anchoredPosition = new Vector2(x, y);
            }
        }

        for(int i = m_index; i<m_pointList.Count; i++)
            m_pointList[m_index].SetActive(false);
    }

    private GameObject setPoint(int etype)
    {
        if(m_index >= m_pointList.Count)
            m_pointList.Add(Instantiate(Point));
        GameObject point = m_pointList[m_index];
        point.SetActive(true);

        Image pi = point.GetComponent<Image>();
        if(etype == Config.ENTITY_ENEMY)
            pi.color = new Color(0.972549f, 0.06727549f, 0.02352939f);
        else if(etype == Config.ENTITY_PLAYER_OTHER)
            pi.color = new Color(0.02352939f, 0.720997f, 0.972549f);
        else
            pi.color = new Color(0.08061034f, 0.972549f, 0.02352939f);

        point.transform.parent = transform;

        m_index ++;
        return point; 
    }   
}
