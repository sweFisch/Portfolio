using Unity.VisualScripting.FullSerializer;
using UnityEngine;

public class DeadState : PlayerState
{
    private PlayerStateMachine _playerMachine;
    public DeadState(PlayerStateMachine player)
    {
        _playerMachine = player;
    }

    public override void EnterState()
    {
        _playerMachine.InvokeOnDeath();
        //Debug.Log($"{Time.frameCount} - Dead State");
    }

    public override void ExitState()
    {

    }

    public override void FixedUpdate()
    {
        _playerMachine.HandleGravity();
        _playerMachine.GroundCheck();
        //_playerMachine.CheckLaneChangingStatus();
        _playerMachine.UpdatePlayerPosition();
    }

    public override void Update()
    {

    }

}
