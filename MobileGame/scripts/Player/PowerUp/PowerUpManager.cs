using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;

public class PowerUpManager : MonoBehaviour
{
    [SerializeField] private List<PowerUp> _powerUpList;
    [SerializeField] private List<PowerUp> _activePowerUpList;

    private void Awake()
    {
        if (_powerUpList == null)
        {
            _powerUpList = new List<PowerUp>();
        }
        if( _activePowerUpList == null)
        {
            _activePowerUpList = new List<PowerUp>();
        }
    }

    public void ActivatePowerUp(EPowerUp powerUpType)
    {
        // Alt- Code for turning off other similar type of code goes here 
        foreach (var powerUp in _powerUpList)
        {
            // Compare enum
            if (powerUp.PowerUpDataSO.ePowerUp == powerUpType)
            {
                foreach (var activePowerUp in _activePowerUpList)
                {
                    if (activePowerUp.PowerUpDataSO.ePowerUp == powerUpType)
                    {
                        //print("Refersh Powerup in list");

                        activePowerUp.ActivatePowerUp();
                        
                        return;
                    }
                }
                // Add to active list
                //print("Added Powerup to list");
                _activePowerUpList.Add(powerUp);

                powerUp.ActivatePowerUp();
                
                // do stuff to add to UI
                powerUp.SetUIConnection(UIManager.Instance.GetUIPowerUpItem());

                return;
            }
        }
    }

    private void Update()
    {
        DebugKeys();

        if (_activePowerUpList.Count <= 0) {return; }

        float timeTick = Time.deltaTime;
        for (int i = _activePowerUpList.Count - 1; i >= 0; i--)
        {
            PowerUp powerUp = _activePowerUpList[i];
            if (powerUp.IsPowerUpDepleted(timeTick))
            {
                powerUp.DeactivatePowerUp(); // Run Deactivation Code
                _activePowerUpList.Remove(powerUp); // remove from list
            }
        }
    }

    public bool IsPowerUpActive(EPowerUp powerUpType)
    {
        if (_activePowerUpList.Count <= 0) {  return false; }

        foreach (var activePowerUp in _activePowerUpList)
        {
            if (activePowerUp.PowerUpDataSO.ePowerUp == powerUpType)
            {
                return true;
            }
        }

        return false;
    }

    public bool UsePowerUpIfActive(EPowerUp powerUpType)
    {
        if (_activePowerUpList.Count <= 0) { return false; }

        foreach (var activePowerUp in _activePowerUpList)
        {
            if (activePowerUp.PowerUpDataSO.ePowerUp == powerUpType)
            {
                activePowerUp.UsePowerUp();
                return true;
            }
        }

        return false;
    }

    private void DebugKeys()
    {
        if (Keyboard.current[Key.Digit1].wasPressedThisFrame)
        {
            ActivatePowerUp(EPowerUp.PowerJump);
        }
        if (Keyboard.current[Key.Digit2].wasPressedThisFrame)
        {
            ActivatePowerUp(EPowerUp.Flying);
            PlayerStateMachine player = GetComponent<PlayerStateMachine>();
            player.EffectPowerUpFlying();
        }
        if (Keyboard.current[Key.Digit3].wasPressedThisFrame)
        {
            ActivatePowerUp(EPowerUp.Sheild);
        }
        if (Keyboard.current[Key.Digit4].wasPressedThisFrame)
        {
            ActivatePowerUp(EPowerUp.DoubleCoins);
        }
        if (Keyboard.current[Key.Digit5].wasPressedThisFrame)
        {
            ActivatePowerUp(EPowerUp.CoinMagnet);
        }
    }
}
