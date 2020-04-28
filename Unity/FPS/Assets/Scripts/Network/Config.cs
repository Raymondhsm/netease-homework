class Config
{

    public const int COMMAND_LENGTH_SIZE = 4;

    public const int COMMAND_MOVE = 0x00;
    public const int COMMAND_RUN = 0x01;
    public const int COMMAND_JUMP = 0x02;
    public const int COMMAND_SHOOT = 0x03;
    public const int COMMAND_HIT = 0x04;
    public const int COMMAND_RELOAD = 0x05;
    public const int COMMAND_DEAD = 0x06;

    public const int COMMAND_NPC_COMMON = 0x80;
    public const int COMMAND_NPC_ATTACK = 0x81;
    public const int COMMAND_NPC_RESET = 0x82;

    public const int COMMAND_NEW_ENTITY = 0x0100;
    public const int COMMAND_DEL_ENTITY = 0x0200;
    public const int COMMAND_REGISTER_WITHOUT_EID = 0x0300;
    public const int COMMAND_UPDATE_ENTITY = 0x0400;

    public const int COMMAND_REGISTER = 0x010000;
    public const int COMMAND_LOGIN = 0x020000;
    public const int COMMAND_NEW_CLIENT = 0x030000;

    public const int ENTITY_PLAYER = 0x01000000;
    public const int ENTITY_PLAYER_OTHER = 0x02000000;
    public const int ENTITY_ENEMY = 0x03000000;
    public const int ENTITY_BULLET = 0x04000000;

}