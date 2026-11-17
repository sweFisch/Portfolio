using UnityEngine;
using UnityEngine.UI;

/// <summary>
/// Base PowerUp  class
/// Write your own, ActivatePowerUp
/// DeactivatePowerUp
/// </summary>
public class PowerUp : MonoBehaviour
{
    public PowerUpDataSO PowerUpDataSO;
    public float _timer;
    public UIPowerUpItem _uiPowerUpItem;

    public virtual Sprite GetIconImage()
    {
        return PowerUpDataSO.IconSprite;
    }
    public virtual string GetDisplayName()
    {
        return PowerUpDataSO.DisplayName;
    }
    public void SetUIConnection(UIPowerUpItem uiPowerUpItem)
    {
        _uiPowerUpItem = uiPowerUpItem;
        _uiPowerUpItem.SetupPowerItem(PowerUpDataSO.IconSprite, PowerUpDataSO.ActiveTime, PowerUpDataSO.ePowerUp);
    }
    public virtual void ActivatePowerUp() 
    {
        _timer = PowerUpDataSO.ActiveTime;
        //Debug.Log($"{PowerUpDataSO.name} is Activated");

    }

    /// <summary>
    /// Tick the timer on the PowerUp
    /// </summary>
    /// <param name="timeTick"></param>
    /// <returns>Returns true if Depleted</returns>
    public bool IsPowerUpDepleted(float timeTick)
    {
        if (_uiPowerUpItem != null)
        {
            _timer -= timeTick;
            _uiPowerUpItem.UpdateDuration(_timer); // Update ui fill image
        }
        return _timer <= 0f;
    }

    public virtual void DeactivatePowerUp() 
    {
        _timer = 0f;
        
        if (_uiPowerUpItem != null)
        {
            _uiPowerUpItem.Deactivate();
        }
        _uiPowerUpItem = null;
        //Debug.Log($"{PowerUpDataSO.name} is depleted");
    }

    public virtual void UsePowerUp() 
    {
        
    }
}
