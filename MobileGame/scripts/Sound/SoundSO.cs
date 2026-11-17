using UnityEngine;

[CreateAssetMenu(fileName = "SoundSO", menuName = ("SoundSO/SoundSO"))]
public class SoundSO : ScriptableObject
{
    public enum AudioTypes
    {
        SFX,
        Music,
    }

    public AudioTypes audioTypes;
    public AudioClip clip;
    public bool Loop = false;
    public bool RandomizePitch = false;
    [Range(0f,1f)]
    public float RandomPitchRangeModifier = 0.1f;
    [Range(0.1f,2f)]
    public float Volume = 1f;
    [Range(0.1f,3f)]
    public float Pitch = 1f;
}
