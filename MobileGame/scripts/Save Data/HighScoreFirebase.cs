using Firebase;
using Firebase.Auth;
using Firebase.Database;
using Firebase.Extensions;

using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using TMPro;
using UnityEditor;
using UnityEngine;

public class HighScoreFirebase : MonoBehaviour, IHighScore
{
    FirebaseAuth _auth;
    FirebaseDatabase _db;

    public static int SortByScore(HighScoreData data1, HighScoreData data2)
    {
        return data1.score.CompareTo(data2.score);
    }
    
    public void SaveHighScore(HighScoreData highScoreData)
    {
        SubmitHighScoreEntry(highScoreData);
    }
    public void SubmitHighScoreEntry(HighScoreData highScoreData)
    {
        FirebaseApp.CheckAndFixDependenciesAsync().ContinueWithOnMainThread(task =>
        {
            if (task.Exception != null)
            {
                Debug.LogError(task.Exception);
            }

            _auth = FirebaseAuth.DefaultInstance;
            _db = FirebaseDatabase.DefaultInstance;


            if (_auth.CurrentUser == null || string.IsNullOrEmpty(_auth.CurrentUser.UserId)) 
            {
                Debug.Log("User is not signed in"); return; 
            }

            //highScoreData = new HighScoreData();
            highScoreData.name = _auth.CurrentUser.DisplayName; // email or display name
            highScoreData.user = "";

            string jsonBlob = JsonUtility.ToJson(highScoreData);
            print(jsonBlob);

            SendHighscoreToFirebase(_auth.CurrentUser.UserId, jsonBlob);
        });
        // call offline save
    }

    private void SendHighscoreToFirebase(string userID, string jsonBlob)
    {
        Debug.Log("Trying to write data...");

        _db.RootReference.Child("highscore").Child(userID).SetRawJsonValueAsync(jsonBlob).ContinueWithOnMainThread(task =>
        {
            if (task.Exception != null)
            {
                Debug.LogWarning(task.Exception);
            }
            else
            {
                Debug.Log("DataTestWrite: Complete");
            }
        });
    }


    public async Task<List<HighScoreData>> AsyncGetHighScoreList()
    {
        var dependecyStatus = await FirebaseApp.CheckAndFixDependenciesAsync();
        if (dependecyStatus != DependencyStatus.Available)
        {
            // Not handling errors so we break out early if it fails
            Debug.LogWarning("Service not available when retreiving HighScore List");
            return null;
        }

        _auth = FirebaseAuth.DefaultInstance;
        if (_auth.CurrentUser == null)
        {
            Debug.LogWarning("Not Signed in when retreiving HighScore List");
            return null;
        }
        _db = FirebaseDatabase.DefaultInstance;
        if (_db == null) { 
            Debug.LogWarning("Not Connection to database when retreiving HighScore List");
            return null; }
        
        int showTop = 100;
        DataSnapshot snapshot = null;

        // data feilds
        int rankingNr = 1;
        int currentPlayerRank = 6;
        List<HighScoreData> scores = null;

        await _db.RootReference.Child("highscore").OrderByChild("score").LimitToLast(showTop).GetValueAsync().ContinueWithOnMainThread(task =>
        {
            if (task.Exception != null)
            {
                Debug.LogWarning(task.Exception);
            }
            else if (task.IsFaulted)
            {
                Debug.LogWarning("Error retrieving highscores: " + task.Exception);
            }
            else if (task.IsCompleted)
            {
                snapshot = task.Result;
                //Debug.Log("task results");
            }
            else
            {
                Debug.Log("No Scores found");

            }
        });
        print(snapshot);
        if (snapshot != null && snapshot.ChildrenCount > 0) // check that list is not empty
        {
            scores = new List<HighScoreData>();
            // Reverse list to display top to bottom
            foreach (DataSnapshot childSnapshot in snapshot.Children.Reverse())
            {
                //print(childSnapshot.Key); // can probably be used to check id instead of email

                HighScoreData highscoreData = new HighScoreData();

                string playerName = childSnapshot.Child("name").Value.ToString();
                int score = int.Parse(childSnapshot.Child("score").Value.ToString());

                highscoreData.name = playerName;
                highscoreData.score = score;
                highscoreData.isCurrentUsersScore = false;

                // Flag that it is current users high score - for highlight in ui
                if (childSnapshot.Key == _auth.CurrentUser.UserId) // id check
                {
                    highscoreData.isCurrentUsersScore = true;
                    currentPlayerRank = rankingNr; // save the current ranking
                }
                scores.Add(highscoreData);
                rankingNr++; // increment rank
            }

            // This should be unnessesary but the indexing does not seem to be reealtime anymore on Firebase?
            scores.Sort(SortByScore);
            scores.Reverse();
            scores.TrimExcess();
        }

        return scores;
    }
}
