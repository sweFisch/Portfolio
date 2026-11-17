using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;

/// <summary>
/// Placed on level chunk prefebs
/// Chunk patterns uses this data and the Level State machine
/// To generate the level
/// </summary>
public class ChunkData : MonoBehaviour
{
    public EChunkType ChunkType;
    
    public ItemPlacemnetData itemPlacementData;
    public List<RemoveDanger> dangerRemovalList = new List<RemoveDanger>();

    private float _laneWidth = 1.5f;

    List<GameObject> coinList = new List<GameObject>();
    List<GameObject> dangerList = new List<GameObject>();
    List<GameObject> powerUpList = new List<GameObject>();


    public void SetupChunk()
    {
        //Debug.Log("Setting up level Chunk - Spawning obstacles and coins ect");
        ReEnableDanger();

        SpawnCoins();
        SpawnDangers();
        SpawnPowerUps();
    }
    public void ReEnableDanger()
    {
        foreach (RemoveDanger dangerRemoved in dangerRemovalList) 
        {
            dangerRemoved.EnableDanger();
        }
    }

    public void SpawnCoins()
    {
        if (itemPlacementData != null && itemPlacementData.coinPlacementDataArray != null)
        {
            Vector3[] coinPlacement = itemPlacementData.GetCoinPlacementPosData();

            for(int i = 0; i< coinPlacement.Length; i++)
            {
                Vector3 pos = coinPlacement[i];

                // Get coin from pool!
                GameObject coin = PoolerService.coinPool.GetPooledObject(); // using my version of a simple pool system
                coin.transform.position = pos;
                coin.transform.SetParent(transform);
                coin.SetActive(true);
                coinList.Add(coin);
            }
        }
    }

    public void spawnCoinsInRandomLane()
    {
        if (UnityEngine.Random.Range(0, 10) > 6) { return; }

        int lane = UnityEngine.Random.Range(-1, 2);
        int startValue = 0;
        int endValue = 10;

        for (int i = startValue; i < endValue; i += 3)
        {
            // Random chance to switch lanes
            if (UnityEngine.Random.Range(0, 10) > 8)
            {
                if (lane == -1) { lane++; }
                else if (lane == 1) { lane--; }
                else { lane = UnityEngine.Random.Range(-1, 2); }
            }
            // Get coin from pool!
            GameObject coin = PoolerService.coinPool.GetPooledObject(); // using my version of a simple pool system
            coin.transform.position = new Vector3(lane * _laneWidth, 1f, i) + transform.position;
            coin.transform.SetParent(transform);
            coin.SetActive(true);
            coinList.Add(coin);

            //Instantiate(_coinPrefabTODO, new Vector3(lane * _laneWidth, 1f, i) + parentPiece.position, Quaternion.identity, parentPiece);
        }
    }

    public void SpawnDangers()
    {
        if (itemPlacementData != null && itemPlacementData.dangerDataArray != null)
        {
            for (int i = 0; i < itemPlacementData.dangerDataArray.Length; i++)
            {
                EDangerProp type = itemPlacementData.dangerDataArray[i].dangerPropType;
                GameObject item = PoolerService.GetDangerProp(type, transform);

                if (type == EDangerProp.HighMoving || type == EDangerProp.LowMoving)
                {
                    MovingPlattform movingPlattform = item.GetComponent<MovingPlattform>();
                    Vector3 posVec = itemPlacementData.dangerDataArray[i].pos;
                    movingPlattform.Initialize(posVec.x,posVec.z);
                    item.SetActive(true);
                    dangerList.Add(item);
                }
                else
                {
                    //item.transform.SetParent(transform);
                    item.transform.localPosition = itemPlacementData.dangerDataArray[i].pos;
                    item.SetActive(true);
                    dangerList.Add(item);
                }
            }
        }
    }

    public void SpawnPowerUps()
    {
        if (itemPlacementData != null && itemPlacementData.powerUpDataArray != null)
        {
            foreach(var power in itemPlacementData.powerUpDataArray)
            {
                GameObject item = PoolerService.GetPowerUp(power.powerUpType,transform);
                item.transform.localPosition = power.pos;
                item.SetActive(true);
                powerUpList.Add(item);
            }
        }
    }

    public void DestroyLevelChunk()
    {
        //Debug.Log("Destroys or recycles Level Chunk");
        for (int i = coinList.Count - 1; i >= 0; i--)
        {
            if (coinList[i] != null && coinList[i].transform.parent == transform)
            {
                PoolerService.coinPool.ReturnGameObjectToPool(coinList[i]);
            }
        }
        coinList.Clear();

        for (int i = dangerList.Count - 1; i >= 0; i--)
        {
            if(dangerList[i] != null)
            {
                ObjectPoolManager.ReturnObjectToPool(dangerList[i]);
            }
        }
        dangerList.Clear();

        for (int i = powerUpList.Count - 1; i >= 0; i--)
        {
            if (powerUpList[i] != null)
            {
                ObjectPoolManager.ReturnObjectToPool(powerUpList[i]);
            }
        }
        powerUpList.Clear();

        // Return this chunk to pool
        ObjectPoolManager.ReturnObjectToPool(this.gameObject);

        //Destroy(this.gameObject);
    }
}