using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RewardEntity : Entity
{
    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        Type = Config.ENTITY_REWARD;
    }

    // Update is called once per frame
    void Update()
    {
        
    }


    public override void PickedUp(PickUpRecv pickUpRecv, string publicID)
    {
        if(pickUpRecv.publicID == publicID)
            Debug.Log("1");
        Status = 2;
        Destroy(gameObject);
    }
}
