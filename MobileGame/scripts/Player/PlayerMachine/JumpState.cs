using UnityEngine;

public class JumpState : PlayerState
{
    private PlayerStateMachine _playerMachine;

    public JumpState(PlayerStateMachine player)
    {
        _playerMachine = player;
    }
    public override void EnterState()
    {
        _playerMachine.InvokeOnJump();
        _playerMachine._isGrounded = false;

        if (_playerMachine._powerUpManager.IsPowerUpActive(EPowerUp.PowerJump))
        {
            _playerMachine._playerVelocity.y = _playerMachine.playerStats.powerJumpForce;
        }
        else
        {
            _playerMachine._playerVelocity.y = _playerMachine.playerStats.jumpForce;
        }
    }

    public override void ExitState()
    {

    }
    public override void Update()
    {
        _playerMachine.HandleChangeLaneInput();

        if (_playerMachine._isGrounded == true)
        {
            _playerMachine.ChangeState(EPlayerState.Run);
            //_playerMachine.InvokeOnLanding();
        }

        if (InputCollector.DownSwipeWasPerformed)
        {
            _playerMachine.ChangeState(EPlayerState.FastFall);
        }
        // Double jump ability?
    }

    public override void FixedUpdate()
    {
        _playerMachine.HandleGravity();
        _playerMachine.GroundCheck();
        _playerMachine.CheckLaneChangingStatus();

        // Check Collisions
        _playerMachine.CheckFrontCollision();
        _playerMachine.CheckSideCollision();

        _playerMachine.UpdatePlayerPosition();
    }


}
