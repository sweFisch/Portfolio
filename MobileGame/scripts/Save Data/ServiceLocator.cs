using System.Collections.Generic;
using System.Collections;
using UnityEngine;
using System;

public class ServiceLocator : MonoBehaviour
{
    // singelton pattern
    public static ServiceLocator _instance;

    public static ServiceLocator Instance 
    {
        get 
        {
            if (_instance == null)
            {
                _instance = new GameObject("Service Locator", typeof(ServiceLocator)).GetComponent<ServiceLocator>();
            }
            return _instance;
        }
        private set
        {
            _instance = value;
        } 
    }

    Dictionary<Type,object> _services = new Dictionary<Type,object>();

    private void Awake()
    {
        if (_instance == null)
        {
            _instance = this;
            DontDestroyOnLoad(gameObject);
        }
        else
        {
            Destroy(gameObject);
        }
    }

    /// <summary>
    /// Register a service of type T to the dictionary
    /// Dictionary -key: type of(T), value: serviceInstance 
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="serviceInstance"></param>
    public void Register<T>(T serviceInstance)
    {
        if (_services.ContainsKey(typeof(T)) == false)
        {
            _services.Add(typeof(T), serviceInstance);
        }
        else
        {
            _services[typeof(T)] = serviceInstance;
        }
    }

    /// <summary>
    /// Register service - Non generic version
    /// Dictionary -key: type, value: serviceInstance 
    /// </summary>
    /// <param name="type"></param>
    /// <param name="serviceInstance"></param>
    public void Register(Type type, object serviceInstance)
    {
        if (_services.ContainsKey(type) == false)
        {
            _services.Add(type, serviceInstance);
        }
        else
        {
            _services[type] = serviceInstance;
        }
    }

    /// <summary>
    /// Get service , returns the service object of type T
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    public T GetService<T>()
    {
        if (_services.TryGetValue(typeof(T), out object serviceObject))
        {
            return (T)serviceObject;
        }
        else
        {
            Debug.LogWarning($"Missing service {typeof(T)}, have you registrerd the service - {typeof(T)} - before use?");
            return default;
        }
    }

    /// <summary>
    /// Try to get service
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="service"></param>
    /// <returns></returns>
    public bool TryGetService<T>(out T service)
    {
        if (_services.TryGetValue(typeof(T), out object serviceObject))
        {
            service = (T)serviceObject;
            return true;
        }
        else
        {
            service = default;
            Debug.LogWarning($"Missing service {typeof(T)}, have you registrerd the service - {typeof(T)} - before use?");
            return false;
        }
    }
    
}
