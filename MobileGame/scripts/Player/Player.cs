using System;
using System.Collections;
using UnityEngine;

/// <summary>
/// Old prototype code before moving to a state machine
/// </summary>
public class Player : MonoBehaviour
{
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

    private Rigidbody _rb;
    private bool _isDead = false;
    
    // Stunned variables - TODO look at!
    private bool _isStunned = false;
    private float _stunnedTimer = 0f;
    Coroutine _stunnedCO;

    // MOVE
    [SerializeField] float _laneSize = 1f; // SO
    [SerializeField] float _coolDown = 0.2f; // SO
    [SerializeField] private float moveXSpeed = 2f; // SO

    [SerializeField] float _lastLangeChange = -10f; // timer
    [SerializeField] bool _movementInputReset = true; // reset bool for input
    private float _targetPosition = 0f; // 
    private float moveLerpTimeX = 0f; // 

    private bool _changingLaneRight = false;
    private bool _changingLaneLeft = false;
    private float _previousTargetLane = 0f;
    private bool _outOfBounds = false;

    // JUMP
    [SerializeField] float _coolDownJump = 0.6f;        // SO
    [SerializeField] float _coolDownJumpTimer = -10f;   // SO
    [SerializeField] float _jumpForce = 5;              // SO
    [SerializeField] float _gravityValue = -10f;        // SO
    private Vector3 _playerVelocity = Vector3.zero;

    [Header("GroundCheck")]
    [SerializeField] private Transform _groundCheckTransform;
    [SerializeField] private float _groundCheckDistance = 0.2f; // SO
    [SerializeField] private LayerMask _groundLayer; // SO
    [SerializeField] private bool _isGrounded = false;
    [SerializeField] private float _downForceFromDownInput = -20f; // SO

    [Header("MainCollider")]
    private float _colliderHeightRunning = 1.4f; // SO
    private float _colliderHeightDucking = 0.6f; // has to be radius * 2 minimum // SO
    private float _capsuleRadius = 0.3f; // SO

    [SerializeField] CapsuleCollider _capsuleCollider;
    private float _currentHeight = 1.4f; 
    //private float _capsuleCenter = 0f;

    private bool _isDucking = false;
    private Coroutine _duckingCoroutine;
    private bool _duckOnLanding = false;

    [Header("Debug")] // SO
    [SerializeField] private bool _debugJump = false;
    [SerializeField] private float _debugGameSpeed = 10f;
    [SerializeField] private int _debugFrames = 60;


    private void Awake()
    {
        _rb = GetComponent<Rigidbody>();
    }

    public void ResetPlayer()
    {
        _isGrounded = false;
        _isStunned = false;
        _stunnedTimer = 0f;
        if (_stunnedCO != null) { StopCoroutine( _stunnedCO ); }
        _stunnedCO = null;

        _currentHeight = _colliderHeightRunning;
        
        _changingLaneRight = false;
        _changingLaneLeft = false;
        _previousTargetLane = 0f; // previuos Target Position
        _outOfBounds = false;

        _playerVelocity = Vector3.zero;

        transform.position = Vector3.up * 0.1f;
        _targetPosition = 0f;

        moveLerpTimeX = 0f;
        _coolDownJumpTimer = -10f;
        _lastLangeChange = -10f;
        
        _isDead = false;
        OnStartGame?.Invoke();
    }

    private void Update()
    {

        if (_isDead) {  return; }

        HandleJump();
        HandleDownInput();

        ChangeLane();

    }

    private void FixedUpdate()
    {
        HandleGravity();

        GroundCheck();

        if (_isDead) { return; }
        CheckLaneChangingStatus();
        CheckCollision();

        // Update player position
        moveLerpTimeX += Time.deltaTime * moveXSpeed;
        moveLerpTimeX = Mathf.Clamp01(moveLerpTimeX);

        float newPositionX = Mathf.Lerp(transform.position.x, _targetPosition, moveLerpTimeX);

        _rb.MovePosition(new Vector3(newPositionX, _rb.position.y + _playerVelocity.y * Time.fixedDeltaTime, 0f));

    }

    private void HandleGravity()
    {
        if (_isGrounded == false)
        {
            _playerVelocity.y += _gravityValue * Time.fixedDeltaTime;
        }
    }

    private void HandleDownInput()
    {
        if ((InputCollector.DownSwipeWasPerformed || _duckOnLanding) && _isGrounded && _duckingCoroutine == null)
        {
            _duckOnLanding = false;

            //print("Ducking");
            OnDownInputDucking?.Invoke();

            if (_duckingCoroutine == null)
            {
                _duckingCoroutine = StartCoroutine(DuckCO());
            }
        }

        // Down force for fast landing
        if (InputCollector.DownSwipeWasPerformed && !_isGrounded && !_duckOnLanding)
        {
            //print("Down");
            OnDownInputJumpCancel?.Invoke();

            //_rb.linearVelocity = new Vector3(_rb.linearVelocity.x,-20f,0f);
            _playerVelocity.y = _downForceFromDownInput;
            
            _coolDownJumpTimer = Time.time - 10f; // Reset Jump Cooldown
            _duckOnLanding = true;
        }
    }

