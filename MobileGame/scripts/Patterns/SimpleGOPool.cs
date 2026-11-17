using nfObjectPool;
using System.Collections.Generic;
using UnityEngine;

public class SimpleGOPool : MonoBehaviour
{
    public GameObject _prefabGO;
    public int _defaultPoolSize = 10;
    public int _maxPoolSize = 100;
    public int _currentSize = 0;
    LinkedList<GameObject> _goList = new LinkedList<GameObject>();


    private void Start()
    {
        // Starting size
        for (int i = 0; i < _defaultPoolSize; i++)
        {
            var go = CreatePooledObject();
            go.gameObject.SetActive(false);
            _goList.AddLast(go);
        }
    }

    private GameObject CreatePooledObject()
    {
        GameObject go = Instantiate(_prefabGO);
        //go.SetPool(this); // Set a reference in the Type of the Pool (this)
        go.transform.parent = transform;

        return go;
    }

    public GameObject GetPooledObject()
    {
        if (_goList.Count == 0)
        {

            return CreatePooledObject();
        }

        // take a object form our list and remove it from the list
        var go = _goList.Last.Value;
        _goList.RemoveLast();
        //go.gameObject.SetActive(true); // dont set active as coins need to be placed first..

        return go; // return the coin
    }

    public void ReturnGameObjectToPool(GameObject go)
    {
        if (_goList.Count > _maxPoolSize)
        {
            //Debug.Log("DestroyGO - COIN!");
            Destroy(go.gameObject);
            return;
        }

        // Reset the go and hide
        go.transform.parent = transform; // put under pool transform
        _goList.AddFirst(go);
        go.gameObject.SetActive(false);
    }

    private void OnDisable()
    {
        foreach (var go in _goList)
        {
            Destroy(go);
        }

        _goList.Clear();
    }
}
