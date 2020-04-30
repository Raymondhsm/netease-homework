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
    public int bulletEid;
    public float endPointX;
    public float endPointY;
    public float endPointZ;
}

public struct EntityEid
{
    public int eid;
}

public struct IDs
{
    public string publicID;
    public string privateID;
}

public struct PlayerUpdateInfo
{
    public int eid;
    public Vector3 pos;
    public Vector3 direction;
    public Vector3 velocity;
}

public struct PlayerUpdateRecv
{
    public int eid;
    public int life;
    public Vector3 pos;
    public Vector3 direction;
    public Vector3 velocity;
}

public struct DeadInfo
{
    public int eid;
    public string privateID;
}

public struct EnemyBehavior
{
    public int eid;
    public Vector3 pos;
}

public struct EntityHit
{
    public int eid;
    public int bulletEid;
    public int bulletDamage;
}
