using UnityEngine;

public class RunState : PlayerState
{
    private PlayerStateMachine _playerMachine;
    public RunState(PlayerStateMachine player)
    {
        _playerMachine = player;
    }

    public override void EnterState()
    {
        _playerMachine.GroundCheck();
        
        if (_playerMachine._isGrounded) // Test!
        {
            _playerMachine.InvokeOnRun(); // ? Maybee ?
        }
    }

    public override void ExitState()
    {
        
    }

    public override void Update()
    {
        _playerMachine.HandleChangeLaneInput();

        if (InputCollector.UpSwipeWasPerformed)
        {
            _playerMachine.ChangeState(EPlayerState.Jump);
        }
        if (InputCollector.DownSwipeWasPerformed)
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
