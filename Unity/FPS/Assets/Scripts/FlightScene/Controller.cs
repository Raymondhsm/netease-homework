using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Controller : MonoBehaviour
{
    public string sessionID;
    public bool fightEnd;

    void Start()
    {
        sessionID = "";
        fightEnd = false;
    }

}
