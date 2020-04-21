class Config
{

    public const int COMMAND_LENGTH_SIZE = 4;
    public const int COMMAND_REGISTER = 0x010000;
    public const int COMMAND_LOGIN = 0x020000;

    public const int COMMAND_MOVE = 0x00;
    public const int COMMAND_RUN = 0x01;
    public const int COMMAND_JUMP = 0x02;
    public const int COMMAND_SHOOT = 0x03;
    public const int COMMAND_HIT = 0x04;
    public const int COMMAND_RELOAD = 0x05;

    public const int COMMAND_NEW_ENTITY = 0x0100;
    public const int COMMAND_DEL_ENTITY = 0x0200;

    public const int ENTITY_PLAYER = 0x01000000;
    public const int ENTITY_BULLET = 0x02000000;

}