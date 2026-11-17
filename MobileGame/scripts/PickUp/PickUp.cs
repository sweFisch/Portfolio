using System.Collections;
using UnityEngine;

public class PickUp : MonoBehaviour
{

    // Play animation
    // Pool?
    // Give change data in game manager or Player

    // Old Coin - not in use!
    private int _value = 1;
    private bool _isActive = true;
    [SerializeField] Animator _animator;

    private static int PICKUP_FAST_SPIN = Animator.StringToHash("Pickup_FastSpin");


    private void OnTriggerEnter(Collider other)
    {
        if (!_isActive) { return; }

        PlayerStateMachine player = other.GetComponent<PlayerStateMachine>();
        if(player != null)
        {
            _isActive = false;
            UIManager.Instance.UpdateCoinsTMP(_value);

            player.GetComponent<PlayerVFX>()?.PlayPickupEffect();

            //_animator.CrossFade(PICKUP_FAST_SPIN, 0f, 0);
            //Destroy(gameObject);
            gameObject.SetActive(false);
        }
    }

}
