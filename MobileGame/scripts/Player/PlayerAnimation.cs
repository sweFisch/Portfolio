using UnityEngine;

public class PlayerAnimation : MonoBehaviour
{
    [SerializeField] Animator _animator;

    private static int RUN_ANIMATION = Animator.StringToHash("PlayerRun");
    private static int JUMP_ANIMATION = Animator.StringToHash("PlayerJump");
    private static int DUCK_ANIMATION = Animator.StringToHash("PlayerDuck");
    private static int DEATH_ANIMATION = Animator.StringToHash("PlayerDeath");

    private int _currentAnimation = RUN_ANIMATION;

    private float _timer = 0;

    private void OnEnable()
    {
        PlayerStateMachine.OnRunning += Player_OnRunning;
        PlayerStateMachine.OnStartGame += Player_OnStartGame;
        PlayerStateMachine.OnJump += Player_OnJump;
        PlayerStateMachine.OnDownInputJumpCancel += Player_OnDownInputJumpCancel;
        PlayerStateMachine.OnDownInputDucking += Player_OnDownInputDucking;
        PlayerStateMachine.OnChangeLeft += Player_OnChangeLeft;
        PlayerStateMachine.OnChangeRight += Player_OnChangeRight;
        PlayerStateMachine.OnStunned += Player_OnSlightHit;
        PlayerStateMachine.OnDeath += Player_OnDeath;
        PlayerStateMachine.OnLanding += Player_OnLanding;
    }

    private void OnDisable()
    {
        PlayerStateMachine.OnRunning -= Player_OnRunning;
        PlayerStateMachine.OnStartGame -= Player_OnStartGame;
        PlayerStateMachine.OnJump -= Player_OnJump;
        PlayerStateMachine.OnDownInputJumpCancel -= Player_OnDownInputJumpCancel;
        PlayerStateMachine.OnDownInputDucking -= Player_OnDownInputDucking;
        PlayerStateMachine.OnChangeLeft -= Player_OnChangeLeft;
        PlayerStateMachine.OnChangeRight -= Player_OnChangeRight;
        PlayerStateMachine.OnStunned -= Player_OnSlightHit;
        PlayerStateMachine.OnDeath -= Player_OnDeath;
        PlayerStateMachine.OnLanding -= Player_OnLanding;

    }
    private void Player_OnStartGame()
    {
        Player_OnRunning();
    }

    private void Player_OnJump()
    {
        _currentAnimation = JUMP_ANIMATION;
        _animator.CrossFade(_currentAnimation, 0f,0);
    }
    private void Player_OnDownInputJumpCancel()
    {
        Player_OnRunning();
    }
    private void Player_OnDownInputDucking()
    {
        _currentAnimation = DUCK_ANIMATION;
        _animator.CrossFade(_currentAnimation, 0f, 0);
    }
    private void Player_OnChangeLeft()
    {
    }
    private void Player_OnChangeRight()
    {
    }
    private void Player_OnSlightHit()
    {
    }
    private void Player_OnDeath()
    {
        _currentAnimation = DEATH_ANIMATION;
        _animator.CrossFade(_currentAnimation,0f,0);
    }

    private void Player_OnRunning()
    {
        _timer = 0f;
        _currentAnimation = RUN_ANIMATION;
        _animator.CrossFade(_currentAnimation, 0.15f, 0);
    }

    private void Player_OnLanding()
    {
        Player_OnRunning();
    }
    private void Update()
    {
        if (_currentAnimation == RUN_ANIMATION)
        {
            return;
        }
        else if (_timer < 1f && _currentAnimation != RUN_ANIMATION)
        {
            _timer += Time.deltaTime;
        }
        else if (_timer >= 1f && _currentAnimation != RUN_ANIMATION) 
        {
            _timer = 0f;
            Player_OnRunning();
        }
    }

}
