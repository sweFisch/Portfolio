using System;
using UnityEngine;
using UnityEngine.Audio;

public class PlayerSounds : MonoBehaviour
{
    // check - D:\UNITY\PROJECTS\gTV_MasterGameFeel\Assets\Scripts\Audio\   AudioManager.cs
    // for other more fleshed out script (Manager)

    [SerializeField] private AudioSource _jumpAudioSource;
    [SerializeField] private AudioSource _landAudioSource;
    [SerializeField] private AudioSource _duckAudioSource;
    [SerializeField] private AudioSource _attackAudioSource;

    [SerializeField] private SoundPlayerCollectionSO _playerCollectionSO;

    private void OnEnable()
    {
        PlayerStateMachine.OnJump += PlayerStateMachine_OnJump;
        PlayerStateMachine.OnLanding += PlayerStateMachine_OnLanding;
        PlayerStateMachine.OnDownInputDucking += PlayerStateMachine_OnDownInputDucking;
        PlayerStateMachine.OnStunned += PlayerStateMachine_OnStunned;
        PlayerStateMachine.OnDeath += PlayerStateMachine_OnDeath;

        PlayerStateMachine.OnChangeLeft += PlayerStateMachine_OnChangeLane;
        PlayerStateMachine.OnChangeRight += PlayerStateMachine_OnChangeLane;

        PlayerAttack.OnAttack += PlayerAttack_OnAttack;
    }



    private void OnDisable()
    {
        PlayerStateMachine.OnJump -= PlayerStateMachine_OnJump;
        PlayerStateMachine.OnLanding -= PlayerStateMachine_OnLanding;
        PlayerStateMachine.OnDownInputDucking -= PlayerStateMachine_OnDownInputDucking;
        PlayerStateMachine.OnStunned -= PlayerStateMachine_OnStunned;
        PlayerStateMachine.OnDeath -= PlayerStateMachine_OnDeath;

        PlayerStateMachine.OnChangeLeft -= PlayerStateMachine_OnChangeLane;
        PlayerStateMachine.OnChangeRight -= PlayerStateMachine_OnChangeLane;

        PlayerAttack.OnAttack -= PlayerAttack_OnAttack;

    }

    #region Sound Methods
    private void PlayRandomSound(SoundSO[] soundSOsArray, AudioSource audioSource)
    {
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


    #region Sound SFX Methods
    private void PlaySound(AudioClip clip, float pitch, float volume, bool loop, AudioSource audioSource)
    {
        audioSource.clip = clip;
        audioSource.pitch = pitch;
        audioSource.volume = volume;
        audioSource.loop = loop;
        audioSource.Play();
    }
    #endregion


    private void PlayerStateMachine_OnDownInputDucking()
    {
        PlayRandomSound(_playerCollectionSO.Roll, _duckAudioSource);
    }

    private void PlayerStateMachine_OnLanding()
    {
        PlayRandomSound(_playerCollectionSO.Land, _landAudioSource);
    }

    private void PlayerStateMachine_OnJump()
    {
        PlayRandomSound(_playerCollectionSO.Jump, _jumpAudioSource);
    }

    private void PlayerStateMachine_OnDeath()
    {
        PlayRandomSound(_playerCollectionSO.PlayerDeath, _jumpAudioSource);
    }

    private void PlayerStateMachine_OnStunned()
    {
        PlayRandomSound(_playerCollectionSO.PlayerDamage, _jumpAudioSource);
    }

    private void PlayerAttack_OnAttack()
    {
        PlayRandomSound(_playerCollectionSO.Attack, _attackAudioSource);
    }

    private void PlayerStateMachine_OnChangeLane()
    {
        PlayRandomSound(_playerCollectionSO.ChangeLane, _landAudioSource);
    }

    #endregion
}
