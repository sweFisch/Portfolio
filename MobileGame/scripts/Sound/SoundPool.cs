using UnityEngine;

/// <summary>
/// Oldest audio source gets reused, Ignores if its still active
/// </summary>
public class SoundPool : MonoBehaviour
{
    [SerializeField] private AudioSource _soundFXObject;

    private AudioSource[] _soundFXArray;
    private int _poolSize = 10;

    private int _index = 0;

    void Start()
    {
        _soundFXArray = new AudioSource[_poolSize];

        for (int i = 0; i < _poolSize; i++)
        {
            AudioSource newAudioSource = Instantiate(_soundFXObject, transform.position, Quaternion.identity, transform);
            _soundFXArray[i] = newAudioSource;
            _soundFXArray[i].gameObject.SetActive(false);
        }
    }

    public AudioSource GetAudioObject(Vector3 point)
    {
        _index++;
        _index = _index % _poolSize;

        _soundFXArray[_index].gameObject.SetActive(true);
        _soundFXArray[_index].transform.position = point;

        return _soundFXArray[_index];
    }

    public void ReturnAudioObject(AudioSource audioObject)
    {
        audioObject.gameObject.SetActive(false);
    }
}
