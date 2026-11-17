using System;
using UnityEngine;
using UnityEngine.InputSystem;

public class InputCollector : MonoBehaviour
{
    private InputSystem_EndlessRunner playerInput;

    //public static event Action OnSwipeUp;
    //public static event Action OnSwipeDown;
    //public static event Action OnSwipeLeft;
    //public static event Action OnSwipeRight;
    //public static event Action OnTap;

    public static Vector2 Movement;
    public static bool UpSwipeWasPerformed;
    public static bool DownSwipeWasPerformed;
    public static bool LeftSwipeWasPerformed;
    public static bool RightSwipeWasPerformed;
    public static bool TapWasPressed;
    public static bool DoubleTapWasPressed;

    private InputAction _moveAction;
    private InputAction _jumpAction;
    private InputAction _downAction;

    private InputAction _restartAction;

    //private InputAction _primaryTouch;
    private InputAction _primaryTouchPress;
    private InputAction _primaryTouchPosition;

    //[SerializeField] bool _useTouch = true;
    private bool _touchActive = false;
    private float touchTimer = 0f;
    private float touchEndTime = 0.15f;
    private float doubleTapTimer = 0f;

    Vector2 startPos = Vector2.zero;
    Vector2 endPos = Vector2.zero;



    private void Awake()
    {
        playerInput = new InputSystem_EndlessRunner();
    }

    private void OnEnable()
    {
        playerInput.Enable();
        playerInput.Touch.Enable();
    }

    private void OnDisable()
    {
        playerInput.Disable();
        playerInput.Touch.Disable();

    }

    private void Start()
    {
        _moveAction = playerInput.Player.Move;
        _jumpAction = playerInput.Player.Jump;
        _downAction = playerInput.Player.Down;
        _restartAction = playerInput.Player.Restart;

        //_primaryTouch = playerInput.Touch.TouchSwipe;
        _primaryTouchPosition = playerInput.Touch.TouchPosition;
        _primaryTouchPress = playerInput.Touch.TouchPress;
    }

    private void Update()
    {
        Movement = Vector2.zero;
        UpSwipeWasPerformed = false;
        DownSwipeWasPerformed = false;
        LeftSwipeWasPerformed = false;
        RightSwipeWasPerformed = false;

        TapWasPressed = false;
        DoubleTapWasPressed = false;

        if (_primaryTouchPress.WasPressedThisFrame())
        {
            startPos = _primaryTouchPosition.ReadValue<Vector2>();
            _touchActive = true;
        }
        if (_touchActive) 
        {
            touchTimer += Time.deltaTime;
            if(touchTimer > touchEndTime)
            {
                EndTouch();
            }
        }
        if (_primaryTouchPress.WasReleasedThisFrame() && _touchActive)
        {
            EndTouch();
        }


        //if(_useTouch == false)
        //{
        //    ComputerControls();
        //}

        //if (_restartAction.WasPressedThisFrame()) 
        //{
        //    RestartLevel();
        //}
    }

    private void EndTouch()
    {
        _touchActive = false;
        touchTimer = 0f;

        endPos = _primaryTouchPosition.ReadValue<Vector2>();
        //print($"StartPos : {startPos}");
        //print($"StartPos : {endPos}");
        
        Vector2 delta = endPos - startPos;
        //print($"Delta : {delta}");

        if(delta.magnitude < 20f) 
        { 
            if (doubleTapTimer + 0.2f > Time.unscaledTime)
            {
                DoubleTapWasPressed = true;
            }
            //print("TapDetected");
            TapWasPressed = true;
            doubleTapTimer = Time.unscaledTime; // Add timestamp
            return;
        }
        
        // Process Swipe direction
        if (Mathf.Abs(delta.x) < Mathf.Abs(delta.y))
        {
            if (delta.y > 0f)
            {
                //print("Up Swipe");
                UpSwipeWasPerformed = true;
            }
            else
            {
                //print("Down Swipe");
                DownSwipeWasPerformed = true;
            }
        }
        else
        {
            if (delta.x > 0f)
            {
                //print("Right Swipe");
                RightSwipeWasPerformed = true;
            }
            else
            {
                //print("Left Swipe");
                LeftSwipeWasPerformed = true;
            }
        }
    }

    private void ComputerControls()
    {
        Movement = _moveAction.ReadValue<Vector2>();
        UpSwipeWasPerformed = _jumpAction.WasPressedThisFrame();
        DownSwipeWasPerformed = _downAction.WasPressedThisFrame();

        //JumpIsHeld = _jumpAction.IsPressed();
        //JumpWasReleased = _jumpAction.WasReleasedThisFrame();
    }
}
