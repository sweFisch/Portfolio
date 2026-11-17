using System;
using System.Collections.Generic;
using UnityEngine;

public class ServiceInstaller : MonoBehaviour
{
    // https://www.youtube.com/watch?v=y4mn3kmxRx0&t=43s
    [Header("Debug commands")]
    [SerializeField] bool useLocalHighscore;

    [Header("Services to install")]
    //[SerializeField] HighScoreJSON highScoreService;
    [SerializeField] HighScoreFirebase highScoreFirebaseService;
    [SerializeField] HighScoreJSON highScoreJSONService;

    [SerializeField] PlayerCredentialsFirebase playerCredentialsFirebaseService;

    [SerializeField] bool installFromList = false;
    [SerializeField] List<MonoBehaviour> servicesToInstallList = new List<MonoBehaviour>();

    private void Awake()
    {
        ServiceLocator serviceLocator = ServiceLocator.Instance;

        // Check if online and if not use offline with timestamp?
        if (useLocalHighscore)
        {
            serviceLocator.Register<IHighScore>(highScoreJSONService);
        }
        else
        {
            serviceLocator.Register<IHighScore>(highScoreFirebaseService);
        }

        serviceLocator.Register<IPlayerCredentials>(playerCredentialsFirebaseService);


        // Possible to create an instance and Register it as well

        if (installFromList)
        {
            InstallServicesFromList(serviceLocator);
        }
    }

    private void InstallServicesFromList(ServiceLocator serviceLocator)
    {
        // This can be problematic if there are multiple interfaces on the class

        foreach (MonoBehaviour service in servicesToInstallList)
        {
            Type serviceType = service.GetType();
            Type serviceInterface = serviceType.GetInterfaces()[0];

            if (serviceInterface != null)
            {
                serviceLocator.Register(serviceInterface, service);
            }
            else
            {
                serviceLocator.Register(serviceType, service);
            }
        }
    }
}
