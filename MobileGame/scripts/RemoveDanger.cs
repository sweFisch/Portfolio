using UnityEngine;

public class RemoveDanger : MonoBehaviour
{
    [SerializeField] GameObject _dangerTransform;
    [SerializeField] GameObject _particleSystem; // temp should call the pooler
    [SerializeField] Transform _particleSpawnPoint;

    public void DestroyDanger()
    {
        // Spawn Particles?
        var particleSystem  = Instantiate(_particleSystem);
        particleSystem.transform.position = _particleSpawnPoint.position;
        particleSystem.transform.rotation = _particleSpawnPoint.rotation; // shold be x -90 or not depending on ps
        Destroy(particleSystem, 2f);

        //ObjectPoolManager.ReturnObjectToPool(_dangerTransform);
        _dangerTransform.SetActive(false); // Todo - > Return to Pool
    }
    public void EnableDanger()
    {
        _dangerTransform.SetActive(true);
    }
}
