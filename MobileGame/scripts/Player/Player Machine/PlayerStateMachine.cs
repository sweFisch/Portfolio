using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class PlayerStateMachine : MonoBehaviour
{
    // State Machine - Base States
    EPlayerState CurrentStateEnum;
    PlayerState CurrentState;

    PlayerMainMenueState playerMainMenueState;
    RunState runState;
    JumpState jumpState;
    FastFallState fastFallState;
    RollState rollState;
    ExternalJumpState externalJumpState;
    DeadState deadState;
    ChangeLaneState changeLaneState;
    ReturnToLaneState returnToLaneState;
    StunnedState stunnedState;
    FlyingState flyingState; // Powerup State
    // ---------------------------

    // Events
    public static event Action OnStartGame;
    public static event Action OnRunning;
    public static event Action OnJump;
    public static event Action OnLanding;
    public static event Action OnDownInputJumpCancel;
    public static event Action OnDownInputDucking;
    public static event Action OnChangeRight;
    public static event Action OnChangeLeft;
    public static event Action OnDeath;
    public static event Action OnStunned;

    internal void InvokeOnStartGame() { OnStartGame?.Invoke(); }
    internal void InvokeOnRun() { OnRunning?.Invoke();}
    internal void InvokeOnJump() { OnJump?.Invoke();}
    internal void InvokeOnLanding() { OnLanding?.Invoke();}
    internal void InvokeOnFastFall() { OnDownInputJumpCancel?.Invoke();}
    internal void InvokeOnDuck() { OnDownInputDucking?.Invoke();}
    internal void InvokeOnChangeLaneRight() { OnChangeRight?.Invoke();}
    internal void InvokeOnChangeLaneLeft() { OnChangeLeft?.Invoke();}
    internal void InvokeOnDeath() { OnDeath?.Invoke();}
    internal void InvokeOnStunned() { OnStunned?.Invoke(); }

    // ---------------------------

    // Stats
    public PlayerStatsSO playerStats;
    [SerializeField] internal PowerUpManager _powerUpManager; // PowerUp Manager Needed for checks
    // ---------------------------

    // Internal Variables
    internal Rigidbody _rb;
    internal float lastStunTime;
    internal Vector3 _playerVelocity = Vector3.zero;

    internal bool _isGrounded;

    internal float _currentHeight;
    public CapsuleCollider _capsuleCollider; // capsule collider

    // Lane Movement variables
    private float _targetPosition = 0f; // Lane target position
    private float moveLerpTimeX = 0f; // Timer for movement
    float _lastLangeChange = -10f; // Timer

    private bool _changingLaneRight = false;
    private bool _changingLaneLeft = false;
    private float _previousTargetLane = 0f;
    private bool _outOfBounds = false;

    // Stunned variables - TODO look at!
    private bool _isStunned = false;
    public float _stunnedTimer { get; set; }
    Coroutine _stunnedCO;

    // ---------------------------


    private void Awake()
    {
        _rb = GetComponent<Rigidbody>();
        SetupStateMachine();
    }

    //void Start()
    //{
    //    //ResetPlayer();
    //}

    private void SetupStateMachine()
    {
        playerMainMenueState = new PlayerMainMenueState();
        runState = new RunState(this);
        jumpState= new JumpState(this);
        fastFallState = new FastFallState(this);
        rollState = new RollState(this);
        externalJumpState = new ExternalJumpState(this);
        deadState = new DeadState(this);
        changeLaneState = new ChangeLaneState(this);
        returnToLaneState = new ReturnToLaneState(this);
        stunnedState = new StunnedState(this);
        flyingState = new FlyingState(this);

        CurrentStateEnum = EPlayerState.MainMenu;
        //CurrentState = runState;
        CurrentState = playerMainMenueState;
    }

    internal void ChangeState(EPlayerState newEPlayerState)
    {
        if (newEPlayerState == CurrentStateEnum) { return; }

        switch (newEPlayerState)
        {
            case EPlayerState.Run:
                ChangeStateLogic(newEPlayerState, runState);
                break;
            case EPlayerState.Jump:
                ChangeStateLogic(newEPlayerState, jumpState);
                break;
            case EPlayerState.FastFall:
                ChangeStateLogic(newEPlayerState, fastFallState);
                break;
            case EPlayerState.Roll:
                ChangeStateLogic(newEPlayerState, rollState);
                break;
            case EPlayerState.ExternalJump:
                ChangeStateLogic(newEPlayerState, externalJumpState);
                break;
            case EPlayerState.Dead:
                ChangeStateLogic(newEPlayerState, deadState);
                break;
            case EPlayerState.ChangeLane:
                ChangeStateLogic(newEPlayerState, changeLaneState);
                break;
            case EPlayerState.ReturnToLane:
                ChangeStateLogic(newEPlayerState, returnToLaneState);
                break;
            case EPlayerState.Stunned: // fundera om det behövs
                ChangeStateLogic(newEPlayerState, stunnedState);
                break;
            case EPlayerState.Flying:
                ChangeStateLogic(newEPlayerState, flyingState);
                break;
            case EPlayerState.MainMenu:
                ChangeStateLogic(newEPlayerState, playerMainMenueState);
                break;
        }
    }
    private void ChangeStateLogic(EPlayerState newEPlayerState, PlayerState newPlayerState)
    {
        CurrentState.ExitState();
        CurrentState = newPlayerState;
        CurrentState.EnterState();
        CurrentStateEnum = newEPlayerState;
    }


    public void ResetPlayer()
    {
        // Reset State - Maybee use reset state machine ?
        CurrentState = runState;
        CurrentStateEnum = EPlayerState.Run;

        _isGrounded = false;
        _isStunned = false;
        _stunnedTimer = 0f;
        if (_stunnedCO != null) { StopCoroutine(_stunnedCO); }
        _stunnedCO = null;

        _currentHeight = playerStats.colliderHeightRun;

        _changingLaneRight = false;
        _changingLaneLeft = false;
        _previousTargetLane = 0f; // previuos Target Position
        _outOfBounds = false;

        _playerVelocity = Vector3.zero;

        transform.position = Vector3.up * 0.1f;
        _targetPosition = 0f;

        moveLerpTimeX = 0f;
        //_coolDownJumpTimer = -10f;
        _lastLangeChange = -10f;

        InvokeOnStartGame();
    }
    
    void Update()
    {
        CurrentState.Update();
    }

    private void FixedUpdate()
    {
        CurrentState.FixedUpdate();
    }

    internal void HandleGravity()
    {
        if (_isGrounded == false)
        {
            _playerVelocity.y += playerStats.gravity * Time.fixedDeltaTime;
        }
    }

    // Common Functions that most States need and runs over state changes
    internal void HandleChangeLaneInput() // ChangeLane()
    {
        if (InputCollector.RightSwipeWasPerformed)
        {
            //print("Right");
            _changingLaneRight = true;
            OnChangeRight?.Invoke();

            moveLerpTimeX = 0f;

            _lastLangeChange = Time.time + playerStats.moveCoolDown;
            _previousTargetLane = _targetPosition;
            _targetPosition += playerStats._laneSize;
            // Check for wall raycast? or on collision
        }
        if (InputCollector.LeftSwipeWasPerformed)
        {
            //print("Left");
            _changingLaneLeft = true;
            OnChangeLeft?.Invoke();

            moveLerpTimeX = 0f;

            _lastLangeChange = Time.time + playerStats.moveCoolDown;
            _previousTargetLane = _targetPosition;
            _targetPosition -= playerStats._laneSize;
        }

        //Handle out of bounds
        if (_targetPosition > playerStats._laneSize) // Right
        {
            _outOfBounds = true;
            _targetPosition = playerStats._laneSize;
            StartStunned();
        }
        if (_targetPosition < -playerStats._laneSize) // Left
        {
            _outOfBounds = true;
            _targetPosition = -playerStats._laneSize;
            StartStunned();
        }
    }

    internal void CheckLaneChangingStatus()
    {
        if ((_changingLaneRight || _changingLaneLeft) == true && _lastLangeChange < Time.time)
        {
            if (Mathf.Abs(transform.position.x) - 0.1f <= Mathf.Abs(_targetPosition))
            {
                //print("Setting changeLane to False");
                _changingLaneRight = false;
                _changingLaneLeft = false;

            }
        }
    }

    public void UpdatePlayerPosition() // run in fixed update
    {
        // Update player position
        moveLerpTimeX += Time.deltaTime * playerStats.moveXSpeed;
        moveLerpTimeX = Mathf.Clamp01(moveLerpTimeX);

        float newPositionX = Mathf.Lerp(transform.position.x, _targetPosition, moveLerpTimeX);

        _rb.MovePosition(new Vector3(newPositionX, _rb.position.y + _playerVelocity.y * Time.fixedDeltaTime, 0f));
    }

    // External triggers and Efeects
    public void EffectEnviromentJump(float externalJumpForce)
    {
        //print("Apply Jump Force from Enviroment");
        if (CurrentStateEnum == EPlayerState.Dead) { return; }

        ChangeState(EPlayerState.ExternalJump);

        _playerVelocity.y = externalJumpForce; // Send event to camera ?

        InvokeOnJump(); // TODO - Should be external Jump specfic
    }

    public void EffectPowerUpFlying()
    {
        if (CurrentStateEnum == EPlayerState.Dead) { return; }
        ChangeState(EPlayerState.Flying);
        // Invoke Flying Event
    }


    #region Checks and Collisions
    internal void GroundCheck()
    {
        Vector3 p1 = _rb.position + Vector3.up * _currentHeight * 0.5f;

        if (Physics.Raycast(p1, Vector3.down, out RaycastHit hit, playerStats.groundCheckDistance + _currentHeight * 0.5f, playerStats.groundLayer)
            && _playerVelocity.y < 0.001f)
        {
            if (_isGrounded == false && CurrentStateEnum != EPlayerState.Dead) // TODO check out death
            {
                OnLanding?.Invoke();
            }

            _isGrounded = true;
            _playerVelocity = Vector3.down;
            _rb.position = (hit.point); // TODO - check if this should be someware else
        }
        else
        {
            _isGrounded = false;
        }
    }
    internal void CheckFrontCollision()
    {
        // TODO prediction adjustment?
        // TODO visualizer for collision detection?

        //float heightCapsule = _currentHeight - 0.1f; 
        Vector3 p1 = _rb.position + Vector3.up * (playerStats.capsuleRadius + 0.1f);
        Vector3 p2 = p1 + Vector3.up * (_currentHeight - playerStats.capsuleRadius * 2);
        float distance = 1f;
        //Debug.DrawLine(p1, p2, Color.red, 1f);

        if (Physics.CapsuleCast(p1, p2, playerStats.capsuleRadius, Vector3.forward, out RaycastHit hitForward, distance, playerStats.groundLayer, QueryTriggerInteraction.Ignore))
        {
            //print(hitForward.distance);
            
            // should be based on the current game speed last frame delta * speed
            if (hitForward.distance < 0.4f)
            {
                //print($"{Time.frameCount} - DEAD - P1 pure Transform - Radius = {(transform.position + Vector3.up * (playerStats.capsuleRadius))}");
                //print($"{Time.frameCount} - DEAD - P1 pure Transform = {(transform.position )}");
                //print($"{Time.frameCount} - DEAD - P1 pure RB pos = {(_rb.position )}");
                //print("Hit in front, currentHeight = " + _currentHeight);
                //Debug.DrawLine(p1, p2, Color.red, 20f);


                // Sheild PowerUp
                if (_powerUpManager.UsePowerUpIfActive(EPowerUp.Sheild)) // Sheild Powerup - implementation for stunned state
                {
                    //Debug.Log("Using power up sheild - During Stunned State");
                    //ChangeState(EPlayerState.Run);
                    UsingSheild();

                    return;
                }

                ChangeState(EPlayerState.Dead);
            }
        }
    }
    internal void CheckSideCollision()
    {
        Vector3 p1 = _rb.position + Vector3.up * (playerStats.capsuleRadius + 0.1f);
        Vector3 p2 = p1 + Vector3.up * (_currentHeight - playerStats.capsuleRadius * 2);
        
        //Debug.DrawLine(p1, p2, Color.red, 1f);

        // Lane change collision Checks
        float sideDistance = playerStats.sideCheckDistance;
        //if (CurrentStateEnum == EPlayerState.ChangeLane) // Change lane Right

        if (_changingLaneRight) // Change lane Right
        {
            if (Physics.CapsuleCast(p1, p2, playerStats.capsuleRadius, Vector3.right, out RaycastHit hitRight, sideDistance, playerStats.groundLayer, QueryTriggerInteraction.Ignore))
            {
                //print("Hit in Right, currentHeight = " + _currentHeight);
                StartStunned(); // change state to stunned!
            }
        }
        if (_changingLaneLeft)// _changingLaneLeft
        {
            if (Physics.CapsuleCast(p1, p2, playerStats.capsuleRadius, Vector3.left, out RaycastHit hitLeft, sideDistance, playerStats.groundLayer, QueryTriggerInteraction.Ignore))
            {
                //print("Hit in Left, currentHeight = " + _currentHeight);
                StartStunned(); // change state to stunned!
            }
        }
    }

    private void OnCollisionEnter(Collision collision)
    {
        DangerProp dangerProp = collision.gameObject.GetComponent<DangerProp>();
        if (dangerProp != null)
        {
            // Sheild PowerUp
            if (_powerUpManager.UsePowerUpIfActive(EPowerUp.Sheild)) // Sheild Powerup - implementation for stunned state
            {
                //Debug.Log("Using power up sheild - During Stunned State");
                //ChangeState(EPlayerState.Run);
                UsingSheild();

                return;
            }

            //print("Hit Danger Prop - Dead");
            ChangeState(EPlayerState.Dead);
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        DangerProp dangerProp = other.gameObject.GetComponent<DangerProp>();
        if (dangerProp != null)
        {
            StartStunned(); // TODO change state to stunned ?? maybe?
            //ChangeState(EPlayerState.Stunned);

            //print("Hit Danger Prop - Half dead");
        }
    }
    #endregion

    #region Stunned and Stunned Coroutine

    private void UsingSheild()
    {
        // What happens with the player after using the sheild
        // _powerUpManager.UsePowerUpIfActive(EPowerUp.Sheild) - if true

        CurrentState = runState;
        CurrentStateEnum = EPlayerState.Run;

        _isGrounded = false;
        _isStunned = false;
        _stunnedTimer = 0f;
        if (_stunnedCO != null) { StopCoroutine(_stunnedCO); }
        _stunnedCO = null;

        _currentHeight = playerStats.colliderHeightRun;

        _changingLaneRight = false;
        _changingLaneLeft = false;
        _previousTargetLane = 0f; // previuos Target Position
        _outOfBounds = false;

        _playerVelocity = Vector3.zero;

        //transform.position = Vector3.up * 0.1f;
        _targetPosition = 0f;

        moveLerpTimeX = 0f;
        //_coolDownJumpTimer = -10f;
        _lastLangeChange = -10f;


        InvokeOnStunned();
    }

    private void StartStunned()
    {
        // TODO - Turn off current direction Check!
        //print($"{Time.frameCount} - Start Stunned");
        
        if (_isStunned == false && _stunnedTimer <= 0.0001f)  // Start the stunned state 
        {
            _isStunned = true;
            ChangeState(EPlayerState.Stunned);

            if (_stunnedCO == null)
            {
                _stunnedCO = StartCoroutine(StunnedCO());
            }
        }
        if (_isStunned && _stunnedTimer > playerStats.stunnedCoolDown) // Stunned Cooldown for letting things play out
        {
            // Sheild PowerUp
            if (_powerUpManager.UsePowerUpIfActive(EPowerUp.Sheild)) // Sheild Powerup - implementation for stunned state
            {
                //Debug.Log("Using power up sheild - During Stunned State");
                //ChangeState(EPlayerState.Run);
                UsingSheild();

                return;
            }

            // Stunned and then Dead!
            //print($"{Time.frameCount} - Stunned and Then Dead");

            if (_changingLaneLeft || _changingLaneRight)
            {
                _targetPosition = _previousTargetLane; // Return to last lane
                if (_changingLaneRight)
                {
                    _targetPosition += 0.5f;
                }
                else if (_changingLaneLeft)
                {
                    _targetPosition += -0.5f;
                }
            }
            ChangeState(EPlayerState.Dead);
        }
    }

    private IEnumerator StunnedCO()
    {
        _stunnedTimer = 0.1f;

        float offset = 0f; // = Mathf.Sign(transform.position.x) * 0.5f;
        if (_outOfBounds == true)
        {
            //print($"{Time.frameCount} - stunnedCO out of bounds start : target : {_targetPosition}");
            if (_changingLaneRight)
            {
                offset = 0.5f;
            }
            else if (_changingLaneLeft)
            {
                offset = -0.5f;
            }
            _changingLaneLeft = false;
            _changingLaneRight = false;
            _outOfBounds = false;

            _targetPosition += offset;
            //print($"{Time.frameCount} - stunnedCO out of bounds new : target : {_targetPosition}");

            yield return new WaitForSeconds(0.1f);
            //print($"{Time.frameCount} - out of bounds still : target? : {_targetPosition}");
        }

        _outOfBounds = false;
        _targetPosition = _previousTargetLane; // Return to last lane
        //print($"{Time.frameCount} - out of bounds False new : target is previous: {_targetPosition}");


        while (_stunnedTimer < 2f)
        {
            _stunnedTimer += Time.deltaTime;
            yield return null;
        }
        _isStunned = false;
        _stunnedTimer = 0f;
        _stunnedCO = null;
        //print("Not stunned Anymore");
    }

    #endregion


    #region Debug
    private void OnDrawGizmosSelected()
    {
        if (playerStats == null || !playerStats._debugJump) { return; }

        Gizmos.color = Color.yellow;
        float deltaT = 1f / 60f;
        Vector3 p1 = transform.position;
        Vector3 p2 = transform.position;
        float currentVelocityY = 0f;
        float lastVelocityY = 0f;

        for (int i = 0; i < playerStats._debugFrames; i++)
        {
            
            currentVelocityY = (playerStats.jumpForce + (playerStats.gravity * (deltaT * i))) * (deltaT);
            p1.y = p2.y + currentVelocityY;
            p1.z = transform.position.z + (playerStats._debugGameSpeed * deltaT) * i;
            p1.x = transform.position.x;
            Gizmos.DrawLine(p1, p2);
            p2 = p1;
            lastVelocityY = currentVelocityY;
        }
    }
    #endregion
}
