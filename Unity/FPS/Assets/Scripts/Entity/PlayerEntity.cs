using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerEntity : Entity
{
    private FpsInput m_fpsInput;
    private test m_moveController;

    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        m_fpsInput = new FpsInput();
        m_moveController = gameObject.GetComponent<test>();
    }

    // Update is called once per frame
    void Update()
    {
        StatusUpload();
    }

    public override void StatusUpload()
    {
        bool toSend = m_fpsInput.Move != 0 || m_fpsInput.Strafe != 0 || m_fpsInput.Run || m_fpsInput.Jump;
        if(!toSend) return;

        EntityMoveInfo en;
        en.move = m_fpsInput.Move;
        en.strafe = m_fpsInput.Strafe;
        en.run = m_fpsInput.Run;
        en.jump = m_fpsInput.Jump;
        en.eid = m_eid;

        string data = JsonUtility.ToJson(en);
        m_network.send(Config.COMMAND_MOVE, data);
    }

    public override void ProcessMoveRecv(EntityMoveInfo entity)
    {
        Debug.Log(entity.move);
        Debug.Log(entity.strafe);
        Debug.Log(entity.run);
        Debug.Log(entity.jump);
        m_moveController.moveCharactor(entity.strafe, entity.move, entity.run);
    }
}
