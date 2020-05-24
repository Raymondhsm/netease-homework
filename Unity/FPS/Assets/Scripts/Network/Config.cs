class Config
{

    public const int COMMAND_LENGTH_SIZE = 4;

    // 命令
    public const int COMMAND_MOVE = 0x00;
    public const int COMMAND_RUN = 0x01;
    public const int COMMAND_JUMP = 0x02;
    public const int COMMAND_SHOOT = 0x03;
    public const int COMMAND_HIT = 0x04;
    public const int COMMAND_RELOAD = 0x05;
    public const int COMMAND_DEAD = 0x06;
    public const int COMMAND_PICK_UP = 0x07;
    public const int COMMAND_USE_PROP = 0x08;
    public const int COMMAND_MAGIC_ARROW = 0x09;
    public const int COMMAND_MAGIC_EXPLOSE = 0x0A;

    // npc操作
    public const int COMMAND_NPC_COMMON = 0x80;
    public const int COMMAND_NPC_ATTACK = 0x81;
    public const int COMMAND_NPC_RESET = 0x82;
    public const int COMMAND_NPC_SHOOT = 0x83;
    public const int COMMAND_NPC_DIZZY = 0x84;

    // 实体操作
    public const int COMMAND_NEW_ENTITY = 0x0100;
    public const int COMMAND_DEL_ENTITY = 0x0200;
    public const int COMMAND_REGISTER_WITHOUT_EID = 0x0300;
    public const int COMMAND_UPDATE_ENTITY = 0x0400;

    // 注册相关
    public const int COMMAND_REGISTER = 0x010000;
    public const int COMMAND_LOGIN = 0x020000;
    public const int COMMAND_NEW_CLIENT = 0x030000;
    public const int COMMAND_UPDATE_INFO = 0x040000;

    // 实体种类
    public const int ENTITY_PLAYER = 0x01000000;
    public const int ENTITY_PLAYER_OTHER = 0x02000000;
    public const int ENTITY_ENEMY = 0x03000000;
    public const int ENTITY_BULLET = 0x04000000;
    public const int ENTITY_REWARD_MEDICINE = 0x05000000;
    public const int ENTITY_REWARD_BULLET = 0x06000000;
    public const int ENTITY_REWARD = 0x07000000;
    public const int ENTITY_KNIFE_ENEMY = 0x08000000;
    public const int ENTITY_GUN_ENEMY = 0x09000000;

}