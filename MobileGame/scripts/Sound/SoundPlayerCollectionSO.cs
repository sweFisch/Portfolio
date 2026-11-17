using UnityEngine;

[CreateAssetMenu(fileName ="PlayerSoundsCollectionSO", menuName =("SoundSO/PlayerSoundsCollectionSO"))]
public class SoundPlayerCollectionSO : ScriptableObject
{
    [Header("SFX")]
    public SoundSO[] Jump;
    public SoundSO[] Land;
    public SoundSO[] Roll;
    public SoundSO[] PlayerDamage;
    public SoundSO[] PlayerDeath;
    public SoundSO[] ChangeLane;
    public SoundSO[] Attack;
}
