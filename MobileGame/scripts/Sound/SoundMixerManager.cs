using UnityEngine;
using UnityEngine.Audio;

public class SoundMixerManager : MonoBehaviour
{
    [SerializeField] private AudioMixer audioMixer;

    // Dont forget to expose the attune parameters in the audio Mixer !
    // https://www.youtube.com/watch?v=DU7cgVsU2rM
    // For the mixer to work correctly make sure all audio sources are correctly assigned the Audio Mixer Channel

    private const float MIN_VALUE = 0.0001f;

    

    private void Start()
    {
        float master = Mathf.Max(PlayerPrefs.GetFloat("MasterVolume", 1f), MIN_VALUE);
        float music = Mathf.Max(PlayerPrefs.GetFloat("MusicVolume", 1f), MIN_VALUE);
        float sfx = Mathf.Max(PlayerPrefs.GetFloat("SfxVolume", 1f), MIN_VALUE);

        //float master = 1f;
        //float music = 1f;
        //float sfx = 1f;

        ApplyVolume("MasterVolume", master);
        ApplyVolume("MusicVolume", music);
        ApplyVolume("SoundFxVolume", sfx);
    }

    private void ApplyVolume(string exposedParam, float value)
    {

        value = Mathf.Max(value, MIN_VALUE);

        audioMixer.SetFloat(exposedParam, Mathf.Log10(value) * 20f);
    }

    public void SetMasterVolume(float level)
    {
        level = Mathf.Max(level, MIN_VALUE);

        ApplyVolume("MasterVolume", level);
        PlayerPrefs.SetFloat("MasterVolume", level);
    }

    public void SetFxVolume(float level)
    {
        level = Mathf.Max(level, MIN_VALUE);

        ApplyVolume("SoundFxVolume", level);
        PlayerPrefs.SetFloat("SfxVolume", level);
    }

    public void SetMusicVolume(float level)
    {
        level = Mathf.Max(level, MIN_VALUE);

        ApplyVolume("MusicVolume", level);
        PlayerPrefs.SetFloat("MusicVolume", level);
    }
}
