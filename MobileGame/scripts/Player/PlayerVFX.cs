using System;
using System.Collections;
using UnityEngine;

public class PlayerVFX : MonoBehaviour
{
    public static PlayerVFX Instance;


    [SerializeField] ParticleSystem _pickupPS;
    [SerializeField] ParticleSystem _stunnedPS;
    [SerializeField] ParticleSystem _landingPS;
    [SerializeField] ParticleSystem _runningPS;

    private bool _onGround = false;

    Coroutine _stunnedCO;
    private void OnEnable()
    {
        PlayerStateMachine.OnStunned += PlayStunnedEffect;
        PlayerStateMachine.OnLanding += PlayLandingEffect;
        PlayerStateMachine.OnJump += PlayJumpEffect;
    }
    private void OnDisable()
    {
        PlayerStateMachine.OnStunned -= PlayStunnedEffect;
        PlayerStateMachine.OnLanding -= PlayLandingEffect;
        PlayerStateMachine.OnJump -= PlayJumpEffect;
    }

    private void FixedUpdate()
    {
        RunningParticleEffect();
    }

    public void RunningParticleEffect()
    {
        var runningEmission = _runningPS.emission;
        if (_onGround && GameManager.Instance.StopGame == false)
        {
            runningEmission.rateOverTime = GameManager.Instance.GameSpeed;
        }
        else
        {
            runningEmission.rateOverTime = 0f;
        }
    }

    public void PlayPickupEffect()
    {
        _pickupPS.Play();
    }

    public void PlayStunnedEffect() 
    {
        if (_stunnedCO != null)
        {
            StopCoroutine(_stunnedCO);
        }
        _stunnedPS.Play();
        _stunnedCO = StartCoroutine(StunnedEffectCO());
    }

    public void PlayLandingEffect()
    {
        _landingPS.Play();
        _onGround = true;
    }

    public void PlayJumpEffect()
    {
        _landingPS.Play();
        _onGround = false;
    }

    private IEnumerator StunnedEffectCO()
    {
        float timer = 0f;
        while (timer < 1f)
        {
            _stunnedPS.transform.Rotate(new Vector3(0f, 0f, Time.deltaTime * 180f));
            yield return null;
        }
        _stunnedCO = null;
    }
}
