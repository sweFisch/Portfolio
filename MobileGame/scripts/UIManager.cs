using System;
using System.Collections;
using System.Collections.Generic;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class UIManager : MonoBehaviour
{
    public static UIManager Instance;

    [Header("Game UI")]
    [SerializeField] private TextMeshProUGUI _scoreTMP;
    [SerializeField] private TextMeshProUGUI _coinTMP;
    [SerializeField] private TextMeshProUGUI _speedTMP;
    [SerializeField] private TextMeshProUGUI _clockTimeTMP;
    [SerializeField] private TextMeshProUGUI _fpsCounterTMP;

    [Header("PowerUp UI Panel")]
    [SerializeField] private Transform _powerUpPanel;
    [SerializeField] private UIPowerUpItem _powerUpItemPrefab;
    private List<UIPowerUpItem> _powerUpItemList = new();


    [Header("Game Over Screen")]
    [SerializeField] private CanvasGroup _gameOverScreen;
    [SerializeField] private Image _blackImage;
    [SerializeField] private GameObject _gameOverText;
    [SerializeField] private TextMeshProUGUI _finalScoreText;
    [SerializeField] private TextMeshProUGUI _messageCurrentFinalScore;
    [SerializeField] private TextMeshProUGUI _currentHighScoreText;

    [Header("PauseMenu")]
    [SerializeField] private GameObject _PauseScreen;

    [Header("Settings Screen")]
    [SerializeField] private GameObject _SettingsScreen;

    [Header("Profile Screen")]
    [SerializeField] private GameObject _ProfileScreen;

    [Header("Shop Screen")]
    [SerializeField] private GameObject _ShopScreen;

    [Header("Missions Screen")]
    [SerializeField] private GameObject _MissionsScreen;

    [Header("Highscore Screen")]
    [SerializeField] private GameObject _HighScoreScreen;

    [Header("HOME SCREEN BUTTONS")]
    [SerializeField] private Button _homeButton;
    [SerializeField] private Button _missionButton;
    [SerializeField] private Button _shopButton;

    public UIPowerUpItem GetUIPowerUpItem()
    {
        if (_powerUpItemList.Count <= 0)
        {
            // Create a new one and return it
            UIPowerUpItem item = Instantiate(_powerUpItemPrefab, _powerUpPanel);
            _powerUpItemList.Add(item);
            return item;
        }
        else
        {
            // Check if there are deactivated objects we can use
            foreach (UIPowerUpItem listItem in _powerUpItemList)
            {
                if (listItem.gameObject.activeInHierarchy == false)
                {
                    return listItem;
                }
            }
            // Create a new one and return it
            UIPowerUpItem item = Instantiate(_powerUpItemPrefab, _powerUpPanel);
            _powerUpItemList.Add(item);
            return item;
        }
    }

    private void Awake()
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

    IHighScore highScoreService;
    void Start()
    {
        highScoreService = ServiceLocator.Instance.GetService<IHighScore>();
        InitializeUI();
    }

    private void SaveHighScore(int score)
    {
        HighScoreData highScoreData = new HighScoreData();
        highScoreData.score = score;
        highScoreData.name = "offline";
        highScoreData.isCurrentUsersScore = true;

        highScoreService.SaveHighScore(highScoreData);
    }

    private void InitializeUI()
    {
        StopAllCoroutines();

        _gameOverScreen.alpha = 0;
        _gameOverScreen.gameObject.SetActive(false);

        HideAllScreens();

        UpdateScoreTMP(0f);
        UpdateCoinsTMP(0);

        _powerUpItemList = new List<UIPowerUpItem>();
        for (int i = 0; i < 3; i++)
        {
            UIPowerUpItem item = Instantiate(_powerUpItemPrefab, _powerUpPanel);
            item.gameObject.SetActive(false);
            _powerUpItemList.Add(item);
        }
        _homeButton.Select();
        
    }

    public void HideAllScreens()
    {
        // Handle Screens
        _PauseScreen.SetActive(false);
        _SettingsScreen.SetActive(false);
        _ProfileScreen.SetActive(false);
        _MissionsScreen.SetActive(false);
        _ShopScreen.SetActive(false);
        _HighScoreScreen.SetActive(false);
    }


    public void ResetUIManager()
    {
        InitializeUI();
    }

    private void OnEnable()
    {
        PlayerStateMachine.OnDeath += Player_OnDeath;
        StopAllCoroutines();
    }


    private void OnDisable()
    {
        PlayerStateMachine.OnDeath -= Player_OnDeath;
        StopAllCoroutines();
    }
    private void Player_OnDeath()
    {
        StartCoroutine(DeathScreenCO());
    }

    private IEnumerator DeathScreenCO()
    {
        int currentHighScore = CheckHighScore();
        yield return new WaitForSeconds(1f);
        //yield return StartCoroutine(FadeImage(_blackImage,0f,0.5f,0.2f));
        //_gameOverText.SetActive(true);
        _gameOverScreen.gameObject.SetActive(true);
        if (_finalScoreText != null) 
        { 
            _finalScoreText.text = Mathf.Round(GameManager.Instance._runData.Score).ToString("#");
        }
        if (_currentHighScoreText != null) 
        { 
            _currentHighScoreText.text = Mathf.Round(currentHighScore).ToString("#");
        }

        if (_messageCurrentFinalScore != null)
        {
            if (currentHighScore == Mathf.Round(GameManager.Instance._runData.Score))
            {
                _messageCurrentFinalScore.text = "New Highscore!";

                // save to firebase after saving score locally...?
                SaveHighScore(currentHighScore); // this might by crazy
            }
            else
            {
                _messageCurrentFinalScore.text = "Good try!";
            }
        }

        yield return StartCoroutine(FadeCanvasGroup(_gameOverScreen, 0f, 1f, 0.3f));
    }

    private int CheckHighScore()
    {
        // load highscore data
        float currentHighScore = (float)SaveData.GetHighScore();
        //print(_currentHighScore);

        if (currentHighScore < GameManager.Instance._runData.Score)
        {
            currentHighScore = GameManager.Instance._runData.Score;
        }
        int saveScore = Mathf.RoundToInt(currentHighScore);


        SaveData.SaveHighScore(saveScore);
        // Save High Score data

        return saveScore;
    }

    // Debug button Hide - TODO
    public void ClearData()
    {
        float currentHighScore = 0;
        int saveScore = Mathf.RoundToInt(currentHighScore);
        SaveData.SaveHighScore(saveScore);
        if(_currentHighScoreText != null)
        {
            _currentHighScoreText.text = Mathf.Round(currentHighScore).ToString("#");
        }

        //SaveData.ClearData();
    }

    private IEnumerator FadeImage(Image image, float startAlpha, float endAlpha, float duration)
    {
        image.color = new Color(image.color.r, image.color.g, image.color.b, startAlpha);
        float currentTime = 0f;
        float currentAlpha = 0f;
        float t;
        yield return null;
        while (currentTime < duration && duration > 0f)
        {
            currentTime += Time.deltaTime;
            t = currentTime / duration;
            currentAlpha = Mathf.Lerp(startAlpha, endAlpha, t);
            image.color = new Color(image.color.r, image.color.g, image.color.b, currentAlpha);
            yield return null;
        }
        image.color = new Color(image.color.r, image.color.g, image.color.b, endAlpha);
    }
    private IEnumerator FadeCanvasGroup(CanvasGroup canvasGroup, float startAlpha, float endAlpha, float duration)
    {
        canvasGroup.alpha = startAlpha;
        float currentTime = 0f;
        float currentAlpha = 0f;
        float t;
        yield return null;
        while (currentTime < duration && duration > 0f)
        {
            currentTime += Time.deltaTime;
            t = currentTime / duration;
            currentAlpha = Mathf.Lerp(startAlpha, endAlpha, t);
            canvasGroup.alpha = currentAlpha;
            yield return null;
        }
        canvasGroup.alpha = endAlpha;
    }



    private void Update()
    {
        if(_fpsCounterTMP != null)
        {
            _fpsCounterTMP.text = ((int)(1f / Time.unscaledDeltaTime)).ToString();
        }
    }
    private void FixedUpdate()
    {
        UpdateScoreTMP(GameManager.Instance._runData.Score);
        UpdateCoinsTMP(GameManager.Instance._runData.Coins);
        UpdateSpeedTMP(GameManager.Instance._runData.Speed);
        UpdateClockTMP();
    }


    #region UPDATE GAME UI SCREEN

    public void UpdateScoreTMP(float value)
    {
        if (_scoreTMP != null)
        {
            _scoreTMP.text = Mathf.RoundToInt(value).ToString();
        }
    }
    public void UpdateCoinsTMP(int value)
    {
        if (_coinTMP != null)
        {
            _coinTMP.text = value.ToString();
        }
    }

    public void UpdateSpeedTMP(float currentSpeed)
    {
        if(_speedTMP != null)
        {
            _speedTMP.text = currentSpeed.ToString("#.##");
        }
    }
    public void UpdateClockTMP()
    {
        if(_clockTimeTMP != null)
        {
            _clockTimeTMP.text = Mathf.Round(GameManager.Instance._runData.RunTime).ToString("###");
        }
    }

    #endregion

    #region Handle PauseMenu

    public void ShowPauseScreen()
    {
        HideAllScreens();
        _PauseScreen.SetActive(true);
        PauseGame();
    }
    public void HidePauseScreen()
    {
        _PauseScreen.SetActive(false);
        UnPauseGame();
    }

    // PAUSE
    private void PauseGame()
    {
        Time.timeScale = 0.0f;
    }
    private void UnPauseGame()
    {
        Time.timeScale = 1.0f;
    }

    #endregion

    #region Settings
    public void ToggleSettingsScreen()
    {
        ToggleScreen(_SettingsScreen);
        //bool setTo = _SettingsScreen.activeInHierarchy;
        //setTo = !setTo;
        //HideAllScreens();

        //_SettingsScreen.SetActive((setTo));
    }
    #endregion

    #region Placeholder Screens
    public void ToggleProfileScreen()
    {
        ToggleScreen(_ProfileScreen);
    }

    public void ToggleShopScreen()
    {
        ToggleScreen(_ShopScreen);
    }
    public void ToggleMissionsScreen()
    {
        ToggleScreen(_MissionsScreen);
    }
    public void ToggleHighScoreScreen()
    {
        //HideAllScreens();
        //bool setTo = _HighScoreScreen.activeInHierarchy;
        //setTo = !setTo;
        //_HighScoreScreen.SetActive(setTo);
        ToggleScreen(_HighScoreScreen);
    }

    #endregion

    private void ToggleScreen(GameObject screenGO)
    {
        bool setTo = screenGO.activeInHierarchy;
        setTo = !setTo;
        HideAllScreens();
        //Debug.Log($"settings are active : {setTo}");
        screenGO.SetActive((setTo));
    }

    public void ReloadCurrentScene()
    {
        UnPauseGame();
        SceneManager.LoadScene(0);
    }
}
