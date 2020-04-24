using UnityEngine;
public struct EntityInfo
{
    public string privateID;
    public int entityCommand;
    public Vector3 pos;
    public Vector3 direction;
    public Vector3 velocity;
    public float life;
}

public struct EntityRecv
{
    public string publicID;
    public int eid;
    public int Type;
    public Vector3 pos;
    public Vector3 direction;
    public Vector3 velocity;
    public float life;
}


public struct EntityMoveInfo
{
    public int eid;
    public float move;
    public float strafe;
    public bool run;
    public bool jump;
    public float rotateX;
    public float rotateY;
}

public struct EntityShootInfo
{
    public int eid;
    public float endPointX;
    public float endPointY;
    public float endPointZ;
}