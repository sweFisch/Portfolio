using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Pool;

public class PoolTester : MonoBehaviour
{
    [SerializeField] private float spawnTime = 0.2f;
    private float timer;

    GameObject tmp;

    //Queue<GameObject> queueGameObjects;

    public GameObject coinPrefab;

    ObjectPool<GameObject> pool;

    private void OnDisable()
    {
        StopAllCoroutines();
    }

    void Start()
    {
        //StartCoroutine(ObjectSpawnerCO());
        //queueGameObjects = new Queue<GameObject>();

        pool = new ObjectPool<GameObject>(
            createFunc: CreateItem,
            actionOnGet: OnGet,
            actionOnRelease: OnRelease,
            actionOnDestroy: OnDestroyItem,
            collectionCheck: true, // catches double release mistakes
            defaultCapacity:10,
            maxSize: 1024
            );
    }

    // Unity Pool Stuff
    private GameObject CreateItem()
    {
        GameObject go = Instantiate(coinPrefab);
        go.SetActive(false);
        return go;
    }

    private void OnGet(GameObject go)
    {
        go.SetActive(true);
    }
    private void OnRelease(GameObject go)
    {
        go.SetActive(false);
    }
    private void OnDestroyItem(GameObject go)
    {
        Destroy(go);
    }
    // -------



    void Update()
    {
        if (Time.time > timer)
        {
            GameObject tempGO = pool.Get();
            tempGO.transform.position = Random.onUnitSphere * Random.value * 5f;
            timer = Time.time + spawnTime;
        }
    }

    public IEnumerator ObjectSpawnerCO()
    {
        while (true)
        {
            yield return new WaitForSeconds(spawnTime);

            // My pool
            //tmp = SimplePool.instance.GetPooledObject();
            //if (tmp == null) 
            //{ 
            //    tmp = queueGameObjects.Dequeue();
            //    SimplePool.instance.ReturnPooledObject(tmp);

            //    tmp = SimplePool.instance.GetPooledObject();
            //}

            ////print("Spawn Coin");
            //tmp.transform.position = Random.onUnitSphere * Random.value * 5f;
            //tmp.SetActive(true);
            //queueGameObjects.Enqueue(tmp);

            GameObject tempGO = pool.Get();
            tempGO.transform.position = Random.onUnitSphere * Random.value * 5f;

            // return after a delay? ->
            // StartCoroutine(ReturnAfter(tempGO, 1f));
        }
    }

    private System.Collections.IEnumerator ReturnAfter(GameObject gameObject, float seconds)
    {
        yield return new WaitForSeconds(seconds);
        // Give it back to the pool.
        pool.Release(gameObject);
    }
}
