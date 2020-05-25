using UnityEngine;

class Service
{
    public delegate void RecvHandler(string str);
    public delegate void ComRecvHandler(int command, string str);

    public RecvHandler loginRecvCallback;
    public RecvHandler registerRecvCallback;
    public RecvHandler updateInfoCallback;
    public RecvHandler beginGameCallback;
    public RecvHandler endGameCallback;
    public RecvHandler checkLogin;
    public RecvHandler EntityNewRecvCallback;
    public RecvHandler EntityMoveCallback;
    public RecvHandler EntityShootCallback;
    public RecvHandler EntityReloadCallback;
    public RecvHandler EntityUpdateCallback;
    public RecvHandler EntityDeadCallback;
    public RecvHandler PlayerPickUp;
    public RecvHandler EntityMagicCallback;

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

            case Config.COMMAND_UPDATE_INFO:
                if(updateInfoCallback != null)
                    updateInfoCallback(dataStr);
                break;

            case Config.COMMAND_ATTEND_GAME:
                if(beginGameCallback != null)
                    beginGameCallback(dataStr);
                break;

            case Config.COMMAND_END_GAME:
                if(endGameCallback != null)
                    endGameCallback(dataStr);
                break;

            case Config.COMMAND_CHECK_LOGIN:
                if(checkLogin != null)
                    checkLogin(dataStr);
                break;

            case Config.COMMAND_NEW_CLIENT:
                IDs ids = JsonUtility.FromJson<IDs>(dataStr);
                PlayerPrefs.SetString("publicID", ids.publicID);
                PlayerPrefs.SetString("privateID", ids.privateID);
                break;

            case Config.COMMAND_NEW_ENTITY:
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
            case Config.COMMAND_NPC_SHOOT:
            case Config.COMMAND_NPC_DIZZY:
                if(EnemyBehaviorCallback != null)
                    EnemyBehaviorCallback(command, dataStr);
                break;

            case Config.COMMAND_DEAD:
                if(EntityDeadCallback != null)
                    EntityDeadCallback(dataStr);
                break;

            case Config.COMMAND_PICK_UP:
                if(PlayerPickUp != null)
                    PlayerPickUp(dataStr);
                break;

            case Config.COMMAND_MAGIC_ARROW:
                if(EntityMagicCallback != null)
                    EntityMagicCallback(dataStr);
                break;
        }
    }

}