using System;
using UnityEngine;
using UnityEngine.InputSystem;

public class GameManager : MonoBehaviour
{
    public static GameManager Instance; // Singelton

    [SerializeField] private bool _limitFramerate = false;
    [SerializeField] private bool _spawnWorld = true;
    

    [SerializeField] private LevelGenerator _levelGenerator;
    
    public RunData _runData { get; private set; }

    public bool StopGame { get; private set; }
    public float GameSpeed { get; set; }
    // socring
    private float _scoreMultiplier = 1f;
    private float _gameSpeedProgress = 0f;

    private Vector3 _gameSpeedVector3 = Vector3.zero;


    void Awake()
    {
        if (Instance == null)
        {
            Instance = this;
        }
        else
        {
            Destroy(gameObject);
        }
    }

    private void Start()
    {
        InitGame();

        StopGame = true;

        //Set main game setting
        if (_limitFramerate)
        {
            Application.targetFrameRate = 60;
        }
    }

    public void StartGame() // called from main menu button
    {
        StopGame = false;
        UIManager.Instance.ResetUIManager();
        PlayerStateMachine player = FindAnyObjectByType(typeof(PlayerStateMachine)) as PlayerStateMachine;
        player?.ResetPlayer();
    }

    public void RestartLevel() // Called from UI button Restart
    {
        _runData = new RunData(); // save current run data

        //_levelGenerator.ResetLevel();
        InitGame();
        UIManager.Instance.ResetUIManager();
        PlayerStateMachine player = FindAnyObjectByType(typeof(PlayerStateMachine)) as PlayerStateMachine;
        player?.ResetPlayer();
    }


    #region Subscribe to events
    private void OnEnable()
    {
        PlayerStateMachine.OnDeath += Player_OnDeath;
    }

    private void OnDisable()
    {
        PlayerStateMachine.OnDeath -= Player_OnDeath;
    }
    #endregion

    private void Player_OnDeath()
    {
        StopGame = true;
    }

    private void InitGame()
    {
        _runData = new RunData();

        if (_spawnWorld == false) { return; }

        _levelGenerator.InitLevel(); // Set up Level Generation
        
        // Reset all varibles
        _scoreMultiplier = 1f;
        GameSpeed = 10f; // todo create a start speed
        _gameSpeedProgress = 0f;
        _gameSpeedVector3 = Vector3.zero;

        StopGame = false;
     
    }

    private void Update()
    {
        HandleDebugKeys();

        if (StopGame || _spawnWorld == false) { return; }

        HandleGameSpeed();
        _levelGenerator.MoveLevel(_gameSpeedVector3);
    }

    private void FixedUpdate()
    {
        if (StopGame) { return; }

        // UI Stuff
        UpdateScore();
        UpdateRunTime();
        _runData.Speed = GameSpeed + _gameSpeedProgress;
    }

    private void HandleDebugKeys()
    {
        if (Keyboard.current[Key.Digit8].wasPressedThisFrame)
        {
            GameSpeed -= 1f;
        }
        if (Keyboard.current[Key.Digit9].wasPressedThisFrame)
        {
            GameSpeed += 1f;
        }

        if (Keyboard.current[Key.P].wasPressedThisFrame) 
        {
            // Purge player Prefs
            print("Purge Player Prefs");
            PlayerPrefs.DeleteAll();
        }
        if (Keyboard.current[Key.Space].wasPressedThisFrame)
        {
            StopGame = !StopGame;
        }

        if (InputCollector.DoubleTapWasPressed)
        {
            _gameSpeedProgress += 1f;
        }
    }

    private void UpdateScore()
    {
        _runData.Score += _scoreMultiplier * GameSpeed * Time.deltaTime;
    }

    private void HandleGameSpeed()
    {
        _gameSpeedProgress += (Time.deltaTime * 0.1f); // Increment Speed over time
        _gameSpeedVector3 = new Vector3(0f, 0f, -1f * (GameSpeed + _gameSpeedProgress) * Time.deltaTime);
    }

    private void UpdateRunTime()
    {
        _runData.RunTime += Time.fixedDeltaTime;
    }
}
