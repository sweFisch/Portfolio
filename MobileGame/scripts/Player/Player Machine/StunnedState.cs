using UnityEngine;

public class StunnedState : PlayerState
{
    private PlayerStateMachine _playerMachine;
    public StunnedState(PlayerStateMachine player)
    {
        _playerMachine = player;
    }

    float exitTime = 0f;

    public override void EnterState()
    {
        _playerMachine.InvokeOnStunned();
        exitTime = Time.time + _playerMachine.playerStats.stunnedCoolDown;
    }

    public override void ExitState()
    {

    }
    public override void Update()
    {
        if (Time.time > exitTime) 
        {
            //Debug.Log($"{Time.frameCount} - Exit Stunned State Cooldown");
            _playerMachine.ChangeState(EPlayerState.Run); // might give some problems (in Air -> stunned -> Run)
        }
    }

    public override void FixedUpdate()
    {
        _playerMachine.HandleGravity();
        _playerMachine.GroundCheck();
        _playerMachine.CheckLaneChangingStatus();

        // Check Collisions
        _playerMachine.CheckFrontCollision();
        //_playerMachine.CheckSideCollision();

        _playerMachine.UpdatePlayerPosition();
    }
}
