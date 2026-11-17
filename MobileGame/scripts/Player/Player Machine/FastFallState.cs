using UnityEngine;

public class FastFallState : PlayerState
{

    private PlayerStateMachine _playerMachine;

    public FastFallState(PlayerStateMachine player)
    {
        _playerMachine = player;
    }

    public override void EnterState()
    {
        _playerMachine.InvokeOnFastFall();
        _playerMachine._playerVelocity.y = _playerMachine.playerStats.fastFallDownForce;

    }

    public override void ExitState()
    {

    }

    public override void Update()
    {
        _playerMachine.HandleChangeLaneInput();

        if (_playerMachine._isGrounded)
        {
            _playerMachine.ChangeState(EPlayerState.Roll);
        }
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
