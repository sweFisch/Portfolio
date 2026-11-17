using System.Collections;
using Unity.VisualScripting.FullSerializer;
using UnityEngine;

public class FlyingState : PlayerState
{
    private PlayerStateMachine _playerMachine;
    public FlyingState(PlayerStateMachine player)
    {
        _playerMachine = player;
    }

    private bool _startFlying = false;
    private float _timeToApex = 1f;
    private float _timer = 0f;
    private float _apexHeight = 10f;
    private float currentHeight = 0f;
    private float startHeight = 0f;
    public override void EnterState()
    {
        //Debug.Log("--- Flying State! ---");
        _playerMachine._isGrounded = false;
        _playerMachine._playerVelocity = Vector3.zero;
        _timer = 0f;
        currentHeight = 0f;
        startHeight = _playerMachine._rb.position.y;
        _startFlying = true;
    }

    public override void ExitState()
    {
        
    }
    public override void Update()
    {
        if (_startFlying) { return; }

        // Possible Duck? mech
        // attack ?

        _playerMachine.HandleChangeLaneInput();

        if (_playerMachine._powerUpManager.IsPowerUpActive(EPowerUp.Flying) == false)
        {
            _playerMachine.ChangeState(EPlayerState.Run);
        }
    }

    public override void FixedUpdate()
    {
        if (_startFlying) 
        { 
            StartFlying();
            return;
        }

        //_playerMachine.HandleGravity();
        //_playerMachine.GroundCheck();
        _playerMachine.CheckLaneChangingStatus();

        // Check Collisions
        _playerMachine.CheckFrontCollision();
        _playerMachine.CheckSideCollision();

        _playerMachine.UpdatePlayerPosition();
    }

    private void StartFlying()
    {
        if (_startFlying && _timer < _timeToApex)
        {
            _timer = _timer + Time.fixedDeltaTime;
            currentHeight = Mathf.SmoothStep(startHeight, _apexHeight, Mathf.Clamp01(_timer / _timeToApex));

            //Debug.Log($"Current height : {currentHeight}");

            _playerMachine._rb.MovePosition(new Vector3(_playerMachine._rb.position.x, currentHeight, 0f));
        }
        else 
        {
            //Debug.Log($"Stopped start Flying: timer {_timer}");
            _startFlying = false; 
        }
    }


}
