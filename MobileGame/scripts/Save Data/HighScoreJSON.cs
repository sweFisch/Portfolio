using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

public class HighScoreJSON : MonoBehaviour, IHighScore
{
    // make this class to a scervice locator ?
    // that can save to either firebase or to JSON
    // persistent data path -> C:\Users\nikla\AppData\LocalLow\BoopleStudio\nfMobileGame

    //const string HIGH_SCORE = "Highscore";
    const string HIGH_SCORE_FILE = "/highscore.json";

    public class JsonHighScoreSaveData
    {
        public List<HighScoreData> saveHighscoreList;
    }

    public async Task<List<HighScoreData>> AsyncGetHighScoreList()
    {
        await Task.Delay(1);
        return GetHighScore();
    }

    public void SaveHighScore(HighScoreData highScoreData)
    {
        JsonHighScoreSaveData saveData = new JsonHighScoreSaveData();
        saveData.saveHighscoreList = new List<HighScoreData>();
        saveData.saveHighscoreList = GetHighScore();
        if (saveData.saveHighscoreList == null || saveData.saveHighscoreList.Count <= 0)
        {
            Debug.Log("Missing highscore list");
            saveData.saveHighscoreList = new List<HighScoreData>();
        }
        if (saveData.saveHighscoreList.Count > 0) // this will make the current score be the only one highlighted offline
        {
            foreach (HighScoreData entry in saveData.saveHighscoreList)
            {
                entry.isCurrentUsersScore = false;
            }
        }

        saveData.saveHighscoreList.Add(highScoreData);
        print("hs data - " + highScoreData);
        print("saveData - " + saveData.saveHighscoreList);
        print("saveData - " + saveData.saveHighscoreList.Count);

        // Save highscore list
        string jsonString = JsonUtility.ToJson(saveData);
        Debug.Log(jsonString);

        // Save to file
        SaveToFile(HIGH_SCORE_FILE, jsonString); // <- should call SaveData.SaveToFile 
    }

    private List<HighScoreData> GetHighScore()
    {
        string fileData = LoadFromFile(HIGH_SCORE_FILE) ?? string.Empty;
        JsonHighScoreSaveData saveData = null;

        if (fileData != null && fileData != string.Empty)
        {
            saveData = JsonUtility.FromJson<JsonHighScoreSaveData>(fileData);
        }

        List<HighScoreData> highScoreDataList = new List<HighScoreData>();

        if (saveData != null && saveData.saveHighscoreList != null && saveData.saveHighscoreList.Count >= 0)
        {
            highScoreDataList = saveData.saveHighscoreList;
        }

        if (highScoreDataList == null) 
        {
            Debug.Log("Loaded List is null");
            return null;
        }
        if (highScoreDataList.Count <= 0)
        {
            Debug.Log("List is empty");
            return null;
        }

        highScoreDataList = highScoreDataList.OrderBy(x => x.score).Reverse().Take(5).ToList();

        return highScoreDataList;
    }

    #region Load and Save Data Functions
    // Saving to File
    public static void SaveToFile(string fileName, string jsonString)
    {

        // Open a file in write mode. This will create the file if it's missing.
        // It is assumed that the path already exists.
        using (FileStream stream = File.OpenWrite(Application.persistentDataPath + fileName))
        {
            // Truncate the file if it exists (we want to overwrite the file)
            stream.SetLength(0);

            // Convert the string into bytes. Assume that the character-encoding is UTF8.
            // Do you not know what encoding you have? Then you have UTF-8
            var bytes = Encoding.UTF8.GetBytes(jsonString);

            // Write the bytes to the hard-drive
            stream.Write(bytes, 0, bytes.Length);

            // The "using" statement will automatically close the stream after we leave
            // the scope - this is VERY important
        }
    }

    // Loading from File
    //Return the content of the file in a string
    public static string LoadFromFile(string fileName)
    {
        if (File.Exists(Application.persistentDataPath + fileName) == false)
        {
            //Debug.Log($"Missing file {Application.persistentDataPath}{fileName}");
            return null;
        }
        // Open a stream for the supplied file name as a text file
        using (var stream = File.OpenText(Application.persistentDataPath + fileName))
        {
            // Read the entire file and return the result. This assumes that we've written the
            // file in UTF-8
            return stream.ReadToEnd();
        }
    }

    internal static void ClearData() // SaveData using a file to clear
    {
        HighScoreData data = new HighScoreData();
        data.score = 0;

        string jsonString = JsonUtility.ToJson(data);
        Debug.Log(jsonString);

        // Save to file
        SaveToFile(HIGH_SCORE_FILE, jsonString);
    }
    #endregion
}
