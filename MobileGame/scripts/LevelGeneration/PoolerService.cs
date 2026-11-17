using UnityEngine;
using System.Collections.Generic;
using System;

public class PoolerService : MonoBehaviour
{
    // Holding reference to a simple coin spawner
    public static SimpleGOPool coinPool;
    public SimpleGOPool _coinPool;

    // Seting up danger props to use in level
    public static DangerPropsSO dangerCollection;
    public DangerPropsSO _dangerCollection;

    // setting up powerUps to use in level
    public static PowerUpPickUpCollectionSO powerUpPickUpCollection;
    public PowerUpPickUpCollectionSO _powerUpPickUpCollection;

    private void Awake()
    {
        coinPool = _coinPool;
        
        dangerCollection = _dangerCollection;

        powerUpPickUpCollection = _powerUpPickUpCollection;
    }
    private void Start()
    {
        SetupDangerObjects();

        SetupPowerUpObjects();
    }

    private void SetupPowerUpObjects()
    {
        Vector3 tempPos = new Vector3(-100,-100,-100);
        List<GameObject> tempCollectionPrefabRef = new List<GameObject>();
        tempCollectionPrefabRef.Add(powerUpPickUpCollection.GetPrefabForEPowerUp(EPowerUp.PowerJump));
        tempCollectionPrefabRef.Add(powerUpPickUpCollection.GetPrefabForEPowerUp(EPowerUp.DoubleCoins));
        tempCollectionPrefabRef.Add(powerUpPickUpCollection.GetPrefabForEPowerUp(EPowerUp.CoinMagnet));
        tempCollectionPrefabRef.Add(powerUpPickUpCollection.GetPrefabForEPowerUp(EPowerUp.DoubleCoins));
        tempCollectionPrefabRef.Add(powerUpPickUpCollection.GetPrefabForEPowerUp(EPowerUp.Sheild));
        tempCollectionPrefabRef.Add(powerUpPickUpCollection.GetPrefabForEPowerUp(EPowerUp.Flying));

        // Prewarm Pool with one type of powerUp
        foreach (GameObject temp in tempCollectionPrefabRef)
        {
            GameObject temp2 = ObjectPoolManager.SpawnObject(temp, tempPos, Quaternion.identity);
            ObjectPoolManager.ReturnObjectToPool(temp2);
        }
    }

    private void SetupDangerObjects() 
    {
        Vector3 tempPos = new Vector3(-100,-100,-100);

        List<GameObject> tempCollectionPrefabRef = new List<GameObject>();

        tempCollectionPrefabRef.Add(dangerCollection.GetProp(EDangerProp.Low));
        tempCollectionPrefabRef.Add(dangerCollection.GetProp(EDangerProp.LowDuck));
        tempCollectionPrefabRef.Add(dangerCollection.GetProp(EDangerProp.High));
        tempCollectionPrefabRef.Add(dangerCollection.GetProp(EDangerProp.HighDuck));
        tempCollectionPrefabRef.Add(dangerCollection.GetProp(EDangerProp.LowPlattform));
        tempCollectionPrefabRef.Add(dangerCollection.GetProp(EDangerProp.HighPlattform));
        tempCollectionPrefabRef.Add(dangerCollection.GetProp(EDangerProp.HighMoving));
        tempCollectionPrefabRef.Add(dangerCollection.GetProp(EDangerProp.LowMoving));

        // Prewarm Pool with one type of prop
        foreach (GameObject temp in tempCollectionPrefabRef)
        {
            GameObject temp2 = ObjectPoolManager.SpawnObject(temp, tempPos, Quaternion.identity);
            ObjectPoolManager.ReturnObjectToPool(temp2);
        }
    }

    public static GameObject GetDangerProp(EDangerProp propType,Transform parent)
    {
        return ObjectPoolManager.SpawnObject((dangerCollection.GetProp(propType)), parent, Quaternion.identity);
    }

    public static GameObject GetDangerProp(EDangerProp propType, Vector3 pos)
    {
        return ObjectPoolManager.SpawnObject((dangerCollection.GetProp(propType)), pos, Quaternion.identity);
    }

    public static GameObject GetPowerUp(EPowerUp ePowerUp,Transform parent)
    {
        return ObjectPoolManager.SpawnObject((powerUpPickUpCollection.GetPrefabForEPowerUp(ePowerUp)),parent,Quaternion.identity);
    }
    public static GameObject GetPowerUp(EPowerUp ePowerUp, Vector3 pos)
    {
        return ObjectPoolManager.SpawnObject((powerUpPickUpCollection.GetPrefabForEPowerUp(ePowerUp)), pos, Quaternion.identity);
    }
}
