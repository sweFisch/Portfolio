using UnityEngine;
using UnityEngine.UI;

public class UISettingsScreen : MonoBehaviour
{
    [SerializeField] private Slider _masterVolumeSlider;
    [SerializeField] private Slider _musicVolumeSlider;
    [SerializeField] private Slider _sfxVolumeSlider;

    private const float MIN_VALUE = 0.0001f;


    private void OnEnable()
    {
        float master = Mathf.Max(PlayerPrefs.GetFloat("MasterVolume", 1f), MIN_VALUE);
        float music = Mathf.Max(PlayerPrefs.GetFloat("MusicVolume", 1f), MIN_VALUE);
        float sfx = Mathf.Max(PlayerPrefs.GetFloat("SfxVolume", 1f), MIN_VALUE);
        _masterVolumeSlider.value = master;
        _musicVolumeSlider.value = music;
        _sfxVolumeSlider.value = sfx;
    }
}
