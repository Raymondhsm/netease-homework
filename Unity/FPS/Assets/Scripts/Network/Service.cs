

class Service
{
    public delegate void RecvHandler(string str);

    public RecvHandler loginRecvCallback;
    public RecvHandler registerRecvCallback;
    public RecvHandler EntityNewRecvCallback;
    public RecvHandler EntityMoveCallback;

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

            case Config.COMMAND_NEW_ENTITY:
                if(EntityNewRecvCallback != null)
                    EntityNewRecvCallback(dataStr);
                break;

            case Config.COMMAND_MOVE:
                if(EntityMoveCallback != null)
                    EntityMoveCallback(dataStr);
                break;
        }
    }
}