    private IEnumerator DuckCO()
    {
        float currentTime = 0f;
        float t = 0f;
        float timeToDuck = 0.1f;
        _currentHeight = _colliderHeightRunning;

        _isDucking = true;

        while (currentTime < timeToDuck)
        {
            currentTime += Time.deltaTime;
            t = currentTime / timeToDuck;
            _currentHeight = Mathf.Lerp(_colliderHeightRunning, _colliderHeightDucking, t);
            _capsuleCollider.height = _currentHeight;
            _capsuleCollider.center = new Vector3(0, _currentHeight/2f, 0);
            //_capsuleCenter = new Vector3(0, _currentHeight / 2f, 0);
            //_rb.linearVelocity = new Vector3(_rb.linearVelocity.x, -20f, 0f); // Extra gravity
            
            yield return null;
        }
        _currentHeight = _colliderHeightDucking;
        _capsuleCollider.height = _currentHeight;
        _capsuleCollider.center = new Vector3(0, _currentHeight / 2f, 0);
        yield return new WaitForSeconds(0.4f);

        currentTime = 0f;
        while (currentTime < timeToDuck)
        {
            currentTime += Time.deltaTime;
            t = currentTime / timeToDuck;
            _currentHeight = Mathf.Lerp(_colliderHeightDucking, _colliderHeightRunning, t);
            _capsuleCollider.height = _currentHeight;
            _capsuleCollider.center = new Vector3(0, _currentHeight / 2f, 0);
            //_rb.linearVelocity = new Vector3(_rb.linearVelocity.x,-20f,0f); // Extra gravity
            yield return null;
        }
        _currentHeight = _colliderHeightRunning;
        _capsuleCollider.height = _currentHeight;
        _capsuleCollider.center = new Vector3(0, _currentHeight / 2f, 0);

        _isDucking = false;
        _duckingCoroutine = null;
        OnRunning?.Invoke();
    }

    private void ChangeLane()
    {
        if (_lastLangeChange > Time.time)
        {
            return;
        }
        //if (InputCollector.RightSwipeWasPerformed)
        //{
        //    _movementInputReset = true;
        //}
        if (InputCollector.RightSwipeWasPerformed)
        {
            _movementInputReset = false;
            _changingLaneRight = true;
            OnChangeRight?.Invoke();

            //print("Right");
            moveLerpTimeX = 0f;
            
            _lastLangeChange = Time.time + _coolDown;
            _previousTargetLane = _targetPosition;
            _targetPosition += _laneSize;
            // Check for wall raycast? or on collision
        }
        if (InputCollector.LeftSwipeWasPerformed)
        {
            _movementInputReset = false;
            _changingLaneLeft = true;
            OnChangeLeft?.Invoke();

            //print("Left");
            moveLerpTimeX = 0f;

            _lastLangeChange = Time.time + _coolDown;
            _previousTargetLane = _targetPosition;
            _targetPosition -= _laneSize;
        }
        if(_targetPosition > _laneSize)
        {
            _outOfBounds = true;
            _targetPosition = _laneSize;
            StartStunned();
        }
        if (_targetPosition < -_laneSize)
        {
            _outOfBounds = true;
            _targetPosition = -_laneSize;
            StartStunned();
        }
    }
    private void CheckLaneChangingStatus()
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

    private void StartStunned()
    {
        // TODO - Turn off current direction Check!

        if (_isStunned == false && _stunnedTimer <= 0.0001f)
        {
            _isStunned = true;
            // Play some kind of animation
            OnStunned?.Invoke();
            if (_stunnedCO == null)
            {
                _stunnedCO = StartCoroutine(StunnedCO());
            }
        }
        else if (_stunnedTimer > 0.3f) // Stunned Cooldown
        {
            //print("Stunned and Then Dead");
            _isDead = true;
            OnDeath?.Invoke();
        }
    }

