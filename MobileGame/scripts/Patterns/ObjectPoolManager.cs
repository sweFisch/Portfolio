using UnityEngine;
using System.Collections.Generic;
using UnityEngine.Pool;

//The Ultimate Clean, GENERIC Object Pool System (Unity Tutorial)
//Sasquatch B Studios
// https://www.youtube.com/watch?v=Ah3epb2HGCw
// last 4 minutes extensions to take a parent Transform is not included did not see the need

public class ObjectPoolManager : MonoBehaviour
{
    [SerializeField] private bool _addToDontDestroyOnLoad = false;

    private GameObject _emptyHolder;

    private static GameObject _particleSystemsEmpty;
    private static GameObject _gameObjectsEmpty;
    private static GameObject _soundFXEmpty;

    private static Dictionary<GameObject, ObjectPool<GameObject>> _objectPoolsDict;
    private static Dictionary<GameObject, GameObject> _cloneToPrefabMapDict;

    public enum PoolType // Tells the pooler where to spawn the objects in
    {
        ParticleSystem,
        GameObjects,
        SoundFX
    }

    public static PoolType PoolingType;

    private void Awake()
    {
        _objectPoolsDict = new Dictionary<GameObject, ObjectPool<GameObject>>();
        _cloneToPrefabMapDict = new Dictionary<GameObject, GameObject>();

        SetUpEmpties();
    }

    private void SetUpEmpties()
    {
        // Create hirarchy for pooling empty objects
        _emptyHolder = new GameObject("Object Pools");

        _gameObjectsEmpty = new GameObject("Game Objects");
        _gameObjectsEmpty.transform.parent = _emptyHolder.transform;
        
        _particleSystemsEmpty = new GameObject("Particle Effects");
        _particleSystemsEmpty.transform.parent = _emptyHolder.transform;
        
        _soundFXEmpty = new GameObject("SoundFX");
        _soundFXEmpty.transform.parent = _emptyHolder.transform;

        if (_addToDontDestroyOnLoad) 
        {
            DontDestroyOnLoad(_soundFXEmpty.transform.root); // If we select dont destroy on load, usally not needed - can be for sound
        }
    }
    // Normal using position
    private static void CreatePool(GameObject prefab, Vector3 pos, Quaternion rot, PoolType poolType = PoolType.GameObjects)
    {
        ObjectPool<GameObject> pool = new ObjectPool<GameObject>(
            createFunc: () => CreateObject(prefab, pos, rot, poolType),
            actionOnGet: OnGetObject,
            actionOnRelease: OnReleaseObject,
            actionOnDestroy: OnDestroyObject
            );

        _objectPoolsDict.Add(prefab,pool);
    }
    // Using a Transform parent
    private static void CreatePool(GameObject prefab, Transform parent, Quaternion rot, PoolType poolType = PoolType.GameObjects)
    {
        ObjectPool<GameObject> pool = new ObjectPool<GameObject>(
            createFunc: () => CreateObject(prefab, parent, rot, poolType),
            actionOnGet: OnGetObject,
            actionOnRelease: OnReleaseObject,
            actionOnDestroy: OnDestroyObject
            );

        _objectPoolsDict.Add(prefab, pool);
    }
    // Position based
    private static GameObject CreateObject(GameObject prefab, Vector3 pos, Quaternion rot, PoolType poolType = PoolType.GameObjects)
    {
        prefab.SetActive(false); // set prefab disabled to awake wont run during spawning

        GameObject obj = Instantiate(prefab, pos, rot);
        
        prefab.SetActive(true);

        GameObject parentObject = SetParentObject(poolType);
        obj.transform.parent = parentObject.transform;

        return obj;
    }
    // Transform parent based
    private static GameObject CreateObject(GameObject prefab, Transform parent, Quaternion rot, PoolType poolType = PoolType.GameObjects)
    {
        prefab.SetActive(false); // set prefab disabled to awake wont run during spawning

        GameObject obj = Instantiate(prefab, parent);

        obj.transform.localPosition = Vector3.zero;
        obj.transform.localRotation = rot;
        obj.transform.localScale = Vector3.one;

        prefab.SetActive(true);

        return obj;
    }
    private static void OnGetObject(GameObject obj)
    {
        // Optional logic
    }
    private static void OnReleaseObject(GameObject obj)
    {
        obj.SetActive(false);
    }
    private static void OnDestroyObject(GameObject obj)
    {
        if (_cloneToPrefabMapDict.ContainsKey(obj))
        {
            _cloneToPrefabMapDict.Remove(obj);
        }
    }

    private static GameObject SetParentObject(PoolType poolType)
    {
        switch (poolType)
        {
            case PoolType.ParticleSystem:
                return _particleSystemsEmpty;
            
            case PoolType.GameObjects:
                return _gameObjectsEmpty;
            
            case PoolType.SoundFX:
                return _soundFXEmpty;

            default:
                return null;
        }
    }

