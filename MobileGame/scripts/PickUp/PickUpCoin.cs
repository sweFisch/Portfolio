using UnityEngine;

public class PickUpCoin : MonoBehaviour
{
    bool _isActive = true;

    private Transform _target; // Magnet uses this

    private void OnEnable()
    {
        _isActive = true;
    }
    private void OnDisable()
    {
        _isActive = true;
        _target = null;
    }
    private void OnDestroy()
    {
        _isActive = false;
        _target = null;
    }

    public void SetTarget (Transform target)
    {
        if( _target == null)
        {
            _target = target;
            transform.parent = null;
        }
    }

    private void FixedUpdate()
    {
        if (_isActive && _target != null)
        { 
            transform.Translate((_target.position - transform.position).normalized * Time.deltaTime * 20f); // Game speed Dependent ? 
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (!_isActive) { return; }

        PlayerStateMachine player = other.GetComponent<PlayerStateMachine>();
        if (player != null)
        {
            _isActive = false;
            _target = null;

            player.GetComponent<PlayerVFX>()?.PlayPickupEffect();

            PowerUpManager powerUpManager = player.GetComponent<PowerUpManager>();
            if (powerUpManager != null && powerUpManager.IsPowerUpActive(EPowerUp.DoubleCoins))
            {
                GameManager.Instance._runData.Coins += 2;
                //UIManager.Instance.UpdateCoinsTMP(2);
            }
            else
            {
                GameManager.Instance._runData.Coins += 1;
                //UIManager.Instance.UpdateCoinsTMP(1);
            }

            // Play Sound
            SoundManager.Instance.PlayCoinPickup(transform.position);

            // Return to pool
            _isActive = false;
            _target = null;
            gameObject.SetActive(false);
            PoolerService.coinPool.ReturnGameObjectToPool(gameObject);
        }
    }
}
