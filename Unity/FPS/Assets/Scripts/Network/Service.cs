

class Service
{
    public delegate void LoginRecvHandler(string str);
    public delegate void RegisterRecvhandler(string str);

    public LoginRecvHandler loginRecvCallback;
    public RegisterRecvhandler registerRecvCallback;

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

    public void ServiceProvess(int command, string dataStr)
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
        }
    }
}