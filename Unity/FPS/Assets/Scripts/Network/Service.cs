using UnityEngine;

class Service
{
    public delegate void RecvHandler(string str);
    public delegate void ComRecvHandler(int command, string str);

    public RecvHandler loginRecvCallback;
    public RecvHandler registerRecvCallback;
    public RecvHandler EntityNewRecvCallback;
    public RecvHandler EntityMoveCallback;
    public RecvHandler EntityShootCallback;
    public RecvHandler EntityReloadCallback;
    public RecvHandler EntityUpdateCallback;
    public RecvHandler EntityDeadCallback;

    public ComRecvHandler EnemyBehaviorCallback;

    private static Service instance;
    public static Service Instance()
    {
        if(instance == null)
        {
            instance = new Service();
        }
        return instance;
    }

    private Service()
    {

    }

    public void ServiceProcess(int command, string dataStr)
    {
        switch(command)
        {
            case Config.COMMAND_LOGIN:
                if(loginRecvCallback != null)
                    loginRecvCallback(dataStr);
                break;

            case Config.COMMAND_REGISTER:
                if(registerRecvCallback != null)
                    registerRecvCallback(dataStr);
                break;

            case Config.COMMAND_NEW_CLIENT:
                IDs ids = JsonUtility.FromJson<IDs>(dataStr);
                PlayerPrefs.SetString("publicID", ids.publicID);
                PlayerPrefs.SetString("privateID", ids.privateID);
                break;

            case Config.COMMAND_NEW_ENTITY:
                Debug.Log(dataStr);
                if(EntityNewRecvCallback != null)
                    EntityNewRecvCallback(dataStr);
                break;

            case Config.COMMAND_MOVE:
                if(EntityMoveCallback != null)
                    EntityMoveCallback(dataStr);
                break;

            case Config.COMMAND_SHOOT:
                if(EntityShootCallback != null)
                    EntityShootCallback(dataStr);
                break;

            case Config.COMMAND_RELOAD:
                if(EntityReloadCallback != null)
                    EntityReloadCallback(dataStr);
                break;

            case Config.COMMAND_UPDATE_ENTITY:
                if(EntityUpdateCallback != null)
                    EntityUpdateCallback(dataStr);
                break;

            case Config.COMMAND_NPC_COMMON:
            case Config.COMMAND_NPC_ATTACK:
            case Config.COMMAND_NPC_RESET:
                if(EnemyBehaviorCallback != null)
                    EnemyBehaviorCallback(command, dataStr);
                break;

            case Config.COMMAND_DEAD:
                if(EntityDeadCallback != null)
                    EntityDeadCallback(dataStr);
                break;
        }
    }

}