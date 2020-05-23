using System;
using UnityEngine;

[Serializable]
class FpsInput
{
    [Tooltip("The name of the virtual axis mapped to rotate the camera around the y axis."), SerializeField]
    private string rotateX = "Mouse X";

    [Tooltip("The name of the virtual axis mapped to rotate the camera around the x axis."), SerializeField]
    private string rotateY = "Mouse Y";

    [Tooltip("back and forth."), SerializeField]
    private string move = "Horizontal";

    [Tooltip("left and right."), SerializeField]
    private string strafe = "Vertical";

    [Tooltip("The name of the virtual button mapped to run."), SerializeField]
    private string run = "Fire3";

    [Tooltip("The name of the virtual button mapped to jump."), SerializeField]
    private string jump = "Jump";

    [Tooltip("reload bullet."), SerializeField]
    private string reload = "Reload";
    [SerializeField]
    private string p1 = "p1";
    [SerializeField]
    private string p2 = "p2";


    /// Returns the value of the virtual axis mapped to rotate the camera around the y axis.
    public float RotateX
    {
        get { return Input.GetAxisRaw(rotateX); }
    }

    /// Returns the value of the virtual axis mapped to rotate the camera around the x axis.        
    public float RotateY
    {
        get { return Input.GetAxisRaw(rotateY); }
    }

    /// Returns the value of the virtual axis mapped to move the character back and forth.        
    public float Move
    {
        get { return Input.GetAxisRaw(move); }
    }

    /// Returns the value of the virtual axis mapped to move the character left and right.         
    public float Strafe
    {
        get { return Input.GetAxisRaw(strafe); }
    }

    /// Returns true while the virtual button mapped to run is held down.          
    public bool Run
    {
        get { return Input.GetButton(run); }
    }

    /// Returns true during the frame the user pressed down the virtual button mapped to jump.          
    public bool Jump
    {
        get { return Input.GetButtonDown(jump); }
    }

    public bool Reload
    {
        get { return Input.GetButtonDown(reload); }
    }

    public bool P1
    {
        get { return Input.GetButtonDown(p1); }
    }

    public bool P2
    {
        get { return Input.GetButtonDown(p2); }
    }

    public bool isChange
    {
        get { return Move != 0 || Strafe != 0 || Run || Jump || RotateX != 0 || RotateY != 0; }
    }
}

[Serializable]
class ShootingInput
{
    [Tooltip("shoot"), SerializeField]
    private string shoot = "Fire1";

    [Tooltip("reload"), SerializeField]
    private string reload = "Reload";

    [Tooltip("Mode"), SerializeField]
    private string mode = "Mode";

    [Tooltip("Magic"), SerializeField]
    private string magic = "Magic";

    public bool Shoot
    {
        get { return LeftButtonDown || LeftButtonHold; }
    }

    public bool LeftButtonDown
    {
        get { return Input.GetButtonDown(shoot); }
    }

    public bool LeftButtonHold
    {
        get { return Input.GetButton(shoot); }
    }

    public bool Mode
    {
        get { return Input.GetButtonDown(mode); }
    }

    public bool Reload
    {
        get { return Input.GetButtonDown(reload); }
    }

    public bool Magic
    {
        get { return Input.GetButtonDown(magic); }
    }
}