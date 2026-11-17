using UnityEngine;

public class RunData
{
    public float Score { get; set; }
    public int Coins { get; set; }
    public float Multiplier { get; set; } // player extra level?
    public float Speed { get; set; } 
    public float RunTime { get; set; }
    public int EnemiesKilled { get; set; }
    // obstacles jumped?
    // obstacles rolled?
    // Powerups collected?

    public RunData()
    {
        Score = 0;
        Coins = 0;
        Multiplier = 1;
        Speed = 0f;
        RunTime = 0f;
        EnemiesKilled = 0;
    }


}
