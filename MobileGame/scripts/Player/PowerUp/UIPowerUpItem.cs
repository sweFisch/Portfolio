using UnityEngine;
using UnityEngine.UI;

public class UIPowerUpItem : MonoBehaviour
{

    [SerializeField] private Image _icon;
    [SerializeField] private Image _timerImage;

    float _maxDuration;
    public EPowerUp _powerUpType;

    public void SetupPowerItem(Sprite icon, float duration, EPowerUp ePowerUp)
    {
        _powerUpType = ePowerUp;
        _icon.sprite = icon;
        _timerImage.fillAmount = 1;
        _maxDuration = duration;
        gameObject.SetActive(true);
    }

    public void UpdateDuration(float duration)
    {
        if (_maxDuration > 0)
        {
            _timerImage.fillAmount = (duration / _maxDuration);
        }
    }

    public void Deactivate()
    {
        gameObject.SetActive(false);
    }
}
