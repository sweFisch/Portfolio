using UnityEngine;

[CreateAssetMenu(fileName = "PowerUpPickUpCollectionSO", menuName = "PowerUp/PowerUpCollectionSO")]

public class PowerUpPickUpCollectionSO : ScriptableObject
{
    public PowerUpDataSO[] powerUpDataSOArray;

    public GameObject GetPrefabForEPowerUp(EPowerUp powerUpType)
    {
        for (int i = 0; i < powerUpDataSOArray.Length; ++i)
        {
            if (powerUpDataSOArray[i].ePowerUp == powerUpType)
            {
               return powerUpDataSOArray[i].prefab;
            }
        }
        Debug.LogWarning("Trying to return a powerUp that does not Exist!");
        return null;
    }
}