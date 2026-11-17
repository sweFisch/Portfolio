using UnityEngine;

public class PowerMagnet : PowerUp
{
    bool _isActive = false;
    float _suckingRadius = 10f;

    public override void ActivatePowerUp()
    {
        _isActive = true;
        base.ActivatePowerUp();
    }

    public override void DeactivatePowerUp()
    {
        _isActive = false;
        base.DeactivatePowerUp();
    }

    private void FixedUpdate()
    {
        if (_isActive == false) { return; }
        
        Collider[] objects = Physics.OverlapSphere(transform.position, _suckingRadius);
        foreach (Collider obj in objects) 
        {
            PickUpCoin coin = obj.GetComponent<PickUpCoin>();
            if( coin != null)
            {
                coin.SetTarget(transform);
            }
        }
    }
}