    // Object is the base class for all Unity Objects (Monobehavior , Components ect)
    private static T SpawnObject<T>(GameObject objectToSpawn, Vector3 spawnPos, Quaternion spawnRotation, PoolType poolType = PoolType.GameObjects) where T : Object
    {
        if (!_objectPoolsDict.ContainsKey(objectToSpawn))
        {
            CreatePool(objectToSpawn, spawnPos, spawnRotation, poolType);
        }

        GameObject obj = _objectPoolsDict[objectToSpawn].Get(); // Get() is from unitys pool system

        if(obj != null)
        {
            // Add to object dictionary so we can return it to the pool correctly
            if (!_cloneToPrefabMapDict.ContainsKey(obj))
            {
                _cloneToPrefabMapDict.Add(obj, objectToSpawn);
            }

            obj.transform.position = spawnPos;
            obj.transform.rotation = spawnRotation;
            obj.gameObject.SetActive(true);

            // if we spawn a GameObject type
            if (typeof(T) == typeof(GameObject)) 
            {
                return obj as T;
            }

            // Otherwise it will be a component
            T component = obj.GetComponent<T>();
            if (component == null) 
            {
                Debug.LogError($"Object Pooler Manager : {objectToSpawn.name} , does not have a component type of {typeof(T)}");
                return null;
            }

            return component;
        }

        return null; // if our object is null
    }
    // Overloaded SpawnObject - Position functions 
    public static T SpawnObject<T>(T typePrefab, Vector3 spawnPos, Quaternion spawnRotation, PoolType poolType = PoolType.GameObjects) where T : Component
    {
        return SpawnObject<T>(typePrefab.gameObject, spawnPos, spawnRotation, poolType);
    }

    public static GameObject SpawnObject(GameObject objectToSpawn, Vector3 spawnPos, Quaternion spawnRotation, PoolType poolType = PoolType.GameObjects)
    {
        return SpawnObject<GameObject>(objectToSpawn, spawnPos, spawnRotation, poolType);
    }

    // Handling parenting directly
    private static T SpawnObject<T>(GameObject objectToSpawn, Transform parent, Quaternion spawnRotation, PoolType poolType = PoolType.GameObjects) where T : Object
    {
        if (!_objectPoolsDict.ContainsKey(objectToSpawn))
        {
            CreatePool(objectToSpawn, parent, spawnRotation, poolType);
        }

        GameObject obj = _objectPoolsDict[objectToSpawn].Get(); // Get() is from unitys pool system

        if (obj != null)
        {
            // Add to object dictionary so we can return it to the pool correctly
            if (!_cloneToPrefabMapDict.ContainsKey(obj))
            {
                _cloneToPrefabMapDict.Add(obj, objectToSpawn);
            }

            obj.transform.SetParent(parent);
            obj.transform.localPosition = Vector3.zero;
            obj.transform.localRotation = spawnRotation;
            //obj.transform.localScale = Vector3.one;
            obj.gameObject.SetActive(true);

            // if we spawn a GameObject type
            if (typeof(T) == typeof(GameObject))
            {
                return obj as T;
            }

            // Otherwise it will be a component
            T component = obj.GetComponent<T>();
            if (component == null)
            {
                Debug.LogError($"Object Pooler Manager : {objectToSpawn.name} , does not have a component type of {typeof(T)}");
                return null;
            }

            return component;
        }

        return null; // if our object is null
    }
    // Overloaded SpawnObject - Parent functions 
    public static T SpawnObject<T>(T typePrefab, Transform parent, Quaternion spawnRotation, PoolType poolType = PoolType.GameObjects) where T : Component
    {
        return SpawnObject<T>(typePrefab.gameObject, parent, spawnRotation, poolType);
    }

    public static GameObject SpawnObject(GameObject objectToSpawn, Transform parent, Quaternion spawnRotation, PoolType poolType = PoolType.GameObjects)
    {
        return SpawnObject<GameObject>(objectToSpawn, parent, spawnRotation, poolType);
    }

    public static void ReturnObjectToPool(GameObject obj, PoolType poolType = PoolType.GameObjects)
    {
        // Checking so object is pooled
        if (_cloneToPrefabMapDict.TryGetValue(obj, out GameObject prefab))
        {
            // Set parent to pool empty
            GameObject parentObject = SetParentObject(poolType);

            if (obj.transform.parent != parentObject.transform)
            {
                obj.transform.SetParent(parentObject.transform);
            }
            // Release object from the pool
            if (_objectPoolsDict.TryGetValue(prefab, out ObjectPool<GameObject> pool))
            {
                pool.Release(obj);
            }

        }
        else
        {
            Debug.LogWarning($"Trying to return an obj that is not pooled: {obj.name}");
        }
    }
}
