using UnityEngine;

public class RollState : PlayerState
{
    private PlayerStateMachine _playerMachine;
    public RollState(PlayerStateMachine player)
    {
        _playerMachine = player;
    }

    float _exitTime;

    public override void EnterState()
    {
        _playerMachine.InvokeOnDuck(); // TODO decide on name

        _exitTime = Time.time + _playerMachine.playerStats.RollTime;

        _playerMachine._currentHeight = _playerMachine.playerStats.colliderHeightRoll;
        //_playerMachine._currentHeight = Mathf.Lerp(_colliderHeightRunning, _colliderHeightDucking, t);
        _playerMachine._capsuleCollider.height = _playerMachine._currentHeight;
        _playerMachine._capsuleCollider.center = new Vector3(0, _playerMachine._currentHeight / 2f, 0);
    }

    public override void ExitState()
    {
        _playerMachine._currentHeight = _playerMachine.playerStats.colliderHeightRun;
        _playerMachine._capsuleCollider.height = _playerMachine._currentHeight;
        _playerMachine._capsuleCollider.center = new Vector3(0, _playerMachine._currentHeight / 2f, 0);
    }

    public override void Update()
    {
        _playerMachine.HandleChangeLaneInput();

        if (_exitTime < Time.time)
        {
            _playerMachine.ChangeState(EPlayerState.Run);
        }
        else if (InputCollector.UpSwipeWasPerformed)
        {
            _playerMachine.ChangeState(EPlayerState.Jump);
        }
        //else if (InputCollector.DownSwipeWasPerformed)
        //{
        //    _playerMachine.ChangeState(EPlayerState.Roll);
        //}
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
