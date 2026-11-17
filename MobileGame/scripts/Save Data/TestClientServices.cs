using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;
using UnityEngine.InputSystem;

public class TestClientServices : MonoBehaviour
{

    IHighScore highScoreService;
    void Start()
    {
        highScoreService = ServiceLocator.Instance.GetService<IHighScore>();
        Application.targetFrameRate = 60;
    }

    // Update is called once per frame
    void Update()
    {
        if (Keyboard.current[Key.L].wasPressedThisFrame) 
        {
            print("Pressed L key");
            LoadHighScoreAsync();
        }

        if (Keyboard.current[Key.S].wasPressedThisFrame)
        {
            print("Pressed S key");
            SaveHighScore();
        }
    }

    private void SaveHighScore()
    {
        HighScoreData highScoreData = new HighScoreData();
        highScoreData.score = UnityEngine.Random.Range(0, 100);
        highScoreData.name = "offline";

        highScoreService.SaveHighScore(highScoreData);
    }

    private async void LoadHighScoreAsync()
    {
        print("Running task - Fetch HighscoreList");

        //print(Time.frameCount);
        List<HighScoreData> highscoreList = new List<HighScoreData>();
        highscoreList = await highScoreService.AsyncGetHighScoreList();
        //print(Time.frameCount);

        if (highscoreList == null) { Debug.Log("Missing Highscore Data localy");return; }

        print(highscoreList);

        foreach(HighScoreData highscore in highscoreList)
        {
            Debug.Log($"{highscore.name} : {highscore.score}");
        }
        print("Done - Fetch HighscoreList");
    }
}