    private IEnumerator StunnedCO()
    {
        _stunnedTimer = 0.1f;

        float offset = 0f; // = Mathf.Sign(transform.position.x) * 0.5f;
        if(_outOfBounds == true)
        {
            if (_changingLaneRight)
            {
                offset = 0.5f;
            }
            else if (_changingLaneLeft)
            {
                offset = -0.5f;
            }

            _targetPosition += offset;
            yield return new WaitForSeconds(0.1f);
        }

        _outOfBounds = false;
        _targetPosition = _previousTargetLane; // Return to last lane


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

    private void HandleJump()
    {
        if (InputCollector.UpSwipeWasPerformed && (_isGrounded || _isDucking))
        {
            //print($"Jump Pressed {Time.time}");

            if(_coolDownJumpTimer < Time.time)
            {
                StopDuckingAndResetHeight();
                OnJump?.Invoke();
                //print("Apply Jump Force");
                //_rb.linearVelocity = new Vector3(_rb.linearVelocity.x, _jumpForce, 0f);

                _playerVelocity.y = _jumpForce;
                //print(_playerVelocity);

                _coolDownJumpTimer = Time.time + _coolDownJump;
            }
            else
            {
                print("jump cooldown active");
            }
        }
    }

    private void StopDuckingAndResetHeight()
    {
        // Collider is running height - Stop Ducking Coroutine
        if (_duckingCoroutine != null)
        {
            StopCoroutine(_duckingCoroutine);
            _duckingCoroutine = null;
        }
        _isDucking = false;
        _capsuleCollider.height = _colliderHeightRunning;
        _capsuleCollider.center = new Vector3(0, _colliderHeightRunning / 2f, 0);

    }

    private void GroundCheck()
    {
        Vector3 p1 = _rb.position + Vector3.up * _currentHeight * 0.5f;

        if ( Physics.Raycast(p1, Vector3.down,out RaycastHit hit, _groundCheckDistance + _currentHeight * 0.5f, _groundLayer) 
            && _playerVelocity.y < 0.001f)
        {
            if (_isGrounded == false)
            {
                OnLanding?.Invoke();
            }

            _isGrounded = true;

            _playerVelocity = Vector3.down;

            _rb.position = (hit.point);
        }
        else
        {
            _isGrounded= false;
        }
    }


    private void CheckCollision()
    {
        //float heightCapsule = _currentHeight - 0.1f; 
        Vector3 p1 = transform.position + Vector3.up * (_capsuleRadius + 0.1f);
        Vector3 p2 = p1 + Vector3.up * (_currentHeight - _capsuleRadius * 2);
        float distance = 1f;
        //Debug.DrawLine(p1, p2, Color.red, 1f);


        if (Physics.CapsuleCast(p1, p2, _capsuleRadius, Vector3.forward, out RaycastHit hitForward, distance, _groundLayer, QueryTriggerInteraction.Ignore))
        {
            //print(hitForward.distance);
            // should be based on the current game speed last frame delta * speed
            if (hitForward.distance < 0.4f)
            {
                //print("Hit in front, currentHeight = " + _currentHeight);

                Debug.DrawLine(p1, p2, Color.red, 20f);
                _isDead = true;
                OnDeath?.Invoke();
            }
        }


        float sideDistance = 0.5f;
        if (_changingLaneRight)
        {
            if (Physics.CapsuleCast(p1, p2, _capsuleRadius, Vector3.right, out RaycastHit hitRight, sideDistance, _groundLayer, QueryTriggerInteraction.Ignore))
            {
                print("Hit in Right, currentHeight = " + _currentHeight);
                StartStunned();
            }
        }
        if (_changingLaneLeft)
        {
            if (Physics.CapsuleCast(p1, p2, _capsuleRadius, Vector3.left, out RaycastHit hitLeft, sideDistance, _groundLayer, QueryTriggerInteraction.Ignore))
            {
                print("Hit in Left, currentHeight = " + _currentHeight);
                StartStunned();
            }
        }
    }

    private void OnCollisionEnter(Collision collision)
    {
        DangerProp dangerProp = collision.gameObject.GetComponent<DangerProp>();
        if( dangerProp != null)
        {
            //print("Hit Danger Prop - Dead");
            OnDeath?.Invoke();
            _isDead = true;
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        DangerProp dangerProp = other.gameObject.GetComponent<DangerProp>();
        if (dangerProp != null)
        {
            StartStunned();
            //print("Hit Danger Prop - Half dead");
        }
    }

    public void EffectEnviromentJump(float externalJumpForce)
    {
        //print("Apply Jump Force from Enviroment");

        StopDuckingAndResetHeight();
        OnJump?.Invoke();

        _playerVelocity.y = externalJumpForce;

        _coolDownJumpTimer = Time.time + _coolDownJump;
    }


    #region Debug
    private void OnDrawGizmosSelected()
    {
        if (!_debugJump) { return; }

        Gizmos.color = Color.yellow;
        float deltaT = 1f / 60f;
        Vector3 p1 = transform.position;
        Vector3 p2 = transform.position;
        float currentVelocityY = 0f;
        float lastVelocityY = 0f;

        for (int i = 0; i < _debugFrames; i++)
        {
            currentVelocityY = (_jumpForce + (_gravityValue * (deltaT * i))) * (deltaT);
            p1.y = p2.y + currentVelocityY;
            p1.z = transform.position.z + (_debugGameSpeed * deltaT) * i;
            p1.x = transform.position.x;
            Gizmos.DrawLine(p1, p2);
            p2 = p1;
            lastVelocityY = currentVelocityY;
        }
    }
    #endregion
}
