using System.Collections.Generic;
using System.Threading.Tasks;
using UnityEngine;

public interface IHighScore
{
    // Interface for the service locator , and usage

    /// <summary>
    /// Get a List<HighScoreData>
    /// </summary>
    /// <returns></returns>
    //List<HighScoreData> GetHighScoreList();

    Task<List<HighScoreData>> AsyncGetHighScoreList();

    void SaveHighScore(HighScoreData highScoreData); // ( the highscore data type? )

}
