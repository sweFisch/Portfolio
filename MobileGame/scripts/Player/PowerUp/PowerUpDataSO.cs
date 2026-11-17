using UnityEngine;
using UnityEngine.UI;

[CreateAssetMenu(fileName ="PowerUpSO", menuName ="PowerUp/PowerUpSO")]
public class PowerUpDataSO : ScriptableObject
{
    public EPowerUp ePowerUp;
    public string DisplayName = "";
    public Sprite IconSprite;
    public float ActiveTime = 5f;
    public string Description = "";
    public GameObject prefab;
}
