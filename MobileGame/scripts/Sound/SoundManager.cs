using UnityEngine;

public class SoundManager : MonoBehaviour
{
    // ATM using concepts from : https://www.youtube.com/watch?v=DU7cgVsU2rM - sasquatch B studios 
    // Maybee look into : https://www.youtube.com/watch?v=QL29aTa7J5Q as well? - Code Monkey

    public static SoundManager Instance; // Singelton

    [SerializeField] private AudioSource _soundFXObject;
    [SerializeField] private AudioSource _musicPlayer;

    [SerializeField] private SoundPool _soundPool;

    // Game Dependent Arrays of clips
    //[SerializeField] private AudioClip[] _coinPickup;
    //[SerializeField] private AudioClip[] _enemyTakeDamage;

    [SerializeField] private SoundsCollectionSO _coinPickUpSO;
    [SerializeField] private SoundsCollectionSO _enemyTakeDamageSO;

    private void Awake()
    {
        if (Instance == null)
        {
            Instance = this;
        }
        else
        {
            Destroy(gameObject);
        }
    }

    private void OnEnable()
    {
        
    }

    private void OnDisable()
    {
        
    }

    #region Play Sounds Main Functions
    public void PlaySoundFXClip(AudioClip audioClip, Vector3 point, float volume = 1f, bool randomPitch = true)
    {
        // Spawn gameobject
        //AudioSource newAudioSource = Instantiate(_soundFXObject, point, Quaternion.identity);
        AudioSource audioSource = _soundPool.GetAudioObject(point);

        // Assign audioClip,set volume and Play
        audioSource.clip = audioClip;
        audioSource.volume = volume;
        if (randomPitch) { audioSource.pitch = Random.Range(0.8f, 1.2f); }
        audioSource.Play();

        // Get lenght of clip
        float clipLenght = audioSource.clip.length;

        // Destroy or return to pool.
        //Destroy(newAudioSource.gameObject, clipLenght);
    }

    public void PlayRandomSoundFXClip(AudioClip[] audioClipsArray, Vector3 point, float volume = 1f, bool randomPitch = true)
    {
        if (audioClipsArray.Length <= 0) { return; }

        // Spawn gameobject
        //AudioSource newAudioSource = Instantiate(_soundFXObject, point, Quaternion.identity);
        AudioSource audioSource = _soundPool.GetAudioObject(point);

        // Assign audioClip from Array, set volume, Play sound
        int index = Random.Range(0, audioClipsArray.Length);
        audioSource.clip = audioClipsArray[index];
        audioSource.volume = volume;
        if (randomPitch) { audioSource.pitch = Random.Range(0.8f, 1.2f); }
        audioSource.Play();

        // Get lenght of clip
        float clipLenght = audioSource.clip.length;

        // Destroy or return to pool.
        //Destroy(newAudioSource.gameObject, clipLenght);
    }

    // Play sound from Sound Collection SO
    private void PlayRandomSoundFromSoundCollectionSO(SoundsCollectionSO soundCollectionSO, Vector3 point)
    {
        SoundSO[] soundSOsArray = soundCollectionSO.SoundSOArray;

        AudioSource audioSource = _soundPool.GetAudioObject(point);
        
        if (soundSOsArray != null && soundSOsArray.Length > 0)
        {
            int index = UnityEngine.Random.Range(0, soundSOsArray.Length);
            SoundToPlay(soundSOsArray[index], audioSource);
        }
    }

    private void SoundToPlay(SoundSO soundSO, AudioSource audioSource)
    {
        AudioClip clip = soundSO.clip;
        float pitch = soundSO.Pitch;
        float volume = soundSO.Volume;
        bool loop = soundSO.Loop;
        if (soundSO.RandomizePitch)
        {
            pitch = soundSO.Pitch + UnityEngine.Random.Range(-soundSO.RandomPitchRangeModifier, soundSO.RandomPitchRangeModifier);
        }

        PlaySound(clip, pitch, volume, loop, audioSource);
    }

    private void PlaySound(AudioClip clip, float pitch, float volume, bool loop, AudioSource audioSource)
    {
        audioSource.clip = clip;
        audioSource.pitch = pitch;
        audioSource.volume = volume;
        //audioSource.loop = loop; // dont allow looping
        audioSource.Play();
    }

    #endregion

    #region Custom Functions Game Dependent Sounds
    public void PlayCoinPickup(Vector3 point) // float volume = 1f
    {
        //PlayRandomSoundFXClip(coinPickup, point, volume);
        PlayRandomSoundFromSoundCollectionSO(_coinPickUpSO,point);
    }

    public void PlayEnemyTakeDamage(Vector3 point) // float volume = 1f
    {
        //PlayRandomSoundFXClip(_enemyTakeDamage, point, volume);
        PlayRandomSoundFromSoundCollectionSO(_enemyTakeDamageSO,point);
    }

    #endregion
}
