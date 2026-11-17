using UnityEngine;

public class ExternalJumpState : PlayerState
{
    private PlayerStateMachine _playerMachine;
    public ExternalJumpState(PlayerStateMachine player)
    {
        _playerMachine = player;
    }

    private float fastFallInputFreezeTime = 0f;

    public override void EnterState()
    {
        _playerMachine._isGrounded = false;
        fastFallInputFreezeTime = Time.time + _playerMachine.playerStats.fastFallInputWaitOnExternalJump;
    }

    public override void ExitState()
    {

    }
    public override void Update()
    {
        // External jump
        _playerMachine.HandleChangeLaneInput();

        if (_playerMachine._isGrounded == true)
        {
            _playerMachine.ChangeState(EPlayerState.Run);
            //_playerMachine.InvokeOnLanding();
        }

        if (Time.time > fastFallInputFreezeTime && InputCollector.DownSwipeWasPerformed)
        {
            _playerMachine.ChangeState(EPlayerState.FastFall);
        }
        
    }

    public override void FixedUpdate()
    {
        _playerMachine.HandleGravity();
        _playerMachine.GroundCheck();
        _playerMachine.CheckLaneChangingStatus();

        // Check Collisions ? maybe not ? TODO ?
        //_playerMachine.CheckFrontCollision();
        //_playerMachine.CheckSideCollision();

        _playerMachine.UpdatePlayerPosition();
    }

}
