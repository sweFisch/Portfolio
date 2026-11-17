using UnityEngine;

public class PickUpPowerUp : MonoBehaviour
{
    [SerializeField] EPowerUp powerUp;
    bool _isActive = true;

    private void OnTriggerEnter(Collider other)
    {
        if (!_isActive) { return; }

        PowerUpManager powerUpManager = other.GetComponent<PowerUpManager>();
        if (powerUpManager != null)
        {
            _isActive = false;

            powerUpManager.ActivatePowerUp(powerUp);

            if (powerUp == EPowerUp.Flying)
            {
                PlayerStateMachine player = other.gameObject.GetComponent<PlayerStateMachine>();
                if (player != null)
                {
                    print("--- calling effectt powerup flying ---");
                    player.EffectPowerUpFlying();
                }
            }

            gameObject.SetActive(false);
            _isActive = true;
        }
    }

}
