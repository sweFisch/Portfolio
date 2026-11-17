using Firebase.Database;
using System;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.UI;

public class HighscoreUITest : MonoBehaviour
{
    public Button _submitScoreButton;
    public Button _updateHighScoreButton;

    public TextMeshProUGUI _currentUserTMP;
    public TextMeshProUGUI _currentScoreTMP;

    public Transform highScoreListTransformRoot;
    public GameObject highscoreEntryPrefab;

    public int EntriesToShow = 5;

    IHighScore highScoreService;
    void Start()
    {
        highScoreService = ServiceLocator.Instance.GetService<IHighScore>();
    }

    private void OnEnable()
    {
        _submitScoreButton?.onClick.AddListener(SubmitHighScoreEntry);
        _updateHighScoreButton?.onClick.AddListener(UpdateHighScoreList);
    }

    private void OnDisable()
    {
        _submitScoreButton?.onClick.RemoveAllListeners();
        _updateHighScoreButton?.onClick.RemoveAllListeners();
    }

    private void Update()
    {
        if (Keyboard.current[Key.Backspace].wasPressedThisFrame)
        {
            print("--- Clear Json Data ---");
            SaveData.ClearData();
        }
    }

    public void SubmitHighScoreEntry()
    {
        SaveHighScore();
    }

    private void UpdateHighScoreList()
    {
        LoadHighScoreAsync();
    }


    private void SaveHighScore()
    {
        HighScoreData highScoreData = new HighScoreData();
        highScoreData.score = UnityEngine.Random.Range(0, 100);
        highScoreData.name = "offline";
        highScoreData.isCurrentUsersScore = true;

        highScoreService.SaveHighScore(highScoreData);
    }

    private async void LoadHighScoreAsync()
    {
        print("Running task - Fetch HighscoreList");

        //print(Time.frameCount);
        List<HighScoreData> highscoreList = new List<HighScoreData>();
        highscoreList = await highScoreService.AsyncGetHighScoreList();
        //print(Time.frameCount);

        //print(highscoreList);

        //foreach (HighScoreData highscore in highscoreList)
        //{
        //    Debug.Log($"{highscore.name} : {highscore.score}");
        //}
        print("Done - Fetch HighscoreList");

        if (highscoreList == null) { Debug.Log("Missing Highscore Data"); return; }

        DisplayHighScore(highscoreList);
    }

    // Assumes ordered list as input
    private void DisplayHighScore(List<HighScoreData> highScoreDataList)
    {
        RemoveAllChildren();

        int rankingNr = 1;
        int currentPlayerRank = 1000;
        int count = 0;
        foreach (HighScoreData highscoreEntry in highScoreDataList)
        {
            string playerName = highscoreEntry.name.ToString();
            //int score = int.Parse(highscoreEntry.score.ToString());
            int score = highscoreEntry.score;

            GameObject newScore = Instantiate(highscoreEntryPrefab, highScoreListTransformRoot.transform);
            newScore.transform.GetChild(1).GetComponent<TextMeshProUGUI>().text = rankingNr.ToString() + " - " + playerName; // Added ranking nr
            newScore.transform.GetChild(2).GetComponent<TextMeshProUGUI>().text = score.ToString();
            
            // Highligt Logged in player if in high score list 
            if (highscoreEntry.isCurrentUsersScore) // id check
            {
                newScore.transform.GetChild(0).GetComponent<Image>().color = Color.red; // change background
                currentPlayerRank = rankingNr; // save the current ranking
            }
            rankingNr++; // increment rank

            count++;
            if(count >= EntriesToShow)
            {
                break;
            }
        }
    }

    private void RemoveAllChildren()
    {
        foreach (Transform child in highScoreListTransformRoot)
        {
            Destroy(child.gameObject);
        }
    }

}
