using System;
using System.Collections.Generic;
using UnityEngine;

public class SimplePool : MonoBehaviour
{
    //public static SimplePool instance;
    public List<GameObject> pooledObjects;
    public GameObject objectToPoolPrefab;
    public int amountToPool = 4;
    public int maxPoolSize = 1000;

    public int activeObjects = 0;

    //Stack<GameObject> pool = new Stack<GameObject>();

    private void Awake()
    {
        //if (instance == null) 
        //{ 
        //    instance = this;
        //    InitializePool(amountToPool);
        //}
        //else
        //{
        //    Destroy(this);
        //}

        InitializePool(amountToPool);

    }

    private void InitializePool(int poolSize)
    {
        pooledObjects = new List<GameObject>();
        GameObject tmp;
        for (int i = 0; i < poolSize; i++)
        {
            tmp = Instantiate(objectToPoolPrefab,transform);
            tmp.SetActive(false);
            pooledObjects.Add(tmp);
        }
    }
    private void GrowList()
    {
        Debug.LogWarning($"Active objects {activeObjects} currentPoolSize = {pooledObjects.Count}");
        amountToPool *= 2; // double size

        GameObject[] objectsInOldList = pooledObjects.ToArray();

        pooledObjects = new List<GameObject>();
        GameObject tmp;
        for (int i = 0; i < amountToPool; i++)
        {
            if (i < objectsInOldList.Length)
            {
                pooledObjects.Add(objectsInOldList[i]);
            }
            else
            {
                tmp = Instantiate(objectToPoolPrefab, transform);
                tmp.SetActive(false);
                pooledObjects.Add(tmp);
            }
        }
        Debug.LogWarning($"Growing complete : Active objects {activeObjects} currentPoolSize = {pooledObjects.Count}");
    }

    public GameObject GetPooledObject()
    {
        if (pooledObjects.Count < maxPoolSize 
            && pooledObjects.Count < activeObjects - 1)
        {
            GrowList();
        }

        for (int i = 0; i < amountToPool; i++)
        {
            if (!pooledObjects[i].activeInHierarchy) // if not active in hierarcy
            {
                activeObjects++;
                return pooledObjects[i]; // return it
            }
        }

        return null;
    }

    public void ReturnPooledObject(GameObject objectToReturn)
    {
        activeObjects--;
        objectToReturn.transform.SetParent(transform);
        objectToReturn.SetActive(false);
    }
}
